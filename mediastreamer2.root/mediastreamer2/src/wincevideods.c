/*
mediastreamer2 library - modular sound and video processing and streaming
Copyright (C) 2006  Simon MORLAT (simon.morlat@linphone.org)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

//#define AYMERIC_TEST
#define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA

#include "mediastreamer2/msvideo.h"
#include "mediastreamer2/msticker.h"
#include "mediastreamer2/msv4l.h"
#include "mediastreamer2/mswebcam.h"

#include <aygshell.h>
#include "nowebcam.h"
#if 0
#include <ffmpeg/avcodec.h>
#endif

#include <dshow.h>
#include <dmodshow.h>
#include <dmoreg.h>

#include <streams.h>
//#include <initguid.h>
#include "dxfilter.h"
#if 0
#include <qedit.h>
#endif
#include <atlbase.h>
#include <atlcom.h>

EXTERN_C const CLSID CLSID_NullRenderer;

typedef struct V4wState{

	char dev[512];
	int devidx;

	CComPtr<IGraphBuilder> m_pGraph;
	CComPtr<ICaptureGraphBuilder2> m_pBuilder;
	CComPtr<IMediaControl> m_pControl;
	CDXFilter *m_pDXFilter;
	CComPtr<IBaseFilter> m_pIDXFilter;	
	CComPtr<IBaseFilter> m_pNullRenderer;
	CComPtr<IBaseFilter> m_pDeviceFilter;
	DWORD rotregvalue;

	MSVideoSize vsize;
	int pix_fmt;
	mblk_t *mire[10];
	queue_t rq;
	ms_mutex_t mutex;
	int frame_ind;
	int frame_max;
	float fps;
	float start_time;
	int frame_count;
	bool_t running;
	bool_t startwith_yuv_bug; /* avoid bug with USB vimicro cards. */

	HWND videoHWND;
	bool priview;
	MSPicture local_pic;
	MSRect local_rect;
	int corner;
}V4wState;

static V4wState *s_callback=NULL;

static void dummy(void*p){
}

HRESULT ( Callback)(IMediaSample* pSample, REFERENCE_TIME* sTime, REFERENCE_TIME* eTime, BOOL changed)
{
	BYTE *byte_buf=NULL;
	mblk_t *buf;

	V4wState *s = s_callback;
	if (s==NULL)
		return S_OK;

	HRESULT hr = pSample->GetPointer(&byte_buf);
	if (FAILED(hr))
	{
		return S_OK;
	}
	int size = pSample->GetActualDataLength();
	if (size>+1000)
	{
		buf=allocb(size,0);
		memcpy(buf->b_wptr, byte_buf, size);
		if (s->pix_fmt==MS_RGB24)
		{

			unsigned long Index,nPixels;
			unsigned char *blue;
			unsigned char tmp;
			short iPixelSize;

			blue=buf->b_wptr;

			nPixels=s->vsize.width*s->vsize.height;
			iPixelSize=24/8;

			for(Index=0;Index!=nPixels;Index++)  // For each pixel
			{
				tmp=*blue;
				*blue=*(blue+2);
				*(blue+2)=tmp;
				blue+=iPixelSize;
			}

			unsigned char *pLine1, *pLine2;
			int iLineLen,iIndex;

			iLineLen=s->vsize.width*iPixelSize;
			pLine1=buf->b_wptr;
			pLine2=&(buf->b_wptr)[iLineLen * (s->vsize.height - 1)];

			for( ;pLine1<pLine2;pLine2-=(iLineLen*2))
			{
				for(iIndex=0;iIndex!=iLineLen;pLine1++,pLine2++,iIndex++)
				{
					tmp=*pLine1;
					*pLine1=*pLine2;
					*pLine2=tmp;       
				}
			}
		}
		buf->b_wptr+=size;  

		ms_mutex_lock(&s->mutex);
		putq(&s->rq, buf);
		ms_mutex_unlock(&s->mutex);

	}
	return S_OK;
}

HRESULT GetFirstCameraDriver( WCHAR *pwzName ) {
	HRESULT hr = S_OK;
	HANDLE handle = NULL;
	DEVMGR_DEVICE_INFORMATION di;
	GUID guidCamera = { 0xCB998A05, 0x122C, 0x4166, 0x84, 0x6A,
		0x93, 0x3E, 0x4D, 0x7E, 0x3C, 0x86 };

	if( pwzName == NULL ) {
		return E_POINTER;
	}

	di.dwSize = sizeof(di);

	handle = FindFirstDevice( DeviceSearchByGuid, &guidCamera, &di );
	if(( handle == NULL ) || ( di.hDevice == NULL )) {
		return S_FALSE;
	}

	StringCchCopy( pwzName, MAX_PATH, di.szLegacyName );

	FindClose( handle );
	return hr;
}

struct VAR_LIST
{
	VARIANT var;
	VAR_LIST *pNext;
	BSTR pBSTRName;
};

class CPropertyBag : public IPropertyBag
{  
public:
	CPropertyBag();
	~CPropertyBag();

	HRESULT STDMETHODCALLTYPE
		Read(
		LPCOLESTR pszPropName, 
		VARIANT *pVar, 
		IErrorLog *pErrorLog
		);


	HRESULT STDMETHODCALLTYPE
		Write(
		LPCOLESTR pszPropName, 
		VARIANT *pVar
		);

	ULONG STDMETHODCALLTYPE AddRef();        
	ULONG STDMETHODCALLTYPE Release();        
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv);   

private:
	ULONG _refCount;
	VAR_LIST *pVar;
};

CPropertyBag::CPropertyBag() : _refCount(1), pVar(0)
{       
}

CPropertyBag::~CPropertyBag()
{
	VAR_LIST *pTemp = pVar;
	HRESULT hr = S_OK;

	while(pTemp)
	{
		VAR_LIST *pDel = pTemp;
		VariantClear(&pTemp->var);
		SysFreeString(pTemp->pBSTRName);
		pTemp = pTemp->pNext;
		delete pDel;
	}

}

HRESULT STDMETHODCALLTYPE
CPropertyBag::Read(LPCOLESTR pszPropName, 
				   VARIANT *_pVar, 
				   IErrorLog *pErrorLog)
{
	VAR_LIST *pTemp = pVar;
	HRESULT hr = S_OK;

	while(pTemp)
	{
		if(0 == wcscmp(pszPropName, pTemp->pBSTRName))
		{
			hr = VariantCopy(_pVar, &pTemp->var);
			break;
		}
		pTemp = pTemp->pNext;
	}
	return hr;
}


HRESULT STDMETHODCALLTYPE
CPropertyBag::Write(LPCOLESTR pszPropName, 
					VARIANT *_pVar)
{
	HRESULT hr = S_OK;
	VAR_LIST *pTemp = new VAR_LIST();
	ASSERT(pTemp);

	if( !pTemp )
	{
		return E_OUTOFMEMORY;
	}

	VariantInit(&pTemp->var);
	pTemp->pBSTRName = SysAllocString(pszPropName);
	pTemp->pNext = pVar;
	pVar = pTemp;
	return VariantCopy(&pTemp->var, _pVar);
}

ULONG STDMETHODCALLTYPE 
CPropertyBag::AddRef() 
{
	return InterlockedIncrement((LONG *)&_refCount);
}

ULONG STDMETHODCALLTYPE 
CPropertyBag::Release() 
{
	ASSERT(_refCount != 0xFFFFFFFF);
	ULONG ret = InterlockedDecrement((LONG *)&_refCount);    
	if(!ret) 
		delete this; 
	return ret;
}

HRESULT STDMETHODCALLTYPE 
CPropertyBag::QueryInterface(REFIID riid, void** ppv) 
{
	if(!ppv) 
		return E_POINTER;
	if(riid == IID_IPropertyBag) 
		*ppv = static_cast<IPropertyBag*>(this);	
	else 
		return *ppv = 0, E_NOINTERFACE;

	return AddRef(), S_OK;	
}

HRESULT GetPinCategory(IPin *pPin, GUID *pPinCategory)
{
	HRESULT hr;
	IKsPropertySet *pKs;
	hr = pPin->QueryInterface(IID_IKsPropertySet, (void **)&pKs);
	if (FAILED(hr))
	{
		// The pin does not support IKsPropertySet.
		return hr;
	}
	// Try to retrieve the pin category.
	DWORD cbReturned;
	hr = pKs->Get(AMPROPSETID_Pin, AMPROPERTY_PIN_CATEGORY, NULL, 0, 
		pPinCategory, sizeof(GUID), &cbReturned);

	// If this succeeded, pPinCategory now contains the category GUID.

	pKs->Release();
	return hr;
}

#define SCALE_FACTOR 4

void set_corner(MSFilter * obj,int icorner)
{
	V4wState *s=(V4wState*)obj->data;
	s->corner = icorner;
	if(s->videoHWND!=NULL)
	{
		RECT rc;
		GetWindowRect(s->videoHWND, &rc );
		s->local_pic.w=((rc.right-rc.left)/SCALE_FACTOR);
		s->local_pic.h=((rc.bottom-rc.top)/SCALE_FACTOR);

		if(s->priview)
		{
			s->local_rect.x=0;
			s->local_rect.y=0;
			s->local_rect.w=rc.right-rc.left;
			s->local_rect.h=rc.bottom-rc.top;
			return ;
		}

		switch(s->corner)
		{
		case 1:
			s->local_rect.x=0;
			s->local_rect.y=0;
			s->local_rect.w=s->local_pic.w;
			s->local_rect.h=s->local_pic.h;
			break;
		case 2:
			s->local_rect.x=rc.right-s->local_pic.w;
			s->local_rect.y=0;
			s->local_rect.w=s->local_pic.w;
			s->local_rect.h=s->local_pic.h;
			break;

		case 3:
			s->local_rect.x=0;
			s->local_rect.y=rc.bottom-s->local_pic.h;
			s->local_rect.w=s->local_pic.w;
			s->local_rect.h=s->local_pic.h;
			break;
		default:
			s->local_rect.x=rc.right-s->local_pic.w;
			s->local_rect.y=rc.bottom-s->local_pic.h;
			s->local_rect.w=s->local_pic.w;
			s->local_rect.h=s->local_pic.h;


		}

	}
	return ;


}

int try_format(V4wState *s, int format, GUID *pPinCategory)
{
	HRESULT hr=S_OK;
	IEnumPins *pEnum=0;
	ULONG ulFound;
	IPin *pPin;

	GUID guid_format;
	DWORD biCompression;
	DWORD biBitCount;

	// Verify input
	if (!s->m_pDeviceFilter)
		return -1;

	if (format == MS_YUV420P)
		guid_format = (GUID)FOURCCMap(MAKEFOURCC('I','4','2','0'));
	else if (format == MS_UYVY)
		guid_format = MEDIASUBTYPE_UYVY;
	else if (format == MS_RGB24)
		guid_format = MEDIASUBTYPE_RGB24;
	else if (format == MS_YUY2)
		guid_format = MEDIASUBTYPE_YV12;
	else if (format == MS_RGB565)
		guid_format = MEDIASUBTYPE_RGB565;




	if (format == MS_YUV420P)
		biCompression = MAKEFOURCC('I','4','2','0');
	else if (format == MS_YUYV)
		biCompression = MAKEFOURCC('Y','U','Y','V');
	else if (format == MS_UYVY)
		biCompression = MAKEFOURCC('U','Y','V','Y');
	else if (format == MS_RGB24)
		biCompression = BI_RGB;
	else if (format == MS_YUY2)
		biCompression = MAKEFOURCC('Y','U','Y','2');
	else if (format == MS_RGB565)
		biCompression = MAKEFOURCC('R','G','B','5');

	if (format == MS_YUV420P)
		biBitCount = 12;
	else if (format == MS_YUYV)
		biBitCount = 16;
	else if (format == MS_UYVY)
		biBitCount = 16;
	else if (format == MS_RGB24)
		biBitCount = 24;
	else if (format == MS_YUY2)
		biBitCount = 16;
	else if (format == MS_RGB565)
		biBitCount = 16;


	// Get pin enumerator
	hr = s->m_pDeviceFilter->EnumPins(&pEnum);
	if(FAILED(hr)) 
		return -1;

	pEnum->Reset();

	// Count every pin on the filter
	while(S_OK == pEnum->Next(1, &pPin, &ulFound))
	{
		PIN_DIRECTION pindir = (PIN_DIRECTION) 3;

		hr = pPin->QueryDirection(&pindir);

		if(pindir != PINDIR_INPUT)
		{
			IEnumMediaTypes *ppEnum;
			ULONG ulFound2;

			GetPinCategory(pPin, pPinCategory);
			if (*pPinCategory!=PIN_CATEGORY_CAPTURE
				&& *pPinCategory!=PIN_CATEGORY_PREVIEW)
				continue;

			hr = pPin->EnumMediaTypes(&ppEnum);
			if(FAILED(hr)) 
				continue;


			AM_MEDIA_TYPE *ppMediaTypes;
			while(S_OK == ppEnum->Next(1, &ppMediaTypes, &ulFound2))
			{
				if (ppMediaTypes->formattype != FORMAT_VideoInfo)
					continue;
				if (ppMediaTypes->majortype != MEDIATYPE_Video)
					continue;
				if (ppMediaTypes->subtype != guid_format)
				{
					ms_error("current subtype %X,%X",ppMediaTypes->subtype,guid_format);
					continue;
				}
				VIDEOINFO *pvi = (VIDEOINFO *)ppMediaTypes->pbFormat;
				/*if (pvi->bmiHeader.biCompression!=biCompression)
				{
				ms_error("current subtype %d,%d",pvi->bmiHeader.biCompression,biCompression);
				continue;
				}*/
				if (pvi->bmiHeader.biBitCount!=biBitCount){
					ms_error("current subtype %d,%d",pvi->bmiHeader.biBitCount,biBitCount);
					continue;
				}
				//ms_error("current subtype %d,%d,%d",pvi->bmiHeader.biBitCount,pvi->bmiHeader.biHeight,pvi->bmiHeader.biWidth);
				//ms_error("current subtype %x,%x,%x",ppMediaTypes->formattype,ppMediaTypes->majortype,ppMediaTypes->subtype);
				//ms_error("current subtype %x,%x,%x,%x,%d,%d,%d",ppMediaTypes->formattype,ppMediaTypes->majortype,ppMediaTypes->subtype,
				//	pvi->bmiHeader.biCompression,pvi->bmiHeader.biBitCount,pvi->bmiHeader.biHeight,pvi->bmiHeader.biWidth);
				pPin->Release();
				pEnum->Release();
				return 0;
			}
		}

		pPin->Release();
	}

	pEnum->Release();
	return -1;
}
static int v4w_openandtry(V4wState *s)
{
	HRESULT hr=s->m_pGraph.CoCreateInstance(CLSID_FilterGraph);
	if(FAILED(hr))
	{
		return -1;
	}

	hr=s->m_pBuilder.CoCreateInstance(CLSID_CaptureGraphBuilder);
	if(FAILED(hr))
	{
		return -2;
	}
	// connect capture graph builder with the graph
	s->m_pBuilder->SetFiltergraph(s->m_pGraph);

	// get mediacontrol so we can start and stop the filter graph
	hr=s->m_pGraph.QueryInterface(&(s->m_pControl));
	if(FAILED(hr))
	{
		return -3;
	}


	WCHAR wzDeviceName[ MAX_PATH + 1 ];
	CComVariant   varCamName;
	CPropertyBag PropBag;
	CComPtr<IPersistPropertyBag>    pPropertyBag;
	GetFirstCameraDriver(wzDeviceName);

	hr = s->m_pDeviceFilter.CoCreateInstance( CLSID_VideoCapture ); 
	if (FAILED(hr))
	{
		return -8;
	}

	s->m_pDeviceFilter.QueryInterface( &pPropertyBag );
	varCamName = wzDeviceName;
	if(( varCamName.vt == VT_BSTR ) == NULL ) {
		return E_OUTOFMEMORY;
	}
	PropBag.Write( L"VCapName", &varCamName );   
	pPropertyBag->Load( &PropBag, NULL );
	pPropertyBag.Release();

	hr = s->m_pGraph->AddFilter( s->m_pDeviceFilter, L"Video capture source" );
	if (FAILED(hr))
	{
		return -8;
	}
	return 0;

}


static int v4w_open_videodevice(V4wState *s)
{
	// Initialize COM
	CoInitialize(NULL);

	if(v4w_openandtry(s)<0) return -8;

	HRESULT hr=S_OK;

	if(s->priview)
	{
		hr = s->m_pBuilder->RenderStream(&PIN_CATEGORY_PREVIEW,
			&MEDIATYPE_Video, s->m_pDeviceFilter,NULL,NULL);
		if (FAILED(hr))
		{
			return -14;
		}

	}

	else
	{
		s->m_pDXFilter = new CDXFilter(NULL, &hr, FALSE);
		if(s->m_pDXFilter==NULL)
		{
			return -4;
		}
		s->m_pDXFilter->AddRef();
		if(FAILED(hr))
		{
			return -4;
		}



		//hr = s->m_pDXFilter->SetAcceptedMediaType(&mt);
		if(FAILED(hr))
		{
			return -5;
		}

		hr = s->m_pDXFilter->SetCallback(Callback); 
		if(FAILED(hr))
		{
			return -6;
		}

		hr = s->m_pDXFilter->QueryInterface(IID_IBaseFilter,
			(LPVOID *)&s->m_pIDXFilter);
		if(FAILED(hr))
		{
			return -7;
		}

		hr = s->m_pGraph->AddFilter(s->m_pIDXFilter, L"DXFilter Filter");
		if(FAILED(hr))
		{
			return -8;
		}

		s->m_pNullRenderer.CoCreateInstance(CLSID_VideoRenderer);
		s->m_pGraph->AddFilter(s->m_pNullRenderer,L"render");

		hr = s->m_pBuilder->RenderStream(&PIN_CATEGORY_PREVIEW,
			&MEDIATYPE_Video, s->m_pDeviceFilter,s->m_pIDXFilter,s->m_pNullRenderer);
		if (FAILED(hr))
		{
			return -14;
		}

	}

	CComQIPtr< IVideoWindow, &IID_IVideoWindow > pWindow = s->m_pGraph;
	if( !pWindow )
	{
		return -15;
	}
	if(s->videoHWND!=NULL)
	{

		hr = pWindow->put_Owner( (OAHWND) s->videoHWND );
		hr = pWindow->put_Left(s->local_rect.x);
		hr = pWindow->put_Top(s->local_rect.y);
		hr = pWindow->put_Width(s->local_rect.w);
		hr = pWindow->put_Height(s->local_rect.h);
		pWindow->put_WindowStyleEx(WS_EX_TOPMOST);
		hr = pWindow->put_WindowStyle( WS_CHILD | WS_CLIPSIBLINGS| WS_BORDER );
		hr = pWindow->put_Visible( OATRUE );
	}

	s_callback = s;
	hr = s->m_pControl->Run();
	if(FAILED(hr))
	{
		return -15;
	}

	s->rotregvalue=1;
	return 0;
}
static int _v4w_start(V4wState *s, void *arg)
{

	int i;
	s->frame_count=-1;
	i = v4w_open_videodevice(s);

	if (s->rotregvalue==0){
		//RemoveGraphFromRot(s->rotregvalue);		
		if (s->m_pNullRenderer!=NULL)
			s->m_pGraph->RemoveFilter(s->m_pNullRenderer);
		if (s->m_pIDXFilter!=NULL)
			s->m_pGraph->RemoveFilter(s->m_pIDXFilter);
		if (s->m_pDeviceFilter!=NULL)
			s->m_pGraph->RemoveFilter(s->m_pDeviceFilter);
		s->m_pBuilder=NULL;
		s->m_pControl=NULL;
		s->m_pIDXFilter=NULL;
		if (s->m_pDXFilter!=NULL)
			s->m_pDXFilter->Release();
		s->m_pDXFilter=NULL;
		s->m_pGraph=NULL;
		s->m_pNullRenderer=NULL;
		s->m_pDeviceFilter=NULL;
		CoUninitialize();
		s_callback = NULL;
		flushq(&s->rq,0);
		ms_message("v4w: graph not started (err=%i)", i);
		s->rotregvalue=0;
	}
	return i;
}



static void v4w_init(MSFilter *f){
	V4wState *s=(V4wState *)ms_new0(V4wState,1);
	int idx;
	s->vsize.width=MS_VIDEO_SIZE_QVGA_H;
	s->vsize.height=MS_VIDEO_SIZE_QVGA_W;
	s->pix_fmt=MS_RGB565;
	s->rotregvalue = 0;
	s->m_pGraph=NULL;
	s->m_pBuilder=NULL;
	s->m_pControl=NULL;
	s->m_pDXFilter=NULL;
	s->m_pIDXFilter=NULL;
	s->m_pDeviceFilter=NULL;


	qinit(&s->rq);
	for (idx=0;idx<10;idx++)
	{
		s->mire[idx]=NULL;
	}
	ms_mutex_init(&s->mutex,NULL);
	s->start_time=0;
	s->frame_count=-1;
	s->fps=15;
	memset(s->dev, 0, sizeof(s->dev));

	f->data=s;




}



static int _v4w_stop(V4wState *s, void *arg){
	s->frame_count=-1;
	if (s->rotregvalue>0){
		HRESULT hr = s->m_pControl->Stop();
		if(FAILED(hr))
		{
			ms_message("v4w: could not stop graph");
		}
		CComQIPtr< IVideoWindow, &IID_IVideoWindow > pWindow = s->m_pGraph;
		if(pWindow)
		{
			pWindow->put_Visible(OAFALSE);
			pWindow->put_Owner(NULL);
		}
		if (s->m_pNullRenderer!=NULL)
			s->m_pGraph->RemoveFilter(s->m_pNullRenderer);
		if (s->m_pIDXFilter!=NULL)
			s->m_pGraph->RemoveFilter(s->m_pIDXFilter);
		if (s->m_pDeviceFilter!=NULL)
		{
			s->m_pDeviceFilter.Release();
		}
		if (s->m_pControl!=NULL)
			s->m_pControl.Release();
		if (s->m_pGraph!=NULL)
			s->m_pGraph.Release();
		if (s->m_pBuilder!=NULL)
			s->m_pBuilder.Release();
		//RemoveGraphFromRot(s->rotregvalue);
		s->m_pBuilder=NULL;
		s->m_pControl=NULL;
		s->m_pIDXFilter=NULL;
		if (s->m_pDXFilter!=NULL)
			s->m_pDXFilter->Release();
		s->m_pDXFilter=NULL;
		s->m_pGraph=NULL;
		s->m_pNullRenderer=NULL;
		s->m_pDeviceFilter=NULL;
		CoUninitialize();
		s_callback = NULL;
		flushq(&s->rq,0);
		ms_message("v4w: graph destroyed");
		s->rotregvalue=0;
	}
	return 0;
}


static int v4w_start(MSFilter *f, void *arg){
	V4wState *s=(V4wState*)f->data;
	_v4w_start(s, NULL);
	return 0;
}

static int v4w_stop(MSFilter *f, void *arg){
	V4wState *s=(V4wState*)f->data;
	_v4w_stop(s, NULL);
	return 0;
}


static void v4w_uninit(MSFilter *f){
	V4wState *s=(V4wState*)f->data;
	int idx;
	flushq(&s->rq,0);
	ms_mutex_destroy(&s->mutex);
	for (idx=0;idx<10;idx++)
	{
		if (s->mire[idx]==NULL)
			break;
		freemsg(s->mire[idx]);
	}
	if (s->rotregvalue>0){
		HRESULT hr = s->m_pControl->Stop();
		if(FAILED(hr))
		{
			ms_message("v4w: could not stop graph");
		}
		CComQIPtr< IVideoWindow, &IID_IVideoWindow > pWindow = s->m_pGraph;
		if(pWindow)
		{
			pWindow->put_Visible(OAFALSE);
			pWindow->put_Owner(NULL);
		}
		if (s->m_pNullRenderer!=NULL)
			s->m_pGraph->RemoveFilter(s->m_pNullRenderer);
		if (s->m_pIDXFilter!=NULL)
			s->m_pGraph->RemoveFilter(s->m_pIDXFilter);
		if (s->m_pDeviceFilter!=NULL){
			s->m_pGraph->RemoveFilter(s->m_pDeviceFilter);
			s->m_pDeviceFilter.Release();
		}
		//RemoveGraphFromRot(s->rotregvalue);
		if (s->m_pControl!=NULL)
			s->m_pControl.Release();
		if (s->m_pGraph!=NULL)
			s->m_pGraph.Release();
		if (s->m_pBuilder!=NULL)
			s->m_pBuilder.Release();

		s->m_pBuilder=NULL;
		s->m_pControl=NULL;
		s->m_pIDXFilter=NULL;
		if (s->m_pDXFilter!=NULL)
			s->m_pDXFilter->Release();
		s->m_pDXFilter=NULL;
		s->m_pGraph=NULL;
		s->m_pNullRenderer=NULL;
		s->m_pDeviceFilter=NULL;
		CoUninitialize();
		s_callback = NULL;
		flushq(&s->rq,0);
		ms_message("v4w: graph destroyed");
		s->rotregvalue=0;
	}
	ms_free(s);
}

static mblk_t * v4w_make_nowebcam(V4wState *s){
#if defined(_WIN32_WCE)
	return NULL;
#else
	int idx;
	int count;
	if (s->mire[0]==NULL && s->frame_ind==0){
		/* load several images to fake a movie */
		for (idx=0;idx<10;idx++)
		{
			s->mire[idx]=ms_load_nowebcam(&s->vsize, idx);
			if (s->mire[idx]==NULL)
				break;
		}
		if (idx==0)
			s->mire[0]=ms_load_nowebcam(&s->vsize, -1);
	}
	for (count=0;count<10;count++)
	{
		if (s->mire[count]==NULL)
			break;
	}

	s->frame_ind++;
	if (count==0)
		return NULL;

	idx = s->frame_ind%count;
	if (s->mire[idx]!=NULL)
		return s->mire[idx];
	return s->mire[0];
#endif
}

static void v4w_preprocess(MSFilter * obj){
	V4wState *s=(V4wState*)obj->data;
	s->running=TRUE;
	set_corner(obj,2);

	if (s->rotregvalue==0)
		_v4w_start(s, NULL);
	if (s->rotregvalue==0)
		s->fps=1;

}


static void v4w_postprocess(MSFilter * obj){
	V4wState *s=(V4wState*)obj->data;
	s->running=FALSE;
}

static void v4w_process(MSFilter * obj){
	V4wState *s=(V4wState*)obj->data;
	mblk_t *m;
	uint32_t timestamp;
	int cur_frame;

	if (s->frame_count==-1){
		s->start_time=obj->ticker->time;
		s->frame_count=0;
	}


	cur_frame=((obj->ticker->time-s->start_time)*s->fps/1000.0);
	if (cur_frame>s->frame_count){
		mblk_t *om=NULL;
		ms_mutex_lock(&s->mutex);
		/*keep the most recent frame if several frames have been captured */
		if (s->rotregvalue!=0){
			while((m=getq(&s->rq))!=NULL){
				if (om!=NULL) freemsg(om);
				om=m;
			}
		}else {
			mblk_t *nowebcam = v4w_make_nowebcam(s);
			if (nowebcam!=NULL)
				om=dupmsg(nowebcam);
		}
		ms_mutex_unlock(&s->mutex);
		if (om!=NULL){
			timestamp=obj->ticker->time*90;/* rtp uses a 90000 Hz clockrate for video*/
			mblk_set_timestamp_info(om,timestamp);
			ms_queue_put(obj->outputs[0],om);
			/*ms_message("picture sent");*/
		}
		s->frame_count++;
	}
}



static int v4w_set_fps(MSFilter *f, void *arg){
	V4wState *s=(V4wState*)f->data;
	s->fps=*((float*)arg);
	return 0;
}

static int v4w_get_pix_fmt(MSFilter *f,void *arg){
	V4wState *s=(V4wState*)f->data;
	*((MSPixFmt*)arg) = (MSPixFmt)s->pix_fmt;
	return 0;
}

static int v4w_set_vsize(MSFilter *f, void *arg){
	V4wState *s=(V4wState*)f->data;
	s->vsize=*((MSVideoSize*)arg);
	return 0;
}

static int v4w_set_video_hwnd(MSFilter *f, void *arg){
	V4wState *s=(V4wState*)f->data;
	s->videoHWND=*((HWND*)arg);
	return 0;
}

static int v4w_set_video_preview(MSFilter *f, void *arg){
	V4wState *s=(V4wState*)f->data;
	s->priview=*((bool*)arg);
	return 0;
}


static int v4w_get_vsize(MSFilter *f, void *arg){
	V4wState *s=(V4wState*)f->data;
	MSVideoSize *vs=(MSVideoSize*)arg;
	vs->width=s->vsize.width;
	vs->height=s->vsize.height;
	return 0;
}

static int v4w_set_name(MSFilter *f, void *arg){
	V4wState *s=(V4wState*)f->data;
	snprintf(s->dev, sizeof(s->dev), (char*)arg);
	return 0;
}
static MSFilterMethod methods[]={
	{	MS_FILTER_SET_FPS	,	v4w_set_fps	},
	{	MS_FILTER_GET_PIX_FMT	,	v4w_get_pix_fmt	},
	{	MS_FILTER_SET_VIDEO_SIZE, v4w_set_vsize	},
	{	MS_FILTER_GET_VIDEO_SIZE, v4w_get_vsize	},
	{	MS_V4L_START			,	v4w_start		},
	{	MS_V4L_STOP			,	v4w_stop		},
	{	MS_FILTER_SET_VIDEO_HWND			,	v4w_set_video_hwnd	},
	{	MS_FILTER_SET_VIDEO_PREVIEW			,	v4w_set_video_preview},
	{	0								,	NULL			}
};

#ifdef _MSC_VER

MSFilterDesc ms_v4w_desc={
	MS_V4L_ID,
	"MSV4w",
	N_("A video4windows compatible source filter to stream pictures."),
	MS_FILTER_OTHER,
	NULL,
	0,
	1,
	v4w_init,
	v4w_preprocess,
	v4w_process,
	v4w_postprocess,
	v4w_uninit,
	methods
};

#else

MSFilterDesc ms_v4w_desc={
	.id=MS_V4L_ID,
	.name="MSV4w",
	.text=N_("A video4windows compatible source filter to stream pictures."),
	.ninputs=0,
	.noutputs=1,
	.category=MS_FILTER_OTHER,
	.init=v4w_init,
	.preprocess=v4w_preprocess,
	.process=v4w_process,
	.postprocess=v4w_postprocess,
	.uninit=v4w_uninit,
	.methods=methods
};

#endif

MS_FILTER_DESC_EXPORT(ms_v4w_desc)

static MSFilter *vfw_create_reader(MSWebCam *obj){
	MSFilter *f=ms_filter_new_from_desc(&ms_v4w_desc);
	v4w_set_name(f,obj->name);
	return f;
}

static void vfw_detect(MSWebCamManager *obj);

static void vfw_cam_init(MSWebCam *cam){
}

MSWebCamDesc ms_directx_cam_desc={
	"DirectX Video Grabber",
	&vfw_detect,
	&vfw_cam_init,
	&vfw_create_reader,
	NULL
};
static void vfw_detect(MSWebCamManager *obj){

	WCHAR wzDeviceName[ MAX_PATH + 1 ];
	GetFirstCameraDriver(wzDeviceName);
	char szName[256];
	WideCharToMultiByte(CP_UTF8,0,wzDeviceName,-1,szName,256,0,0);

	MSWebCam *cam=ms_web_cam_new(&ms_directx_cam_desc);
	cam->name=ms_strdup(szName);
	ms_web_cam_manager_add_cam(obj,cam);

}


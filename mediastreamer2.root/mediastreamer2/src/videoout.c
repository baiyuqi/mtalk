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

#ifdef HAVE_CONFIG_H
#include "mediastreamer-config.h"
#endif

#include "mediastreamer2/msfilter.h"
#include "mediastreamer2/msvideo.h"

/*required for dllexport of win_display_desc */
#define INVIDEOUT_C 1
#include "mediastreamer2/msvideoout.h"
#include "dxfilter.h"

#if 0
#include <qedit.h>
#endif
#include "JRtpRecVideo.h"
 VideoOut::VideoOut()
{
	
	m_pGraph = NULL;
	m_pControl= NULL;
	m_pNullRenderer= NULL;
	m_pIRender= NULL;
	m_pRender= NULL;

	MSVideoSize* def_size=(MSVideoSize*)ms_new(MSVideoSize,1);;
	ratio.num=11;
	ratio.den=9;
	def_size->width=MS_VIDEO_SIZE_QVGA_H;
	def_size->height=MS_VIDEO_SIZE_QVGA_W;
	local_msg=NULL;
	sws1=NULL;
	sws2=NULL;
	own_display=FALSE;
	ready=FALSE;
	autofit=FALSE;
	mirror=FALSE;
	own_display=TRUE;
	set_vsize(def_size);
	
	

}

 VideoOut::~VideoOut()
{

}



void VideoOut::set_vsize(MSVideoSize *sz){
	fbuf.w=sz->width;
	fbuf.h=sz->height;
}

static void video_out_init(MSFilter  *f){
	VideoOut *obj= new VideoOut();
	f->data=obj;
}


static void video_out_uninit(MSFilter *f){
	VideoOut *obj=(VideoOut*)f->data;
	if (obj->sws1!=NULL){
		sws_freeContext(obj->sws1);
		obj->sws1=NULL;
	}
	if (obj->sws2!=NULL){
		sws_freeContext(obj->sws2);
		obj->sws2=NULL;
	}
	if (obj->local_msg!=NULL) {
		freemsg(obj->local_msg);
		obj->local_msg=NULL;
	}
	delete obj;
}
IPin * GetPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir)   
{   
	BOOL       bFound = FALSE;   
	IEnumPins  *pEnum;   
	IPin       *pPin;   
	// Begin by enumerating all the pins on a filter    
	HRESULT hr = pFilter->EnumPins(&pEnum);   
	if (FAILED(hr))   
	{   
		return NULL;   
	}   

	// Now look for a pin that matches the direction characteristic.    
	// When we've found it, we'll return with it.    
	while(pEnum->Next(1, &pPin, 0) == S_OK)   
	{   
		PIN_DIRECTION PinDirThis;   
		pPin->QueryDirection(&PinDirThis);   
		if (bFound = (PinDir == PinDirThis))   
			break;   
		pPin->Release();   
	}   
	pEnum->Release();   
	return (bFound ? pPin : NULL);     
} 
static void video_out_prepare(MSFilter *f){
	VideoOut *obj=(VideoOut*)f->data;
	obj->ready=TRUE;

	HRESULT hr=obj->m_pGraph.CoCreateInstance(CLSID_FilterGraph);
	if(FAILED(hr))
	{
		return ;
	}

	hr=obj->m_pGraph.QueryInterface(&(obj->m_pControl));
	if(FAILED(hr))
	{
		return ;
	}
	obj->m_pRender = new CJRtpRecVideo(NULL,&hr);
	obj->m_pRender->AddRef();
	obj->m_pRender->setMSFilter(f);

	hr = obj->m_pRender->QueryInterface(IID_IBaseFilter,
		(LPVOID *)&obj->m_pIRender);
	if(FAILED(hr))
	{
		return ;
	}

	hr = obj->m_pGraph->AddFilter(obj->m_pIRender,L"mydesc");

	
	obj->m_pNullRenderer.CoCreateInstance(CLSID_VideoRenderer);
	obj->m_pGraph->AddFilter(obj->m_pNullRenderer,L"render");

	IPin *pPinOut = GetPin( obj->m_pIRender, PINDIR_OUTPUT );   
	IPin * pPinIn =  GetPin( obj->m_pNullRenderer, PINDIR_INPUT );

	hr= obj->m_pGraph->Connect(pPinOut,pPinIn);
	obj->m_pControl->Run();

	CComQIPtr< IVideoWindow, &IID_IVideoWindow > pWindow = obj->m_pNullRenderer;
	if( pWindow !=NULL )
	{
	HWND hwndPreview = (HWND)obj->window_id;
	if(hwndPreview!=NULL)
	{
		HWND hwndPreview = (HWND)obj->window_id;
		RECT rc;
		::GetWindowRect( hwndPreview, &rc );
		int twidth=rc.right-rc.left;
		int theight=rc.bottom-rc.top;

		hr = pWindow->put_Owner( (OAHWND) hwndPreview );
		hr = pWindow->put_Left(0);
		hr = pWindow->put_Top(0);
		hr = pWindow->put_Width(twidth);
		hr = pWindow->put_Height(theight);
		//pWindow->put_WindowStyleEx(WS_EX_TOPMOST);
		hr = pWindow->put_WindowStyle( WS_CHILD | WS_CLIPSIBLINGS |WS_BORDER);
		hr = pWindow->put_Visible( OATRUE );
	}

	}

	
	}

static int video_out_handle_resizing(MSFilter *f, void *data){
	VideoOut *s=(VideoOut*)f->data;
	
	return 0;
}

static void video_out_preprocess(MSFilter *f){
	video_out_prepare(f);
}


static void video_out_process(MSFilter *f){
	VideoOut *obj=(VideoOut*)f->data;
	mblk_t *inm;

	ms_filter_lock(f);
	if (!obj->ready) video_out_prepare(f);

	if (f->inputs[0]!=NULL && (inm=ms_queue_peek_last(f->inputs[0]))!=0) {

		obj->m_pRender->dateReceive(inm);
		
		ms_queue_flush(f->inputs[0]);
	}

	if (obj->local_msg!=NULL){
		/*MSPicture corner=obj->fbuf;
		MSVideoSize roi;
		roi.width=obj->local_pic.w;
		roi.height=obj->local_pic.h;
		corner.w=obj->local_pic.w;
		corner.h=obj->local_pic.h;
		corner.planes[0]+=obj->local_rect.x+(obj->local_rect.y*corner.strides[0]);
		corner.planes[1]+=(obj->local_rect.x/2)+((obj->local_rect.y/2)*corner.strides[1]);
		corner.planes[2]+=(obj->local_rect.x/2)+((obj->local_rect.y/2)*corner.strides[2]);
		corner.planes[3]=0;*/
	}
	ms_filter_unlock(f);
}

static int video_out_set_vsize(MSFilter *f,void *arg){
	VideoOut *s=(VideoOut*)f->data;
	ms_filter_lock(f);
	s->set_vsize((MSVideoSize*)arg);
	ms_filter_unlock(f);
	return 0;
}



static int video_out_auto_fit(MSFilter *f, void *arg){
	VideoOut *s=(VideoOut*)f->data;
	s->autofit=*(int*)arg;
	return 0;
}



static int video_out_enable_mirroring(MSFilter *f,void *arg){
	VideoOut *s=(VideoOut*)f->data;
	s->mirror=*(int*)arg;
	return 0;
}
static int video_out_set_native_window_id(MSFilter *f, void*arg){
	VideoOut *s=(VideoOut*)f->data;
	unsigned long *id=(unsigned long*)arg;
	s->window_id = *id;
	return 0;
;
}

static int video_out_get_native_window_id(MSFilter *f, void*arg){
	VideoOut *s=(VideoOut*)f->data;
	unsigned long *id=(unsigned long*)arg;
	*id=0;
	*id=s->window_id;
	return 0;
}

static MSFilterMethod methods[]={
	{	MS_FILTER_SET_VIDEO_SIZE	,	video_out_set_vsize },
	{	MS_VIDEO_OUT_AUTO_FIT		,	video_out_auto_fit},
	{	MS_VIDEO_OUT_HANDLE_RESIZING	,	video_out_handle_resizing},
	{	MS_VIDEO_OUT_ENABLE_MIRRORING	,	video_out_enable_mirroring},
	{	MS_VIDEO_OUT_GET_NATIVE_WINDOW_ID,	video_out_get_native_window_id},
	{	MS_VIDEO_OUT_SET_NATIVE_WINDOW_ID,	video_out_set_native_window_id},
	{	0	,NULL}
};

#ifdef _MSC_VER

MSFilterDesc ms_video_out_desc={
	MS_VIDEO_OUT_ID,
	"MSVideoOut",
	N_("A generic video display"),
	MS_FILTER_OTHER,
	NULL,
	2,
	0,
	video_out_init,
	video_out_preprocess,
	video_out_process,
	NULL,
	video_out_uninit,
	methods
};

#else

MSFilterDesc ms_video_out_desc={
	.id=MS_VIDEO_OUT_ID,
	.name="MSVideoOut",
	.text=N_("A generic video display"),
	.category=MS_FILTER_OTHER,
	.ninputs=2,
	.noutputs=0,
	.init=video_out_init,
	.preprocess=video_out_preprocess,
	.process=video_out_process,
	.uninit=video_out_uninit,
	.methods=methods
};

#endif

MS_FILTER_DESC_EXPORT(ms_video_out_desc)

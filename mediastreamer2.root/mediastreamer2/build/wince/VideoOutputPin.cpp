#include <streams.h>
#include "VideoOutputPin.h"
#include "mediastreamer2/msfilter.h"
#include "JRtpRecVideo.h"
CVideoOutputPin* g_classVideoOutputPin = NULL;

CVideoOutputPin::CVideoOutputPin(HRESULT* phr, CSource* pParent, LPCWSTR pPinName):
CSourceStream(NAME("Video Out"),phr, pParent, pPinName)
{
	g_classVideoOutputPin	= (CVideoOutputPin*)this;
	m_pVideoOutputQueue		= NULL;

	m_bStartPlay		= FALSE;
	m_nVideoFormat      = 1;
	m_pMediaType		= new AM_MEDIA_TYPE;
	m_pMediaType->subtype = MEDIASUBTYPE_YV12;
	g_eVideoResolution =  FORMAT_QCIF;
}

CVideoOutputPin::~CVideoOutputPin(void)
{
	delete m_pMediaType;
}
HRESULT CVideoOutputPin::GetMediaType(int iPosition, CMediaType *pmt)
{
	CAutoLock lock(m_pLock);

	CheckPointer(pmt, E_POINTER);


	if (iPosition < 0)
	{
		return E_INVALIDARG;
	}

	/* Have we run off the end of types? */
	if (iPosition > 0)
	{
		return VFW_S_NO_MORE_ITEMS;
	}


	pmt->majortype	= MEDIATYPE_Video;
	pmt->subtype	= m_pMediaType->subtype;

	VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *) pmt->AllocFormatBuffer(sizeof(VIDEOINFOHEADER));
	if (NULL == pvi)
	{
		return(E_OUTOFMEMORY);
	}
	ZeroMemory(pvi, sizeof(VIDEOINFOHEADER));
	pvi->bmiHeader.biBitCount	= 12;
	pvi->bmiHeader.biSize		= sizeof(BITMAPINFOHEADER);
	
	switch (g_eVideoResolution)
	{
	case FORMAT_CIF4:
		pvi->bmiHeader.biWidth		= 704;
		pvi->bmiHeader.biHeight		= 576;
		break;
	case FORMAT_CIF:
		pvi->bmiHeader.biWidth		= 240;
		pvi->bmiHeader.biHeight		= 320;
		break;
	case FORMAT_QCIF:
		pvi->bmiHeader.biWidth		= 176;
		pvi->bmiHeader.biHeight		= 144;
		break;
	default:
		pvi->bmiHeader.biWidth		= 352;
		pvi->bmiHeader.biHeight		= 288;
		break;
	}
    
	

	pvi->bmiHeader.biPlanes			= 1;
	//pvi->dwBitRate					= 30;
	//pvi->AvgTimePerFrame			= 50;
	pvi->bmiHeader.biSizeImage		= GetBitmapSize(&pvi->bmiHeader);
	pvi->bmiHeader.biClrImportant	= 0;
	pvi->bmiHeader.biCompression	= MAKEFOURCC('Y','V','1','2');

	SetRectEmpty(&(pvi->rcSource));		/* we want the whole image area rendered. */
	SetRectEmpty(&(pvi->rcTarget));		/* no particular destination rectangle */

	pmt->bFixedSizeSamples			= FALSE;
	pmt->bTemporalCompression		= TRUE;
	pmt->SetFormatType(&FORMAT_VideoInfo);


	
   m_pMediaType->lSampleSize = pvi->bmiHeader.biSizeImage;

	return NOERROR;
}

HRESULT CVideoOutputPin::CheckMediaType(const CMediaType *pMediaType)
{
	CAutoLock lock(m_pLock);

	CheckPointer(pMediaType, E_POINTER);

	/* Check for the subtypes we support */
	const GUID *SubType = pMediaType->Subtype();
	if (SubType == NULL)
	{
		return E_INVALIDARG;
	}


	return NOERROR;
}

HRESULT CVideoOutputPin::DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *pProperties)
{
	CAutoLock lock(m_pLock);

	CheckPointer(pAlloc, E_POINTER);
	CheckPointer(pProperties, E_POINTER);
	HRESULT hr = NOERROR;

	VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *)m_mt.Format();
	ASSERT(NULL != pvi);

	pProperties->cBuffers	= 1;
	pProperties->cbBuffer	=  m_pMediaType->lSampleSize;
	pProperties->cbAlign	= 1;
	pProperties->cbPrefix	= 0;

	ASSERT(pProperties->cbBuffer);

	/*
	* Ask the allocator to reserve us some sample memory, NOTE the function
	* can succeed (that is return NOERROR) but still not have allocated the
	* memory that we requested, so we must check we got whatever we wanted
	*/

	ALLOCATOR_PROPERTIES Actual;
	hr = pAlloc->SetProperties(pProperties, &Actual);
	if (FAILED(hr)) 
	{
		return hr;
	}

	/* Is this allocator unsuitable */
	if (Actual.cbBuffer < pProperties->cbBuffer) 
	{
		return E_FAIL;
	}

	ASSERT(Actual.cBuffers == 1);

	return NOERROR;
}


//**************************************************************************
//
//**************************************************************************
//
HRESULT CVideoOutputPin::OnThreadStartPlay()
{ 
	m_bStartPlay = TRUE;

	ASSERT(NULL != m_pFilter);

	//((CJRtpRecVideo*)m_pFilter)->m_pDataArray->ResetAllVideoNode();

	HRESULT hr = CSourceStream::OnThreadStartPlay();
	return hr;
}

HRESULT CVideoOutputPin::OnThreadDestroy()
{
	return CSourceStream::OnThreadDestroy();
}

HRESULT CVideoOutputPin::Inactive()
{ 
	m_bStartPlay = FALSE;
	ASSERT(NULL != m_pFilter);	

	HRESULT hr = CSourceStream::Inactive(); 

	//((CJRtpRecVideo*)m_pFilter)->m_pDataArray->ResetAllVideoNode();

	return hr;
}

//**************************************************************************
// FillBuffer. This routine fills up the given IMediaSample
//**************************************************************************

HRESULT CVideoOutputPin::FillBuffer(IMediaSample *pms)
{
	if(NULL == pms)
	{
		return E_POINTER;
	}	

	if(!m_bStartPlay)
	{
		return NOERROR;
	}

	return NOERROR;
}

int CVideoOutputPin::Receive(mblk_t *inm)
{
	IMediaSample *pSample;

	HRESULT hr = GetDeliveryBuffer(&pSample,NULL,NULL,0);
	hr = FillBuffer(pSample);

	unsigned char* pbuffer;
	pSample->GetPointer(&pbuffer);
	int size=inm->b_wptr-inm->b_rptr;
	memcpy(pbuffer,inm->b_rptr,size);
	pSample->SetActualDataLength(size);

	if (hr == S_OK) 
	{
		hr = Deliver(pSample);
		pSample->Release();

		// downstream filter returns S_FALSE if it wants us to
		// stop or an error if it's reporting an error.
		if(hr != S_OK)
		{
			DbgLog((LOG_TRACE, 2, TEXT("Deliver() returned %08x; stopping"), hr));
			return S_OK;
		}
	} 
	else if (hr == S_FALSE) 
	{
		// derived class wants us to stop pushing data
		pSample->Release();
		DeliverEndOfStream();
		return S_OK;
	} 
	else 
	{
		// derived class encountered an error
		pSample->Release();
		DbgLog((LOG_ERROR, 1, TEXT("Error %08lX from FillBuffer!!!"), hr));
		DeliverEndOfStream();
		m_pFilter->NotifyEvent(EC_ERRORABORT, hr, 0);
		return hr;
	}

	return 0;
}

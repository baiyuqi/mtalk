
#include <streams.h>
#include "JRtpRecVideo.h"
#include "VideoOutputPin.h"
#include "../../src/dxfilter.h"
#include "mediastreamer2/msfilter.h"

CJRtpRecVideo::CJRtpRecVideo(LPUNKNOWN pUnk, HRESULT *phr) : CSource(NAME("JRtpRecVideo"), pUnk, CLSID_JRtpRecVideo)
{
	m_paStreams = (CSourceStream **) new CSourceStream*[1];
    if(NULL == m_paStreams) 
    {
        *phr = E_OUTOFMEMORY;
        return;
    }

	// สำฦต
    m_pVideoOutputPin      = new CVideoOutputPin(phr, this, L"Mp4Video");
	m_paStreams[0] = m_pVideoOutputPin;
    if (m_paStreams[0] == NULL) 
    {
        *phr = E_OUTOFMEMORY;
        return;
    }

	

}

CJRtpRecVideo::~CJRtpRecVideo(void)
{
	if (NULL != m_pVideoOutputPin)
	{
		delete m_pVideoOutputPin;
		m_pVideoOutputPin = NULL;
	}
}
CBasePin* CJRtpRecVideo::GetPin(IN  int Index)
{
	switch(Index)
	{
	case 0:
		return m_paStreams[0];
		break;
	default:
		break;
	}

	return NULL;
}

int CJRtpRecVideo::GetPinCount()
{
	if (NULL != m_pVideoOutputPin)
	{
		return 1;
	}
    return 0;
}

CVideoOutputPin* CJRtpRecVideo::GetVideoOutputPin()
{
	return (CVideoOutputPin*)GetPin(0);
}
CUnknown * WINAPI CJRtpRecVideo::CreateInstance(LPUNKNOWN pUnk, HRESULT * phr)
{
    return new CJRtpRecVideo(pUnk, phr);
}
void CJRtpRecVideo::setMSFilter(MSFilter* filter)
{
	this->m_pMSFilter = filter;
}

int CJRtpRecVideo::dateReceive(mblk_t *inm)
{
	this->m_pVideoOutputPin->Receive(inm);
	return 0;
}

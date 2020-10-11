#pragma once
//#include "source.h"
typedef struct _MSFilter MSFilter;
typedef struct msgb mblk_t;
class CJRtpRecVideo :
	public CSource
{

friend class CVideoOutputPin;
public:
	//CJRtpRecVideo(void);
	CJRtpRecVideo(LPUNKNOWN pUnk, HRESULT *phr);
	~CJRtpRecVideo(void);
private:
	CVideoOutputPin*	m_pVideoOutputPin;	
	CBasePin*			GetPin(IN int Index);
	int					GetPinCount();


public:
	 static CUnknown * WINAPI CreateInstance(LPUNKNOWN lpunk, HRESULT *phr);
	CVideoOutputPin*	GetVideoOutputPin();

	CCritSec*	pStateLock(void) { return &m_cStateLock; }	/* provide our critical section */

protected:	
    int			m_iPins;		/* The number of pins on this filter. Updated by CSourceStream */
    CCritSec	m_cStateLock;	/* Lock this to serialize function accesses to the filter state */
	MSFilter    *m_pMSFilter;
public:
	void setMSFilter(MSFilter* filter);
	int dateReceive(mblk_t *inm);
};

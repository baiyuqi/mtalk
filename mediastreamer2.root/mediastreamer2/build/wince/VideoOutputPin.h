#pragma once
typedef enum _enumVideoResolutionFormat
{
	FORMAT_CIF4	= 1,
	FORMAT_CIF	= 2,
	FORMAT_QCIF	= 3,
	FORMAT_SQCIF	= 4
} VIDEO_FORMAT;

typedef struct msgb mblk_t;
class CVideoOutputPin : public CSourceStream   
{
public:
	CVideoOutputPin(HRESULT* phr, CSource* pParent, LPCWSTR pPinName);
	~CVideoOutputPin(void);
public:
	
    //HRESULT CVideoOutputPin::DoBufferProcessingLoop(void);
	HRESULT FillBuffer(IMediaSample *pms);
    HRESULT DecideBufferSize(IMemAllocator *pIMemAlloc, ALLOCATOR_PROPERTIES *pProperties);
                             
    // negotiate these for the correct output type
    HRESULT CheckMediaType(const CMediaType *pMediaType);
    HRESULT GetMediaType(int iPosition, CMediaType *pmt);
	HRESULT OnThreadDestroy(); 
    HRESULT OnThreadStartPlay( );
	HRESULT Inactive();
    // need to override or we'll get an assert
    STDMETHODIMP Notify( IBaseFilter * pFilter, Quality q ) 
    { 
        return S_OK; 
    }
public:
	AM_MEDIA_TYPE*		m_pMediaType;      //媒体类型
	COutputQueue*	m_pVideoOutputQueue;
private:
	int	                m_nVideoFormat;    //视频格式
	BOOL				m_bStartPlay;      //TRUR:Filter已运行;FALSE:未运行
public:
	int Receive(mblk_t *inm);
	int g_eVideoResolution;

};

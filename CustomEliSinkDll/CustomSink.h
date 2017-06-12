#include "pch.h"
#include "BaseAttributes.h"
#include <assert.h>
#include <mfapi.h>
#include <mfidl.h>
#include <Mferror.h>
#include <wincodec.h>

// {4BD133CC-EB9B-496E-8865-0813BFBC6FAA}
DEFINE_GUID(MF_STREAMSINK_ID, 0x4bd133cc, 0xeb9b, 0x496e, 0x88, 0x65, 0x8, 0x13, 0xbf, 0xbc, 0x6f, 0xaa);

// {C9E22A8C-6A50-4D78-9183-0834A02A3780}
DEFINE_GUID(MF_STREAMSINK_MEDIASINKINTERFACE,
	0xc9e22a8c, 0x6a50, 0x4d78, 0x91, 0x83, 0x8, 0x34, 0xa0, 0x2a, 0x37, 0x80);

// {DABD13AB-26B7-47C2-97C1-4B04C187B838}
DEFINE_GUID(MF_MEDIASINK_PREFERREDTYPE,
	0xdabd13ab, 0x26b7, 0x47c2, 0x97, 0xc1, 0x4b, 0x4, 0xc1, 0x87, 0xb8, 0x38);

// {5B0426CA-6D63-46AD-87ED-ABF23C296AA4}
DEFINE_GUID(MF_MEDIASINK_AUDIOPREFERREDTYPE,
	0x5b0426ca, 0x6d63, 0x46ad, 0x87, 0xed, 0xab, 0xf2, 0x3c, 0x29, 0x6a, 0xa4);

// MF_CUSTOMSINK_CALLBACK: {26957AA7-AFF4-464c-BB8B-07BA65CE11DF}
// Type: IUnknown*
DEFINE_GUID(MF_CUSTOMSINK_CALLBACK, 
	0x26957aa7, 0xaff4, 0x464c, 0xbb, 0x8b, 0x7, 0xba, 0x65, 0xce, 0x11, 0xdf);

#ifndef __cplusplus_winrt
#define ICustomSinkCallbackObj _ComPtr<ICustomSinkCallback>
#define IID_ICustomSinkCallback __uuidof(ICustomSinkCallback)
#else
#define ICustomSinkCallbackObj ICustomSinkCallback^
DEFINE_GUID(IID_ICustomSinkCallback, 0x45b51a86, 0xadb, 0x4206, 0x94, 0x4c, 0x79, 0x45, 0x77, 0xc3, 0x72, 0x55);
#endif

#ifndef _WINRT_DLL
#define StreamSink_SOURCE_ID 0
#define DISPID_PROPERTYCHANGED 1
#define IStreamSinkDispEventFTMImpl IDispEventFTMImpl<StreamSink_SOURCE_ID, StreamSink, &DIID_DPropertyChangedEvents, &LIBID_SecCamCapLib, 1, 0>
#else
#include <wrl\implements.h>
#include <wrl\wrappers\corewrappers.h>
#include <windows.media.h>
#include <windows.media.capture.h>
#define ICustomStreamSink StreamSink

#define CUSTOMSINK_PROP_CALLBACK L"sinkcallback"
#define CUSTOMSINK_PROP_VIDTYPE L"vidtype"
#define CUSTOMSINK_PROP_VIDENCPROPS L"videncprops"
#define CUSTOMSINK_PROP_AUDENCPROPS L"audencprops"
#endif

#ifdef __cplusplus_winrt
[uuid("45B51A86-0ADB-4206-944C-794577C37255")]
public interface class ICustomSinkCallback
#else
interface DECLSPEC_UUID("45B51A86-0ADB-4206-944C-794577C37255") DECLSPEC_NOVTABLE ICustomSinkCallback
#if _WINRT_DLL
	: public IInspectable
#else
	: public IUnknown
#endif
#endif
{
#ifdef __cplusplus_winrt
	virtual void Initialize() = 0;
	virtual void Cleanup() = 0;
	virtual void RemoveNotifyPropChange() = 0;
	virtual void PrepareToProcessSample() = 0;
	virtual void ProcessSample(BYTE* pSrc, INT32 lDefaultStride, UINT32 imageWidthInPixels, UINT32 imageHeightInPixels, LONGLONG hnsTime, Platform::Guid guiCurrentSubtype) = 0;
#else
	virtual STDMETHODIMP Initialize() = 0;
	virtual STDMETHODIMP Cleanup() = 0;
	virtual STDMETHODIMP RemoveNotifyPropChange() = 0;
	virtual STDMETHODIMP PrepareToProcessSample() = 0;
	virtual STDMETHODIMP ProcessSample(const BYTE* pSrc, LONG lDefaultStride, UINT32 imageWidthInPixels, UINT32 imageHeightInPixels, LONGLONG hnsTime, const GUID guiCurrentSubtype) = 0;
#endif
#ifdef _WINRT_DLL
#ifdef __cplusplus_winrt
	virtual void AddNotifyPropChange(TimeStamp::WebCamSecUtils::PropertyChangedHandler^ pHandler) = 0;
#else
	virtual STDMETHODIMP AddNotifyPropChange(ABI::Windows::Foundation::__FIEventHandler_1_IInspectable_t* pHandler) = 0;
#endif
#else
	virtual STDMETHODIMP AddNotifyPropChange(IStreamSinkDispEventFTMImpl* pHandler) = 0;
#endif
};

#ifdef _WINRT_DLL
#ifdef __cplusplus_winrt
#define StreamSinkBases IMFStreamSink, IMFMediaEventGenerator, IMFMediaTypeHandler, CBaseAttributes<>
#else
#define StreamSinkBases ABI::Windows::Foundation::IEventHandler<IInspectable*>, IMFStreamSink, IMFMediaEventGenerator, IMFMediaTypeHandler, CBaseAttributes<>
#endif
#endif

class StreamSink :
#ifdef _WINRT_DLL
	public Microsoft::WRL::RuntimeClass<
	Microsoft::WRL::RuntimeClassFlags< Microsoft::WRL::RuntimeClassType::ClassicCom>,
	StreamSinkBases >
#else
	public CComObjectRootEx<CComGlobalsThreadModel>,
	public CComCoClass<StreamSink, &CLSID_StreamSink>,
	public IMFStreamSink,
	public IMFMediaTypeHandler,
	public IStreamSinkDispEventFTMImpl,
	public CBaseAttributes<>,
	public ICustomStreamSink
#endif
{
#ifdef _WINRT_DLL

	//temporary not clean override must explicitly derive IMarshal and aggregate each interface call to the FTM
	//WinRT does not seem to support QI aggregation and FTMBase does not properly support IMarshal either

	//cannot be inspectable since to have its own free threaded marshaller
public:
	// IUnknown methods
	STDMETHOD(QueryInterface)(REFIID riid, _Outptr_result_nullonfailure_ void **ppv)
	{
		if (ppv == nullptr) {
			return E_POINTER;
		}
		(*ppv) = nullptr;
		HRESULT hr = S_OK;
		if (riid == IID_IMarshal) {
			return MarshalQI(riid, ppv);
		}
		else {
			hr = RuntimeClassT::QueryInterface(riid, ppv);
		}

		return hr;
	}

	STDMETHOD(RuntimeClassInitialize)() { return S_OK; }
#ifndef __cplusplus_winrt
	STDMETHODIMP Invoke(IInspectable *, IInspectable *) {
		m_bDirty = TRUE;
		return S_OK;
	}
#endif
#else
public:
	static HRESULT WINAPI CustomQI(void* pv, REFIID riid, LPVOID* ppv, DWORD_PTR dw)
	{
		if (dw == 0) {
			return ((StreamSink*)pv)->MarshalQI(riid, ppv);
		}
		return S_FALSE;
	}

	BEGIN_COM_MAP(StreamSink)
		COM_INTERFACE_ENTRY(IMFStreamSink)
		COM_INTERFACE_ENTRY2(IMFMediaEventGenerator, IMFStreamSink)
		COM_INTERFACE_ENTRY(IMFMediaTypeHandler)
		COM_INTERFACE_ENTRY(IMFAttributes)
		COM_INTERFACE_ENTRY(ICustomStreamSink)
		COM_INTERFACE_ENTRY_FUNC(__uuidof(IMarshal), 0, CustomQI)
	END_COM_MAP()

	BEGIN_SINK_MAP(StreamSink)
		SINK_ENTRY_EX(StreamSink_SOURCE_ID, DIID_DPropertyChangedEvents, DISPID_PROPERTYCHANGED, &OnPropertyChanged)
	END_SINK_MAP()

	void OnPropertyChanged()
	{
		m_bDirty = TRUE;
	}
#endif
	HRESULT MarshalQI(REFIID riid, LPVOID* ppv)
	{
		HRESULT hr = S_OK;
		if (m_spFTM == nullptr) {
			EnterCriticalSection(&m_critSec);
			if (m_spFTM == nullptr) {
				hr = CoCreateFreeThreadedMarshaler((IMFStreamSink*)this, &m_spFTM);
			}
			LeaveCriticalSection(&m_critSec);
		}

		if (SUCCEEDED(hr)) {
			if (m_spFTM == nullptr) {
				hr = E_UNEXPECTED;
			}
			else {
				hr = m_spFTM.Get()->QueryInterface(riid, ppv);
			}
		}
		return hr;
	}
	enum State
	{
		State_TypeNotSet = 0,	// No media type is set
		State_Ready,			 // Media type is set, Start has never been called.
		State_Started,
		State_Stopped,
		State_Paused,
		State_Count			  // Number of states
	};
	StreamSink() : m_IsShutdown(false),
		m_StartTime(0), m_fGetStartTimeFromSample(false), m_fWaitingForFirstSample(false),
		m_state(State_TypeNotSet), m_pParent(nullptr), m_bDirty(true),
		m_imageWidthInPixels(0), m_imageHeightInPixels(0) {
		InitializeCriticalSectionEx(&m_critSec, 3000, 0);
		ZeroMemory(&m_guiCurrentSubtype, sizeof(m_guiCurrentSubtype));
		CBaseAttributes<>::Initialize(0U);
#ifdef _DEBUG
		Inspection::Trace(_T("StreamSink::StreamSink\n"));
#endif
	}
	virtual ~StreamSink() {
#ifdef _DEBUG
		Inspection::Trace(_T("StreamSink::~StreamSink\n"));
#endif
		DeleteCriticalSection(&m_critSec);
		assert(m_IsShutdown);
	}

	STDMETHODIMP Initialize()
	{
		HRESULT hr;
		// Create the event queue helper.
		hr = MFCreateEventQueue(&m_spEventQueue);
		if (SUCCEEDED(hr))
		{
			_ComPtr<IMFMediaSink> pMedSink;
			hr = CBaseAttributes<>::GetUnknown(MF_STREAMSINK_MEDIASINKINTERFACE, __uuidof(IMFMediaSink), (LPVOID*)pMedSink.GetAddressOf());
			assert(pMedSink.Get() != NULL);
			if (SUCCEEDED(hr)) {
				ICustomSinkCallbackObj pCallback;
				if (SUCCEEDED(hr = pMedSink.Get()->QueryInterface(IID_PPV_ARGS(&m_pParent)))) {
					if (SUCCEEDED(hr = PropBinder<ICustomSinkCallbackObj>::GetPropCallback(m_pParent, MF_CUSTOMSINK_CALLBACK, IID_ICustomSinkCallback, pCallback))) {
#ifdef __cplusplus_winrt
						pCallback->AddNotifyPropChange(ref new TimeStamp::WebCamSecUtils::PropertyChangedHandler([this]() { m_bDirty = true; }));
#else
						pCallback->AddNotifyPropChange(this);
#endif
					}
					if (SUCCEEDED(hr)) {
						pCallback->Initialize();
					}
				}
			}
		}
		return hr;
	}

	HRESULT CheckShutdown() const
	{
		if (m_IsShutdown)
		{
			return MF_E_SHUTDOWN;
		}
		else
		{
			return S_OK;
		}
	}
	// Called when the presentation clock starts.
	STDMETHODIMP Start(MFTIME start)
	{
		HRESULT hr = S_OK;
		EnterCriticalSection(&m_critSec);
		if (m_state != State_TypeNotSet) {
			if (start != PRESENTATION_CURRENT_POSITION)
			{
				m_StartTime = start;		// Cache the start time.
				m_fGetStartTimeFromSample = false;
			}
			else
			{
				m_fGetStartTimeFromSample = true;
			}
			m_state = State_Started;
			m_bDirty = true;
			GUID guiMajorType;
			m_fWaitingForFirstSample = SUCCEEDED(m_spCurrentType->GetMajorType(&guiMajorType)) && (guiMajorType == MFMediaType_Video);
			hr = QueueEvent(MEStreamSinkStarted, GUID_NULL, hr, NULL);
			if (SUCCEEDED(hr)) {
				hr = QueueEvent(MEStreamSinkRequestSample, GUID_NULL, hr, NULL);
			}
		}
		else hr = MF_E_NOT_INITIALIZED;
		LeaveCriticalSection(&m_critSec);
		return hr;
	}

	// Called when the presentation clock pauses.
	STDMETHODIMP Pause()
	{
		EnterCriticalSection(&m_critSec);

		HRESULT hr = S_OK;

		if (m_state != State_Stopped && m_state != State_TypeNotSet) {
			m_state = State_Paused;
			hr = QueueEvent(MEStreamSinkPaused, GUID_NULL, hr, NULL);
		}
		else if (hr == State_TypeNotSet)
			hr = MF_E_NOT_INITIALIZED;
		else
			hr = MF_E_INVALIDREQUEST;
		LeaveCriticalSection(&m_critSec);
		return hr;
	}
	// Called when the presentation clock restarts.
	STDMETHODIMP Restart()
	{
		EnterCriticalSection(&m_critSec);

		HRESULT hr = S_OK;

		if (m_state == State_Paused) {
			m_state = State_Started;
			hr = QueueEvent(MEStreamSinkStarted, GUID_NULL, hr, NULL);
			if (SUCCEEDED(hr)) {
				hr = QueueEvent(MEStreamSinkRequestSample, GUID_NULL, hr, NULL);
			}
		}
		else if (hr == State_TypeNotSet)
			hr = MF_E_NOT_INITIALIZED;
		else
			hr = MF_E_INVALIDREQUEST;
		LeaveCriticalSection(&m_critSec);
		return hr;
	}
	// Called when the presentation clock stops.
	STDMETHODIMP Stop()
	{
		EnterCriticalSection(&m_critSec);

		HRESULT hr = S_OK;

		if (m_state != State_TypeNotSet) {
			m_state = State_Stopped;
			hr = QueueEvent(MEStreamSinkStopped, GUID_NULL, hr, NULL);
			ICustomSinkCallbackObj pCallback;
			if (SUCCEEDED(hr = PropBinder<ICustomSinkCallbackObj>::GetPropCallback(m_pParent, MF_CUSTOMSINK_CALLBACK, IID_ICustomSinkCallback, pCallback))) {
				pCallback->Cleanup();
			}
		}
		else hr = MF_E_NOT_INITIALIZED;
		LeaveCriticalSection(&m_critSec);
		return hr;
	}

	// Shuts down the stream sink.
	STDMETHODIMP Shutdown()
	{
		HRESULT hr = S_OK;
		assert(!m_IsShutdown);

		if (m_spEventQueue) {
			hr = m_spEventQueue->Shutdown();
		}
		if (m_pParent)
			m_pParent->Release();
		m_spCurrentType.Reset();
		ICustomSinkCallbackObj pCallback;
		if (SUCCEEDED(hr = PropBinder<ICustomSinkCallbackObj>::GetPropCallback(m_pParent, MF_CUSTOMSINK_CALLBACK, IID_ICustomSinkCallback, pCallback))) {
			pCallback->Cleanup();
		}
#ifndef __cplusplus_winrt
		hr = 
#endif
		pCallback->RemoveNotifyPropChange();
		m_IsShutdown = TRUE;

		return hr;
	}

	//IMFStreamSink
	HRESULT STDMETHODCALLTYPE GetMediaSink(
		/* [out] */ __RPC__deref_out_opt IMFMediaSink **ppMediaSink) {
		if (ppMediaSink == NULL)
		{
			return E_INVALIDARG;
		}

		EnterCriticalSection(&m_critSec);

		HRESULT hr = CheckShutdown();

		if (SUCCEEDED(hr))
		{
			_ComPtr<IMFMediaSink> pMedSink;
			hr = CBaseAttributes<>::GetUnknown(MF_STREAMSINK_MEDIASINKINTERFACE, __uuidof(IMFMediaSink), (LPVOID*)pMedSink.GetAddressOf());
			if (SUCCEEDED(hr)) {
				*ppMediaSink = pMedSink.Detach();
			}
		}

		LeaveCriticalSection(&m_critSec);
#ifdef _DEBUG
		Inspection::Trace(_T("StreamSink::GetMediaSink: HRESULT=%08lX\n"), hr);
#endif
		return hr;
	}

	HRESULT STDMETHODCALLTYPE GetIdentifier(
		/* [out] */ __RPC__out DWORD *pdwIdentifier) {
		if (pdwIdentifier == NULL)
		{
			return E_INVALIDARG;
		}

		EnterCriticalSection(&m_critSec);

		HRESULT hr = CheckShutdown();

		if (SUCCEEDED(hr))
		{
			hr = GetUINT32(MF_STREAMSINK_ID, (UINT32*)pdwIdentifier);
		}

		LeaveCriticalSection(&m_critSec);
#ifdef _DEBUG
		Inspection::Trace(_T("StreamSink::GetIdentifier: HRESULT=%08lX\n"), hr);
#endif
		return hr;
	}

	HRESULT STDMETHODCALLTYPE GetMediaTypeHandler(
		/* [out] */ __RPC__deref_out_opt IMFMediaTypeHandler **ppHandler) {
		if (ppHandler == NULL)
		{
			return E_INVALIDARG;
		}

		EnterCriticalSection(&m_critSec);

		HRESULT hr = CheckShutdown();

		// This stream object acts as its own type handler, so we QI ourselves.
		if (SUCCEEDED(hr))
		{
			hr = QueryInterface(IID_IMFMediaTypeHandler, (void**)ppHandler);
		}

		LeaveCriticalSection(&m_critSec);
#ifdef _DEBUG
		Inspection::Trace(_T("StreamSink::GetMediaTypeHandler: HRESULT=%08lX\n"), hr);
#endif
		return hr;
	}

	HRESULT STDMETHODCALLTYPE ProcessSample(IMFSample *pSample) {
		_ComPtr<IMFMediaBuffer> pInput;
		BYTE *pSrc = NULL;		  // Source buffer.
		LONG lSrcStride = 0;		// Source stride.
		// Stride if the buffer does not support IMF2DBuffer
		LONG lDefaultStride = 0;
		LONGLONG hnsTime = 0;
		bool bCheck = FALSE;
		if (pSample == NULL)
		{
			return E_INVALIDARG;
		}
		HRESULT hr = S_OK;
		//transform is doing the work
		if (InterlockedExchange((UINT*)&m_bDirty, (UINT)bCheck)) {
			ICustomSinkCallbackObj pCallback;
			if (SUCCEEDED(hr = PropBinder<ICustomSinkCallbackObj>::GetPropCallback(m_pParent, MF_CUSTOMSINK_CALLBACK, IID_ICustomSinkCallback, pCallback))) {
				pCallback->PrepareToProcessSample();
			}
		}

		EnterCriticalSection(&m_critSec);

		if (m_state != State_Started && m_state != State_Paused) {
			if (m_state == State_TypeNotSet)
				hr = MF_E_NOT_INITIALIZED;
			else
				hr = MF_E_INVALIDREQUEST;
		}
		if (SUCCEEDED(hr))
			hr = CheckShutdown();
		if (SUCCEEDED(hr)) {
			hr = pSample->ConvertToContiguousBuffer(&pInput);
			if (SUCCEEDED(hr)) {
				hr = pSample->GetSampleTime(&hnsTime);
			}
			if (SUCCEEDED(hr)) {
				hr = MotionDetection::GetDefaultStride(m_spCurrentType.Get(), &lDefaultStride);
			}
			VideoBufferLock inputLock(pInput.Get());
			if (SUCCEEDED(hr)) {
				hr = inputLock.LockBuffer(lDefaultStride, m_imageHeightInPixels, &pSrc, &lSrcStride);
			}
			if (SUCCEEDED(hr)) {
				ICustomSinkCallbackObj pCallback;
				if (SUCCEEDED(hr = PropBinder<ICustomSinkCallbackObj>::GetPropCallback(m_pParent, MF_CUSTOMSINK_CALLBACK, IID_ICustomSinkCallback, pCallback))) {
					pCallback->ProcessSample(pSrc, lDefaultStride, m_imageWidthInPixels, m_imageHeightInPixels, hnsTime, m_guiCurrentSubtype);
				}
			}
			if (SUCCEEDED(hr)) {
				hr = QueueEvent(MEStreamSinkRequestSample, GUID_NULL, S_OK, NULL);
			}

		}
		LeaveCriticalSection(&m_critSec);
		return hr;
	}

	HRESULT STDMETHODCALLTYPE PlaceMarker(
		/* [in] */ MFSTREAMSINK_MARKER_TYPE eMarkerType,
		/* [in] */ __RPC__in const PROPVARIANT *pvarMarkerValue,
		/* [in] */ __RPC__in const PROPVARIANT *pvarContextValue) {
		EnterCriticalSection(&m_critSec);

		HRESULT hr = S_OK;
		if (m_state == State_TypeNotSet)
			hr = MF_E_NOT_INITIALIZED;

		if (SUCCEEDED(hr))
			hr = CheckShutdown();

		if (SUCCEEDED(hr))
		{
			//at shutdown will receive MFSTREAMSINK_MARKER_ENDOFSEGMENT
			hr = QueueEvent(MEStreamSinkRequestSample, GUID_NULL, S_OK, NULL);
		}

		LeaveCriticalSection(&m_critSec);
#ifdef _DEBUG
		Inspection::Trace(_T("StreamSink::PlaceMarker: %s[%08lX] HRESULT=%08lX\n"), StreamSinkMarkerTypeMap.at(eMarkerType).c_str(), eMarkerType, hr);
#endif
		return hr;
	}

	HRESULT STDMETHODCALLTYPE Flush(void) {
		EnterCriticalSection(&m_critSec);

		HRESULT hr = CheckShutdown();

		if (SUCCEEDED(hr))
		{
			ICustomSinkCallbackObj pCallback;
			if (SUCCEEDED(hr = PropBinder<ICustomSinkCallbackObj>::GetPropCallback(m_pParent, MF_CUSTOMSINK_CALLBACK, IID_ICustomSinkCallback, pCallback))) {
				pCallback->Cleanup();
			}
		}

		LeaveCriticalSection(&m_critSec);
#ifdef _DEBUG
		Inspection::Trace(_T("StreamSink::Flush: HRESULT=%08lX\n"), hr);
#endif
		return hr;
	}

	//IMFMediaEventGenerator
	HRESULT STDMETHODCALLTYPE GetEvent(
		DWORD dwFlags, IMFMediaEvent **ppEvent) {
		// NOTE:
		// GetEvent can block indefinitely, so we don't hold the lock.
		// This requires some juggling with the event queue pointer.

		HRESULT hr = S_OK;

		_ComPtr<IMFMediaEventQueue> pQueue;

		{
			EnterCriticalSection(&m_critSec);

			// Check shutdown
			hr = CheckShutdown();

			// Get the pointer to the event queue.
			if (SUCCEEDED(hr))
			{
				pQueue = m_spEventQueue.Get();
			}
			LeaveCriticalSection(&m_critSec);
		}

		// Now get the event.
		if (SUCCEEDED(hr))
		{
			hr = pQueue->GetEvent(dwFlags, ppEvent);
		}
		MediaEventType meType;
		if (SUCCEEDED(hr) && SUCCEEDED((*ppEvent)->GetType(&meType)) && meType == MEStreamSinkStopped) {
		}
#ifdef _DEBUG
		HRESULT hrStatus;
		if (SUCCEEDED(hr))
			hr = (*ppEvent)->GetStatus(&hrStatus);
		if (SUCCEEDED(hr))
			Inspection::Trace(_T("StreamSink::GetEvent: %s[%08lX] HRESULT=%08lX\n"), MediaEventTypeMap.at(meType).c_str(), meType, hrStatus);
		else
			Inspection::Trace(_T("StreamSink::GetEvent: HRESULT=%08lX\n"), hr);
#endif
		return hr;
	}

	HRESULT STDMETHODCALLTYPE BeginGetEvent(
		IMFAsyncCallback *pCallback, IUnknown *punkState) {
		HRESULT hr = S_OK;

		EnterCriticalSection(&m_critSec);

		hr = CheckShutdown();

		if (SUCCEEDED(hr))
		{
			hr = m_spEventQueue->BeginGetEvent(pCallback, punkState);
		}
		LeaveCriticalSection(&m_critSec);
#ifdef _DEBUG
		Inspection::Trace(_T("StreamSink::BeginGetEvent: HRESULT=%08lX\n"), hr);
#endif
		return hr;
	}

	HRESULT STDMETHODCALLTYPE EndGetEvent(
		IMFAsyncResult *pResult, IMFMediaEvent **ppEvent) {
		HRESULT hr = S_OK;

		EnterCriticalSection(&m_critSec);

		hr = CheckShutdown();

		if (SUCCEEDED(hr))
		{
			hr = m_spEventQueue->EndGetEvent(pResult, ppEvent);
		}
		MediaEventType meType;
		if (SUCCEEDED(hr) && SUCCEEDED((*ppEvent)->GetType(&meType)) && meType == MEStreamSinkStopped) {
		}

		LeaveCriticalSection(&m_critSec);
#ifdef _DEBUG
		HRESULT hrStatus;
		if (SUCCEEDED(hr))
			hr = (*ppEvent)->GetStatus(&hrStatus);
		if (SUCCEEDED(hr))
			Inspection::Trace(_T("StreamSink::EndGetEvent: %s[%08lX] HRESULT=%08lX\n"), MediaEventTypeMap.at(meType).c_str(), meType, hrStatus);
		else
			Inspection::Trace(_T("StreamSink::EndGetEvent: HRESULT=%08lX\n"), hr);
#endif
		return hr;
	}

	HRESULT STDMETHODCALLTYPE QueueEvent(
		MediaEventType met, REFGUID guidExtendedType,
		HRESULT hrStatus, const PROPVARIANT *pvValue) {
		HRESULT hr = S_OK;

		EnterCriticalSection(&m_critSec);

		hr = CheckShutdown();

		if (SUCCEEDED(hr))
		{
			hr = m_spEventQueue->QueueEventParamVar(met, guidExtendedType, hrStatus, pvValue);
		}

		LeaveCriticalSection(&m_critSec);
#ifdef _DEBUG
		Inspection::Trace(_T("StreamSink::QueueEvent: HRESULT=%08lX %s[%08lX] HRESULT=%08lX\n"), hr, MediaEventTypeMap.at(met).c_str(), met, hrStatus);
#endif
		return hr;
	}

	/// IMFMediaTypeHandler methods

	// Check if a media type is supported.
	STDMETHODIMP IsMediaTypeSupported(
		/* [in] */ IMFMediaType *pMediaType,
		/* [out] */ IMFMediaType **ppMediaType)
	{
		if (pMediaType == nullptr)
		{
			return E_INVALIDARG;
		}

		EnterCriticalSection(&m_critSec);

		GUID majorType = GUID_NULL;

		HRESULT hr = CheckShutdown();

		if (SUCCEEDED(hr))
		{
			hr = pMediaType->GetGUID(MF_MT_MAJOR_TYPE, &majorType);
		}

		// First make sure it's video or audio type.
		if (SUCCEEDED(hr))
		{
			if (majorType != MFMediaType_Video && majorType != MFMediaType_Audio)
			{
				hr = MF_E_INVALIDTYPE;
			}
		}

		if (SUCCEEDED(hr) && m_spCurrentType != nullptr)
		{
			GUID guiNewSubtype;
			if (FAILED(pMediaType->GetGUID(MF_MT_SUBTYPE, &guiNewSubtype)) ||
				guiNewSubtype != m_guiCurrentSubtype)
			{
				hr = MF_E_INVALIDTYPE;
			}
		}

		// We don't return any "close match" types.
		if (ppMediaType)
		{
			*ppMediaType = nullptr;
		}

		if (ppMediaType && SUCCEEDED(hr)) {
			_ComPtr<IMFMediaType> pType;
			hr = MFCreateMediaType(ppMediaType);
			DWORD dwIdentifier;
			if (SUCCEEDED(hr)) {
				hr = GetUINT32(MF_STREAMSINK_ID, (UINT32*)&dwIdentifier);
			}
			if (SUCCEEDED(hr)) {
				hr = m_pParent->GetUnknown(dwIdentifier == AUDIO_IDENTIFIER ? MF_MEDIASINK_AUDIOPREFERREDTYPE : MF_MEDIASINK_PREFERREDTYPE, __uuidof(IMFMediaType), (LPVOID*)&pType);
			}
			if (SUCCEEDED(hr)) {
				hr = pType->LockStore();
			}
			bool bLocked = false;
			if (SUCCEEDED(hr)) {
				bLocked = true;
				UINT32 uiCount;
				UINT32 uiTotal;
				hr = pType->GetCount(&uiTotal);
				for (uiCount = 0; SUCCEEDED(hr) && uiCount < uiTotal; uiCount++) {
					GUID guid;
					PROPVARIANT propval;
					hr = pType->GetItemByIndex(uiCount, &guid, &propval);
					if (SUCCEEDED(hr) && (guid == MF_MT_MAJOR_TYPE || guid == MF_MT_SUBTYPE || guid == MF_MT_ALL_SAMPLES_INDEPENDENT ||
						(dwIdentifier == AUDIO_IDENTIFIER ? 0 :
						(guid == MF_MT_FRAME_SIZE || guid == MF_MT_PIXEL_ASPECT_RATIO || guid == MF_MT_INTERLACE_MODE)))) {
						hr = (*ppMediaType)->SetItem(guid, propval);
						PropVariantClear(&propval);
					}
				}
			}
			if (bLocked) {
				hr = pType->UnlockStore();
			}
		}

		LeaveCriticalSection(&m_critSec);
#ifdef _DEBUG
		Inspection::Trace(_T("StreamSink::IsMediaTypeSupported: HRESULT=%08lX\n"), hr);
		Inspection::InspectMediaType(pMediaType);
#endif
		return hr;
	}


	// Return the number of preferred media types.
	STDMETHODIMP GetMediaTypeCount(DWORD *pdwTypeCount)
	{
		if (pdwTypeCount == nullptr)
		{
			return E_INVALIDARG;
		}

		EnterCriticalSection(&m_critSec);

		HRESULT hr = CheckShutdown();

		if (SUCCEEDED(hr))
		{
			// We've got only one media type
			*pdwTypeCount = 1;
		}

		LeaveCriticalSection(&m_critSec);
#ifdef _DEBUG
		Inspection::Trace(_T("StreamSink::GetMediaTypeCount: HRESULT=%08lX\n"), hr);
#endif
		return hr;
	}


	// Return a preferred media type by index.
	STDMETHODIMP GetMediaTypeByIndex(
		/* [in] */ DWORD dwIndex,
		/* [out] */ IMFMediaType **ppType)
	{
		if (ppType == NULL) {
			return E_INVALIDARG;
		}

		EnterCriticalSection(&m_critSec);

		HRESULT hr = CheckShutdown();

		if (dwIndex > 0)
		{
			hr = MF_E_NO_MORE_TYPES;
		}
		else {
			//return preferred type based on media capture library 6 elements preferred preview type
			//hr = m_spCurrentType.CopyTo(ppType);
			if (SUCCEEDED(hr)) {
				_ComPtr<IMFMediaType> pType;
				hr = MFCreateMediaType(ppType);
				DWORD dwIdentifier;
				if (SUCCEEDED(hr)) {
					hr = GetUINT32(MF_STREAMSINK_ID, (UINT32*)&dwIdentifier);
				}
				if (SUCCEEDED(hr)) {
					hr = m_pParent->GetUnknown(dwIdentifier == AUDIO_IDENTIFIER ? MF_MEDIASINK_AUDIOPREFERREDTYPE : MF_MEDIASINK_PREFERREDTYPE, __uuidof(IMFMediaType), (LPVOID*)&pType);
				}
				if (SUCCEEDED(hr)) {
					hr = pType->LockStore();
				}
				bool bLocked = false;
				if (SUCCEEDED(hr)) {
					bLocked = true;
					UINT32 uiCount;
					UINT32 uiTotal;
					hr = pType->GetCount(&uiTotal);
					for (uiCount = 0; SUCCEEDED(hr) && uiCount < uiTotal; uiCount++) {
						GUID guid;
						PROPVARIANT propval;
						hr = pType->GetItemByIndex(uiCount, &guid, &propval);
						if (SUCCEEDED(hr) && (guid == MF_MT_MAJOR_TYPE || guid == MF_MT_SUBTYPE || guid == MF_MT_ALL_SAMPLES_INDEPENDENT ||
							(dwIdentifier == AUDIO_IDENTIFIER ? 0 :
							(guid == MF_MT_FRAME_SIZE || guid == MF_MT_PIXEL_ASPECT_RATIO || guid == MF_MT_INTERLACE_MODE)))) {
							hr = (*ppType)->SetItem(guid, propval);
							PropVariantClear(&propval);
						}
					}
				}
				if (bLocked) {
					hr = pType->UnlockStore();
				}
			}
		}

		LeaveCriticalSection(&m_critSec);
#ifdef _DEBUG
		Inspection::Trace(_T("StreamSink::GetMediaTypeByIndex: HRESULT=%08lX\n"), hr);
#endif
		return hr;
	}


	// Set the current media type.
	STDMETHODIMP SetCurrentMediaType(IMFMediaType *pMediaType)
	{
		if (pMediaType == NULL) {
			return E_INVALIDARG;
		}
		EnterCriticalSection(&m_critSec);

		HRESULT hr = S_OK;
		if (m_state != State_TypeNotSet && m_state != State_Ready)
			hr = MF_E_INVALIDREQUEST;
		if (SUCCEEDED(hr))
			hr = CheckShutdown();

		// We don't allow format changes after streaming starts.

		// We set media type already
		if (m_state >= State_Ready)
		{
			if (SUCCEEDED(hr))
			{
				hr = IsMediaTypeSupported(pMediaType, NULL);
			}
		}

		if (SUCCEEDED(hr))
		{
			hr = MFCreateMediaType(m_spCurrentType.ReleaseAndGetAddressOf());
			if (SUCCEEDED(hr))
			{
				hr = pMediaType->CopyAllItems(m_spCurrentType.Get());
			}
			if (SUCCEEDED(hr))
			{
				hr = m_spCurrentType->GetGUID(MF_MT_SUBTYPE, &m_guiCurrentSubtype);
			}
			GUID guid;
			if (SUCCEEDED(hr)) {
				hr = m_spCurrentType->GetMajorType(&guid);
			}
			if (SUCCEEDED(hr) && guid == MFMediaType_Video) {
				hr = MFGetAttributeSize(m_spCurrentType.Get(), MF_MT_FRAME_SIZE, &m_imageWidthInPixels, &m_imageHeightInPixels);
			}
			if (SUCCEEDED(hr))
			{
				m_state = State_Ready;
			}
		}

		LeaveCriticalSection(&m_critSec);
#ifdef _DEBUG
		Inspection::Trace(_T("StreamSink::SetCurrentMediaType: HRESULT=%08lX\n"), hr);
		Inspection::InspectMediaType(pMediaType);
#endif
		return hr;
	}

	// Return the current media type, if any.
	STDMETHODIMP GetCurrentMediaType(IMFMediaType **ppMediaType)
	{
		if (ppMediaType == NULL) {
			return E_INVALIDARG;
		}

		EnterCriticalSection(&m_critSec);

		HRESULT hr = CheckShutdown();
		if (SUCCEEDED(hr)) {
			if (m_spCurrentType == nullptr) {
				hr = MF_E_NOT_INITIALIZED;
			}
		}

		if (SUCCEEDED(hr)) {
			hr = m_spCurrentType.CopyTo(ppMediaType);
		}

		LeaveCriticalSection(&m_critSec);
#ifdef _DEBUG
		Inspection::Trace(_T("StreamSink::GetCurrentMediaType: HRESULT=%08lX\n"), hr);
#endif
		return hr;
	}


	// Return the major type GUID.
	STDMETHODIMP GetMajorType(GUID *pguidMajorType)
	{
		HRESULT hr = S_OK;
		if (pguidMajorType == nullptr) {
			return E_INVALIDARG;
		}

		_ComPtr<IMFMediaType> pType;
		DWORD dwIdentifier;
		hr = GetUINT32(MF_STREAMSINK_ID, (UINT32*)&dwIdentifier);
		if (SUCCEEDED(hr)) {
			hr = m_pParent->GetUnknown(dwIdentifier == AUDIO_IDENTIFIER ? MF_MEDIASINK_AUDIOPREFERREDTYPE : MF_MEDIASINK_PREFERREDTYPE, __uuidof(IMFMediaType), (LPVOID*)&pType);
		}
		if (SUCCEEDED(hr)) {
			hr = pType->GetMajorType(pguidMajorType);
		}
#ifdef _DEBUG
		Inspection::Trace(_T("StreamSink::GetMajorType: HRESULT=%08lX\n"), hr);
#endif
		return hr;
	}
private:
	bool m_IsShutdown;				// Flag to indicate if Shutdown() method was called.
	CRITICAL_SECTION m_critSec;
	IMFAttributes*				m_pParent;
	_ComPtr<IMFMediaType>		m_spCurrentType;
	_ComPtr<IMFMediaEventQueue>  m_spEventQueue;			  // Event queue

	_ComPtr<IUnknown>			m_spFTM;
	State					   m_state;
	bool						m_fGetStartTimeFromSample;
	bool						m_fWaitingForFirstSample;
	MFTIME					  m_StartTime;				 // Presentation time when the clock started.
	GUID						m_guiCurrentSubtype;
	volatile bool				m_bDirty;
	UINT32					  m_imageWidthInPixels;
	UINT32					  m_imageHeightInPixels;
};

#include "LinkList.h"

namespace TimeStamp {
	namespace WebCamSecExtensions {
		class
#ifndef _WINRT_DLL
			DECLSPEC_UUID("9F352FE5-3501-4A3E-9C2D-8E3B198D0C82")
#endif
		MediaSink:
#ifdef _WINRT_DLL
		public Microsoft::WRL::RuntimeClass<
			Microsoft::WRL::RuntimeClassFlags< Microsoft::WRL::RuntimeClassType::WinRtClassicComMix >,
			Microsoft::WRL::Implements<ABI::Windows::Media::IMediaExtension>,
			IMFMediaSink,
			IMFClockStateSink,
			Microsoft::WRL::FtmBase,
			CBaseAttributes<> >
#else
		//still being implemented on CSecCamCapture...
		public CComObjectRootEx<CComGlobalsThreadModel>,
			public CComCoClass<MediaSink, &CLSID_MediaSink>,
			public IMFMediaSink, public IMFClockStateSink, public CBaseAttributes<>
#endif
		{
#ifdef _WINRT_DLL
			InspectableClass(RuntimeClass_TimeStamp_WebCamSecExtensions_MediaSink, BaseTrust)
#else
		public:
			BEGIN_COM_MAP(MediaSink)
				COM_INTERFACE_ENTRY(IMFMediaSink)
				COM_INTERFACE_ENTRY(IMFClockStateSink)
				COM_INTERFACE_ENTRY(IMFAttributes)
			END_COM_MAP()

#endif
		public:
			MediaSink() : m_IsShutdown(false), m_llStartTime(0) {
				HRESULT hr = CBaseAttributes<>::Initialize(0U);
				InitializeCriticalSectionEx(&m_critSec, 3000, 0);
#ifdef _DEBUG
				Inspection::Trace(_T("MediaSink::MediaSink\n"));
#endif
			}

			virtual ~MediaSink() {
#ifdef _DEBUG
				Inspection::Trace(_T("MediaSink::~MediaSink\n"));
#endif
				DeleteCriticalSection(&m_critSec);
				assert(m_IsShutdown);
			}
			HRESULT CheckShutdown() const
			{
				if (m_IsShutdown)
				{
					return MF_E_SHUTDOWN;
				}
				else
				{
					return S_OK;
				}
			}
#ifdef _WINRT_DLL
			STDMETHODIMP SetProperties(ABI::Windows::Foundation::Collections::IPropertySet *pConfiguration)
			{
				HRESULT hr = S_OK;
				if (pConfiguration) {
					Microsoft::WRL::ComPtr<IInspectable> spInsp;
					Microsoft::WRL::ComPtr<ABI::Windows::Foundation::Collections::IMap<HSTRING, IInspectable *>> spSetting;
					Microsoft::WRL::ComPtr<ABI::Windows::Foundation::IPropertyValue> spPropVal;
					Microsoft::WRL::ComPtr<ABI::Windows::Media::MediaProperties::IMediaEncodingProperties> pMedEncProps;
					UINT32 uiType = ABI::Windows::Media::Capture::MediaStreamType_VideoPreview;

					hr = pConfiguration->QueryInterface(IID_PPV_ARGS(&spSetting));
					if (FAILED(hr)) {
						hr = E_FAIL;
					}

					if (SUCCEEDED(hr)) {
						hr = spSetting->Lookup(Microsoft::WRL::Wrappers::HStringReference(CUSTOMSINK_PROP_CALLBACK).Get(), spInsp.ReleaseAndGetAddressOf());
						if (FAILED(hr)) {
							hr = E_INVALIDARG;
						}
						if (SUCCEEDED(hr)) {
							hr = PropBinder<ICustomSinkCallbackObj>::SetPropCallback(this, MF_CUSTOMSINK_CALLBACK, IID_ICustomSinkCallback, spInsp.Get());
						}
					}
					if (SUCCEEDED(hr)) {
						hr = spSetting->Lookup(Microsoft::WRL::Wrappers::HStringReference(CUSTOMSINK_PROP_VIDTYPE).Get(), spInsp.ReleaseAndGetAddressOf());
						if (FAILED(hr)) {
							hr = E_INVALIDARG;
						}
						if (SUCCEEDED(hr)) {
							if (SUCCEEDED(hr = spInsp.As(&spPropVal))) {
								hr = spPropVal->GetUInt32(&uiType);
							}
						}
					}
					if (SUCCEEDED(hr)) {
						hr = spSetting->Lookup(Microsoft::WRL::Wrappers::HStringReference(CUSTOMSINK_PROP_VIDENCPROPS).Get(), spInsp.ReleaseAndGetAddressOf());
						if (FAILED(hr)) {
							hr = E_INVALIDARG;
						}
						if (SUCCEEDED(hr)) {
							hr = spInsp.As(&pMedEncProps);
						}
					}
					if (SUCCEEDED(hr)) {
						hr = SetMediaStreamProperties((ABI::Windows::Media::Capture::MediaStreamType)uiType, pMedEncProps.Get());
					}
					if (SUCCEEDED(hr)) {
						hr = spSetting->Lookup(Microsoft::WRL::Wrappers::HStringReference(CUSTOMSINK_PROP_AUDENCPROPS).Get(), spInsp.ReleaseAndGetAddressOf());
						if (FAILED(hr)) {
							hr = E_INVALIDARG;
						}
						if (SUCCEEDED(hr)) {
							hr = spInsp.As(&pMedEncProps);
						}
					}
					if (SUCCEEDED(hr)) {
						hr = SetMediaStreamProperties(ABI::Windows::Media::Capture::MediaStreamType::MediaStreamType_Audio, pMedEncProps.Get());
					}
				}

				return hr;
			}
			static DWORD GetStreamId(ABI::Windows::Media::Capture::MediaStreamType mediaStreamType)
			{
				return 3 - mediaStreamType;
			}
			static HRESULT AddAttribute(_In_ GUID guidKey, _In_ ABI::Windows::Foundation::IPropertyValue *pValue, _In_ IMFAttributes* pAttr)
			{
				HRESULT hr = S_OK;
				PROPVARIANT var;
				ABI::Windows::Foundation::PropertyType type;
				hr = pValue->get_Type(&type);
				ZeroMemory(&var, sizeof(var));

				if (SUCCEEDED(hr))
				{
					switch (type)
					{
					case ABI::Windows::Foundation::PropertyType_UInt8Array:
					{
						UINT32 cbBlob;
						BYTE *pbBlog = nullptr;
						hr = pValue->GetUInt8Array(&cbBlob, &pbBlog);
						if (SUCCEEDED(hr))
						{
							if (pbBlog == nullptr)
							{
								hr = E_INVALIDARG;
							}
							else
							{
								hr = pAttr->SetBlob(guidKey, pbBlog, cbBlob);
							}
						}
						CoTaskMemFree(pbBlog);
					}
						break;

					case ABI::Windows::Foundation::PropertyType_Double:
					{
						DOUBLE value;
						hr = pValue->GetDouble(&value);
						if (SUCCEEDED(hr))
						{
							hr = pAttr->SetDouble(guidKey, value);
						}
					}
						break;

					case ABI::Windows::Foundation::PropertyType_Guid:
					{
						GUID value;
						hr = pValue->GetGuid(&value);
						if (SUCCEEDED(hr))
						{
							hr = pAttr->SetGUID(guidKey, value);
						}
					}
						break;

					case ABI::Windows::Foundation::PropertyType_String:
					{
						Microsoft::WRL::Wrappers::HString value;
						hr = pValue->GetString(value.GetAddressOf());
						if (SUCCEEDED(hr))
						{
							UINT32 len = 0;
							LPCWSTR szValue = WindowsGetStringRawBuffer(value.Get(), &len);
							hr = pAttr->SetString(guidKey, szValue);
						}
					}
						break;

					case ABI::Windows::Foundation::PropertyType_UInt32:
					{
						UINT32 value;
						hr = pValue->GetUInt32(&value);
						if (SUCCEEDED(hr))
						{
							pAttr->SetUINT32(guidKey, value);
						}
					}
						break;

					case ABI::Windows::Foundation::PropertyType_UInt64:
					{
						UINT64 value;
						hr = pValue->GetUInt64(&value);
						if (SUCCEEDED(hr))
						{
							hr = pAttr->SetUINT64(guidKey, value);
						}
					}
						break;

					case ABI::Windows::Foundation::PropertyType_Inspectable:
					{
						Microsoft::WRL::ComPtr<IInspectable> value;
						hr = TYPE_E_TYPEMISMATCH;
						if (SUCCEEDED(hr))
						{
							pAttr->SetUnknown(guidKey, value.Get());
						}
					}
						break;

						// ignore unknown values
					}
				}

				return hr;
			}
			static HRESULT ConvertPropertiesToMediaType(_In_ ABI::Windows::Media::MediaProperties::IMediaEncodingProperties *pMEP, _Outptr_ IMFMediaType **ppMT)
			{
				HRESULT hr = S_OK;
				_ComPtr<IMFMediaType> spMT;
				Microsoft::WRL::ComPtr<ABI::Windows::Foundation::Collections::IMap<GUID, IInspectable*>> spMap;
				Microsoft::WRL::ComPtr<ABI::Windows::Foundation::Collections::IIterable<ABI::Windows::Foundation::Collections::IKeyValuePair<GUID, IInspectable*>*>> spIterable;
				Microsoft::WRL::ComPtr<ABI::Windows::Foundation::Collections::IIterator<ABI::Windows::Foundation::Collections::IKeyValuePair<GUID, IInspectable*>*>> spIterator;

				if (pMEP == nullptr || ppMT == nullptr)
				{
					return E_INVALIDARG;
				}
				*ppMT = nullptr;

				hr = pMEP->get_Properties(spMap.GetAddressOf());
				if (SUCCEEDED(hr))
				{
					hr = spMap.As(&spIterable);
				}
				if (SUCCEEDED(hr))
				{
					hr = spIterable->First(&spIterator);
				}
				if (SUCCEEDED(hr))
				{
					MFCreateMediaType(spMT.ReleaseAndGetAddressOf());
				}

				boolean hasCurrent = false;
				if (SUCCEEDED(hr))
				{
					hr = spIterator->get_HasCurrent(&hasCurrent);
				}

				while (hasCurrent)
				{
					Microsoft::WRL::ComPtr<ABI::Windows::Foundation::Collections::IKeyValuePair<GUID, IInspectable*> > spKeyValuePair;
					Microsoft::WRL::ComPtr<IInspectable> spValue;
					Microsoft::WRL::ComPtr<ABI::Windows::Foundation::IPropertyValue> spPropValue;
					GUID guidKey;

					hr = spIterator->get_Current(&spKeyValuePair);
					if (FAILED(hr))
					{
						break;
					}
					hr = spKeyValuePair->get_Key(&guidKey);
					if (FAILED(hr))
					{
						break;
					}
					hr = spKeyValuePair->get_Value(&spValue);
					if (FAILED(hr))
					{
						break;
					}
					hr = spValue.As(&spPropValue);
					if (FAILED(hr))
					{
						break;
					}
					hr = AddAttribute(guidKey, spPropValue.Get(), spMT.Get());
					if (FAILED(hr))
					{
						break;
					}

					hr = spIterator->MoveNext(&hasCurrent);
					if (FAILED(hr))
					{
						break;
					}
				}


				if (SUCCEEDED(hr))
				{
					Microsoft::WRL::ComPtr<IInspectable> spValue;
					Microsoft::WRL::ComPtr<ABI::Windows::Foundation::IPropertyValue> spPropValue;
					GUID guiMajorType;

					hr = spMap->Lookup(MF_MT_MAJOR_TYPE, spValue.GetAddressOf());

					if (SUCCEEDED(hr))
					{
						hr = spValue.As(&spPropValue);
					}
					if (SUCCEEDED(hr))
					{
						hr = spPropValue->GetGuid(&guiMajorType);
					}
					if (SUCCEEDED(hr))
					{
						if (guiMajorType != MFMediaType_Video && guiMajorType != MFMediaType_Audio)
						{
							hr = E_UNEXPECTED;
						}
					}
				}

				if (SUCCEEDED(hr))
				{
					*ppMT = spMT.Detach();
				}

				return hr;
			}
			//this should be passed through SetProperties!
			HRESULT SetMediaStreamProperties(ABI::Windows::Media::Capture::MediaStreamType MediaStreamType,
				_In_opt_ ABI::Windows::Media::MediaProperties::IMediaEncodingProperties *mediaEncodingProperties)
			{
				HRESULT hr = S_OK;
				_ComPtr<IMFMediaType> spMediaType;

				if (MediaStreamType != ABI::Windows::Media::Capture::MediaStreamType_VideoPreview &&
					MediaStreamType != ABI::Windows::Media::Capture::MediaStreamType_VideoRecord &&
					MediaStreamType != ABI::Windows::Media::Capture::MediaStreamType_Audio)
				{
					return E_INVALIDARG;
				}

				RemoveStreamSink(GetStreamId(MediaStreamType));

				if (mediaEncodingProperties != nullptr)
				{
					_ComPtr<IMFStreamSink> spStreamSink;
					hr = ConvertPropertiesToMediaType(mediaEncodingProperties, &spMediaType);
					if (SUCCEEDED(hr))
					{
						hr = AddStreamSink(GetStreamId(MediaStreamType), nullptr, spStreamSink.GetAddressOf());
					}
					if (SUCCEEDED(hr)) {
						hr = SetUnknown(MediaStreamType == ABI::Windows::Media::Capture::MediaStreamType_Audio ? MF_MEDIASINK_AUDIOPREFERREDTYPE : MF_MEDIASINK_PREFERREDTYPE, spMediaType.Detach());
					}
				}

				return hr;
			}
#endif
			//IMFMediaSink
			HRESULT STDMETHODCALLTYPE GetCharacteristics(
				/* [out] */ __RPC__out DWORD *pdwCharacteristics) {
				HRESULT hr;
				if (pdwCharacteristics == NULL) return E_INVALIDARG;
				EnterCriticalSection(&m_critSec);
				if (SUCCEEDED(hr = CheckShutdown())) {
					//if had an activation object for the sink, shut down would be managed and MF_STREAM_SINK_SUPPORTS_ROTATION appears to be setable to TRUE
					*pdwCharacteristics = MEDIASINK_FIXED_STREAMS;// | MEDIASINK_REQUIRE_REFERENCE_MEDIATYPE;
				}
				LeaveCriticalSection(&m_critSec);
#ifdef _DEBUG
				Inspection::Trace(_T("MediaSink::GetCharacteristics: HRESULT=%08lX\n"), hr);
#endif
				return hr;
			}

			HRESULT STDMETHODCALLTYPE AddStreamSink(
				DWORD dwStreamSinkIdentifier, IMFMediaType* pMediaType, IMFStreamSink **ppStreamSink) {
				_ComPtr<IMFStreamSink> spMFStream;
				_ComPtr<ICustomStreamSink> pStream;
				EnterCriticalSection(&m_critSec);
				HRESULT hr = CheckShutdown();

				if (SUCCEEDED(hr))
				{
					hr = GetStreamSinkById(dwStreamSinkIdentifier, &spMFStream);
				}

				if (SUCCEEDED(hr))
				{
					hr = MF_E_STREAMSINK_EXISTS;
				}
				else
				{
					hr = S_OK;
				}

				if (SUCCEEDED(hr))
				{
#ifdef _WINRT_DLL
					pStream = Microsoft::WRL::Make<StreamSink>();
					if (pStream == nullptr) {
						hr = E_OUTOFMEMORY;
					}
					if (SUCCEEDED(hr))
						hr = pStream.As<IMFStreamSink>(&spMFStream);
#else
					hr = StreamSink::CreateInstance<IMFStreamSink>((IMFStreamSink**)spMFStream.ReleaseAndGetAddressOf());
					if (SUCCEEDED(hr)) {
						hr = spMFStream.As(&pStream);
					}
#endif
				}
				// Initialize the stream.
				_ComPtr<IMFAttributes> pAttr;
				if (SUCCEEDED(hr)) {
					hr = pStream.As(&pAttr);
				}
				if (SUCCEEDED(hr)) {
					hr = pAttr->SetUINT32(MF_STREAMSINK_ID, dwStreamSinkIdentifier);
					if (SUCCEEDED(hr)) {
						hr = pAttr->SetUnknown(MF_STREAMSINK_MEDIASINKINTERFACE, (IMFMediaSink*)this);
					}
				}
				if (SUCCEEDED(hr)) {
					hr = pStream->Initialize();
				}

				if (SUCCEEDED(hr))
				{
					ComPtrList<IMFStreamSink>::POSITION pos = m_streams.FrontPosition();
					ComPtrList<IMFStreamSink>::POSITION posEnd = m_streams.EndPosition();

					// Insert in proper position
					for (; pos != posEnd; pos = m_streams.Next(pos))
					{
						DWORD dwCurrId;
						_ComPtr<IMFStreamSink> spCurr;
						hr = m_streams.GetItemPos(pos, &spCurr);
						if (FAILED(hr))
						{
							break;
						}
						hr = spCurr->GetIdentifier(&dwCurrId);
						if (FAILED(hr))
						{
							break;
						}

						if (dwCurrId > dwStreamSinkIdentifier)
						{
							break;
						}
					}

					if (SUCCEEDED(hr))
					{
						hr = m_streams.InsertPos(pos, spMFStream.Get());
					}
				}

				if (SUCCEEDED(hr))
				{
					*ppStreamSink = spMFStream.Detach();
				}
				LeaveCriticalSection(&m_critSec);
#ifdef _DEBUG
				Inspection::Trace(_T("MediaSink::AddStreamSink: HRESULT=%08lX\n"), hr);
#endif
				return hr;
			}

			HRESULT STDMETHODCALLTYPE RemoveStreamSink(DWORD dwStreamSinkIdentifier) {
				EnterCriticalSection(&m_critSec);
				HRESULT hr = CheckShutdown();
				ComPtrList<IMFStreamSink>::POSITION pos = m_streams.FrontPosition();
				ComPtrList<IMFStreamSink>::POSITION endPos = m_streams.EndPosition();
				_ComPtr<IMFStreamSink> spStream;

				if (SUCCEEDED(hr))
				{
					for (; pos != endPos; pos = m_streams.Next(pos))
					{
						hr = m_streams.GetItemPos(pos, &spStream);
						DWORD dwId;

						if (FAILED(hr))
						{
							break;
						}

						hr = spStream->GetIdentifier(&dwId);
						if (FAILED(hr) || dwId == dwStreamSinkIdentifier)
						{
							break;
						}
					}

					if (pos == endPos)
					{
						hr = MF_E_INVALIDSTREAMNUMBER;
					}
				}

				if (SUCCEEDED(hr))
				{
					hr = m_streams.Remove(pos, nullptr);
					_ComPtr<ICustomStreamSink> spCustomSink;
#ifdef _WINRT_DLL
					spCustomSink = static_cast<StreamSink*>(spStream.Get());
					hr = S_OK;
#else
					hr = spStream.As(&spCustomSink);
#endif
					if (SUCCEEDED(hr))
						hr = spCustomSink->Shutdown();
				}
				LeaveCriticalSection(&m_critSec);
#ifdef _DEBUG
				Inspection::Trace(_T("MediaSink::RemoveStreamSink: HRESULT=%08lX\n"), hr);
#endif

				return hr;
			}

			HRESULT STDMETHODCALLTYPE GetStreamSinkCount(DWORD *pStreamSinkCount) {
				if (pStreamSinkCount == NULL)
				{
					return E_INVALIDARG;
				}

				EnterCriticalSection(&m_critSec);

				HRESULT hr = CheckShutdown();

				if (SUCCEEDED(hr))
				{
					*pStreamSinkCount = m_streams.GetCount();
				}

				LeaveCriticalSection(&m_critSec);
#ifdef _DEBUG
				Inspection::Trace(_T("MediaSink::GetStreamSinkCount: HRESULT=%08lX\n"), hr);
#endif
				return hr;
			}

			HRESULT STDMETHODCALLTYPE GetStreamSinkByIndex(
				DWORD dwIndex, IMFStreamSink **ppStreamSink) {
				if (ppStreamSink == NULL)
				{
					return E_INVALIDARG;
				}

				_ComPtr<IMFStreamSink> spStream;
				EnterCriticalSection(&m_critSec);
				DWORD cStreams = m_streams.GetCount();

				if (dwIndex >= cStreams)
				{
					return MF_E_INVALIDINDEX;
				}

				HRESULT hr = CheckShutdown();

				if (SUCCEEDED(hr))
				{
					ComPtrList<IMFStreamSink>::POSITION pos = m_streams.FrontPosition();
					ComPtrList<IMFStreamSink>::POSITION endPos = m_streams.EndPosition();
					DWORD dwCurrent = 0;

					for (; pos != endPos && dwCurrent < dwIndex; pos = m_streams.Next(pos), ++dwCurrent)
					{
						// Just move to proper position
					}

					if (pos == endPos)
					{
						hr = MF_E_UNEXPECTED;
					}
					else
					{
						hr = m_streams.GetItemPos(pos, &spStream);
					}
				}

				if (SUCCEEDED(hr))
				{
					*ppStreamSink = spStream.Detach();
				}
				LeaveCriticalSection(&m_critSec);
#ifdef _DEBUG
				Inspection::Trace(_T("MediaSink::GetStreamSinkByIndex: HRESULT=%08lX\n"), hr);
#endif
				return hr;
			}

			HRESULT STDMETHODCALLTYPE GetStreamSinkById(
				DWORD dwStreamSinkIdentifier, IMFStreamSink **ppStreamSink) {
				if (ppStreamSink == NULL)
				{
					return E_INVALIDARG;
				}

				EnterCriticalSection(&m_critSec);
				HRESULT hr = CheckShutdown();
				_ComPtr<IMFStreamSink> spResult;

				if (SUCCEEDED(hr))
				{
					ComPtrList<IMFStreamSink>::POSITION pos = m_streams.FrontPosition();
					ComPtrList<IMFStreamSink>::POSITION endPos = m_streams.EndPosition();

					for (; pos != endPos; pos = m_streams.Next(pos))
					{
						_ComPtr<IMFStreamSink> spStream;
						hr = m_streams.GetItemPos(pos, &spStream);
						DWORD dwId;

						if (FAILED(hr))
						{
							break;
						}

						hr = spStream->GetIdentifier(&dwId);
						if (FAILED(hr))
						{
							break;
						}
						else if (dwId == dwStreamSinkIdentifier)
						{
							spResult = spStream;
							break;
						}
					}

					if (pos == endPos)
					{
						hr = MF_E_INVALIDSTREAMNUMBER;
					}
				}

				if (SUCCEEDED(hr))
				{
					assert(spResult);
					*ppStreamSink = spResult.Detach();
				}
				LeaveCriticalSection(&m_critSec);
#ifdef _DEBUG
				Inspection::Trace(_T("MediaSink::GetStreamSinkById: HRESULT=%08lX\n"), hr);
#endif
				return hr;
			}

			HRESULT STDMETHODCALLTYPE SetPresentationClock(
				IMFPresentationClock *pPresentationClock) {
				EnterCriticalSection(&m_critSec);

				HRESULT hr = CheckShutdown();

				// If we already have a clock, remove ourselves from that clock's
				// state notifications.
				if (SUCCEEDED(hr)) {
					if (m_spClock) {
						hr = m_spClock->RemoveClockStateSink(this);
					}
				}

				// Register ourselves to get state notifications from the new clock.
				if (SUCCEEDED(hr)) {
					if (pPresentationClock) {
						hr = pPresentationClock->AddClockStateSink(this);
					}
				}

				if (SUCCEEDED(hr)) {
					// Release the pointer to the old clock.
					// Store the pointer to the new clock.
					m_spClock = pPresentationClock;
				}
				LeaveCriticalSection(&m_critSec);
#ifdef _DEBUG
				Inspection::Trace(_T("MediaSink::SetPresentationClock: HRESULT=%08lX\n"), hr);
#endif
				return hr;
			}

			HRESULT STDMETHODCALLTYPE GetPresentationClock(
				IMFPresentationClock **ppPresentationClock) {
				if (ppPresentationClock == NULL) {
					return E_INVALIDARG;
				}

				EnterCriticalSection(&m_critSec);

				HRESULT hr = CheckShutdown();

				if (SUCCEEDED(hr)) {
					if (m_spClock == NULL) {
						hr = MF_E_NO_CLOCK; // There is no presentation clock.
					}
					else {
						// Return the pointer to the caller.
						hr = m_spClock.CopyTo(ppPresentationClock);
					}
				}
				LeaveCriticalSection(&m_critSec);
#ifdef _DEBUG
				Inspection::Trace(_T("MediaSink::GetPresentationClock: HRESULT=%08lX\n"), hr);
#endif
				return hr;
			}

			HRESULT STDMETHODCALLTYPE Shutdown(void) {
				EnterCriticalSection(&m_critSec);

				HRESULT hr = CheckShutdown();

				if (SUCCEEDED(hr)) {

					ForEach(m_streams, ShutdownFunc());
					m_streams.Clear();
					m_spClock.ReleaseAndGetAddressOf();

					hr = PropBinder<ICustomSinkCallbackObj>::ClearPropCallback(this, MF_CUSTOMSINK_CALLBACK, IID_ICustomSinkCallback);
					_ComPtr<IMFMediaType> pType;
					hr = CBaseAttributes<>::GetUnknown(MF_MEDIASINK_PREFERREDTYPE, __uuidof(IMFMediaType), (LPVOID*)pType.GetAddressOf());
					if (SUCCEEDED(hr)) {
						hr = DeleteItem(MF_MEDIASINK_PREFERREDTYPE);
					}
					hr = CBaseAttributes<>::GetUnknown(MF_MEDIASINK_AUDIOPREFERREDTYPE, __uuidof(IMFMediaType), (LPVOID*)pType.ReleaseAndGetAddressOf());
					if (SUCCEEDED(hr)) {
						hr = DeleteItem(MF_MEDIASINK_AUDIOPREFERREDTYPE);
					}

					m_IsShutdown = true;
				}

				LeaveCriticalSection(&m_critSec);
#ifdef _DEBUG
				Inspection::Trace(_T("MediaSink::Shutdown: HRESULT=%08lX\n"), hr);
#endif
				return hr;
			}
			class ShutdownFunc
			{
			public:
				HRESULT operator()(IMFStreamSink *pStream) const
				{
					_ComPtr<ICustomStreamSink> spCustomSink;
					HRESULT hr;
#ifdef _WINRT_DLL
					spCustomSink = static_cast<StreamSink*>(pStream);
#else
					hr = pStream->QueryInterface(IID_PPV_ARGS(spCustomSink.GetAddressOf()));
					if (FAILED(hr)) return hr;
#endif
					hr = spCustomSink->Shutdown();
					return hr;
				}
			};

			class StartFunc
			{
			public:
				StartFunc(LONGLONG llStartTime)
					: _llStartTime(llStartTime)
				{
				}

				HRESULT operator()(IMFStreamSink *pStream) const
				{
					_ComPtr<ICustomStreamSink> spCustomSink;
					HRESULT hr;
#ifdef _WINRT_DLL
					spCustomSink = static_cast<StreamSink*>(pStream);
#else
					hr = pStream->QueryInterface(IID_PPV_ARGS(spCustomSink.GetAddressOf()));
					if (FAILED(hr)) return hr;
#endif
					hr = spCustomSink->Start(_llStartTime);
					return hr;
				}

				LONGLONG _llStartTime;
			};

			class StopFunc
			{
			public:
				HRESULT operator()(IMFStreamSink *pStream) const
				{
					_ComPtr<ICustomStreamSink> spCustomSink;
					HRESULT hr;
#ifdef _WINRT_DLL
					spCustomSink = static_cast<StreamSink*>(pStream);
#else
					hr = pStream->QueryInterface(IID_PPV_ARGS(spCustomSink.GetAddressOf()));
					if (FAILED(hr)) return hr;
#endif
					hr = spCustomSink->Stop();
					return hr;
				}
			};

			template <class T, class TFunc>
			HRESULT ForEach(ComPtrList<T> &col, TFunc fn)
			{
				ComPtrList<T>::POSITION pos = col.FrontPosition();
				ComPtrList<T>::POSITION endPos = col.EndPosition();
				HRESULT hr = S_OK;

				for (; pos != endPos; pos = col.Next(pos))
				{
					_ComPtr<T> spStream;

					hr = col.GetItemPos(pos, &spStream);
					if (FAILED(hr))
					{
						break;
					}

					hr = fn(spStream.Get());
				}

				return hr;
			}
			//IMFClockStateSink
			HRESULT STDMETHODCALLTYPE OnClockStart(
				MFTIME hnsSystemTime,
				LONGLONG llClockStartOffset) {
				EnterCriticalSection(&m_critSec);
				HRESULT hr = CheckShutdown();

				if (SUCCEEDED(hr))
				{
					// Start each stream.
					m_llStartTime = llClockStartOffset;
					hr = ForEach(m_streams, StartFunc(llClockStartOffset));
				}
				LeaveCriticalSection(&m_critSec);
#ifdef _DEBUG
				Inspection::Trace(_T("MediaSink::OnClockStart: HRESULT=%08lX\n"), hr);
#endif
				return hr;
			}

			HRESULT STDMETHODCALLTYPE OnClockStop(
				MFTIME hnsSystemTime) {
				EnterCriticalSection(&m_critSec);
				HRESULT hr = CheckShutdown();

				if (SUCCEEDED(hr
					))
				{
					// Stop each stream
					hr = ForEach(m_streams, StopFunc());
				}
				LeaveCriticalSection(&m_critSec);
#ifdef _DEBUG
				Inspection::Trace(_T("MediaSink::OnClockStop: HRESULT=%08lX\n"), hr);
#endif
				return hr;
			}

			HRESULT STDMETHODCALLTYPE OnClockPause(
				MFTIME hnsSystemTime) {
#ifdef _DEBUG
				Inspection::Trace(_T("MediaSink::OnClockPause: HRESULT=%08lX\n"), MF_E_INVALID_STATE_TRANSITION);
#endif
				return MF_E_INVALID_STATE_TRANSITION;
			}

			HRESULT STDMETHODCALLTYPE OnClockRestart(
				MFTIME hnsSystemTime) {
#ifdef _DEBUG
				Inspection::Trace(_T("MediaSink::OnClockRestart: HRESULT=%08lX\n"), MF_E_INVALID_STATE_TRANSITION);
#endif
				return MF_E_INVALID_STATE_TRANSITION;
			}

			HRESULT STDMETHODCALLTYPE OnClockSetRate(
				MFTIME hnsSystemTime,
				float flRate) {
#ifdef _DEBUG
				Inspection::Trace(_T("MediaSink::OnClockSetRate: HRESULT=%08lX\n"), S_OK);
#endif
				return S_OK;
			}
		private:
			CRITICAL_SECTION			m_critSec;
			bool						m_IsShutdown;
			ComPtrList<IMFStreamSink>	m_streams;
			_ComPtr<IMFPresentationClock>	m_spClock;
			LONGLONG						m_llStartTime;
		};
	}
}
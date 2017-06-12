#pragma once
#include <CustomMediaSink.h>
#include <wrl/client.h>
#include <Mfidl.h>

class VideoStreamSink : public IMFStreamSink, public IMFMediaTypeHandler {

public:
	VideoStreamSink() {};//Empty constructor

	VideoStreamSink(DWORD _ID, IMFMediaSink *_mediaSink, IMFMediaType *_mediaType):
		refcount(1)
	{
		ID = _ID;
		mediaSink = _mediaSink;
		mediaType = _mediaType;
		//Create queue
		MFCreateEventQueue(&_eventQueue);
	};

	HRESULT STDMETHODCALLTYPE Flush() override;
	HRESULT STDMETHODCALLTYPE GetIdentifier(DWORD *pdwIdentifier) override;
	HRESULT STDMETHODCALLTYPE GetMediaSink(IMFMediaSink **ppMediaSink) override;
	HRESULT STDMETHODCALLTYPE GetMediaTypeHandler(IMFMediaTypeHandler **ppHandler);
	HRESULT STDMETHODCALLTYPE PlaceMarker(MFSTREAMSINK_MARKER_TYPE eMarkerType,
		const PROPVARIANT *pvarMarkerValue,
		const PROPVARIANT *pvarContextValue) override;
	HRESULT STDMETHODCALLTYPE ProcessSample(IMFSample *pSample) override;
	HRESULT STDMETHODCALLTYPE ShutDown() {
		//TODO

		_eventQueue.Reset();
	
	};

	//IUnknown override methods:
	ULONG STDMETHODCALLTYPE AddRef() override;
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void  **ppvObject) override;
	ULONG STDMETHODCALLTYPE Release() override;

	//IMFMediaEventGenerator override methods:

	HRESULT STDMETHODCALLTYPE BeginGetEvent(IMFAsyncCallback *pCallback, IUnknown *punkState) override;
	HRESULT STDMETHODCALLTYPE EndGetEvent(IMFAsyncResult *pResult, IMFMediaEvent  **ppEvent) override;
	HRESULT STDMETHODCALLTYPE GetEvent(DWORD dwFlags, IMFMediaEvent **ppEvent) override;
	HRESULT STDMETHODCALLTYPE QueueEvent(
		MediaEventType met,
		REFGUID guidExtendedType,
		HRESULT hrStatus,
		const PROPVARIANT *pvValue) override;

	// IMFMediaTypeHandler override methods
	HRESULT STDMETHODCALLTYPE IsMediaTypeSupported(IMFMediaType *pMediaType, IMFMediaType **ppMediaType) override;
	HRESULT STDMETHODCALLTYPE GetMediaTypeCount(DWORD *pdwTypeCount) override;
	HRESULT STDMETHODCALLTYPE GetMediaTypeByIndex(DWORD dwIndex, IMFMediaType **ppType) override;
	HRESULT STDMETHODCALLTYPE SetCurrentMediaType(IMFMediaType *pMediaType) override;
	HRESULT STDMETHODCALLTYPE GetCurrentMediaType(IMFMediaType **ppMediaType) override;
	HRESULT STDMETHODCALLTYPE GetMajorType(GUID *pguidMajorType) override;

private:
	DWORD ID;
	IMFMediaSink *mediaSink;
	bool _isShutDown;
	long refcount;
	bool busy = false;
	Microsoft::WRL::ComPtr<IMFMediaEventQueue> _eventQueue;
	IMFMediaType *mediaType;
	bool _fIsVideo;

	HRESULT IsShutdown() const
	{
		if (_isShutDown)
		{
			return MF_E_SHUTDOWN;
		}
		else
		{
			return S_OK;
		}
	}



};
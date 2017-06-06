#include "pch.h"
#include "VideoStreamSink.h"
#include <chrono>
#include <thread>

HRESULT VideoStreamSink::Flush()
{
	return E_NOTIMPL;
}

HRESULT VideoStreamSink::GetIdentifier(DWORD * pdwIdentifier)
{
	return ID;
}

HRESULT VideoStreamSink::GetMediaSink(IMFMediaSink ** ppMediaSink)
{
	*ppMediaSink = mediaSink;
	return S_OK;
}

HRESULT VideoStreamSink::GetMediaTypeHandler(IMFMediaTypeHandler ** ppHandler)
{

	streamSink->GetMediaTypeHandler(ppHandler);
	return S_OK;
}

HRESULT VideoStreamSink::PlaceMarker(MFSTREAMSINK_MARKER_TYPE eMarkerType, const PROPVARIANT * pvarMarkerValue, const PROPVARIANT * pvarContextValue)
{
	//TODO
	switch (eMarkerType) {
	case MFSTREAMSINK_MARKER_TICK:
		//Error!
		return MF_E_STREAMSINK_REMOVED;
	case MFSTREAMSINK_MARKER_ENDOFSEGMENT:
		//End of stream
		return MF_E_STREAMSINK_REMOVED;
	default:
		return	S_OK;
	}

	return	S_OK;
}

HRESULT VideoStreamSink::ProcessSample(IMFSample * pSample)
{
	OutputDebugString(L"It works!!!!!");
	//TODO
	return S_OK;
}

//IUnknown override methods implemented:
ULONG VideoStreamSink::AddRef()
{
	InterlockedIncrement(&refcount);
	return refcount;
}

HRESULT VideoStreamSink::QueryInterface(REFIID riid, void ** ppvObject)
{
	// Always set out parameter to NULL, validating it first.
	if (!ppvObject)
		return E_INVALIDARG;
	*ppvObject = NULL;
	if (riid == IID_IUnknown || riid == IID_IMFStreamSink)
	{
		// Increment the reference count and return the pointer.
		*ppvObject = (LPVOID)this;
		AddRef();
		return NOERROR;
	}
	return E_NOINTERFACE;
}

ULONG VideoStreamSink::Release()
{
	// Decrement the object's internal counter.
	ULONG ulRefCount = InterlockedDecrement(&refcount);
	if (0 == refcount)
	{
		delete this;
	}
	return ulRefCount;
}

HRESULT VideoStreamSink::BeginGetEvent(IMFAsyncCallback *pCallback, IUnknown *punkState)
{
	return _eventQueue->BeginGetEvent(pCallback, punkState);
}

HRESULT VideoStreamSink::EndGetEvent(IMFAsyncResult * pResult, IMFMediaEvent ** ppEvent)
{
	return _eventQueue->EndGetEvent(pResult, ppEvent);
}

HRESULT VideoStreamSink::GetEvent(DWORD dwFlags, IMFMediaEvent ** ppEvent)
{
	return _eventQueue->GetEvent(dwFlags, ppEvent);
}

HRESULT VideoStreamSink::QueueEvent(MediaEventType met, REFGUID guidExtendedType, HRESULT hrStatus, const PROPVARIANT * pvValue)
{
	_eventQueue->QueueEventParamVar(met, guidExtendedType, hrStatus, pvValue);
}


//

HRESULT VideoStreamSink::IsMediaTypeSupported(IMFMediaType * pMediaType, IMFMediaType ** ppMediaType)
{
	//TODO implement

	return E_NOTIMPL;
}

HRESULT VideoStreamSink::GetMediaTypeCount(DWORD * pdwTypeCount)
{
	//TODO implement
	return E_NOTIMPL;
}

HRESULT VideoStreamSink::GetMediaTypeByIndex(DWORD dwIndex, IMFMediaType ** ppType)
{
	//TODO implement
	return E_NOTIMPL;
}

HRESULT VideoStreamSink::SetCurrentMediaType(IMFMediaType * pMediaType)
{
	//TODO implement
	return E_NOTIMPL;
}

HRESULT VideoStreamSink::GetCurrentMediaType(IMFMediaType ** ppMediaType)
{
	//TODO implement
	return E_NOTIMPL;
}

HRESULT VideoStreamSink::GetMajorType(GUID * pguidMajorType)
{
	//TODO implement
	return E_NOTIMPL;
}







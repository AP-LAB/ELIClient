#include "pch.h"
#include "VideoStreamSink.h"
#include <chrono>
#include <thread>

HRESULT VideoStreamSink::Flush()
{
	//TODO
	return E_NOTIMPL;
}

HRESULT VideoStreamSink::GetIdentifier(DWORD * pdwIdentifier)
{
	*pdwIdentifier = ID;
	return IsShutdown();
}

HRESULT VideoStreamSink::GetMediaSink(IMFMediaSink ** ppMediaSink)
{
	*ppMediaSink = mediaSink;
	return IsShutdown();
}

HRESULT VideoStreamSink::GetMediaTypeHandler(IMFMediaTypeHandler ** ppHandler)
{
	if (ppHandler == NULL)
	{
		return E_INVALIDARG;
	}

	HRESULT hr = IsShutdown();

	// This stream object acts as its own type handler, so we QI ourselves.
	if (SUCCEEDED(hr))
	{
		hr = this->QueryInterface(IID_IMFMediaTypeHandler, (void**)ppHandler);
	}
	return hr;
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
	return InterlockedIncrement(&refcount);
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
	if (pMediaType == nullptr)
	{
		return E_INVALIDARG;
	}

	GUID majorType = GUID_NULL;
	UINT cbSize = 0;

	HRESULT hr = IsShutdown();

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

	if (SUCCEEDED(hr) && mediaType != nullptr)
	{
		GUID guiNewSubtype;
		if (FAILED(pMediaType->GetGUID(MF_MT_SUBTYPE, &guiNewSubtype)))
		{
			hr = MF_E_INVALIDTYPE;
		}
	}

	// We don't return any "close match" types.
	if (ppMediaType)
	{
		*ppMediaType = nullptr;
	}

	return hr;
}

HRESULT VideoStreamSink::GetMediaTypeCount(DWORD * pdwTypeCount)
{
	//TODO implement
	if (pdwTypeCount == NULL)
	{
		return E_INVALIDARG;
	}

	HRESULT hr = IsShutdown();

	if (SUCCEEDED(hr))
	{
		*pdwTypeCount = 1;
	}

	return hr;
}

HRESULT VideoStreamSink::GetMediaTypeByIndex(DWORD dwIndex, IMFMediaType ** ppType)
{
	//TODO implement or check
	if (ppType == nullptr)
	{
		return E_INVALIDARG;
	}


	HRESULT hr = IsShutdown();

	if (dwIndex > 0)
	{
		hr = MF_E_NO_MORE_TYPES;
	}
	else
	{
		*ppType = mediaType;
		if (*ppType != nullptr)
		{
			(*ppType)->AddRef();
		}
	}
	return hr;
}

HRESULT VideoStreamSink::SetCurrentMediaType(IMFMediaType * pMediaType)
{
	HRESULT hr = S_OK;
		if (pMediaType == nullptr)
		{
			hr = E_INVALIDARG;
		}

		if (!SUCCEEDED(hr)) {
			return hr;
		}
			
		hr = (IsMediaTypeSupported(pMediaType, nullptr));
		
		if (!SUCCEEDED(hr)) {
			return hr;
		}

		GUID guiMajorType;
		pMediaType->GetMajorType(&guiMajorType);
		_fIsVideo = (guiMajorType == MFMediaType_Video);

		MFCreateMediaType(&mediaType);
		(pMediaType->CopyAllItems(mediaType));
			
	return hr;
}

HRESULT VideoStreamSink::GetCurrentMediaType(IMFMediaType ** ppMediaType)
{
	*ppMediaType = mediaType;
	return IsShutdown();
}

HRESULT VideoStreamSink::GetMajorType(GUID * pguidMajorType)
{
	if (pguidMajorType == nullptr)
	{
		return E_INVALIDARG;
	}

	if (!mediaType)
	{
		return MF_E_NOT_INITIALIZED;
	}

	*pguidMajorType = (_fIsVideo) ? MFMediaType_Video : MFMediaType_Audio;

	return S_OK;
}







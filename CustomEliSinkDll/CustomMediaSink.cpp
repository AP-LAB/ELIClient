#include "pch.h"
#include "CustomMediaSink.h"


//IMFMediaSink overriden methods:

HRESULT CustomMediaSink::GetCharacteristics(DWORD *pdwCharacteristics) {	
	*pdwCharacteristics = 0;
	//Check if shutdown is toggled?
	return IsShutdown();
}

HRESULT CustomMediaSink::AddStreamSink(DWORD dwStreamSinkIdentifier, IMFMediaType * pMediaType, IMFStreamSink ** ppStreamSink)
{
	HRESULT hr = IsShutdown();

	//Check if streams can be added
	DWORD pdwCharacteristics;
	GetCharacteristics(&pdwCharacteristics);
	if (pdwCharacteristics && MEDIASINK_FIXED_STREAMS) {
		return MF_E_STREAMSINKS_FIXED;
	}

	//Check if stream exists
	IMFStreamSink *pFoundStream;
	GetStreamSinkById(dwStreamSinkIdentifier, &pFoundStream);
	if (!pFoundStream) {
		return MF_E_STREAMSINK_EXISTS;
	}

	VideoStreamSink *newSink = new VideoStreamSink(dwStreamSinkIdentifier, this, pMediaType);
	streams[dwStreamSinkIdentifier] = (newSink);

	//Set the returned sink
	**ppStreamSink = (*newSink);

	return hr;
}

//Return the set presentation clock, if it was set.
//Otherwise, return 
HRESULT CustomMediaSink::GetPresentationClock(IMFPresentationClock ** ppPresentationClock)
{

	HRESULT hr = IsShutdown();

	if (SUCCEEDED(hr))
	{
		if (clock == NULL)
		{
			hr = MF_E_NO_CLOCK; // There is no presentation clock.
		}
		else
		{
			// Return the pointer to the caller.
			*ppPresentationClock = clock.Get();
			(*ppPresentationClock)->AddRef();
		}
	}

	return hr;
}

HRESULT CustomMediaSink::GetStreamSinkById(DWORD dwStreamSinkIdentifier, IMFStreamSink ** ppStreamSink)
{
	auto iterator = streams.find(dwStreamSinkIdentifier);
	if (iterator != streams.end()) {
		*ppStreamSink = iterator->second;
	}		
	return IsShutdown();
}

HRESULT CustomMediaSink::GetStreamSinkByIndex(DWORD dwIndex, IMFStreamSink ** ppStreamSink)
{
	auto iterator = streams.begin();
	std::advance(iterator, dwIndex - 1);
	*ppStreamSink = iterator->second;
	return IsShutdown();
}

HRESULT CustomMediaSink::GetStreamSinkCount(DWORD * pcStreamSinkCount)
{
	*pcStreamSinkCount = streams.size();
	return IsShutdown();
}

HRESULT CustomMediaSink::RemoveStreamSink(DWORD dwStreamSinkIdentifier)
{
	//Check if streams can be added
	DWORD pdwCharacteristics;
	GetCharacteristics(&pdwCharacteristics);
	if (pdwCharacteristics && MEDIASINK_FIXED_STREAMS) {
		return MF_E_STREAMSINKS_FIXED;
	}

	VideoStreamSink *spStream;
	//Search for the stream
	auto iterator = streams.find(dwStreamSinkIdentifier);
	//Flush the sink
	iterator->second->Flush();
	//Remove the sink from the streams map
	streams.erase(iterator);	
	return IsShutdown();
}

//Code from the SimpleConnection Sample
HRESULT CustomMediaSink::SetPresentationClock(IMFPresentationClock * pPresentationClock)
{
	HRESULT hr = IsShutdown();
	// If we already have a clock, remove ourselves from that clock's
	// state notifications.
	if (SUCCEEDED(hr))
	{
		if (clock)
		{
			hr = clock->RemoveClockStateSink(this);
		}
	}

	// Register ourselves to get state notifications from the new clock.
	if (SUCCEEDED(hr))
	{
		if (pPresentationClock)
		{
			hr = pPresentationClock->AddClockStateSink(this);
		}
	}

	if (SUCCEEDED(hr))
	{
		// Release the pointer to the old clock.
		// Store the pointer to the new clock.
		clock = pPresentationClock;
	}

	return hr;
}

HRESULT CustomMediaSink::Shutdown()
{

	//Stop all streams


	clock.Reset();
	_IsShutdown = true;


	return S_OK;
}

//IUnknown override methods implemented:
ULONG CustomMediaSink::AddRef()
{
	InterlockedIncrement(&refcount);
	return refcount;
}

HRESULT CustomMediaSink::QueryInterface(REFIID riid, void ** ppvObject)
{
	// Always set out parameter to NULL, validating it first.
	if (!ppvObject)
		return E_INVALIDARG;
	*ppvObject = NULL;
	if (riid == IID_IUnknown || riid == IID_IMFMediaSink)
	{
		// Increment the reference count and return the pointer.
		*ppvObject = (LPVOID)this;
		AddRef();
		return NOERROR;
	}
	return E_NOINTERFACE;
}

ULONG CustomMediaSink::Release()
{
	// Decrement the object's internal counter.
	ULONG ulRefCount = InterlockedDecrement(&refcount);
	if (0 == refcount)
	{
		delete this;
	}
	return ulRefCount;
}

//IMFClockStateSink  override methods:

HRESULT CustomMediaSink::OnClockPause(MFTIME hnsSystemTime)
{
	//TODO
	return IsShutdown();
}

HRESULT CustomMediaSink::OnClockRestart(MFTIME hnsSystemTime)
{
	return IsShutdown();
}

HRESULT CustomMediaSink::OnClockSetRate(MFTIME hnsSystemTime, float flRate)
{
	return IsShutdown();
}

HRESULT CustomMediaSink::OnClockStart(MFTIME hnsSystemTime, LONGLONG llClockStartOffset)
{
	return IsShutdown();
}

HRESULT CustomMediaSink::OnClockStop(MFTIME hnssSystemTime)
{
	return IsShutdown();
}





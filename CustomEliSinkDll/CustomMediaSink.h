#pragma once
#include "pch.h"
#include <VideoStreamSink.h>
#include <iostream>
#include <map>
#include <list>
#include <wrl/client.h>



class CustomMediaSink : IMFMediaSink, IMFClockStateSink

{
public:

	//IMFMediaSink override methods:
	HRESULT STDMETHODCALLTYPE GetCharacteristics(DWORD *pdwCharacteristics) override;

	HRESULT STDMETHODCALLTYPE AddStreamSink(
		DWORD         dwStreamSinkIdentifier,
		IMFMediaType  *pMediaType,
		IMFStreamSink **ppStreamSink
	) override;

	HRESULT STDMETHODCALLTYPE GetPresentationClock(IMFPresentationClock **ppPresentationClock) override;

	HRESULT STDMETHODCALLTYPE GetStreamSinkById(DWORD dwStreamSinkIdentifier, IMFStreamSink **ppStreamSink) override;

	HRESULT STDMETHODCALLTYPE GetStreamSinkByIndex(DWORD dwIndex, IMFStreamSink **ppStreamSink) override;

	HRESULT STDMETHODCALLTYPE GetStreamSinkCount(DWORD *pcStreamSinkCount) override;

	HRESULT STDMETHODCALLTYPE RemoveStreamSink(DWORD dwStreamSinkIdentifier) override;

	HRESULT STDMETHODCALLTYPE SetPresentationClock(IMFPresentationClock *pPresentationClock) override;

	HRESULT STDMETHODCALLTYPE Shutdown() override;

	HRESULT STDMETHODCALLTYPE IsShutdown() const {
		if (_IsShutdown)
		{
			return MF_E_SHUTDOWN;
		}
		else
		{
			return S_OK;
		}
	}

	//IUnknown override methods:
	ULONG STDMETHODCALLTYPE AddRef() override;
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void  **ppvObject) override;
	ULONG STDMETHODCALLTYPE Release() override;


	//IMFClockStateSink  override methods:
	HRESULT STDMETHODCALLTYPE OnClockPause(MFTIME hnsSystemTime) override;
	HRESULT STDMETHODCALLTYPE OnClockRestart(MFTIME hnsSystemTime) override;
	HRESULT STDMETHODCALLTYPE OnClockSetRate(MFTIME hnsSystemTime, float  flRate) override;
	HRESULT STDMETHODCALLTYPE OnClockStart(MFTIME hnsSystemTime, LONGLONG llClockStartOffset) override;
	HRESULT STDMETHODCALLTYPE OnClockStop(MFTIME hnssSystemTime);

	void ConvertPropertiesToMediaType(ABI::Windows::Media::MediaProperties::IMediaEncodingProperties * mep, IMFMediaType ** ppMT);

	
	private:
		bool _IsShutdown;
		std::map<DWORD, VideoStreamSink*> streams;
		Microsoft::WRL::ComPtr<IMFPresentationClock> clock;
		long refcount;
		
};
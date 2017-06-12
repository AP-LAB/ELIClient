#pragma once

#ifdef _WINRT_DLL
#include "WRLUtilities.h"
#endif

#ifdef _DEBUG
#include <map>
#include <utility>
#ifdef _UNICODE
#define MAKE_MAP(e) std::map<e, std::wstring>
#define MAKE_ENUM(e) std::pair<e, std::wstring>
#define MAKE_ENUM_PAIR(e, str) std::pair<e, std::wstring>(str, L#str)
#else
#define MAKE_MAP(e) std::map<e, std::string>
#define MAKE_ENUM(e) std::pair<e, std::string>
#define MAKE_ENUM_PAIR(e, str) std::pair<e, std::string>(str, #str)
#endif
#endif

#include <mmdeviceapi.h>
#include <mfobjects.h>
#ifdef _WINRT_DLL
#include <wrl\client.h>
#include <windows.foundation.h>
#endif

#ifdef _WINRT_DLL
#ifdef __cplusplus_winrt
namespace TimeStamp {
	namespace WebCamSecUtils {
		public delegate void PropertyChangedHandler();
	}
}
template <class T, typename TWrapper, typename THandler, Windows::Foundation::EventRegistrationToken(*TAddPropFunc)(TWrapper, THandler), void(*TRemovePropFunc)(TWrapper, Windows::Foundation::EventRegistrationToken)>
#else
template <class T, typename TWrapper, typename THandler, HRESULT (STDMETHODCALLTYPE T::*TAddPropName)(__FIEventHandler_1_IInspectable *, EventRegistrationToken *), HRESULT (STDMETHODCALLTYPE T::*TRemovePropName)(EventRegistrationToken)>
#endif
#else
template <class TWrapper, typename THandler>
#endif
class EventBinder
{
public:
	EventBinder()
	{
#ifdef _WINRT_DLL
#ifdef __cplusplus_winrt
		m_token.Value = 0;
#else
		m_token.value = 0;
#endif
#else
		m_bConnected = false;
		m_pHandler = nullptr;
#endif
		m_piCallback = nullptr;
	}
	~EventBinder()
	{
		RemoveNotifyPropChange();
	}
	HRESULT AddNotifyPropChange(TWrapper & iCallback, THandler pHandler)
	{
		HRESULT hr;
		hr = RemoveNotifyPropChange();
		if (iCallback != nullptr) {
#ifdef _WINRT_DLL
#ifdef __cplusplus_winrt
			hr = S_OK;
			m_token = (*TAddPropFunc)(iCallback, pHandler);
#else
			hr = ((*iCallback.Get()).*TAddPropName)(pHandler, &m_token);
#endif
#else
			hr = pHandler->DispEventAdvise(iCallback);
			if (SUCCEEDED(hr)) {
				m_bConnected = true;
				m_pHandler = pHandler;
			}
#endif
			if (SUCCEEDED(hr)) m_piCallback = iCallback;
		}
		return hr;
	}
#ifdef _WINRT_DLL
#ifdef __cplusplus_winrt
	HRESULT RemoveNotifyPropChange()
#else
	HRESULT RemoveNotifyPropChange()
#endif
#else
	HRESULT RemoveNotifyPropChange()
#endif
	{
		HRESULT hr = S_OK;
#ifdef _WINRT_DLL
#ifdef __cplusplus_winrt
		if (m_piCallback && m_token.Value != 0) {
			(*TRemovePropFunc)(m_piCallback, m_token);
		}
		m_token.Value = 0;
#else
		if (m_piCallback && m_token.value != 0) {
			hr = ((*m_piCallback.Get()).*TRemovePropName)(m_token);
		}
		m_token.value = 0;
#endif
#else
		if (m_piCallback && m_bConnected) {
			hr = m_pHandler->DispEventUnadvise(m_piCallback);
		}
		m_bConnected = false;
		m_pHandler = nullptr;
#endif
		m_piCallback = nullptr;
		return hr;
	}
private:
#ifdef _WINRT_DLL
#ifdef __cplusplus_winrt
	Windows::Foundation::EventRegistrationToken m_token;
#else
	EventRegistrationToken m_token;
#endif
#else
	THandler m_pHandler;
	bool m_bConnected;
#endif
	TWrapper m_piCallback;
};
template <typename TWrapper>
class PropBinder
{
public:
	static HRESULT GetPropCallback(IMFAttributes* pAttributes, const GUID & guid, const IID & iid, TWrapper & iCallback)
	{
#ifdef __cplusplus_winrt
		return pAttributes->GetUnknown(guid, iid, reinterpret_cast<LPVOID*>(&iCallback));
#else
		return pAttributes->GetUnknown(guid, iid, (LPVOID*)iCallback.ReleaseAndGetAddressOf());
#endif
	}
	static HRESULT ClearPropCallback(IMFAttributes* pAttributes, const GUID & guid, const IID & iid)
	{
		HRESULT hr;
		TWrapper iCallback;
		hr = GetPropCallback(pAttributes, guid, iid, iCallback);
		if (FAILED(hr)) return hr;
		iCallback = nullptr;
		hr = pAttributes->DeleteItem(guid);
		return hr;
	}
	static HRESULT SetPropCallback(IMFAttributes* pAttributes, const GUID & guid, const IID & iid, IUnknown* spInsp)
	{
		HRESULT hr;
		hr = ClearPropCallback(pAttributes, guid, iid);
		if (spInsp == nullptr) return S_OK;
#ifdef __cplusplus_winrt
		Microsoft::WRL::ComPtr<IUnknown> iCallback;
#else
		TWrapper iCallback;
#endif
		hr = spInsp->QueryInterface(iid, (void**)iCallback.ReleaseAndGetAddressOf());
		if (FAILED(hr)) {
			hr = E_INVALIDARG;
		}
		if (SUCCEEDED(hr)) {
			hr = pAttributes->SetUnknown(guid, iCallback.Get());
		}
		return hr;
	}
};


#ifdef _DEBUG
class Inspection
{
public:
	static void Trace(TCHAR* FormatString, va_list args)
	{
		int iCount = _vsctprintf(FormatString, args) + 1;
		TCHAR* szString = new TCHAR[iCount];
		if (!szString) return;
		if (_vstprintf(szString, iCount, FormatString, args) < 0) return;
		OutputDebugString(szString);
		free(szString);
	}
	static void Trace(TCHAR* FormatString, ...)
	{
		va_list args;
		va_start(args, FormatString);
		Trace(FormatString, args);
		va_end(args);
	}
	static void StringFromGUID(OLECHAR(&szGUID)[sizeof(GUID)* 2 + 4 + 1], GUID guid)
	{
		swprintf_s(szGUID, L"%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X", guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
	}
	static void GUIDFromString(GUID & guid, LPOLESTR szGUID)
	{
		_ASSERTE(swscanf_s(szGUID, L"%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X", &guid.Data1, &guid.Data2, &guid.Data3, &guid.Data4[0], &guid.Data4[1], &guid.Data4[2], &guid.Data4[3], &guid.Data4[4], &guid.Data4[5], &guid.Data4[6], &guid.Data4[7]) == 11);
	}
#ifdef _WINRT_DLL
	static void InspectInterface(IInspectable* pInspectable)
	{
		HRESULT hr;
		ULONG uCount = 0;
		IID* piids = NULL;
		HSTRING rname;
		UINT32 uLen;
		if (SUCCEEDED(hr = pInspectable->GetRuntimeClassName(&rname))) {
			Trace(_T("%s:\n"), WindowsGetStringRawBuffer(rname, &uLen));
			hr = WindowsDeleteString(rname);
		}
		if (SUCCEEDED(hr = pInspectable->GetIids(&uCount, &piids))) {
			ULONG count;
			for (count = 0; count < uCount; count++) {
				//HKEY_CLASSES_ROOT\Interfaces to read interface name and information
				//only accessible to desktop applications however
				OLECHAR szGUID[sizeof(GUID)* 2 + 4 + 1];
				StringFromGUID(szGUID, piids[count]);
				//do not want enclosing braces
				//if (StringFromGUID2(piids[count], szGUID, sizeof(szGUID)))
				Trace(_T("%s\n"), szGUID);
			}
			CoTaskMemFree(piids);
		}
	}
#endif
	static void InspectMediaType(IMFMediaType* pType) {
		UINT32 uiCount;
		UINT32 uiTotal;
		UINT32 uiSubCount;
		GUID guid;
		PROPVARIANT propval;
		HRESULT hr = pType->LockStore();
		if (SUCCEEDED(hr)) {
			hr = pType->GetCount(&uiTotal);
		}
		if (SUCCEEDED(hr)) {
			Trace(_T("Count: %lX\n"), uiTotal);
		}
		for (uiCount = 0; SUCCEEDED(hr) && uiCount < uiTotal; uiCount++) {
			if (SUCCEEDED(pType->GetItemByIndex(uiCount, &guid, &propval))) {
				OLECHAR szGUID[sizeof(GUID)* 2 + 4 + 1];
				StringFromGUID(szGUID, guid);
				Trace(_T(" [%ls]: "), szGUID);
				switch (propval.vt) {
				case MF_ATTRIBUTE_UINT32:
					Trace(_T("%08lX"), propval.ulVal);
					break;
				case MF_ATTRIBUTE_UINT64:
					Trace(_T("%016llX"), propval.uhVal);
					break;
				case MF_ATTRIBUTE_DOUBLE:
					Trace(_T("%f"), propval.dblVal);
					break;
				case MF_ATTRIBUTE_GUID:
					StringFromGUID(szGUID, *propval.puuid);
					Trace(_T("%ls"), szGUID);
					break;
				case MF_ATTRIBUTE_STRING:
					Trace(_T("%ls"), propval.bstrVal);
					break;
				case MF_ATTRIBUTE_BLOB:
					for (uiSubCount = 0; uiSubCount < propval.caub.cElems; uiSubCount++)
						Trace(_T("%02X"), propval.caub.pElems[uiSubCount]);
					break;
				case MF_ATTRIBUTE_IUNKNOWN:
					Trace(_T("%IX"), propval.punkVal);
					break;
				default:
					break;
				}
				Trace(_T("\n"));
				PropVariantClear(&propval);
			}
		}
		if (SUCCEEDED(hr)) {
			hr = pType->UnlockStore();
		}
	}
};
#endif

#ifdef _WINRT_DLL
#include "TimeStamp.h"

#else
//#define nullptr NULL
#include "CritSec.h"

template <class T>
class ComPtr : public CComPtr<T>
{
public:
	ComPtr() throw()
	{
	}
	ComPtr(int nNull) throw() :
		CComPtr<T>((T*)nNull)
	{
	}
	ComPtr(T* lp) throw() :
		CComPtr<T>(lp)

	{
	}
	ComPtr(_In_ const CComPtr<T>& lp) throw() :
		CComPtr<T>(lp.p)
	{
	}
	virtual ~ComPtr() {}

	T* const* GetAddressOf() const throw()
	{
		return &p;
	}

	T** GetAddressOf() throw()
	{
		return &p;
	}

	T** ReleaseAndGetAddressOf() throw()
	{
		InternalRelease();
		return &p;
	}

	T* Get() const throw()
	{
		return p;
	}
	ComPtr& operator=(decltype(__nullptr)) throw()
	{
		InternalRelease();
		return *this;
	}
	ComPtr& operator=(_In_ const int nNull) throw()
	{
		ASSERT(nNull == 0);
		(void)nNull;
		InternalRelease();
		return *this;
	}
	unsigned long Reset()
	{
		return InternalRelease();
	}
	// query for U interface
	template<typename U>
	HRESULT As(_Inout_ U** lp) const throw()
	{
		return p->QueryInterface(__uuidof(U), (void**)lp);
	}
	// query for U interface
	template<typename U>
	HRESULT As(_Out_ ComPtr<U>* lp) const throw()
	{
		return p->QueryInterface(__uuidof(U), reinterpret_cast<void**>(lp->ReleaseAndGetAddressOf()));
	}
private:
	unsigned long InternalRelease() throw()
	{
		unsigned long ref = 0;
		T* temp = p;

		if (temp != nullptr)
		{
			p = nullptr;
			ref = temp->Release();
		}

		return ref;
	}
};

template <UINT nID, class T, const IID* pdiid = &IID_NULL, const GUID* plibid = &GUID_NULL,
	WORD wMajor = 0, WORD wMinor = 0, class tihclass = CComTypeInfoHolder>
class IDispEventFTMImpl
	: public IDispEventImpl <nID, T, pdiid, plibid, wMajor, wMinor, tihclass>
{
public:
	virtual ~IDispEventFTMImpl<nID, T, pdiid, plibid, wMajor, wMinor, tihclass>() {}
	STDMETHOD(_LocDEQueryInterface)(_In_ REFIID riid, _COM_Outptr_ void** ppvObject)
	{
		if (InlineIsEqualGUID(riid, __uuidof(IMarshal)))  {
			HRESULT hr = S_OK;
			if (m_spFTM == nullptr) {
				AutoLock lock(m_critSec);
				if (m_spFTM == nullptr) {
					//IDispEventImpl fakes IUnknown so it can override QueryInterface
					hr = CoCreateFreeThreadedMarshaler((IUnknown*)static_cast<IDispEventImpl<nID, T, pdiid, plibid, wMajor, wMinor, tihclass>*>(this), &m_spFTM);
				}
			}

			if (SUCCEEDED(hr)) {
				if (m_spFTM == nullptr) {
					hr = E_UNEXPECTED;
				}
				else {
					hr = m_spFTM.Get()->QueryInterface(riid, ppvObject);
				}
			}
			return hr;
		}
		return IDispEventImpl<nID, T, pdiid, plibid, wMajor, wMinor, tihclass>::_LocDEQueryInterface(riid, ppvObject);
	}
	HRESULT DispEventUnadvise(IUnknown* pUnk)
	{
		HRESULT hr = IDispEventImpl<nID, T, pdiid, plibid, wMajor, wMinor, tihclass>::DispEventUnadvise(pUnk);
		{
			AutoLock lock(m_critSec);
			m_spFTM = nullptr;
		}
		return hr;
	}
private:
	ComPtr<IUnknown> m_spFTM;
	CritSec m_critSec;
};

#define _ComPtr ComPtr
#include <ppltasks.h>
#define DEFINE_TASK Concurrency::task
#define DEFINE_RET_FORMAL(x) x
#endif
#pragma once

#include <winstring.h>
#include <stdio.h>
#include <tchar.h>
#include <crtdbg.h>
#include <array>
#include <vector>

#include <wrl\implements.h>
#include <wrl\event.h>
#include <inspectable.h>
#ifndef __cplusplus_winrt
#include <windows.foundation.h>

__declspec(noreturn) void __stdcall __abi_WinRTraiseException(long);

inline void __abi_ThrowIfFailed(long __hrArg)
{
	if (__hrArg < 0)
	{
		__abi_WinRTraiseException(__hrArg);
	}
}

struct Guid
{
public:
	Guid();
	Guid(__rcGUID_t);
	operator ::__rcGUID_t();
	bool Equals(Guid __guidArg);
	bool Equals(__rcGUID_t __guidArg);
	Guid(unsigned int __aArg, unsigned short __bArg, unsigned short __cArg, unsigned __int8 __dArg,
		unsigned __int8 __eArg, unsigned __int8 __fArg, unsigned __int8 __gArg, unsigned __int8 __hArg,
		unsigned __int8 __iArg, unsigned __int8 __jArg, unsigned __int8 __kArg);
	Guid(unsigned int __aArg, unsigned short __bArg, unsigned short __cArg, const unsigned __int8* __dArg);
private:
	unsigned long  __a;
	unsigned short __b;
	unsigned short __c;
	unsigned char __d;
	unsigned char __e;
	unsigned char __f;
	unsigned char __g;
	unsigned char __h;
	unsigned char __i;
	unsigned char __j;
	unsigned char __k;
};

static_assert(sizeof(Guid) == sizeof(::_GUID), "Incorect size for Guid");
static_assert(sizeof(__rcGUID_t) == sizeof(::_GUID), "Incorect size for __rcGUID_t");

////////////////////////////////////////////////////////////////////////////////
inline Guid::Guid() : __a(0), __b(0), __c(0), __d(0), __e(0), __f(0), __g(0), __h(0), __i(0), __j(0), __k(0)
{
}

inline Guid::Guid(__rcGUID_t __guid) :
__a(reinterpret_cast<const __s_GUID&>(__guid).Data1),
__b(reinterpret_cast<const __s_GUID&>(__guid).Data2),
__c(reinterpret_cast<const __s_GUID&>(__guid).Data3),
__d(reinterpret_cast<const __s_GUID&>(__guid).Data4[0]),
__e(reinterpret_cast<const __s_GUID&>(__guid).Data4[1]),
__f(reinterpret_cast<const __s_GUID&>(__guid).Data4[2]),
__g(reinterpret_cast<const __s_GUID&>(__guid).Data4[3]),
__h(reinterpret_cast<const __s_GUID&>(__guid).Data4[4]),
__i(reinterpret_cast<const __s_GUID&>(__guid).Data4[5]),
__j(reinterpret_cast<const __s_GUID&>(__guid).Data4[6]),
__k(reinterpret_cast<const __s_GUID&>(__guid).Data4[7])
{
}

inline Guid::operator ::__rcGUID_t()
{
	return reinterpret_cast<__rcGUID_t>(*this);
}

inline bool Guid::Equals(Guid __guidArg)
{
	return *this == __guidArg;
}

inline bool Guid::Equals(__rcGUID_t __guidArg)
{
	return *this == static_cast< Guid>(__guidArg);
}

inline bool operator==(Guid __aArg, Guid __bArg)
{
	auto __a = reinterpret_cast<unsigned long*>(&__aArg);
	auto __b = reinterpret_cast<unsigned long*>(&__bArg);

	return (__a[0] == __b[0] && __a[1] == __b[1] && __a[2] == __b[2] && __a[3] == __b[3]);
}

inline bool operator!=(Guid __aArg, Guid __bArg)
{
	return !(__aArg == __bArg);
}

inline bool operator<(Guid __aArg, Guid __bArg)
{
	auto __a = reinterpret_cast<unsigned long*>(&__aArg);
	auto __b = reinterpret_cast<unsigned long*>(&__bArg);

	if (__a[0] != __b[0])
	{
		return __a[0] < __b[0];
	}

	if (__a[1] != __b[1])
	{
		return __a[1] < __b[1];
	}

	if (__a[2] != __b[2])
	{
		return __a[2] < __b[2];
	}

	if (__a[3] != __b[3])
	{
		return __a[3] < __b[3];
	}

	return false;
}

inline Guid::Guid(unsigned int __aArg, unsigned short __bArg, unsigned short __cArg, unsigned __int8 __dArg,
	unsigned __int8 __eArg, unsigned __int8 __fArg, unsigned __int8 __gArg, unsigned __int8 __hArg,
	unsigned __int8 __iArg, unsigned __int8 __jArg, unsigned __int8 __kArg) :
	__a(__aArg), __b(__bArg), __c(__cArg), __d(__dArg), __e(__eArg), __f(__fArg), __g(__gArg), __h(__hArg), __i(__iArg), __j(__jArg), __k(__kArg)
{
}

inline Guid::Guid(unsigned int __aArg, unsigned short __bArg, unsigned short __cArg, const unsigned __int8 __dArg[8]) :
__a(__aArg), __b(__bArg), __c(__cArg)
{
	__d = __dArg[0];
	__e = __dArg[1];
	__f = __dArg[2];
	__g = __dArg[3];
	__h = __dArg[4];
	__i = __dArg[5];
	__j = __dArg[6];
	__k = __dArg[7];
}

__declspec(selectany) Guid __winrt_GUID_NULL(0x00000000, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);

//
//// Don't want to define the real IUnknown from unknown.h here. That would means if the user has
//// any broken code that uses it, compile errors will take the form of e.g.:
////     predefined C++ WinRT types (compiler internal)(41) : see declaration of 'IUnknown::QueryInterface'
//// This is not helpful. If they use IUnknown, we still need to point them to the actual unknown.h so
//// that they can see the original definition.
////
//// For WinRT, we'll instead have a parallel COM interface hierarchy for basic interfaces starting with _.
//// The type mismatch is not an issue. COM passes types through GUID / void* combos - the original type
//// doesn't come into play unless the user static_casts an implementation type to one of these, but
//// the WinRT implementation types are hidden.
__interface __declspec(uuid("00000000-0000-0000-C000-000000000046")) __abi_IUnknown
{
public:
	virtual long __stdcall __abi_QueryInterface(Guid&, void**) = 0;
	virtual unsigned long __stdcall __abi_AddRef() = 0;
	virtual unsigned long __stdcall __abi_Release() = 0;
};

__declspec(dllexport) __declspec(noreturn) void __stdcall __abi_WinRTraiseNotImplementedException();
__declspec(dllexport) __declspec(noreturn) void __stdcall __abi_WinRTraiseInvalidCastException();
__declspec(dllexport) __declspec(noreturn) void __stdcall __abi_WinRTraiseNullReferenceException();
__declspec(dllexport) __declspec(noreturn) void __stdcall __abi_WinRTraiseOperationCanceledException();
__declspec(dllexport) __declspec(noreturn) void __stdcall __abi_WinRTraiseFailureException();
__declspec(dllexport) __declspec(noreturn) void __stdcall __abi_WinRTraiseAccessDeniedException();
__declspec(dllexport) __declspec(noreturn) void __stdcall __abi_WinRTraiseOutOfMemoryException();
__declspec(dllexport) __declspec(noreturn) void __stdcall __abi_WinRTraiseInvalidArgumentException();
__declspec(dllexport) __declspec(noreturn) void __stdcall __abi_WinRTraiseOutOfBoundsException();
__declspec(dllexport) __declspec(noreturn) void __stdcall __abi_WinRTraiseChangedStateException();
__declspec(dllexport) __declspec(noreturn) void __stdcall __abi_WinRTraiseClassNotRegisteredException();
__declspec(dllexport) __declspec(noreturn) void __stdcall __abi_WinRTraiseWrongThreadException();
__declspec(dllexport) __declspec(noreturn) void __stdcall __abi_WinRTraiseDisconnectedException();
__declspec(dllexport) __declspec(noreturn) void __stdcall __abi_WinRTraiseObjectDisposedException();
__declspec(dllexport) __declspec(noreturn) void __stdcall __abi_WinRTraiseCOMException(long);

__declspec(noreturn) inline void __stdcall __abi_WinRTraiseException(long __hrArg)
{
	switch (__hrArg)
	{
	case 0x80004001L: // E_NOTIMPL
		__abi_WinRTraiseNotImplementedException();

	case 0x80004002L: // E_NOINTERFACE
		__abi_WinRTraiseInvalidCastException();

	case 0x80004003L: // E_POINTER
		__abi_WinRTraiseNullReferenceException();

	case 0x80004004L: // E_ABORT
		__abi_WinRTraiseOperationCanceledException();

	case 0x80004005L: // E_FAIL
		__abi_WinRTraiseFailureException();

	case 0x80070005L: // E_ACCESSDENIED
		__abi_WinRTraiseAccessDeniedException();

	case 0x8007000EL: // E_OUTOFMEMORY
		__abi_WinRTraiseOutOfMemoryException();

	case 0x80070057L: // E_INVALIDARG
		__abi_WinRTraiseInvalidArgumentException();

	case 0x8000000BL: // E_BOUNDS
		__abi_WinRTraiseOutOfBoundsException();

	case 0x8000000CL: // E_CHANGED_STATE
		__abi_WinRTraiseChangedStateException();

	case 0x80040154L: // REGDB_E_CLASSNOTREG
		__abi_WinRTraiseClassNotRegisteredException();

	case 0x8001010EL: // RPC_E_WRONG_THREAD
		__abi_WinRTraiseWrongThreadException();

	case 0x80010108L: // RPC_E_DISCONNECTED
		__abi_WinRTraiseDisconnectedException();

	case 0x80000013L: // RO_E_CLOSED
		__abi_WinRTraiseObjectDisposedException();

	default:
		__abi_WinRTraiseCOMException(__hrArg);
		break;
	}
}

struct __abi_CaptureBase
{
protected:
	virtual __stdcall ~__abi_CaptureBase()  {}

public:
	static const size_t __smallCaptureSize = 4 * sizeof(void*);
	void* operator new(size_t __sizeArg, void* __pSmallCaptureArg)
	{
		if (__sizeArg > __smallCaptureSize)
		{
			return reinterpret_cast<__abi_CaptureBase*>(HeapAlloc(GetProcessHeap(), 0, __sizeArg));
		}

		return __pSmallCaptureArg;
	}

	void operator delete(void* __ptrArg, void* __pSmallCaptureArg)
	{
			__abi_CaptureBase* __pThis = static_cast<__abi_CaptureBase*>(__ptrArg);
			__pThis->Delete(__pThis, __pSmallCaptureArg);
	}

	inline void* GetVFunction(int __slotArg)
	{
		return (*reinterpret_cast<void***>(this))[__slotArg];
	}

	void Delete(__abi_CaptureBase* __pThisArg, void* __pSmallCaptureArg)
	{
		__pThisArg->~__abi_CaptureBase();
		if (__pThisArg != __pSmallCaptureArg)
		{
			HeapFree(GetProcessHeap(), 0, __pThisArg);
		}
	}
};

struct __abi_CapturePtr
{
	char* smallCapture[__abi_CaptureBase::__smallCaptureSize];
	__abi_CaptureBase* ptr;
	__abi_CapturePtr() : ptr(reinterpret_cast<__abi_CaptureBase*>(smallCapture))  {}
	~__abi_CapturePtr()
	{
		ptr->Delete(ptr, smallCapture);
	}
};

template <typename __TFunctor, typename __TReturnType>
struct __abi_FunctorCapture0 : public __abi_CaptureBase
{
	__TFunctor functor;
	__abi_FunctorCapture0(__TFunctor __functor) : functor(__functor) {}
	virtual __TReturnType __stdcall Invoke() { return functor(); }
};
template <typename __TFunctor, typename __TReturnType, typename __TArg0>
struct __abi_FunctorCapture1 : public __abi_CaptureBase
{
	__TFunctor functor;
	__abi_FunctorCapture1(__TFunctor __functor) : functor(__functor) {}
	virtual __TReturnType __stdcall Invoke(__TArg0 __arg0) { return functor(__arg0); }
};
template <typename __TFunctor, typename __TReturnType, typename __TArg0, typename __TArg1>
struct __abi_FunctorCapture2 : public __abi_CaptureBase
{
	__TFunctor functor;
	__abi_FunctorCapture2(__TFunctor __functor) : functor(__functor) {}
	virtual __TReturnType __stdcall Invoke(__TArg0 __arg0, __TArg1 __arg1) { return functor(__arg0, __arg1); }
};
template <typename __TFunctor, typename __TReturnType, typename __TArg0, typename __TArg1, typename __TArg2>
struct __abi_FunctorCapture3 : public __abi_CaptureBase
{
	__TFunctor functor;
	__abi_FunctorCapture3(__TFunctor __functor) : functor(__functor) {}
	virtual __TReturnType __stdcall Invoke(__TArg0 __arg0, __TArg1 __arg1, __TArg2 __arg2) { return functor(__arg0, __arg1, __arg2); }
};
template <typename __TFunctor, typename __TReturnType, typename __TArg0, typename __TArg1, typename __TArg2, typename __TArg3>
struct __abi_FunctorCapture4 : public __abi_CaptureBase
{
	__TFunctor functor;
	__abi_FunctorCapture4(__TFunctor __functor) : functor(__functor) {}
	virtual __TReturnType __stdcall Invoke(__TArg0 __arg0, __TArg1 __arg1, __TArg2 __arg2, __TArg3 __arg3) { return functor(__arg0, __arg1, __arg2, __arg3); }
};
template <typename __TFunctor, typename __TReturnType, typename __TArg0, typename __TArg1, typename __TArg2, typename __TArg3, typename __TArg4>
struct __abi_FunctorCapture5 : public __abi_CaptureBase
{
	__TFunctor functor;
	__abi_FunctorCapture5(__TFunctor __functor) : functor(__functor) {}
	virtual __TReturnType __stdcall Invoke(__TArg0 __arg0, __TArg1 __arg1, __TArg2 __arg2, __TArg3 __arg3, __TArg4 __arg4) { return functor(__arg0, __arg1, __arg2, __arg3, __arg4); }
};
template <typename __TFunctor, typename __TReturnType, typename __TArg0, typename __TArg1, typename __TArg2, typename __TArg3, typename __TArg4, typename __TArg5>
struct __abi_FunctorCapture6 : public __abi_CaptureBase
{
	__TFunctor functor;
	__abi_FunctorCapture6(__TFunctor __functor) : functor(__functor) {}
	virtual __TReturnType __stdcall Invoke(__TArg0 __arg0, __TArg1 __arg1, __TArg2 __arg2, __TArg3 __arg3, __TArg4 __arg4, __TArg5 __arg5) { return functor(__arg0, __arg1, __arg2, __arg3, __arg4, __arg5); }
};
template <typename __TFunctor, typename __TReturnType, typename __TArg0, typename __TArg1, typename __TArg2, typename __TArg3, typename __TArg4, typename __TArg5, typename __TArg6>
struct __abi_FunctorCapture7 : public __abi_CaptureBase
{
	__TFunctor functor;
	__abi_FunctorCapture7(__TFunctor __functor) : functor(__functor) {}
	virtual __TReturnType __stdcall Invoke(__TArg0 __arg0, __TArg1 __arg1, __TArg2 __arg2, __TArg3 __arg3, __TArg4 __arg4, __TArg5 __arg5, __TArg6 __arg6) { return functor(__arg0, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6); }
};
template <typename __TFunctor, typename __TReturnType, typename __TArg0, typename __TArg1, typename __TArg2, typename __TArg3, typename __TArg4, typename __TArg5, typename __TArg6, typename __TArg7>
struct __abi_FunctorCapture8 : public __abi_CaptureBase
{
	__TFunctor functor;
	__abi_FunctorCapture8(__TFunctor __functor) : functor(__functor) {}
	virtual __TReturnType __stdcall Invoke(__TArg0 __arg0, __TArg1 __arg1, __TArg2 __arg2, __TArg3 __arg3, __TArg4 __arg4, __TArg5 __arg5, __TArg6 __arg6, __TArg7 __arg7) { return functor(__arg0, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7); }
};
template <typename __TFunctor, typename __TReturnType, typename __TArg0, typename __TArg1, typename __TArg2, typename __TArg3, typename __TArg4, typename __TArg5, typename __TArg6, typename __TArg7, typename __TArg8>
struct __abi_FunctorCapture9 : public __abi_CaptureBase
{
	__TFunctor functor;
	__abi_FunctorCapture9(__TFunctor __functor) : functor(__functor) {}
	virtual __TReturnType __stdcall Invoke(__TArg0 __arg0, __TArg1 __arg1, __TArg2 __arg2, __TArg3 __arg3, __TArg4 __arg4, __TArg5 __arg5, __TArg6 __arg6, __TArg7 __arg7, __TArg8 __arg8) { return functor(__arg0, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7, __arg8); }
};
template <typename __TFunctor, typename __TReturnType, typename __TArg0, typename __TArg1, typename __TArg2, typename __TArg3, typename __TArg4, typename __TArg5, typename __TArg6, typename __TArg7, typename __TArg8, typename __TArg9>
struct __abi_FunctorCapture10 : public __abi_CaptureBase
{
	__TFunctor functor;
	__abi_FunctorCapture10(__TFunctor __functor) : functor(__functor) {}
	virtual __TReturnType __stdcall Invoke(__TArg0 __arg0, __TArg1 __arg1, __TArg2 __arg2, __TArg3 __arg3, __TArg4 __arg4, __TArg5 __arg5, __TArg6 __arg6, __TArg7 __arg7, __TArg8 __arg8, __TArg9 __arg9) { return functor(__arg0, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7, __arg8, __arg9); }
};

#define __is_winrt_array(type) (type == ABI::Windows::Foundation::PropertyType::PropertyType_UInt8Array || type == ABI::Windows::Foundation::PropertyType::PropertyType_Int16Array ||\
	type == ABI::Windows::Foundation::PropertyType::PropertyType_UInt16Array || type == ABI::Windows::Foundation::PropertyType::PropertyType_Int32Array ||\
	type == ABI::Windows::Foundation::PropertyType::PropertyType_UInt32Array || type == ABI::Windows::Foundation::PropertyType::PropertyType_Int64Array ||\
	type == ABI::Windows::Foundation::PropertyType::PropertyType_UInt64Array || type == ABI::Windows::Foundation::PropertyType::PropertyType_SingleArray ||\
	type == ABI::Windows::Foundation::PropertyType::PropertyType_DoubleArray || type == ABI::Windows::Foundation::PropertyType::PropertyType_Char16Array ||\
	type == ABI::Windows::Foundation::PropertyType::PropertyType_BooleanArray || type == ABI::Windows::Foundation::PropertyType::PropertyType_StringArray ||\
	type == ABI::Windows::Foundation::PropertyType::PropertyType_InspectableArray || type == ABI::Windows::Foundation::PropertyType::PropertyType_DateTimeArray ||\
	type == ABI::Windows::Foundation::PropertyType::PropertyType_TimeSpanArray || type == ABI::Windows::Foundation::PropertyType::PropertyType_GuidArray ||\
	type == ABI::Windows::Foundation::PropertyType::PropertyType_PointArray || type == ABI::Windows::Foundation::PropertyType::PropertyType_SizeArray ||\
	type == ABI::Windows::Foundation::PropertyType::PropertyType_RectArray || type == ABI::Windows::Foundation::PropertyType::PropertyType_OtherTypeArray)

template<typename _Type, bool bUnknown = std::is_base_of<IUnknown, _Type>::value>
struct winrt_type
{
};
template<typename _Type>
struct winrt_type<_Type, true>
{
	static IUnknown* create(_Type* _ObjInCtx) {
		return reinterpret_cast<IUnknown*>(_ObjInCtx);
	}
	static IID getuuid() { return __uuidof(_Type); }
	static const ABI::Windows::Foundation::PropertyType _PropType = ABI::Windows::Foundation::PropertyType::PropertyType_OtherType;
};
template <typename _Type>
struct winrt_type<_Type, false>
{
	static IUnknown* create(_Type* _ObjInCtx) {
		Microsoft::WRL::ComPtr<IInspectable> _PObj;
		Microsoft::WRL::ComPtr<IActivationFactory> objFactory;
		HRESULT hr = Windows::Foundation::GetActivationFactory(Microsoft::WRL::Wrappers::HStringReference(RuntimeClass_Windows_Foundation_PropertyValue).Get(), objFactory.ReleaseAndGetAddressOf());
		if (FAILED(hr)) return nullptr;
		Microsoft::WRL::ComPtr<ABI::Windows::Foundation::IPropertyValueStatics> spPropVal;
		if (SUCCEEDED(hr))
			hr = objFactory.As(&spPropVal);
		if (SUCCEEDED(hr)) {
			hr = winrt_type<_Type>::create(spPropVal.Get(), _ObjInCtx, _PObj.GetAddressOf());
			if (SUCCEEDED(hr))
				return reinterpret_cast<IUnknown*>(_PObj.Detach());
		}
		return nullptr;
	}
	static IID getuuid() { return __uuidof(ABI::Windows::Foundation::IPropertyValue); }
	static const ABI::Windows::Foundation::PropertyType _PropType = ABI::Windows::Foundation::PropertyType::PropertyType_OtherType;
};

template<>
struct winrt_type<void>
{
	static HRESULT create(ABI::Windows::Foundation::IPropertyValueStatics* spPropVal, void* _ObjInCtx, IInspectable** ppInsp) {
		(void)_ObjInCtx;
		return spPropVal->CreateEmpty(ppInsp);
	}
	static const ABI::Windows::Foundation::PropertyType _PropType = ABI::Windows::Foundation::PropertyType::PropertyType_Empty;
};
#define MAKE_TYPE(Type, Name) template<>\
struct winrt_type<Type>\
{\
	static HRESULT create(ABI::Windows::Foundation::IPropertyValueStatics* spPropVal, Type* _ObjInCtx, IInspectable** ppInsp) {\
	return spPropVal->Create##Name(*_ObjInCtx, ppInsp);\
}\
	static const ABI::Windows::Foundation::PropertyType _PropType = ABI::Windows::Foundation::PropertyType::PropertyType_##Name;\
};

template<typename _Type>
struct winrt_array_type
{
	static IUnknown* create(_Type* _ObjInCtx, size_t N) {
		Microsoft::WRL::ComPtr<IInspectable> _PObj;
		Microsoft::WRL::ComPtr<IActivationFactory> objFactory;
		HRESULT hr = Windows::Foundation::GetActivationFactory(Microsoft::WRL::Wrappers::HStringReference(RuntimeClass_Windows_Foundation_PropertyValue).Get(), objFactory.ReleaseAndGetAddressOf());
		if (FAILED(hr)) return nullptr;
		Microsoft::WRL::ComPtr<ABI::Windows::Foundation::IPropertyValueStatics> spPropVal;
		if (SUCCEEDED(hr))
			hr = objFactory.As(&spPropVal);
		if (SUCCEEDED(hr)) {
			hr = winrt_array_type<_Type>::create(spPropVal.Get(), N, _ObjInCtx, _PObj.GetAddressOf());
			if (SUCCEEDED(hr))
				return reinterpret_cast<IUnknown*>(_PObj.Detach());
		}
		return nullptr;
	}
	static const ABI::Windows::Foundation::PropertyType _PropType = ABI::Windows::Foundation::PropertyType::PropertyType_OtherTypeArray;
};
template<int>
struct winrt_prop_type {};

template <>
struct winrt_prop_type<ABI::Windows::Foundation::PropertyType_Empty> {
	typedef void _Type;
};

template <>
struct winrt_prop_type<ABI::Windows::Foundation::PropertyType_OtherType> {
	typedef void _Type;
};

template <>
struct winrt_prop_type<ABI::Windows::Foundation::PropertyType_OtherTypeArray> {
	typedef void _Type;
};

#define MAKE_PROP(Prop, Type) template <>\
struct winrt_prop_type<ABI::Windows::Foundation::PropertyType_##Prop> {\
	typedef Type _Type;\
};

#define MAKE_ARRAY_TYPE(Type, Name) MAKE_PROP(Name, Type)\
	MAKE_PROP(Name##Array, Type*)\
	MAKE_TYPE(Type, Name)\
	template<>\
struct winrt_array_type<Type*>\
{\
	static HRESULT create(ABI::Windows::Foundation::IPropertyValueStatics* spPropVal, UINT32 __valueSize, Type** _ObjInCtx, IInspectable** ppInsp) {\
	return spPropVal->Create##Name##Array(__valueSize, *_ObjInCtx, ppInsp);\
}\
	static const ABI::Windows::Foundation::PropertyType _PropType = ABI::Windows::Foundation::PropertyType::PropertyType_##Name##Array;\
	static std::vector<Type> PropertyValueToVector(ABI::Windows::Foundation::IPropertyValue* propValue)\
{\
	UINT32 uLen = 0;\
	Type* pArray = nullptr;\
	propValue->Get##Name##Array(&uLen, &pArray);\
	return std::vector<Type>(pArray, pArray + uLen);\
}\
};
MAKE_ARRAY_TYPE(BYTE, UInt8)
MAKE_ARRAY_TYPE(INT16, Int16)
MAKE_ARRAY_TYPE(UINT16, UInt16)
MAKE_ARRAY_TYPE(INT32, Int32)
MAKE_ARRAY_TYPE(UINT32, UInt32)
MAKE_ARRAY_TYPE(INT64, Int64)
MAKE_ARRAY_TYPE(UINT64, UInt64)
MAKE_ARRAY_TYPE(FLOAT, Single)
MAKE_ARRAY_TYPE(DOUBLE, Double)
MAKE_ARRAY_TYPE(WCHAR, Char16)
//MAKE_ARRAY_TYPE(boolean, Boolean) //conflict with identical type in C++ of BYTE/UInt8
MAKE_ARRAY_TYPE(HSTRING, String)
MAKE_ARRAY_TYPE(IInspectable*, Inspectable)
MAKE_ARRAY_TYPE(GUID, Guid)
MAKE_ARRAY_TYPE(ABI::Windows::Foundation::DateTime, DateTime)
MAKE_ARRAY_TYPE(ABI::Windows::Foundation::TimeSpan, TimeSpan)
MAKE_ARRAY_TYPE(ABI::Windows::Foundation::Point, Point)
MAKE_ARRAY_TYPE(ABI::Windows::Foundation::Size, Size)
MAKE_ARRAY_TYPE(ABI::Windows::Foundation::Rect, Rect)

template < typename T >
struct DerefHelper
{
	typedef T DerefType;
};

template < typename T >
struct DerefHelper<T*>
{
	typedef T DerefType;
};

#define __is_valid_winrt_type(_Type) (std::is_void<_Type>::value || \
	std::is_same<_Type, BYTE>::value || \
	std::is_same<_Type, INT16>::value || \
	std::is_same<_Type, UINT16>::value || \
	std::is_same<_Type, INT32>::value || \
	std::is_same<_Type, UINT32>::value || \
	std::is_same<_Type, INT64>::value || \
	std::is_same<_Type, UINT64>::value || \
	std::is_same<_Type, FLOAT>::value || \
	std::is_same<_Type, DOUBLE>::value || \
	std::is_same<_Type, WCHAR>::value || \
	std::is_same<_Type, boolean>::value || \
	std::is_same<_Type, HSTRING>::value || \
	std::is_same<_Type, IInspectable *>::value || \
	std::is_base_of<Microsoft::WRL::Details::RuntimeClassBase, _Type>::value || \
	std::is_base_of<IInspectable, typename DerefHelper<_Type>::DerefType>::value || \
	std::is_same<_Type, GUID>::value || \
	std::is_same<_Type, ABI::Windows::Foundation::DateTime>::value || \
	std::is_same<_Type, ABI::Windows::Foundation::TimeSpan>::value || \
	std::is_same<_Type, ABI::Windows::Foundation::Point>::value || \
	std::is_same<_Type, ABI::Windows::Foundation::Size>::value || \
	std::is_same<_Type, ABI::Windows::Foundation::Rect>::value || \
	std::is_same<_Type, BYTE*>::value || \
	std::is_same<_Type, INT16*>::value || \
	std::is_same<_Type, UINT16*>::value || \
	std::is_same<_Type, INT32*>::value || \
	std::is_same<_Type, UINT32*>::value || \
	std::is_same<_Type, INT64*>::value || \
	std::is_same<_Type, UINT64*>::value || \
	std::is_same<_Type, FLOAT*>::value || \
	std::is_same<_Type, DOUBLE*>::value || \
	std::is_same<_Type, WCHAR*>::value || \
	std::is_same<_Type, boolean*>::value || \
	std::is_same<_Type, HSTRING*>::value || \
	std::is_same<_Type, IInspectable **>::value || \
	std::is_same<_Type, GUID*>::value || \
	std::is_same<_Type, ABI::Windows::Foundation::DateTime*>::value || \
	std::is_same<_Type, ABI::Windows::Foundation::TimeSpan*>::value || \
	std::is_same<_Type, ABI::Windows::Foundation::Point*>::value || \
	std::is_same<_Type, ABI::Windows::Foundation::Size*>::value || \
	std::is_same<_Type, ABI::Windows::Foundation::Rect*>::value)

#include <EventToken.h>
#include <windows.foundation.collections.h>

#include <wrl\wrappers\corewrappers.h>
#endif
#include "ppltasks_winrt.h"

#ifdef __cplusplus_winrt
#define _ContextCallback Concurrency::details::_ContextCallback
#define BEGIN_CALL_IN_CONTEXT(hr, var, ...) hr = S_OK;\
	var._CallInContext([__VA_ARGS__]() {
#define END_CALL_IN_CONTEXT(hr) if (FAILED(hr)) throw Platform::Exception::CreateException(hr);\
});
#else
#define _ContextCallback Concurrency_winrt::details::_ContextCallback
#define BEGIN_CALL_IN_CONTEXT(hr, var, ...) hr = var._CallInContext([__VA_ARGS__]() -> HRESULT {
#define END_CALL_IN_CONTEXT(hr) return hr;\
});
#endif
#define GET_CURRENT_CONTEXT _ContextCallback::_CaptureCurrent()
#define SAVE_CURRENT_CONTEXT(var) _ContextCallback var = GET_CURRENT_CONTEXT

template <class T>
class MarshalEventWrapper : public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags< Microsoft::WRL::RuntimeClassType::ClassicCom>, IUnknown>
{
	MixInHelper()
public:
	MarshalEventWrapper(T *handler) {
		ComPtr<IStream> pStream;
		//cannot use CoMarshalInterThreadInterfaceInStream as it only allows one use of the marshalled interface
		HRESULT hr = CreateStreamOnHGlobal(NULL, TRUE, pStream.ReleaseAndGetAddressOf());
		if (FAILED(hr)) return hr;
		hr = CoMarshalInterface(pStream.Get(), __uuidof(*handler), handler, MSHCTX_INPROC, NULL, MSHLFLAGS_TABLESTRONG);
		if (SUCCEEDED(hr)) {
			m_pStream = pStream.Detach();
		}
	}
	virtual ~MarshalEventWrapper()
	{
		LARGE_INTEGER li = { 0 };
		m_pStream->Seek(li, STREAM_SEEK_SET, NULL);
		CoReleaseMarshalData(m_pStream);
		m_pStream->Release();
	}
	_Check_return_ HRESULT Invoke() throw()
	{
		ComPtr<T> handler;
		LARGE_INTEGER li = { 0 };
		HRESULT hr = m_pStream->Seek(li, STREAM_SEEK_SET, NULL);
		if (FAILED(hr)) return hr;
		hr = CoUnmarshalInterface(m_pStream, IID_PPV_ARGS(handler.ReleaseAndGetAddressOf()));
		if (FAILED(hr)) return hr;
		return handler->Invoke();
	}

	template < typename T0 > _Check_return_ HRESULT Invoke(T0 arg0) throw()
	{
		ComPtr<T> handler;
		LARGE_INTEGER li = { 0 };
		HRESULT hr = m_pStream->Seek(li, STREAM_SEEK_SET, NULL);
		if (FAILED(hr)) return hr;
		hr = CoUnmarshalInterface(m_pStream, IID_PPV_ARGS(handler.ReleaseAndGetAddressOf()));
		if (FAILED(hr)) return hr;
		return handler->Invoke(arg0);
	}

	template < typename T0, typename T1 > _Check_return_ HRESULT Invoke(T0 arg0, T1 arg1) throw()
	{
		ComPtr<T> handler;
		LARGE_INTEGER li = { 0 };
		HRESULT hr = m_pStream->Seek(li, STREAM_SEEK_SET, NULL);
		if (FAILED(hr)) return hr;
		hr = CoUnmarshalInterface(m_pStream, IID_PPV_ARGS(handler.ReleaseAndGetAddressOf()));
		if (FAILED(hr)) return hr;
		return handler->Invoke(arg0, arg1);
	}

	template < typename T0, typename T1, typename T2 > _Check_return_ HRESULT Invoke(T0 arg0, T1 arg1, T2 arg2) throw()
	{
		ComPtr<T> handler;
		LARGE_INTEGER li = { 0 };
		HRESULT hr = m_pStream->Seek(li, STREAM_SEEK_SET, NULL);
		if (FAILED(hr)) return hr;
		hr = CoUnmarshalInterface(m_pStream, IID_PPV_ARGS(handler.ReleaseAndGetAddressOf()));
		if (FAILED(hr)) return hr;
		return handler->Invoke(arg0, arg1, arg2);
	}

	template < typename T0, typename T1, typename T2, typename T3> _Check_return_ HRESULT Invoke(T0 arg0, T1 arg1, T2 arg2, T3 arg3) throw()
	{
		ComPtr<T> handler;
		LARGE_INTEGER li = { 0 };
		HRESULT hr = m_pStream->Seek(li, STREAM_SEEK_SET, NULL);
		if (FAILED(hr)) return hr;
		hr = CoUnmarshalInterface(m_pStream, IID_PPV_ARGS(handler.ReleaseAndGetAddressOf()));
		if (FAILED(hr)) return hr;
		return handler->Invoke(arg0, arg1, arg2, arg3);
	}

	template < typename T0, typename T1, typename T2, typename T3, typename T4>
	_Check_return_ HRESULT Invoke(T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4) throw()
	{
		ComPtr<T> handler;
		LARGE_INTEGER li = { 0 };
		HRESULT hr = m_pStream->Seek(li, STREAM_SEEK_SET, NULL);
		if (FAILED(hr)) return hr;
		hr = CoUnmarshalInterface(m_pStream, IID_PPV_ARGS(handler.ReleaseAndGetAddressOf()));
		if (FAILED(hr)) return hr;
		return handler->Invoke(arg0, arg1, arg2, arg3, arg4);
	}

	template < typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
	_Check_return_ HRESULT Invoke(T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5) throw()
	{
		ComPtr<T> handler;
		LARGE_INTEGER li = { 0 };
		HRESULT hr = m_pStream->Seek(li, STREAM_SEEK_SET, NULL);
		if (FAILED(hr)) return hr;
		hr = CoUnmarshalInterface(m_pStream, IID_PPV_ARGS(handler.ReleaseAndGetAddressOf()));
		if (FAILED(hr)) return hr;
		return handler->Invoke(arg0, arg1, arg2, arg3, arg4, arg5);
	}

	template < typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	_Check_return_ HRESULT Invoke(T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6) throw()
	{
		ComPtr<T> handler;
		LARGE_INTEGER li = { 0 };
		HRESULT hr = m_pStream->Seek(li, STREAM_SEEK_SET, NULL);
		if (FAILED(hr)) return hr;
		hr = CoUnmarshalInterface(m_pStream, IID_PPV_ARGS(handler.ReleaseAndGetAddressOf()));
		if (FAILED(hr)) return hr;
		return handler->Invoke(arg0, arg1, arg2, arg3, arg4, arg5, arg6);
	}

	template < typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
	_Check_return_ HRESULT Invoke(T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7) throw()
	{
		ComPtr<T> handler;
		LARGE_INTEGER li = { 0 };
		HRESULT hr = m_pStream->Seek(li, STREAM_SEEK_SET, NULL);
		if (FAILED(hr)) return hr;
		hr = CoUnmarshalInterface(m_pStream, IID_PPV_ARGS(handler.ReleaseAndGetAddressOf()));
		if (FAILED(hr)) return hr;
		return handler->Invoke(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
	}

	template < typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
	_Check_return_ HRESULT Invoke(T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8) throw()
	{
		ComPtr<T> handler;
		LARGE_INTEGER li = { 0 };
		HRESULT hr = m_pStream->Seek(li, STREAM_SEEK_SET, NULL);
		if (FAILED(hr)) return hr;
		hr = CoUnmarshalInterface(m_pStream, IID_PPV_ARGS(handler.ReleaseAndGetAddressOf()));
		if (FAILED(hr)) return hr;
		return handler->Invoke(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
	}

	template < typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9 >
	_Check_return_ HRESULT Invoke(T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9) throw()
	{
		ComPtr<T> handler;
		LARGE_INTEGER li = { 0 };
		HRESULT hr = m_pStream->Seek(li, STREAM_SEEK_SET, NULL);
		if (FAILED(hr)) return hr;
		hr = CoUnmarshalInterface(m_pStream, IID_PPV_ARGS(handler.ReleaseAndGetAddressOf()));
		if (FAILED(hr)) return hr;
		return handler->Invoke(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
	}
private:
	LPSTREAM m_pStream;
};

template <class T>
class GITEventWrapper : public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags< Microsoft::WRL::RuntimeClassType::ClassicCom>, IUnknown>
{
	MixInHelper()
public:
	GITEventWrapper(T *handler) {
		FtmBase::CreateGlobalInterfaceTable(m_git.ReleaseAndGetAddressOf());
		HRESULT hr = m_git->RegisterInterfaceInGlobal(handler, __uuidof(*handler), (DWORD*)&m_dwCookie);
	}
	virtual ~GITEventWrapper()
	{
		m_git->RevokeInterfaceFromGlobal(m_dwCookie);
	}
	_Check_return_ HRESULT Invoke() throw()
	{
		ComPtr<T> handler;
		HRESULT hr = m_git->GetInterfaceFromGlobal(m_dwCookie, IID_PPV_ARGS(handler.ReleaseAndGetAddressOf()));
		if (FAILED(hr)) return hr;
		return handler->Invoke();
	}

	template < typename T0 > _Check_return_ HRESULT Invoke(T0 arg0) throw()
	{
		ComPtr<T> handler;
		HRESULT hr = m_git->GetInterfaceFromGlobal(m_dwCookie, IID_PPV_ARGS(handler.ReleaseAndGetAddressOf()));
		if (FAILED(hr)) return hr;
		return handler->Invoke(arg0);
	}

	template < typename T0, typename T1 > _Check_return_ HRESULT Invoke(T0 arg0, T1 arg1) throw()
	{
		ComPtr<T> handler;
		HRESULT hr = m_git->GetInterfaceFromGlobal(m_dwCookie, IID_PPV_ARGS(handler.ReleaseAndGetAddressOf()));
		if (FAILED(hr)) return hr;
		return handler->Invoke(arg0, arg1);
	}

	template < typename T0, typename T1, typename T2 > _Check_return_ HRESULT Invoke(T0 arg0, T1 arg1, T2 arg2) throw()
	{
		ComPtr<T> handler;
		HRESULT hr = m_git->GetInterfaceFromGlobal(m_dwCookie, IID_PPV_ARGS(handler.ReleaseAndGetAddressOf()));
		if (FAILED(hr)) return hr;
		return handler->Invoke(arg0, arg1, arg2);
	}

	template < typename T0, typename T1, typename T2, typename T3> _Check_return_ HRESULT Invoke(T0 arg0, T1 arg1, T2 arg2, T3 arg3) throw()
	{
		ComPtr<T> handler;
		HRESULT hr = m_git->GetInterfaceFromGlobal(m_dwCookie, IID_PPV_ARGS(handler.ReleaseAndGetAddressOf()));
		if (FAILED(hr)) return hr;
		return handler->Invoke(arg0, arg1, arg2, arg3);
	}

	template < typename T0, typename T1, typename T2, typename T3, typename T4>
	_Check_return_ HRESULT Invoke(T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4) throw()
	{
		ComPtr<T> handler;
		HRESULT hr = m_git->GetInterfaceFromGlobal(m_dwCookie, IID_PPV_ARGS(handler.ReleaseAndGetAddressOf()));
		if (FAILED(hr)) return hr;
		return handler->Invoke(arg0, arg1, arg2, arg3, arg4);
	}

	template < typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
	_Check_return_ HRESULT Invoke(T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5) throw()
	{
		ComPtr<T> handler;
		HRESULT hr = m_git->GetInterfaceFromGlobal(m_dwCookie, IID_PPV_ARGS(handler.ReleaseAndGetAddressOf()));
		if (FAILED(hr)) return hr;
		return handler->Invoke(arg0, arg1, arg2, arg3, arg4, arg5);
	}

	template < typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	_Check_return_ HRESULT Invoke(T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6) throw()
	{
		ComPtr<T> handler;
		HRESULT hr = m_git->GetInterfaceFromGlobal(m_dwCookie, IID_PPV_ARGS(handler.ReleaseAndGetAddressOf()));
		if (FAILED(hr)) return hr;
		return handler->Invoke(arg0, arg1, arg2, arg3, arg4, arg5, arg6);
	}

	template < typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
	_Check_return_ HRESULT Invoke(T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7) throw()
	{
		ComPtr<T> handler;
		HRESULT hr = m_git->GetInterfaceFromGlobal(m_dwCookie, IID_PPV_ARGS(handler.ReleaseAndGetAddressOf()));
		if (FAILED(hr)) return hr;
		return handler->Invoke(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
	}

	template < typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
	_Check_return_ HRESULT Invoke(T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8) throw()
	{
		ComPtr<T> handler;
		HRESULT hr = m_git->GetInterfaceFromGlobal(m_dwCookie, IID_PPV_ARGS(handler.ReleaseAndGetAddressOf()));
		if (FAILED(hr)) return hr;
		return handler->Invoke(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
	}

	template < typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9 >
	_Check_return_ HRESULT Invoke(T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9) throw()
	{
		ComPtr<T> handler;
		HRESULT hr = m_git->GetInterfaceFromGlobal(m_dwCookie, IID_PPV_ARGS(handler.ReleaseAndGetAddressOf()));
		if (FAILED(hr)) return hr;
		return handler->Invoke(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
	}
private:
	Microsoft::WRL::ComPtr<IGlobalInterfaceTable> m_git;
	DWORD m_dwCookie;
};

template <class T>
class ContextEventWrapper : public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags< Microsoft::WRL::RuntimeClassType::ClassicCom>, IUnknown>
{
	MixInHelper();
public:
	ContextEventWrapper(T *handler) {
		m_callback = _ContextCallback::_CaptureCurrent();
		m_handler = handler;
	}
	virtual ~ContextEventWrapper()
	{
	}
	_Check_return_ HRESULT Invoke() throw()
	{
		Microsoft::WRL::ComPtr<T> handler = m_handler;
		HRESULT hr;
		BEGIN_CALL_IN_CONTEXT(hr, m_callback, handler)
			HRESULT hr = handler->Invoke();
		END_CALL_IN_CONTEXT(hr)
		return hr;
	}

	template < typename T0 > _Check_return_ HRESULT Invoke(T0 arg0) throw()
	{
		Microsoft::WRL::ComPtr<T> handler = m_handler;
		HRESULT hr;
		BEGIN_CALL_IN_CONTEXT(hr, m_callback, handler, arg0)
			HRESULT hr = handler->Invoke(arg0);
		END_CALL_IN_CONTEXT(hr)
		return hr;
	}

	template < typename T0, typename T1 > _Check_return_ HRESULT Invoke(T0 arg0, T1 arg1) throw()
	{
		Microsoft::WRL::ComPtr<T> handler = m_handler;
		HRESULT hr;
		BEGIN_CALL_IN_CONTEXT(hr, m_callback, handler, arg0, arg1)
			HRESULT hr = handler->Invoke(arg0, arg1);
		END_CALL_IN_CONTEXT(hr)
		return hr;
	}

	template < typename T0, typename T1, typename T2 > _Check_return_ HRESULT Invoke(T0 arg0, T1 arg1, T2 arg2) throw()
	{
		Microsoft::WRL::ComPtr<T> handler = m_handler;
		HRESULT hr;
		BEGIN_CALL_IN_CONTEXT(hr, m_callback, handler, arg0, arg1, arg2)
			HRESULT hr = handler->Invoke(arg0, arg1, arg2);
		END_CALL_IN_CONTEXT(hr)
		return hr;
	}

	template < typename T0, typename T1, typename T2, typename T3> _Check_return_ HRESULT Invoke(T0 arg0, T1 arg1, T2 arg2, T3 arg3) throw()
	{
		Microsoft::WRL::ComPtr<T> handler = m_handler;
		HRESULT hr;
		BEGIN_CALL_IN_CONTEXT(hr, m_callback, handler, arg0, arg1, arg2, arg3)
			HRESULT hr = handler->Invoke(arg0, arg1, arg2, arg3);
		END_CALL_IN_CONTEXT(hr)
		return hr;
	}

	template < typename T0, typename T1, typename T2, typename T3, typename T4>
	_Check_return_ HRESULT Invoke(T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4) throw()
	{
		Microsoft::WRL::ComPtr<T> handler = m_handler;
		HRESULT hr;
		BEGIN_CALL_IN_CONTEXT(hr, m_callback, handler, arg0, arg1, arg2, arg3, arg4)
			HRESULT hr = handler->Invoke(arg0, arg1, arg2, arg3, arg4);
		END_CALL_IN_CONTEXT(hr)
		return hr;
	}

	template < typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
	_Check_return_ HRESULT Invoke(T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5) throw()
	{
		Microsoft::WRL::ComPtr<T> handler = m_handler;
		HRESULT hr;
		BEGIN_CALL_IN_CONTEXT(hr, m_callback, handler, arg0, arg1, arg2, arg3, arg4, arg5)
			HRESULT hr = handler->Invoke(arg0, arg1, arg2, arg3, arg4, arg5);
		END_CALL_IN_CONTEXT(hr)
		return hr;
	}

	template < typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	_Check_return_ HRESULT Invoke(T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6) throw()
	{
		Microsoft::WRL::ComPtr<T> handler = m_handler;
		HRESULT hr;
		BEGIN_CALL_IN_CONTEXT(hr, m_callback, handler, arg0, arg1, arg2, arg3, arg4, arg5, arg6)
			HRESULT hr = handler->Invoke(arg0, arg1, arg2, arg3, arg4, arg5, arg6);
		END_CALL_IN_CONTEXT(hr)
		return hr;
	}

	template < typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
	_Check_return_ HRESULT Invoke(T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7) throw()
	{
		Microsoft::WRL::ComPtr<T> handler = m_handler;
		HRESULT hr;
		BEGIN_CALL_IN_CONTEXT(hr, m_callback, handler, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7)
			HRESULT hr = handler->Invoke(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
		END_CALL_IN_CONTEXT(hr)
		return hr;
	}

	template < typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
	_Check_return_ HRESULT Invoke(T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8) throw()
	{
		Microsoft::WRL::ComPtr<T> handler = m_handler;
		HRESULT hr;
		BEGIN_CALL_IN_CONTEXT(hr, m_callback, handler, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8)
			HRESULT hr = handler->Invoke(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
		END_CALL_IN_CONTEXT(hr)
		return hr;
	}

	template < typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9 >
	_Check_return_ HRESULT Invoke(T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9) throw()
	{
		Microsoft::WRL::ComPtr<T> handler = m_handler;
		HRESULT hr;
		BEGIN_CALL_IN_CONTEXT(hr, m_callback, handler, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9)
			HRESULT hr = handler->Invoke(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
		END_CALL_IN_CONTEXT(hr)
		return hr;
	}
private:
	_ContextCallback m_callback;
	Microsoft::WRL::ComPtr<T> m_handler;
};

#define COMMA ,

#ifdef __cplusplus_winrt
#define _Object Platform::Object^
#define _ObjectObj Platform::Object^
#define _String Platform::String^
#define _StringObj Platform::String^
#define _StringReference ref new Platform::String
#define _StringReferenceObj Platform::String^
#define _DeviceInformationCollection Windows::Devices::Enumeration::DeviceInformationCollection
#define _MediaCapture Windows::Media::Capture::MediaCapture
#define _MediaCaptureVideoPreview Windows::Media::Capture::MediaCapture
#define _MediaCaptureInitializationSettings Windows::Media::Capture::MediaCaptureInitializationSettings
#define _VideoDeviceController Windows::Media::Devices::VideoDeviceController
#define _MediaDeviceController Windows::Media::Devices::VideoDeviceController
#define _MediaEncodingProperties Windows::Media::MediaProperties::IMediaEncodingProperties
#define _VideoEncodingProperties Windows::Media::MediaProperties::VideoEncodingProperties
#define _MediaStreamType Windows::Media::Capture::MediaStreamType
#define _AsyncInfo Windows::Foundation::IAsyncInfo
#define _AsyncAction Windows::Foundation::IAsyncAction
#define _AsyncOperation Windows::Foundation::IAsyncOperation
#define _AsyncActionWithProgress Windows::Foundation::IAsyncActionWithProgress
#define _AsyncOperationWithProgress Windows::Foundation::IAsyncOperationWithProgress
#define _AsyncActionProgressHandler Windows::Foundation::AsyncActionProgressHandler
#define _AsyncOperationProgressHandler Windows::Foundation::AsyncOperationProgressHandler
#define _DateTime Windows::Foundation::DateTime
#define _FileIO Windows::Storage::FileIO
#define _ReadTextWithEncodingAsync ReadTextAsync
#define _StorageFile Windows::Storage::StorageFile
#define _StorageFileClass Windows::Storage::StorageFile
#define _StorageFolder Windows::Storage::StorageFolder
#define _CreationCollisionOption Windows::Storage::CreationCollisionOption
#define _UnicodeEncoding Windows::Storage::Streams::UnicodeEncoding
#define _ApplicationData Windows::Storage::ApplicationData
#define _ApplicationDataStatics Windows::Storage::ApplicationData
#define _KnownFoldersStatics Windows::Storage::KnownFolders
#define _StorageDeleteOption Windows::Storage::StorageDeleteOption
#define _StorageItem Windows::Storage::StorageFile
#define _SyndicationClient Windows::Web::Syndication::SyndicationClient
#define _SyndicationFeed Windows::Web::Syndication::SyndicationFeed
#define _ISyndicationFeed Windows::Web::Syndication::SyndicationFeed
#define _SyndicationItem Windows::Web::Syndication::SyndicationItem
#define _ISyndicationItem Windows::Web::Syndication::SyndicationItem
#define _SyndicationContent Windows::Web::Syndication::SyndicationContent
#define _SyndicationLink Windows::Web::Syndication::SyndicationLink
#define _ISyndicationLink Windows::Web::Syndication::SyndicationLink
#define _SyndicationPerson Windows::Web::Syndication::SyndicationPerson
#define _ISyndicationPerson Windows::Web::Syndication::SyndicationPerson
#define _SyndicationText Windows::Web::Syndication::ISyndicationText
#define _SyndicationFormat Windows::Web::Syndication::SyndicationFormat
#define _RetrievalProgress Windows::Web::Syndication::RetrievalProgress
#define _ErrorInfo Platform::Exception
#define _IVector Windows::Foundation::Collections::IVector
#define _Uri Windows::Foundation::Uri
#define _CreateUri(Url, UrlObj) UrlObj = ref new _Uri(Url);
#define _Vector Platform::Collections::Vector
#define _DeviceClass Windows::Devices::Enumeration::DeviceClass
#define _IDeviceInformation Windows::Devices::Enumeration::DeviceInformation
#define _DeviceInformation Windows::Devices::Enumeration::DeviceInformation
#define _DeviceInformationStatics Windows::Devices::Enumeration::DeviceInformation
#define _MediaEncodingProfile Windows::Media::MediaProperties::MediaEncodingProfile
#define _StreamingCaptureMode Windows::Media::Capture::StreamingCaptureMode
#define _PropertySet Windows::Foundation::Collections::PropertySet
#define _Map Windows::Foundation::Collections::PropertySet
#define _PropertyValueStatics Windows::Foundation::PropertyValue
#define _VectorView Windows::Foundation::Collections::IVectorView
#define _StartPreviewToCustomSinkIdAsync StartPreviewToCustomSinkAsync
#define _InitializeWithSettingsAsync InitializeAsync
#define _FindAllAsyncDeviceClass FindAllAsync
#define _MediaExtension Windows::Media::IMediaExtension
#define BEGIN_CREATE_ASYNC(type, ...) (Concurrency::create_async([__VA_ARGS__]() {
#define END_CREATE_ASYNC(hr) if (FAILED(hr)) throw Platform::Exception::CreateException(hr);\
}))
#define DEFINE_TASK Concurrency::task
#define CREATE_TASK Concurrency::create_task
#define CREATE_AUDIO_TASK Concurrency::create_audio_task
#define CREATE_OR_CONTINUE_TASK(_task, rettype, func) _task = (_task == Concurrency::task<rettype>()) ? Concurrency::create_task(func) : _task.then([func](rettype) -> rettype { return func(); });
#define DEFINE_RET_VAL(x)
#define DEFINE_RET_TYPE(x)
#define DEFINE_RET_FORMAL(x) x
#define RET_VAL(x) return x;
#define RET_VAL_BASE
#define MAKE_STRING(str) str
#define GET_STL_STRING(str) std::wstring(str->Data())
#define GET_STL_STRING_RAW(str) std::wstring(str->Data())
#define MAKE_WRL_OBJ(x) x^
#define MAKE_WRL_REF(x) x^
#define MAKE_OBJ_REF(x) x^
#define MAKE_WRL_AGILE_REF(x) Platform::Agile<x^>
#define MAKE_PROPERTY_BACKING(Type, PropName) property Type PropName;
#define MAKE_PROPERTY(Type, PropName, PropValue)
#define MAKE_PROPERTY_STRING(Type, PropName, PropValue)
#define MAKE_READONLY_PROPERTY(Type, PropName, PropValue) property Type PropName\
{\
	Type get() { return PropValue; }\
}
#define THROW_INVALID_ARG throw ref new Platform::InvalidArgumentException();
#define RELEASE_AGILE_WRL(x) x = nullptr;
#define RELEASE_WRL(x) x = nullptr;
#define GET_WRL_OBJ_FROM_REF(objtype, obj, orig, hr) objtype^ obj = orig;\
	hr = S_OK;
#define GET_WRL_OBJ_FROM_OBJ(objtype, obj, orig, hr) objtype^ obj = safe_cast<objtype^>(orig);\
	hr = S_OK;
#define WRL_ENUM_GET(obj, prefix, prop) obj::##prop
#define WRL_PROP_GET(obj, prop, arg, hr) arg = obj->##prop;\
	hr = S_OK;
#define WRL_PROP_PUT(obj, prop, arg, hr) obj->##prop = arg;\
	hr = S_OK;
#define WRL_METHOD_BASE(obj, method, ret, hr) ret = obj->##method();\
	hr = S_OK;
#define WRL_METHOD(obj, method, ret, hr, ...) ret = obj->##method(__VA_ARGS__);\
	hr = S_OK;
#define WRL_METHOD_NORET_BASE(obj, method, hr) obj->##method();\
	hr = S_OK;
#define WRL_METHOD_NORET(obj, method, hr, ...) obj->##method(__VA_ARGS__);\
	hr = S_OK;
#define REF_WRL_OBJ(obj) &obj
#define DEREF_WRL_OBJ(obj) obj
#define DEREF_AGILE_WRL_OBJ(obj) obj.Get()
#define DEREF_AS_NATIVE_WRL_OBJ(type, obj) reinterpret_cast<type*>(obj)
#define PREPARE_TRANSFER_WRL_OBJ(obj) obj
#define ACTIVATE_LOCAL_OBJ_BASE(objtype) ref new objtype()
#define ACTIVATE_LOCAL_OBJ(objtype, ...) ref new objtype(__VA_ARGS__)
#define ACTIVATE_EVENT_HANDLER(objtype, ...) ref new objtype(__VA_ARGS__)
#define ACTIVATE_OBJ(rtclass, objtype, obj, hr) MAKE_WRL_OBJ(objtype) obj = ref new objtype();\
hr = S_OK;
#define ACTIVATE_STATIC_OBJ(rtclass, objtype, obj, hr) objtype obj;\
hr = S_OK;
#else

#define _Object IInspectable*
#define _ObjectObj Microsoft::WRL::ComPtr<IInspectable>
#define _String HSTRING
#define _StringObj Microsoft::WRL::Wrappers::HString
#define _StringReference Microsoft::WRL::Wrappers::HStringReference
#define _StringReferenceObj Microsoft::WRL::Wrappers::HStringReference
#define _DeviceInformationCollection ABI::Windows::Devices::Enumeration::DeviceInformationCollection
#define _MediaCapture ABI::Windows::Media::Capture::IMediaCapture
#define _MediaCaptureVideoPreview ABI::Windows::Media::Capture::IMediaCaptureVideoPreview
#define _MediaCaptureInitializationSettings ABI::Windows::Media::Capture::IMediaCaptureInitializationSettings
#define _VideoDeviceController ABI::Windows::Media::Devices::IVideoDeviceController
#define _MediaDeviceController ABI::Windows::Media::Devices::IMediaDeviceController
#define _MediaEncodingProperties ABI::Windows::Media::MediaProperties::IMediaEncodingProperties
#define _VideoEncodingProperties ABI::Windows::Media::MediaProperties::IVideoEncodingProperties
#define _MediaStreamType ABI::Windows::Media::Capture::MediaStreamType
#define _AsyncInfo ABI::Windows::Foundation::IAsyncInfo
#define _AsyncAction ABI::Windows::Foundation::IAsyncAction
#define _AsyncOperation ABI::Windows::Foundation::IAsyncOperation
#define _AsyncActionWithProgress ABI::Windows::Foundation::IAsyncActionWithProgress
#define _AsyncOperationWithProgress ABI::Windows::Foundation::IAsyncOperationWithProgress
#define _AsyncActionProgressHandler ABI::Windows::Foundation::IAsyncActionProgressHandler
#define _AsyncOperationProgressHandler ABI::Windows::Foundation::IAsyncOperationProgressHandler
#define _DateTime ABI::Windows::Foundation::DateTime
#define _FileIO ABI::Windows::Storage::IFileIOStatics
#define _ReadTextWithEncodingAsync ReadTextWithEncodingAsync
#define _StorageFile ABI::Windows::Storage::IStorageFile
#define _StorageFileClass ABI::Windows::Storage::StorageFile
#define _StorageFolder ABI::Windows::Storage::IStorageFolder
#define _CreationCollisionOption ABI::Windows::Storage::CreationCollisionOption
#define _UnicodeEncoding ABI::Windows::Storage::Streams::UnicodeEncoding
#define _ApplicationData ABI::Windows::Storage::IApplicationData
#define _ApplicationDataStatics ABI::Windows::Storage::IApplicationDataStatics
#define _KnownFoldersStatics ABI::Windows::Storage::IKnownFoldersStatics
#define _StorageDeleteOption ABI::Windows::Storage::StorageDeleteOption
#define _StorageItem ABI::Windows::Storage::IStorageItem
#define _SyndicationClient ABI::Windows::Web::Syndication::ISyndicationClient
#define _SyndicationFeed ABI::Windows::Web::Syndication::SyndicationFeed
#define _ISyndicationFeed ABI::Windows::Web::Syndication::ISyndicationFeed
#define _SyndicationItem ABI::Windows::Web::Syndication::SyndicationItem
#define _ISyndicationItem ABI::Windows::Web::Syndication::ISyndicationItem
#define _SyndicationContent ABI::Windows::Web::Syndication::ISyndicationContent
#define _SyndicationLink ABI::Windows::Web::Syndication::SyndicationLink
#define _ISyndicationLink ABI::Windows::Web::Syndication::ISyndicationLink
#define _SyndicationPerson ABI::Windows::Web::Syndication::SyndicationPerson
#define _ISyndicationPerson ABI::Windows::Web::Syndication::ISyndicationPerson
#define _SyndicationText ABI::Windows::Web::Syndication::ISyndicationText
#define _SyndicationFormat ABI::Windows::Web::Syndication::SyndicationFormat
#define _RetrievalProgress ABI::Windows::Web::Syndication::RetrievalProgress
#define _ErrorInfo IRestrictedErrorInfo
#define _IVector ABI::Windows::Foundation::Collections::IVector
#define _Uri ABI::Windows::Foundation::IUriRuntimeClass
#define _CreateUri(Url, UrlObj) ACTIVATE_STATIC_OBJ(RuntimeClass_Windows_Foundation_Uri, MAKE_WRL_OBJ(ABI::Windows::Foundation::IUriRuntimeClassFactory), UriFactory, hr)\
	UriFactory->CreateUri(Url, &UrlObj);
#define _Vector Collections_winrt::Vector
#define _DeviceClass ABI::Windows::Devices::Enumeration::DeviceClass
#define _IDeviceInformation ABI::Windows::Devices::Enumeration::IDeviceInformation
#define _DeviceInformation ABI::Windows::Devices::Enumeration::DeviceInformation
#define _DeviceInformationStatics ABI::Windows::Devices::Enumeration::IDeviceInformationStatics
#define _MediaEncodingProfile ABI::Windows::Media::MediaProperties::IMediaEncodingProfile
#define _StreamingCaptureMode ABI::Windows::Media::Capture::StreamingCaptureMode
#define _PropertySet ABI::Windows::Foundation::Collections::IPropertySet
#define _Map ABI::Windows::Foundation::Collections::IMap<HSTRING, IInspectable *>
#define _PropertyValueStatics ABI::Windows::Foundation::IPropertyValueStatics
#define _VectorView ABI::Windows::Foundation::Collections::IVectorView
#define _StartPreviewToCustomSinkIdAsync StartPreviewToCustomSinkIdAsync
#define _InitializeWithSettingsAsync InitializeWithSettingsAsync
#define _FindAllAsyncDeviceClass FindAllAsyncDeviceClass
#define _MediaExtension ABI::Windows::Media::IMediaExtension
#define BEGIN_CREATE_ASYNC(type, ...) Concurrency_winrt::create_async<type>([__VA_ARGS__]() -> HRESULT {
#define END_CREATE_ASYNC(hr) return hr;\
})
#define DEFINE_TASK Concurrency_winrt::task
#define CREATE_TASK Concurrency_winrt::create_task
#define CREATE_AUDIO_TASK Concurrency_winrt::create_audio_task
#define CREATE_OR_CONTINUE_TASK(_task, rettype, func) _task = (_task == Concurrency_winrt::task<rettype>()) ? Concurrency_winrt::create_task<rettype>(func) : _task.then([func](rettype, rettype* retVal) -> HRESULT { return func(retVal); });
#define DEFINE_RET_VAL(x) x* retVal
#define DEFINE_RET_TYPE(x) <x>
#define DEFINE_RET_FORMAL(x) HRESULT
#define RET_VAL(x) *retVal = x;\
return S_OK;
#define RET_VAL_BASE return S_OK;
#define MAKE_STRING(str) Microsoft::WRL::Wrappers::HStringReference(L##str)
#define GET_STL_STRING(str) std::wstring(str.GetRawBuffer(NULL))
#define GET_STL_STRING_RAW(str) WindowsGetStringRawBuffer(str, NULL)
#define MAKE_WRL_OBJ(x) Microsoft::WRL::ComPtr<x>
#define MAKE_WRL_REF(x) x*
#define MAKE_OBJ_REF(x) x
#define MAKE_WRL_AGILE_REF(x) x*
#define MAKE_PROPERTY_BACKING(Type, PropName) Type PropName;
#define MAKE_PROPERTY(Type, PropName, PropValue) STDMETHODIMP get_##PropName(Type* pVal) { if (pVal) { *pVal = PropValue; } else { return E_INVALIDARG; } return S_OK; }\
	STDMETHODIMP put_##PropName(Type Val) { PropValue = Val; return S_OK; }
#define MAKE_PROPERTY_STRING(Type, PropName, PropValue) STDMETHODIMP get_##PropName(Type* pVal) { if (pVal) { return ::WindowsDuplicateString(PropValue.Get(), pVal); } else { return E_INVALIDARG; } }\
	STDMETHODIMP put_##PropName(Type Val) { return PropValue.Set(Val); }
#define MAKE_READONLY_PROPERTY(Type, PropName, PropValue) STDMETHODIMP get_##PropName(Type* pVal) { if (pVal) { *pVal = PropValue; } else { return E_INVALIDARG; } return S_OK; }
#define THROW_INVALID_ARG RoOriginateError(E_INVALIDARG, nullptr);
#define RELEASE_AGILE_WRL(x) if (x) { (x)->Release(); x = nullptr; }
#define RELEASE_WRL(x) if (x) { (x)->Release(); x = nullptr; }
#define GET_WRL_OBJ_FROM_REF(objtype, obj, orig, hr) Microsoft::WRL::ComPtr<objtype> obj;\
hr = orig->QueryInterface(__uuidof(objtype), &obj);
#define GET_WRL_OBJ_FROM_OBJ(objtype, obj, orig, hr) Microsoft::WRL::ComPtr<objtype> obj;\
hr = orig.As(&obj);
#define WRL_ENUM_GET(obj, prefix, prop) obj::prefix##_##prop
#define WRL_PROP_GET(obj, prop, arg, hr) hr = obj->get_##prop(&arg);
#define WRL_PROP_PUT(obj, prop, arg, hr) hr = obj->put_##prop(arg);
#define WRL_METHOD_BASE(obj, method, ret, hr) hr = obj->##method(&ret);
#define WRL_METHOD(obj, method, ret, hr, ...) hr = obj->##method(__VA_ARGS__, &ret);
#define WRL_METHOD_NORET_BASE(obj, method, hr) hr = obj->##method();
#define WRL_METHOD_NORET(obj, method, hr, ...) hr = obj->##method(__VA_ARGS__);
#define REF_WRL_OBJ(obj) obj.GetAddressOf()
#define DEREF_WRL_OBJ(obj) obj.Get()
#define DEREF_AGILE_WRL_OBJ(obj) obj
#define DEREF_AS_NATIVE_WRL_OBJ(type, obj) obj.Get()
#define PREPARE_TRANSFER_WRL_OBJ(obj) obj.Detach()
#define ACTIVATE_LOCAL_OBJ_BASE(objtype) Microsoft::WRL::Make<objtype>()
#define ACTIVATE_LOCAL_OBJ(objtype, ...) Microsoft::WRL::Make<objtype>(__VA_ARGS__)
#define ACTIVATE_EVENT_HANDLER(objtype, ...) Microsoft::WRL::Callback<objtype>(__VA_ARGS__).Get()
#define ACTIVATE_OBJ(rtclass, objtype, obj, hr) MAKE_WRL_OBJ(objtype) obj;\
{\
	Microsoft::WRL::ComPtr<IActivationFactory> objFactory;\
	hr = Windows::Foundation::GetActivationFactory(Microsoft::WRL::Wrappers::HStringReference(rtclass).Get(), objFactory.ReleaseAndGetAddressOf());\
	if (SUCCEEDED(hr)) {\
		Microsoft::WRL::ComPtr<IInspectable> pInsp;\
		hr = objFactory->ActivateInstance(pInsp.GetAddressOf());\
		if (SUCCEEDED(hr)) hr = pInsp.As(&obj);\
	}\
}
#define ACTIVATE_STATIC_OBJ(rtclass, objtype, obj, hr) objtype obj;\
{\
	Microsoft::WRL::ComPtr<IActivationFactory> objFactory;\
	hr = Windows::Foundation::GetActivationFactory(Microsoft::WRL::Wrappers::HStringReference(rtclass).Get(), objFactory.ReleaseAndGetAddressOf());\
	if (SUCCEEDED(hr)) {\
		if (SUCCEEDED(hr)) hr = objFactory.As(&obj);\
	}\
}
#endif

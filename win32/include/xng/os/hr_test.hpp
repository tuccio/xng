#pragma once

#include <xng/core/platform.hpp>

#define XNG_DEBUG_LOG(Message) OutputDebugStringA(Message)
#define XNG_HR_LOG(HR)\
		{\
			LPSTR output;\
			FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,\
			nullptr,\
			HR,\
			MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),\
			(LPSTR) &output,\
			0x0,\
			nullptr);\
			XNG_DEBUG_LOG(output);\
			LocalFree((HLOCAL) output);\
			XNG_DEBUGBREAK();\
		}

#define XNG_HR_CHECK(HR)  { HRESULT hr = HR; if (FAILED(hr)) XNG_HR_LOG(hr); }
#define XNG_HR_FAILED(HR) ([] (HRESULT hr) { if (FAILED(hr)) { XNG_HR_LOG(hr); return true; } return false; } (HR))

#define XNG_BLOB_ARGS(Blob) Blob->GetBufferPointer(), Blob->GetBufferSize()
#define XNG_BLOB_LOG(Blob) { const char * msg = static_cast<const char *>(Blob->GetBufferPointer()); XNG_DEBUG_LOG(msg); }

#define XNG_HR_CHECK_BLOB(HR, Blob) { HRESULT hr = HR; if (FAILED(hr)) XNG_BLOB_LOG(Blob); }
#define XNG_HR_FAILED_BLOB(HR, Blob) ([&Blob] (HRESULT hr) { if (FAILED(hr)) { XNG_HR_LOG(hr); XNG_BLOB_LOG(Blob); return true; } return false; } (HR))
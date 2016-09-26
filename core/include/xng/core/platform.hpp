#pragma once

#ifdef XNG_MSVC

#define XNG_EXPORT __declspec(dllexport)
#define XNG_IMPORT __declspec(dllimport)

#ifdef XNG_DEBUG
#define XNG_DEBUGBREAK() __debugbreak()
#else
#define XNG_DEBUGBREAK()
#endif

#endif
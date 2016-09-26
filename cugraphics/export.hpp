#pragma once

#ifdef XNG_MSVC

#define XNG_EXPORT __declspec(dllexport)
#define XNG_IMPORT __declspec(dllimport)

#endif
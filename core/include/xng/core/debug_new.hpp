#pragma once

#include <fstream>
#include <xng/core/singleton.hpp>

#ifdef XNG_DEBUG_NEW

#ifdef XNG_WIN32

#define _CRTDBG_MAP_ALLOC

#include <stdlib.h>
#include <crtdbg.h>

#define XNG_NEW                new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define XNG_DELETE             delete
#define XNG_DEBUG_NEW_REPORT() _CrtDumpMemoryLeaks()

#endif

#else

#define XNG_NEW    new
#define XNG_DELETE delete
#define XNG_DEBUG_NEW_REPORT()

#endif
#pragma once

#include <fstream>
#include <xng/core/singleton.hpp>

#ifdef XNG_DEBUG_NEW

#ifdef XNG_WIN32

#define _CRTDBG_MAP_ALLOC

#include <stdlib.h>
#include <crtdbg.h>

#define xng_new                new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define xng_delete             delete
#define XNG_DEBUG_NEW_REPORT() _CrtDumpMemoryLeaks()
#define XNG_DEBUG_NEW_INIT()   _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

#endif

#else

#define XNG_NEW    new
#define XNG_DELETE delete
#define XNG_DEBUG_NEW_REPORT()
#define XNG_DEBUG_NEW_INIT()

#endif
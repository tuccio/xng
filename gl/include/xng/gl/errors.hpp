#pragma once

#ifdef XNG_GL_DEBUG

#include <xng/core/logger.hpp>
#include <iomanip>
#include <type_traits>

#define XNG_GL_INTERNAL_LIST_ERRORS(Error, Stream) { Stream << "0x" << std::setfill('0') << std::setw(4) << std::hex; while (Error != GL_NO_ERROR) { Stream << Error; Error = glGetError(); if (Error != GL_NO_ERROR) Stream << ", "; } }
#define XNG_GL_INTERNAL_LOG_CALL_ERROR(Label, Call) { GLenum error = glGetError(); if (error != GL_NO_ERROR) { auto stream = XNG_LOG_STREAM(); stream << "An error was raised by OpenGL during call " << #Call << " [ ";; XNG_GL_INTERNAL_LIST_ERRORS(error, stream); stream << " ]."; XNG_LOG(Label, stream); } }
#define XNG_GL_INTERNAL_LOG_LAST_ERROR(Label, Call) { GLenum error = glGetError(); if (error != GL_NO_ERROR) { auto stream = XNG_LOG_STREAM(); stream << "An error was raised by OpenGL " << #Call << " [ "; XNG_GL_INTERNAL_LIST_ERRORS(error, stream); stream << " ]."; XNG_LOG(Label, stream); } }

#define XNG_GL_CHECK(Call) { Call; XNG_GL_INTERNAL_LOG_CALL_ERROR(XNG_LOG_DEBUG_LABEL, Call) }
#define XNG_GL_RETURN_CHECK(Call) ([=](const char * label) { auto value = Call; XNG_GL_INTERNAL_LOG_CALL_ERROR(label, Call); return value; } (XNG_LOG_DEBUG_LABEL))
#define XNG_GL_CHECK_LAST_ERROR() { XNG_GL_INTERNAL_LOG_LAST_ERROR(XNG_LOG_DEBUG_LABEL, Call); }

#else

#define XNG_GL_CHECK(Call) Call
#define XNG_GL_RETURN_CHECK(Call) Call

#endif
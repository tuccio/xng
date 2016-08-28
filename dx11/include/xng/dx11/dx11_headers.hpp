#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>

#include <xng/win32/com_ptr.hpp>
#include <xng/win32/hr_test.hpp>

namespace xng { namespace dx11 { using namespace xng::win32; } }

#define XNG_DX11_SHADER_FILE(File) (XNG_DX11_SHADER_FOLDER ## File)
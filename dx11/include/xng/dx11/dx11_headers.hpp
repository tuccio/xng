#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>

#include <xng/os/com_ptr.hpp>
#include <xng/os/hr_test.hpp>

namespace xng { namespace dx11 { using namespace xng::os; } }

#define XNG_DX11_SHADER_FILE(File) (XNG_DX11_SHADER_FOLDER ## File)
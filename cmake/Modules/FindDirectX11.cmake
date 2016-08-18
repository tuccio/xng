if (WIN32)

	if (CMAKE_CL_64)
		set ( DIRECTX11_ARCH "x64" )
	else()
		set ( DIRECTX11_ARCH "x86" )
	endif()

	set (ENV_PROGRAMFILES $ENV{ProgramFiles\(x86\)})
	
	set (
		DirectX11_INC_PATH_SUFFIXES
		um
	)
	
	set (
		DirectX11_LIB_PATH_SUFFIXES
		${DIRECTX11_ARCH}
		um
	)
	set (
		WindowsSDK_ROOT
		"${ENV_PROGRAMFILES}/Windows Kits"
	)
	
	find_path ( DirectX11_INCLUDE_DIR NAMES d3d11.h HINTS "${WindowsSDK_ROOT}/10/Include/*" PATH_SUFFIXES ${DirectX11_INC_PATH_SUFFIXES} )
	
	find_library ( DirectX11_D3D11_LIBRARY NAMES d3d11 HINTS "${WindowsSDK_ROOT}/10/Lib/*/*" PATH_SUFFIXES ${DirectX11_LIB_PATH_SUFFIXES} )
	find_library ( DirectX11_D3DCOMPILER_LIBRARY NAMES d3dcompiler HINTS "${WindowsSDK_ROOT}/10/Lib/*/*" PATH_SUFFIXES ${DirectX11_LIB_PATH_SUFFIXES} )
	
	if (NOT DirectX11_INCLUDE_DIR)
		set ( DirectX11_FOUND FALSE)
	else ()
		set ( DirectX11_FOUND TRUE )
	endif ()
	
	if (DirectX11_FOUND)
	
		set ( DirectX11_LIBRARY ${DirectX11_D3D11_LIBRARY} ${DirectX11_D3DCOMPILER_LIBRARY} )
		
	elseif (DirectX11_FIND_REQUIRED)
	
		message ( FATAL_ERROR "Could not find DirectX11" )
		
	endif()

endif ()
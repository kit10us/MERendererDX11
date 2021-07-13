// Copyright (c) 2002 - 2018, Kit10 Studios LLC
// All Rights Reserved
#pragma once

// A common header to include Direct-X and Windows in a clean manor.

#define WIN32_LEAN_AND_MEAN // For winsock.

#ifdef _DEBUG
#define D3D_DEBUG_INFO
#else // NDEBUG
#endif

#pragma warning( push )

#pragma warning ( disable: 4221 ) // unused lib
#pragma warning ( disable: 4006 ) // duplicate definition
#pragma warning( disable: 4005 ) // warning C4005: 'MAKEFOURCC': macro redefinition\

#include <D3D11.h>
#include <D3DCompiler.h>
#include <DirectXMath.h>

#pragma comment( lib, "d3d11" )
#pragma comment( lib, "d3dcompiler" )
#pragma comment( lib, "dxguid" )

#pragma comment( lib, "d2d1.lib" )
#pragma comment( lib, "dxgi.lib" )
#pragma comment( lib, "dwrite.lib" )
#pragma comment( lib, "winmm.lib" )

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#pragma warning( pop )
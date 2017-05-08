#pragma once

//Windows headers
#if _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3dcompiler.h>
#include <wrl.h>
#endif

#include <DirectXHelpers.h>
#include "Renderer/D3DCommon/D3DHelpers.h"

#ifdef DX11
#include <d3d11_1.h>
#endif

#ifdef DX12
#include <d3d12.h>
#include <dxgi1_4.h>
#endif

//SDL
#define SDL_MAIN_HANDLED
#include "SDL.h"
#pragma once

//Windows headers
#if _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3dcompiler.h>
#include <wrl.h>
#endif

#include "Graphics/D3DCommon/D3DHelpers.h"

#include <d3d12.h>
#include <dxgi1_4.h>

//SDL
#define SDL_MAIN_HANDLED
#include "SDL.h"

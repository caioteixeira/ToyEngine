#pragma once

//Windows headers
#if _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#endif

//SDL
#define SDL_MAIN_HANDLED
#include "SDL.h"

//DX11
#include "D3D11Types.h"
#include "D3D11GraphicsDevice.h"
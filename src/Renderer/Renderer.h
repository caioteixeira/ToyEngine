#ifdef DX11
#include "D3D11/D3D11Renderer.h"
typedef D3D11Renderer Renderer;
#endif

#ifdef DX12
#include "D3D12/D3D12Renderer.h"
typedef D3D12Renderer Renderer;
#endif
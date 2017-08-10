﻿#pragma once

#if DX12
#include <memory>
#include "../../WindowsHeaders.h"

//enum EBindflags
//{
//	EBF_VertexBuffer = D3D11_BIND_VERTEX_BUFFER,
//	EBF_IndexBuffer = D3D11_BIND_INDEX_BUFFER,
//	EBF_ConstantBuffer = D3D11_BIND_CONSTANT_BUFFER,
//	EBF_ShaderResource = D3D11_BIND_SHADER_RESOURCE,
//	EBF_RenderTarget = D3D11_BIND_RENDER_TARGET,
//	EBF_DeptStencil = D3D11_BIND_DEPTH_STENCIL
//};
//
//enum EGraphicsBufferUsage
//{
//	EGBU_Default = D3D11_USAGE_DEFAULT,
//	EGBU_Immutable = D3D11_USAGE_IMMUTABLE,
//	EGBU_Dynamic = D3D11_USAGE_DYNAMIC,
//	EGBU_Staging = D3D11_USAGE_STAGING,
//};

//enum ECPUAccessFlags
//{
//	ECPUAF_Neither = 0,
//	ECPUAF_CanWrite = D3D11_CPU_ACCESS_WRITE,
//	ECPUAF_CanRead = D3D11_CPU_ACCESS_READ,
//};

enum EGFormat
{
	EGF_R32G32B32A32_Float = DXGI_FORMAT_R32G32B32A32_FLOAT,
	EGF_R32G32B32_Float = DXGI_FORMAT_R32G32B32_FLOAT,
	EGF_R32G32_Float = DXGI_FORMAT_R32G32_FLOAT,
	EGF_R8G8B8A8_UNorm = DXGI_FORMAT_R8G8B8A8_UNORM,
	EGF_R8G8B8A8_UInt = DXGI_FORMAT_R8G8B8A8_UINT,
};

enum EFillMode
{
	EFM_Wireframe = D3D12_FILL_MODE_WIREFRAME,
	EFM_Solid = D3D12_FILL_MODE_SOLID,
};

enum EPrimitiveTopology
{
	EPT_TriangleList = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
	EPT_TriangleStrip = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
	EPT_LineList = D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
	EPT_PointList = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
};

enum EComparisonFunc
{
	ECF_Never = D3D12_COMPARISON_FUNC_NEVER,
	ECF_Less = D3D12_COMPARISON_FUNC_LESS,
	ECF_Equal = D3D12_COMPARISON_FUNC_EQUAL,
	ECF_LessEqual = D3D12_COMPARISON_FUNC_LESS_EQUAL,
	ECF_Greater = D3D12_COMPARISON_FUNC_GREATER,
	ECF_NotEqual = D3D12_COMPARISON_FUNC_NOT_EQUAL,
	ECF_GreaterEqual = D3D12_COMPARISON_FUNC_GREATER_EQUAL,
	ECF_Always = D3D12_COMPARISON_FUNC_ALWAYS,
};

enum ETextureFormat
{
	ETF_RGBA = DXGI_FORMAT_R8G8B8A8_UNORM,
	EXT_BGRA = DXGI_FORMAT_B8G8R8A8_UNORM,
};

//struct InputLayoutElement : D3D11_INPUT_ELEMENT_DESC
//{
//	InputLayoutElement(const char* inSemanticName, uint32_t inSemanticIndex, EGFormat inFormat, uint32_t inByteOffset)
//	{
//		SemanticName = inSemanticName;
//		SemanticIndex = inSemanticIndex;
//		Format = static_cast<DXGI_FORMAT>(inFormat);
//		InputSlot = 0;
//		AlignedByteOffset = inByteOffset;
//		InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
//		InstanceDataStepRate = 0;
//	}
//};

struct GraphicsBuffer
{
	Microsoft::WRL::ComPtr<ID3D12Resource> buffer = nullptr;
	UINT byteStride;
	UINT byteSize;
};
typedef  std::shared_ptr<GraphicsBuffer>  GraphicsBufferPtr;

struct GraphicsTexture
{
	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> uploadHeap = nullptr;
};
typedef std::shared_ptr<GraphicsTexture> GraphicsTexturePtr;

struct PipelineState
{
	ID3D12PipelineState * PSO;
};
typedef std::shared_ptr<PipelineState> PipelineStatePtr;

// typedef std::shared_ptr<ID3D12InputLayout>			InputLayoutPtr;
//typedef std::shared_ptr<ID3D11PixelShader>			PixelShaderPtr;
//typedef std::shared_ptr<ID3D11VertexShader>			VertexShaderPtr;
//typedef std::shared_ptr<ID3D11RenderTargetView>		RenderTargetPtr;
//typedef std::shared_ptr<ID3D11DepthStencilView>		DepthStencilPtr;
//typedef std::shared_ptr<ID3D11SamplerState>			SamplerStatePtr;
//typedef std::shared_ptr<ID3D11DepthStencilState>	DepthStencilStatePtr;
//typedef std::shared_ptr<ID3D11RasterizerState>		RasterizerStatePtr;
//typedef std::shared_ptr<ID3D11BlendState>			BlendStatePtr;

#endif
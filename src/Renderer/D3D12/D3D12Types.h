#pragma once
#include "d3dx12.h"

#if DX12
#include <memory>
#include "../../WindowsHeaders.h"

#define D3D12_GPU_VIRTUAL_ADDRESS_NULL      ((D3D12_GPU_VIRTUAL_ADDRESS)0)
#define D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN   ((D3D12_GPU_VIRTUAL_ADDRESS)-1)

enum EGraphicsBufferTypes
{	EGBT_VertexBuffer,
	EGBT_IndexBuffer,
};

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
	EPT_TriangleList = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
	EPT_TriangleStrip = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
	EPT_LineList = D3D_PRIMITIVE_TOPOLOGY_LINELIST,
	EPT_PointList = D3D_PRIMITIVE_TOPOLOGY_POINTLIST
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

struct GraphicsResource
{
	Microsoft::WRL::ComPtr<ID3D12Resource> buffer;
	size_t bufferSize;

	D3D12_RESOURCE_STATES state;
};
typedef std::shared_ptr<GraphicsResource> GraphicsResourcePtr;

struct GraphicsBuffer
{
	GraphicsResourcePtr resource = nullptr;
	size_t elementSize;
	size_t numElements;

	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView()
	{
		D3D12_VERTEX_BUFFER_VIEW view;
		view.BufferLocation = resource->buffer->GetGPUVirtualAddress();
		view.StrideInBytes = static_cast<UINT>(elementSize);
		view.SizeInBytes = static_cast<UINT>(resource->bufferSize);
		return view;
	};

	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView()
	{
		D3D12_INDEX_BUFFER_VIEW view;
		view.BufferLocation = resource->buffer->GetGPUVirtualAddress();
		view.Format = DXGI_FORMAT_R32_UINT;
		view.SizeInBytes = static_cast<UINT>(resource->bufferSize);
		return view;
	};
};
typedef  std::shared_ptr<GraphicsBuffer>  GraphicsBufferPtr;

struct GraphicsTexture
{
	GraphicsTexture()
	{
		resource = std::make_shared<GraphicsResource>();
	}

	GraphicsResourcePtr resource;
	//CPU visible discriptor heap, this should not be binded directly to the pipeline, 
	// but copied to the GPU visible heap every frame
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptor;
};
typedef std::shared_ptr<GraphicsTexture> GraphicsTexturePtr;

struct PipelineState
{
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
};
typedef std::shared_ptr<PipelineState> PipelineStatePtr;

#endif
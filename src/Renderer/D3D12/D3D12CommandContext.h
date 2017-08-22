#pragma once
#if DX12

#include "../../Math.h"
#include "D3D12Types.h"
#include "DynamicUploadHeap.h"

class D3D12CommandQueue;
class D3D12GraphicsDevice;
class CommandContextManager;

class D3D12CommandContext
{
friend D3D12CommandQueue;

public:
	explicit D3D12CommandContext(D3D12_COMMAND_LIST_TYPE type, D3D12GraphicsDevice* device);
	~D3D12CommandContext();

	//NonCopyable
	D3D12CommandContext() = default;
	D3D12CommandContext(const D3D12CommandContext&) = delete;
	D3D12CommandContext & operator=(const D3D12CommandContext&) = delete;

	uint64_t Flush(bool waitCompletion = false);
	uint64_t Finish(bool waitCompletion = false);

	void CopyBuffer(GraphicsResourcePtr dest, GraphicsResourcePtr src);
	DynamicAllocation ReserveUploadMemory(size_t sizeInBytes);

	void ClearRenderTargetView(D3D12_CPU_DESCRIPTOR_HANDLE& rtvHandle, Color& clearColor);
	void ClearDepthStencilView(D3D12_CPU_DESCRIPTOR_HANDLE& dsvHandle, float depth, UINT8 stencil);

	void SetRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE& rtv, D3D12_CPU_DESCRIPTOR_HANDLE& dsv);

	void SetIndexBuffer(const GraphicsBufferPtr buffer);
	void SetVertexBuffer(const GraphicsBufferPtr buffer);

	void SetViewport(const D3D12_VIEWPORT& viewport);
	void SetScissor(const D3D12_RECT& rect);

	void Draw(int vertexCount, int vertexStartOffset = 0);
	void DrawIndexed(int indexCount, int startIndexLocation, int baseVertexLocation = 0);

	void TransitionResource(ID3D12Resource * resource, D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter);
	void TransitionResource(GraphicsResourcePtr resource, D3D12_RESOURCE_STATES state);

	ID3D12GraphicsCommandList* GetCommandList() { return mCommandList; }

	void Reset();
private:
	ID3D12GraphicsCommandList* mCommandList;
	ID3D12CommandAllocator* mAllocator;

	CommandContextManager* mContextManager;
	D3D12GraphicsDevice* mDevice;

	DynamicUploadHeap mUploadHeap;
};
#endif


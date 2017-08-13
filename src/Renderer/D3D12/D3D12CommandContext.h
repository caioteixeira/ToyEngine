#pragma once
#if DX12

#include "D3D12Types.h"

class D3D12CommandQueue;
class D3D12GraphicsDevice;

class D3D12CommandContext
{
friend D3D12CommandQueue;

public:
	explicit D3D12CommandContext(D3D12_COMMAND_LIST_TYPE type, D3D12GraphicsDevice* device);
	~D3D12CommandContext();

	void Flush(bool waitCompletion = false);
	void Finish(bool waitCompletion = false);

	void CopyBuffer(GraphicsBufferPtr dest, GraphicsBufferPtr src);

	void SetIndexBuffer(const GraphicsBufferPtr buffer);
	void SetVertexBuffer(const GraphicsBufferPtr buffer);

	void Draw(int vertexCount, int vertexStartOffset = 0);
	void DrawIndexed(int indexCount, int startIndexLocation, int baseVertexLocation = 0);

	void BeginResourceTransition(ID3D12Resource * resource, D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter);

private:
	ID3D12GraphicsCommandList* mCommandList;
	ID3D12CommandAllocator* mAllocator;

	D3D12GraphicsDevice* mDevice;
};
#endif


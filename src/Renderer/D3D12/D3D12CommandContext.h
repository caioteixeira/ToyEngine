#pragma once
#if DX12

#include "D3D12Types.h"

class D3D12CommandContext
{
public:
	D3D12CommandContext();
	~D3D12CommandContext();

	void Flush(bool waitCompletion = false);
	void Finish(bool waitCompletion = false);

	void CopyBuffer(GraphicsBufferPtr dest, GraphicsBufferPtr src);

	void SetIndexBuffer(const GraphicsBufferPtr buffer);
	void SetVertexBuffer(const GraphicsBufferPtr buffer);

	void Draw(int vertexCount, int vertexStartOffset = 0);
	void DrawIndexed(int indexCount, int startIndexLocation, int baseVertexLocation = 0);

private:
	ID3D12GraphicsCommandList* mCommandList;
};
#endif


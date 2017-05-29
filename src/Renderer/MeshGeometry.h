#pragma once
#include "Types.h"

//TODO: Implement support to submeshes
struct MeshGeometry
{
	MeshGeometry(GraphicsBufferPtr mVertexBuffer, GraphicsBufferPtr mIndexBuffer, int indexCount, InputLayoutPtr mInputLayout);

	GraphicsBufferPtr GetVertexBuffer() const { return mVertexBuffer; }
	GraphicsBufferPtr GetIndexBuffer() const { return mIndexBuffer; }
	InputLayoutPtr GetInputLayout() const { return mInputLayout;  }

	int indexCount;
private:
	GraphicsBufferPtr mVertexBuffer;
	GraphicsBufferPtr mIndexBuffer;
	InputLayoutPtr mInputLayout;
};

typedef std::shared_ptr<MeshGeometry> MeshGeometryPtr;

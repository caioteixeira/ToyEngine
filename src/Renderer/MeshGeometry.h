#pragma once
#include "Types.h"

//TODO: Implement support to submeshes
struct MeshGeometry
{
	MeshGeometry(GraphicsBufferPtr mVertexBuffer, GraphicsBufferPtr mIndexBuffer, int indexCount);

	GraphicsBufferPtr GetVertexBuffer() const { return mVertexBuffer; }
	GraphicsBufferPtr GetIndexBuffer() const { return mIndexBuffer; }

	int indexCount;
private:
	GraphicsBufferPtr mVertexBuffer;
	GraphicsBufferPtr mIndexBuffer;
};

typedef std::shared_ptr<MeshGeometry> MeshGeometryPtr;

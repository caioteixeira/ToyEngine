#pragma once
#include "Renderer.h"

//TODO: Implement support to submeshes
struct MeshGeometry
{
	MeshGeometry(GraphicsBufferPtr mVertexBuffer, GraphicsBufferPtr mIndexBuffer, InputLayoutPtr mInputLayout);

	GraphicsBufferPtr GetVertexBuffer() const { return mVertexBuffer; }
	GraphicsBufferPtr GetIndexBuffer() const { return mIndexBuffer; }

private:
	GraphicsBufferPtr mVertexBuffer;
	GraphicsBufferPtr mIndexBuffer;
	InputLayoutPtr mInputLayout;
};

typedef std::shared_ptr<MeshGeometry> MeshGeometryPtr;

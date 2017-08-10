#include "MeshGeometry.h"

MeshGeometry::MeshGeometry(GraphicsBufferPtr vertexBuffer, GraphicsBufferPtr indexBuffer, int indexCount) :
	mVertexBuffer(vertexBuffer), mIndexBuffer(indexBuffer), indexCount(indexCount)
{
}

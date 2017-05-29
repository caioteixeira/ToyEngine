#include "MeshGeometry.h"

MeshGeometry::MeshGeometry(GraphicsBufferPtr vertexBuffer, GraphicsBufferPtr indexBuffer, int indexCount, InputLayoutPtr inputLayout) :
	mVertexBuffer(vertexBuffer), mIndexBuffer(indexBuffer), indexCount(indexCount), mInputLayout(inputLayout)
{
}

#include "MeshGeometry.h"

MeshGeometry::MeshGeometry(GraphicsBufferPtr vertexBuffer, GraphicsBufferPtr indexBuffer, InputLayoutPtr inputLayout) :
	mVertexBuffer(vertexBuffer), mIndexBuffer(indexBuffer), mInputLayout(inputLayout)
{
}

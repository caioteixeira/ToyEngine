#pragma once
#include "../Math.h"
#include "Mesh.h"
#include <vector>

struct MeshElement
{
	MeshGeometryPtr mesh;
	MaterialPtr material;
};

struct FramePacket
{
	Matrix viewMatrix;
	std::vector<MeshElement> meshes;
};
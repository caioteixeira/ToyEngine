#pragma once
#include "../Math.h"
#include "Mesh.h"
#include <vector>

struct MeshElement
{
	MeshGeometryPtr mesh;
	MaterialPtr material;
	Matrix worldTransform;
	GraphicsBufferPtr constantBuffer;
};

struct FramePacket
{
	Matrix viewMatrix;
	Vector3 cameraPos;
	std::vector<MeshElement> meshes;

	Color ambientLightColor;
};
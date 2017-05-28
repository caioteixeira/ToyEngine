#pragma once
#include "MeshGeometry.h"
#include "Material.h"

class Mesh
{
public:
	Mesh(MeshGeometryPtr geometry, MaterialPtr material);

private:
	MeshGeometryPtr mGeometry;
	MaterialPtr mMaterial;
};

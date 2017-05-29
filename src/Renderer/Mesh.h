#pragma once
#include "MeshGeometry.h"
#include "Material.h"

class Mesh
{
public:
	Mesh(MeshGeometryPtr geometry, MaterialPtr material);
	MeshGeometryPtr GetGeometry() const { return mGeometry; }
	MaterialPtr GetMaterial() const { return mMaterial; }

private:
	MeshGeometryPtr mGeometry;
	MaterialPtr mMaterial;
};

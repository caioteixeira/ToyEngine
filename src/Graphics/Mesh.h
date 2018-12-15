#pragma once
#include "MeshGeometry.h"
#include "Material.h"

struct MeshRenderer
{
    MeshRenderer(const MeshGeometryPtr& geometry, const MaterialPtr& material) : geometry(geometry), material(material) {}
    MeshGeometryPtr geometry;
    MaterialPtr material;
};


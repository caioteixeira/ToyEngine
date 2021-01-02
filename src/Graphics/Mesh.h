#pragma once
#include "MeshGeometry.h"
#include "Material.h"

struct MeshRenderer
{
    MeshRenderer(const MeshGeometry& geometry, const MaterialPtr& material) : geometry(geometry), material(material) {}
    MeshGeometry geometry;
    MaterialPtr material;
};


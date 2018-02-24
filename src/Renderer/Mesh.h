#pragma once
#include "MeshGeometry.h"
#include "Material.h"

struct Mesh
{
    MeshGeometryPtr geometry;
    MaterialPtr material;
    GraphicsBufferPtr perObjectBuffer;
};

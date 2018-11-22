#pragma once
#include "MeshGeometry.h"
#include "Material.h"

typedef uint32_t MeshHandle;

struct Mesh
{
    MeshGeometryPtr geometry;
    MaterialPtr material;
    GraphicsBufferPtr perObjectBuffer;
};

struct MeshRenderer
{
    MeshRenderer(const MeshHandle meshHandle) : meshHandle(meshHandle) {}

    MeshHandle meshHandle;
};


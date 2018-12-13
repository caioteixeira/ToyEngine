#pragma once
#include "../Math.h"
#include "Mesh.h"
#include <vector>
#include "ConstantBufferStructs.h"

struct MeshElement
{
    Matrix worldTransform;
    MeshGeometryPtr geometry;
    MaterialPtr material;
};

struct FramePacket
{
    Matrix viewProjMatrix;
    Vector3 cameraPos;
    std::vector<MeshElement> meshes;

    Vector3 ambientLightColor;
    PointLightData pointLights[MAX_POINT_LIGHTS];
};

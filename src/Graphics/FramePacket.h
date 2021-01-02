#pragma once
#include "../Math.h"
#include "Mesh.h"
#include <vector>
#include "ConstantBufferStructs.h"

struct MeshElement
{
    Matrix worldTransform;
    MeshGeometry geometry;
    GraphicsBufferHandle constantBuffer;
    PipelineStateHandle pipelineState;
    std::vector<TextureHandle> textures;
};

struct FramePacket
{
    Matrix viewProjMatrix;
    Vector3 cameraPos;
    std::vector<MeshElement> meshes;

    Vector3 ambientLightColor;
    PointLightData pointLights[MAX_POINT_LIGHTS];
};

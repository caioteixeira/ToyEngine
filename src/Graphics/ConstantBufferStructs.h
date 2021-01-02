#pragma once
#include "../Math.h"

static const int MAX_POINT_LIGHTS = 8;

struct PointLightData
{
    DirectX::XMFLOAT3 mDiffuse;
    float mInnerRadius;
    DirectX::XMFLOAT3 mPosition;
    float mOuterRadius;
    unsigned int mEnabled;
    float p1, p2, p3; //padding
};

struct GlobalConstants
{
    Matrix viewProjMatrix;
    Vector3 cameraPos;
    float p; //padding;
    Vector3 ambientColor;
    float p2; //padding
    PointLightData pointLights[MAX_POINT_LIGHTS];
};

struct PerObjectConstants
{
    Matrix worldTransform;
};

struct MaterialConstants
{
    DirectX::XMFLOAT3 kd;
    float d = 1.0f;
    DirectX::XMFLOAT3 ks;
    float ns;
    DirectX::XMFLOAT3 ka;
    float tr = 1.0f;
};

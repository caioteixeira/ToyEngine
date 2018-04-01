#pragma once

#include "Math.h"

struct Camera
{
    Camera(const float nearZ, const float farZ, const float aspect, const float fovY)
        : right(1.0f, 0.0f, 0.0f),
          up(0.0f, 1.0f, 0.0f),
          look(0.0f, 0.0f, 1.0f),
          nearZ(nearZ),
          farZ(farZ),
          aspect(aspect),
          fovY(fovY)
    {
        nearWindowHeight = 2.0f * nearZ * tanf(0.5f * fovY);
        farWindowHeight = 2.0f * farZ * tanf(0.5f * fovY);
        proj = DirectX::XMMatrixPerspectiveFovLH(fovY, aspect, nearZ, farZ);
    }

    Vector3 right;
    Vector3 up;
    Vector3 look;

    // Cache frustum properties.
    float nearZ = 0.0f;
    float farZ = 0.0f;
    float aspect = 0.0f;
    float fovY = 0.0f;
    float nearWindowHeight = 0.0f;
    float farWindowHeight = 0.0f;

    Matrix proj = Matrix::Identity;
    Matrix view = Matrix::Identity;
};

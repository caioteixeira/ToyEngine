#include "FPSCameraSystem.h"
#include "Camera.h"
#include "Transform.h"
#include "WindowsHeaders.h"
#include "Input.h"

using namespace DirectX;

void UpdateViewMatrix(entityx::ComponentHandle<Camera>& camera, entityx::ComponentHandle<Transform>& transform)
{
    XMVECTOR r = camera->right;
    XMVECTOR u = camera->up;
    XMVECTOR l = camera->look;
    XMVECTOR p = transform->position;

    // Keep camera's axes orthogonal to each other and of unit length.
    l = XMVector3Normalize(l);
    u = XMVector3Normalize(XMVector3Cross(l, r));

    // U, L already ortho-normal, so no need to normalize cross product.
    r = XMVector3Cross(u, l);

    // Fill in the view matrix entries.
    const float x = -XMVectorGetX(XMVector3Dot(p, r));
    const float y = -XMVectorGetX(XMVector3Dot(p, u));
    const float z = -XMVectorGetX(XMVector3Dot(p, l));

    camera->right = r;
    camera->up =  u;
    camera->look = l;

    camera->view(0, 0) = camera->right.x;
    camera->view(1, 0) = camera->right.y;
    camera->view(2, 0) = camera->right.z;
    camera->view(3, 0) = x;

    camera->view(0, 1) = camera->up.x;
    camera->view(1, 1) = camera->up.y;
    camera->view(2, 1) = camera->up.z;
    camera->view(3, 1) = y;

    camera->view(0, 2) = camera->look.x;
    camera->view(1, 2) = camera->look.y;
    camera->view(2, 2) = camera->look.z;
    camera->view(3, 2) = z;

    camera->view(0, 3) = 0.0f;
    camera->view(1, 3) = 0.0f;
    camera->view(2, 3) = 0.0f;
    camera->view(3, 3) = 1.0f;
}

inline void RotateY(entityx::ComponentHandle<Camera>& camera, const float angle)
{
    const XMMATRIX r = XMMatrixRotationY(angle);

    camera->right = XMVector3TransformNormal(camera->right, r);
    camera->up = XMVector3TransformNormal(camera->up, r);
    camera->look = XMVector3TransformNormal(camera->look, r);
}

inline void Strafe(entityx::ComponentHandle<Camera>& camera, entityx::ComponentHandle<Transform> transform, const float d)
{
    const XMVECTOR s = XMVectorReplicate(d);
    const XMVECTOR r = camera->right;
    const XMVECTOR p = transform->position;
    transform->position = XMVectorMultiplyAdd(s, r, p);
}

inline void Walk(entityx::ComponentHandle<Camera>& camera, entityx::ComponentHandle<Transform> transform, const float d)
{
    const XMVECTOR s = XMVectorReplicate(d);
    const XMVECTOR l = camera->look;
    const XMVECTOR p = transform->position;
    transform->position = XMVectorMultiplyAdd(s, l, p);
}

void Pitch(entityx::ComponentHandle<Camera>& camera, const float angle)
{
    const XMMATRIX r = XMMatrixRotationAxis(camera->right, angle);
    camera->up = XMVector3TransformNormal(camera->up, r);
    camera->look = XMVector3TransformNormal(camera->look, r);
}

void FPSCameraSystem::Update(entityx::EntityManager& entities, entityx::EventManager& events,
    entityx::TimeDelta dt)
{
    entityx::ComponentHandle<Transform> transform;
    entityx::ComponentHandle<Camera> camera;

    for (auto entity : entities.entities_with_components(transform, camera))
    {
        UpdateCamera(camera, transform, dt);

        //Expect only one camera
        break;
    }
}

void FPSCameraSystem::UpdateCamera(entityx::ComponentHandle<Camera>& camera,
    entityx::ComponentHandle<Transform>& transform, const entityx::TimeDelta dt)
{
    const auto cvar = CVar::Get("cameraSpeed");
    const float cameraSpeed = cvar->floatValue;

    if (Input::IsKeyDown(W))
    {
        Walk(camera, transform, cameraSpeed * dt);
    }
    if (Input::IsKeyDown(S))
    {
        Walk(camera, transform, -cameraSpeed * dt);
    }
    if (Input::IsKeyDown(A))
    {
        Strafe(camera, transform, -cameraSpeed * dt);
    }
    if (Input::IsKeyDown(D))
    {
        Strafe(camera, transform, cameraSpeed * dt);
    }

    if (Input::IsRightMouseButtonDown())
    {
        int x, y;
        Input::GetMousePosition(x, y);
        if (!mCapturingMousePosition)
        {
            mLastMousePosX = x;
            mLastMousePosY = y;
            mCapturingMousePosition = true;
        }

        const float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePosX));
        const float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePosY));
        Pitch(camera, dy);
        RotateY(camera, dx);

        mLastMousePosX = x;
        mLastMousePosY = y;
    }
    else
    {
        mCapturingMousePosition = false;
    }

    UpdateViewMatrix(camera, transform);
}

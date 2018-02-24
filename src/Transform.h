#pragma once
#include "Math.h"

struct Transform
{
    Vector3 position = Vector3::Zero;
    Vector3 rotation = Vector3::Zero;
    Vector3 scale = Vector3::One;
};

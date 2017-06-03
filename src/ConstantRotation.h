#pragma once
#include "Math.h"

struct ConstantRotation
{
	ConstantRotation(float x, float y, float z) : rotationBySecond(x, y, z) {};
	ConstantRotation(Vector3 rotation) : rotationBySecond(rotation) {};
	Vector3 rotationBySecond;
};
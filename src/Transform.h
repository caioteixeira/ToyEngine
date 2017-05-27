#pragma once
#include "Math.h"

struct Transform
{
	Transform(Vector3 pos) : position(pos) {};

	Vector3 position;
};
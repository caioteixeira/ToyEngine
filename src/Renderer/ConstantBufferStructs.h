#pragma once
#include "../Math.h"

struct GlobalConstants
{
	Matrix projMatrix;
};

struct PerObjectConstants
{
	Matrix worldTransform;
};

struct MaterialConstants
{
	Color kd;
	Color ks;
	Color ka;
	float ns;
	float d = 1.0f;
	float tr = 1.0f;
};
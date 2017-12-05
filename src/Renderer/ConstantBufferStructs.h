#pragma once
#include "../Math.h"
#include "FramePacket.h"

struct PointLightData
{
	Color mDiffuse;
	Color mSpecular;
	Vector3 mPosition;
	float mSpecularPower;
	float mInnerRadius;
	float mOuterRadius;
	unsigned int mEnabled;
	float p; //padding
};

struct GlobalConstants
{
	Matrix projMatrix;
	Vector3 cameraPos;
	float p; //padding;
	Color ambientColor;
	PointLightData pointLights[8];
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
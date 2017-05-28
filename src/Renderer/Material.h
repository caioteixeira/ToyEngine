#pragma once
#include <memory>
#include "Types.h"
#include "Texture.h"
#include "../Math.h"

enum MaterialProperties
{
	None = 0,
	ConstantColor = 1,
	Diffuse = 2,
	SpecularDiffuse = 4,
	DiffuseTexture = 8,
	ReflectionTexture = 16
};

struct Material
{
	MaterialProperties properties = None;

	Color ambientColor; 
	Color diffuseColor;
	Color specularColor;
	float shininess;

	TexturePtr diffuseTexture;
	TexturePtr specularTexture;

	void SetProperty(MaterialProperties property);
};

typedef std::shared_ptr<Material> MaterialPtr;
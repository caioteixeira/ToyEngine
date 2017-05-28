#pragma once
#include "Types.h"
#include <memory>

class Texture
{
public:
	Texture(GraphicsTexturePtr texture, int width, int height);

	GraphicsTexturePtr GetGraphicsTexture() const { return mTexture; }
	int GetWidth() const { return mWidth; }
	int GetHeight() const { return mHeight; }
	
private:
	GraphicsTexturePtr mTexture;
	int mWidth;
	int mHeight;
};

typedef std::shared_ptr<Texture> TexturePtr;
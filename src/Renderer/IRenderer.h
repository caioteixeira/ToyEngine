#pragma once
#include <memory>

struct FramePacket;
class IResourceManager;

class IRenderer
{
public:
	IRenderer() {};
	virtual ~IRenderer() = 0;

	void Init(int width, int height);
	void RenderFrame(FramePacket & framePacket);
	IResourceManager* GetResourceManager();
};


#pragma once

class IRenderer
{
public:
	IRenderer() {};
	virtual ~IRenderer() = 0;

	void Init(int width, int height);
	void RenderFrame();
};


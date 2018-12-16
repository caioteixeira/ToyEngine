#include "GraphicsCore.h"

std::unique_ptr<Renderer> Graphics::mRenderer = nullptr;

void Graphics::Init()
{
    mRenderer = std::make_unique<Renderer>();

    const auto windowWidth = CVar::Get("windowWidth")->intValue;
    const auto windowHeight = CVar::Get("windowHeight")->intValue;
    if (!mRenderer->Init(windowWidth, windowHeight))
    {
        Logger::DebugLog("Failed to initialized Renderer.");
    }
}

Renderer* Graphics::GetRenderer()
{
    return mRenderer.get();
}

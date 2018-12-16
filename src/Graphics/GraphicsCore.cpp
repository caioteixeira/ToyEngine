#include "GraphicsCore.h"

std::unique_ptr<Renderer> Graphics::mRenderer = nullptr;
std::unique_ptr<GraphicsResourcesManager> Graphics::mResourcesManager = nullptr;

void Graphics::Init()
{
    mRenderer = std::make_unique<Renderer>();

    const auto windowWidth = CVar::Get("windowWidth")->intValue;
    const auto windowHeight = CVar::Get("windowHeight")->intValue;
    if (!mRenderer->Init(windowWidth, windowHeight))
    {
        Logger::DebugLog("Failed to initialized Renderer.");
    }

    mResourcesManager = std::make_unique<GraphicsResourcesManager>(mRenderer->GetResourceManager());
}

Renderer* Graphics::GetRenderer()
{
    return mRenderer.get();
}

GraphicsResourcesManager* Graphics::GetResourcesManager()
{
    return mResourcesManager.get();
}

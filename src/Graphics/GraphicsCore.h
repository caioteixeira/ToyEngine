#pragma once
#include "Renderer.h"
#include "GraphicsResourcesManager.h"

namespace Engine
{
    class Graphics
    {
    public:
        static void Init();
        static Renderer* GetRenderer();
        static GraphicsResourcesManager* GetResourcesManager();
    private:
        static std::unique_ptr<Renderer> mRenderer;
        static std::unique_ptr<GraphicsResourcesManager> mResourcesManager;
    };
}

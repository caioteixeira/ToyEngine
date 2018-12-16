#pragma once
#include "Renderer.h"

namespace Engine
{
    class Graphics
    {
    public:
        static void Init();
        static Renderer* GetRenderer();
    private:
        static std::unique_ptr<Renderer> mRenderer;
    };
}

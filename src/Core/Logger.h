#pragma once

namespace DirectX {
    namespace SimpleMath {
        struct Matrix;
    }
}

namespace Engine::Logger
{
    void DebugLog(const char* format, ...);
    void DebugLogError(const char* format, ...);
    void LogMatrix(const DirectX::SimpleMath::Matrix matrix);
}

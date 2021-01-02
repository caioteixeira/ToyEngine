#pragma once
#include <string>
#include "../../EngineCore.h"

using namespace Engine;

inline void ThrowIfFailed(HRESULT hr, const char* str = nullptr)
{
    if (hr != S_OK)
    {
        if (str != nullptr)
        {
            Logger::DebugLogError(str);
        }

        //TODO: Create DXException
        throw 1;
    }
}

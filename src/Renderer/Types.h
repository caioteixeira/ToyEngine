#pragma once

#include "D3D12/D3D12Types.h"

struct GraphicBufferHandle
{
    uint32_t value;
};

struct TextureHandle
{
    uint32_t value;

    TextureHandle() : value(-1)
    {
    }

    explicit TextureHandle(uint32_t value)
        : value(value)
    {
    }

    static TextureHandle NullHandle()
    {
        return TextureHandle(-1);
    }

    bool operator!=(const TextureHandle& rhs) const;

    bool operator==(const TextureHandle& rhs) const;
};

struct PipelineStateHandle
{
    uint32_t value;
};

#include "Types.h"

bool TextureHandle::operator!=(const TextureHandle & rhs) const
{
    return value != rhs.value;
}

bool TextureHandle::operator==(const TextureHandle& rhs) const
{
    return value == rhs.value;
}

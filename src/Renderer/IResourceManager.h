#pragma once
#include <string>
#include "Mesh.h"
#include <vector>
#include "Types.h"

class IResourceManager
{
public:
    MeshGeometryPtr GetMeshGeometry(const std::string& path, const std::string& inputLayoutName);
    TextureHandle GetTexture(const std::string& path);
};

#pragma once
#include <string>
#include "Mesh.h"
#include <vector>
#include "Types.h"

class IResourceManager
{
public:
    void LoadObjFile(const std::string& path, std::vector<Mesh>& outMeshes);
    MeshGeometryPtr GetMeshGeometry(const std::string& path, const std::string& inputLayoutName);
    TextureHandle GetTexture(const std::string& path);
};

#pragma once
#include <string>
#include "Texture.h"
#include "Mesh.h"
#include <vector>

class IResourceManager
{
public:
    void LoadObjFile(const std::string& path, std::vector<Mesh>& outMeshes);
    MeshGeometryPtr GetMeshGeometry(const std::string& path, const std::string& inputLayoutName);
    TexturePtr GetTexture(const std::string& path);
};

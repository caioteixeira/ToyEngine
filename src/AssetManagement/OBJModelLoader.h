#pragma once

#include <vector>
#include "../Renderer/Vertex.h"
#include "../Renderer/Material.h"
#include <tiny_obj_loader.h>

namespace OBJModelLoader
{
    struct SubmeshDesc
    {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        std::string materialName;
    };

    struct MaterialDesc
    {
        std::string name;
        MaterialProperties properties = None;

        Color ambient;
        Color diffuse;
        Color specular;
        float shininess;

        std::string diffuseTexName = "";
        std::string specularTexName = "";
    };

    MaterialDesc LoadMaterial(tinyobj::material_t material);

    void LoadObjFile(std::string path, std::vector<SubmeshDesc>& outSubmeshes,
                     std::vector<MaterialDesc>& outMaterials);
}

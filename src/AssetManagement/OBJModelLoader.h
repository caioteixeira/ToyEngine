#pragma once

#include <vector>
#include "../Graphics/Vertex.h"
#include "../Graphics/Material.h"
#include <tiny_obj_loader.h>

namespace OBJModelLoader
{
    struct MeshDesc
    {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        std::string materialName;
    };

    struct PhongMaterialDesc
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

    PhongMaterialDesc LoadMaterial(tinyobj::material_t material);

    void LoadObjFile(std::string path, std::vector<MeshDesc>& outSubmeshes,
                     std::vector<PhongMaterialDesc>& outMaterials);
}

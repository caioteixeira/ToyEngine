#pragma once

#include <vector>
#include "Vertex.h"
#include "Material.h"
#include <unordered_map>
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

	void LoadObjFile(std::string path,  std::vector<SubmeshDesc>& outSubmeshes, std::unordered_map<std::string, MaterialDesc>& outMaterials);

	/**
	 * \brief Loads an obj file and merges its vertices on a unique geometry
	 * \param path 
	 * \param vertices 
	 * \param indices 
	 */
	void LoadModel(const std::string& path, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
}

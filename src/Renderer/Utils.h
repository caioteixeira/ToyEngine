#pragma once

#include <vector>
#include "Vertex.h"
#include "Material.h"
#include <unordered_map>

namespace Utils
{
	struct SubmeshData
	{
		std::vector<int> indices;
		std::string materialName;
	};

	struct MaterialData
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

	void LoadObjFile(std::string path, std::vector<Vertex>& vertices, std::vector<SubmeshData>& outSubmeshes, std::unordered_map<std::string, MaterialData> outMaterials);

	/**
	 * \brief Loads an obj file and merges its vertices on a unique geometry
	 * \param path 
	 * \param vertices 
	 * \param indices 
	 */
	void LoadModel(const std::string& path, std::vector<Vertex>& vertices, std::vector<int>& indices);
}

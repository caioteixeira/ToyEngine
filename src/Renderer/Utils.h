#pragma once

#include <vector>
#include "Vertex.h"
#include "Material.h"
#include <unordered_map>

namespace Utils
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

	void LoadObjFile(std::string path,  std::vector<SubmeshDesc>& outSubmeshes, std::unordered_map<std::string, MaterialDesc>& outMaterials);

	/**
	 * \brief Loads an obj file and merges its vertices on a unique geometry
	 * \param path 
	 * \param vertices 
	 * \param indices 
	 */
	void LoadModel(const std::string& path, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);

	/**
	 * \brief Divides a vector into N ranges
	 * \tparam Iterator type
	 * \param begin begin iterator
	 * \param end end iterator
	 * \param n number of chuncks
	 * \return returns an array of ranges
	 */
	template <typename Iterator>
	std::vector<std::pair<Iterator, Iterator>> DivideWork(Iterator begin, Iterator end, std::size_t n)
	{
		std::vector<std::pair<Iterator, Iterator>> ranges;
		if (n == 0) 
		{
			return ranges;
		}
		ranges.reserve(n);

		auto dist = std::distance(begin, end);
		n = std::min<size_t>(n, dist);
		auto chunk = dist / n;
		auto remainder = dist % n;

		for (size_t i = 0; i < n - 1; ++i) 
		{
			auto next_end = std::next(begin, chunk + (remainder ? 1 : 0));
			ranges.emplace_back(begin, next_end);

			begin = next_end;
			if (remainder) remainder -= 1;
		}

		// last chunk
		ranges.emplace_back(begin, end);
		return ranges;
	}
}

#include "Utils.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <unordered_map>

void Utils::LoadModel(std::string& path, std::vector<Vertex>& vertices, std::vector<int>& indices)
{
	//Init tinyobj loader
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;
	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path.c_str()))
	{
		throw std::runtime_error(err);
	}

	for(const auto& shape : shapes)
	{
		for(const auto& index: shape.mesh.indices)
		{
			Vertex vertex = {};
			vertex.position.x = attrib.vertices[3 * index.vertex_index + 0];
			vertex.position.y = attrib.vertices[3 * index.vertex_index + 1];
			vertex.position.z = attrib.vertices[3 * index.vertex_index + 2];

			vertex.normal.y = attrib.normals[3 * index.normal_index + 0];
			vertex.normal.x = attrib.normals[3 * index.normal_index + 1];
			vertex.normal.z = attrib.normals[3 * index.normal_index + 2];

			vertex.texCoord.x = attrib.texcoords[2 * index.texcoord_index + 0];
			vertex.texCoord.y = attrib.texcoords[2 * index.texcoord_index + 1];

			//TODO: Avoid duplicated vertices
			indices.push_back(vertices.size());
			vertices.push_back(vertex);
		}
	}
}

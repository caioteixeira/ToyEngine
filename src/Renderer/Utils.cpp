#include "Utils.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <unordered_map>

void LoadSubMesh(std::vector<Vertex>& vertices, std::vector<size_t>& indices, tinyobj::attrib_t & attrib, const tinyobj::shape_t& shape)
{
	auto allocSize = shape.mesh.indices.size();
	indices.reserve(allocSize);
	vertices.reserve(allocSize);

	for(const auto& index : shape.mesh.indices)
	{
		Vertex vertex = {};
		vertex.position.x = attrib.vertices[3 * index.vertex_index + 0];
		vertex.position.y = attrib.vertices[3 * index.vertex_index + 1];
		vertex.position.z = attrib.vertices[3 * index.vertex_index + 2];

		if(attrib.normals.size() > 0)
		{
			vertex.normal.y = attrib.normals[3 * index.normal_index + 0];
			vertex.normal.x = attrib.normals[3 * index.normal_index + 1];
			vertex.normal.z = attrib.normals[3 * index.normal_index + 2];
		}

		vertex.texCoord.x = attrib.texcoords[2 * index.texcoord_index + 0];
		vertex.texCoord.y = 1.0f - attrib.texcoords[2 * index.texcoord_index + 1];

		//TODO: Avoid duplicated vertices
		indices.push_back(vertices.size());
		vertices.push_back(vertex);
	}
}

void Utils::LoadObjFile(std::string path, std::vector<SubmeshDesc>& outSubmeshes, std::unordered_map<std::string, MaterialDesc>& outMaterials)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;

	SDL_Log("Started to load Obj File");
	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path.c_str(), "Assets/"))
	{
		throw std::runtime_error(err);
	}
	SDL_Log("Succesfully loaded obj file");

	for (const auto& shape : shapes)
	{
		SubmeshDesc data;
		LoadSubMesh(data.vertices, data.indices, attrib, shape);
		data.materialName = materials[shape.mesh.material_ids[0]].name;
		outSubmeshes.push_back(data);

		SDL_Log("Succesfully loaded a submesh");
	}

	for( const auto& material : materials)
	{
		MaterialDesc data;
		data.name = material.name;

		data.properties = material.illum == 1 ?  Diffuse : None;

		data.ambient = Color(material.ambient[0], material.ambient[1], material.ambient[2]);
		data.diffuse = Color(material.diffuse[0], material.diffuse[1], material.diffuse[2]);
		data.specular = Color(material.specular[0], material.specular[1], material.specular[2]);
		data.shininess = material.shininess;
		data.diffuseTexName = material.diffuse_texname;
		data.specularTexName = material.specular_texname;
		outMaterials[data.name] = data;

		SDL_Log("Succesfully loaded a material");
	}
}

void Utils::LoadModel(const std::string& path, std::vector<Vertex>& vertices, std::vector<size_t>& indices)
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
		LoadSubMesh(vertices, indices, attrib, shape);
	}
}

#pragma once

#include <vector>
#include "Vertex.h"

namespace Utils
{
	void LoadModel(const std::string& path, std::vector<Vertex>& vertices, std::vector<int>& indices);
}

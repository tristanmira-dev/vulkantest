#include "vertex.hpp"
#include "HelloTriangleApplication.hpp"
#include <algorithm>

void HelloTriangleApplication::readVertices(std::initializer_list<Vertex> verticeValues) {
	vertices = std::vector<Vertex>(verticeValues.size());
	std::copy(verticeValues.begin(), verticeValues.end(), vertices.begin());
}

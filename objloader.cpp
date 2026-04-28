#include <tiny_obj_loader.h>
#include "HelloTriangleApplication.hpp"
#include <fstream>
#include <iostream>

void HelloTriangleApplication::loadModel() {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err, warn;

	std::string path{ "E:/Important_Repos/vulkan/vulkantest/extern/assets/viking_room.obj" };


	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str())) {
		throw std::runtime_error("Bruh");
	}

	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			Vertex vertex{};

			vertex.pos = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			vertex.uv = {
				attrib.texcoords[2 * index.texcoord_index + 0],
				1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
			};

			vertex.col = { 1.0f, 1.0f, 1.0f };

			vertices.push_back(vertex);
			indices.push_back(indices.size());
		}
	}


	gameObjects.addMeshObject(
		vertices,
		indices
	);

}
#ifndef VERTEX_HPP
#define VERTEX_HPP

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>
#include <array>


struct Vertex {
	glm::vec3 pos;
	glm::vec3 col;

	static vk::VertexInputBindingDescription getBindingDescription() {
		return { 0, sizeof(Vertex), vk::VertexInputRate::eVertex };
	}

	static std::array<vk::VertexInputAttributeDescription, 2> getAttributeDescription() {
		return {
			vk::VertexInputAttributeDescription{0, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, pos) },
			vk::VertexInputAttributeDescription{1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, col) }
		};
	}
};




#endif
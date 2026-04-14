#ifndef VERTEX_HPP
#define VERTEX_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan.hpp>
#include <array>


struct Vertex {
	glm::vec3 pos;
	glm::vec3 col;
	glm::vec2 uv{0.f, 0.f};

	static vk::VertexInputBindingDescription getBindingDescription() {
		return { 0, sizeof(Vertex), vk::VertexInputRate::eVertex };
	}

	static std::array<vk::VertexInputAttributeDescription, 3> getAttributeDescription() {
		return {
			vk::VertexInputAttributeDescription{ 0, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, pos) },
			vk::VertexInputAttributeDescription{ 1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, col) },
			vk::VertexInputAttributeDescription{ 2, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, uv) }
		};
	}
};

struct UniformBufferObject {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
};

struct GameObjectInfo {
	glm::mat4 transform;

	GameObjectInfo() : transform{  } {
		transform = glm::identity<glm::mat4>();
	}

	void moveTo(glm::vec3 position) {
		transform[3][0] += position.x;
		transform[3][1] += position.y;
		transform[3][2] += position.z;
	}
};




#endif
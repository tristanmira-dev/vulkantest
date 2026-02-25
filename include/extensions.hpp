#ifndef EXTENSIONS_HPP
#define EXTENSIONS_HPP

#include <vector>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_raii.hpp>

std::vector<const char*> getRequiredInstanceExtensions();

VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
	vk::DebugUtilsMessageTypeFlagsEXT type,
	const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData
);


#endif
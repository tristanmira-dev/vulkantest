#include "extensions.hpp"
#include <iostream>

#if NDEBUG
bool enableValidationLayers{ false };
#else
constexpr bool enableValidationLayers{ true };
#endif


/*Get the glfw required extensions*/
std::vector<const char*> getRequiredInstanceExtensions() {
	uint32_t glfwExtensionCount{};
	auto glfwExtensions{ glfwGetRequiredInstanceExtensions(&glfwExtensionCount) };

	auto extensions{ std::vector<const char*>(glfwExtensions, glfwExtensions + glfwExtensionCount) };

	if (enableValidationLayers) extensions.push_back(vk::EXTDebugUtilsExtensionName);

	
	return  extensions;
}

VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
	vk::DebugUtilsMessageTypeFlagsEXT type,
	const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData
) {

	std::cerr << "validation layer: type " << to_string(type) << " msg: " << pCallbackData->pMessage << std::endl;

	return vk::False;
}
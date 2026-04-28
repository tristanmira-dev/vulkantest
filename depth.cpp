#include "HelloTriangleApplication.hpp"

vk::Format HelloTriangleApplication::findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features) {
	for (const vk::Format format : candidates) {
		vk::FormatProperties props = physicalDevice.getFormatProperties(format);
		if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features) {
			return format;
		} if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features) {
			return format;
		}
	}

	throw std::runtime_error("NO AVAILABLE FORMAT FOUND");
}

vk::Format HelloTriangleApplication::findDepthFormat() {
	return findSupportedFormat({ vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint }, vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eDepthStencilAttachment);
}

bool HelloTriangleApplication::hasStencilComponent(vk::Format format) {
	return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
}

void HelloTriangleApplication::createDepthResource() {

	vk::Format depthFormat{ findDepthFormat() };

	createImage(swapChainExtent.width, swapChainExtent.height, depthFormat, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::MemoryPropertyFlagBits::eDeviceLocal, depthImage, depthImageDeviceMemory);

	depthImageView = createImageViewHelper(depthImage, depthFormat, vk::ImageAspectFlagBits::eDepth);


}
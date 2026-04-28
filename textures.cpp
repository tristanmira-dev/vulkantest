#include "HelloTriangleApplication.hpp"
#include <stb_image.h>
#include <filesystem>
#include <iostream>

void HelloTriangleApplication::copyBufferToImage(const vk::raii::Buffer& buffer, vk::raii::Image &image, uint32_t width, uint32_t height) {
	vk::raii::CommandBuffer commandBuffer{ beginSingleTimeCommand() };

	vk::BufferImageCopy region{ .bufferOffset = 0, .bufferRowLength = 0, .bufferImageHeight = 0, .imageSubresource = {vk::ImageAspectFlagBits::eColor, 0, 0, 1}, .imageOffset = {0, 0, 0}, .imageExtent = {width, height, 1} };

	commandBuffer.copyBufferToImage(buffer, textureImage, vk::ImageLayout::eTransferDstOptimal, { region });

	endSingleTimeCommands(commandBuffer);
}

vk::raii::CommandBuffer HelloTriangleApplication::beginSingleTimeCommand() {
	vk::CommandBufferAllocateInfo commandBuff{ .commandPool = commandPool, .level = vk::CommandBufferLevel::ePrimary, .commandBufferCount = 1 };

	vk::raii::CommandBuffer commBuff = std::move(vk::raii::CommandBuffers(device, commandBuff).front());

	vk::CommandBufferBeginInfo submitInfo{ .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit };

	commBuff.begin(submitInfo);

	return commBuff;
}

void HelloTriangleApplication::transitionImageLayout(const vk::raii::Image& image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout) {
	vk::raii::CommandBuffer commandBuff{ beginSingleTimeCommand() };

	vk::ImageMemoryBarrier barrier{ .oldLayout = oldLayout, .newLayout = newLayout, .image = image, .subresourceRange = {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1} };

	vk::PipelineStageFlags srcStage;
	vk::PipelineStageFlags dstStage;


	if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal) {

		barrier.srcAccessMask = {};
		barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

		srcStage = vk::PipelineStageFlagBits::eTopOfPipe;
		dstStage = vk::PipelineStageFlagBits::eTransfer;


	} else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {

		barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

		srcStage = vk::PipelineStageFlagBits::eTransfer;
		dstStage = vk::PipelineStageFlagBits::eFragmentShader;


	} else {
		throw std::runtime_error("Image transition not supported");
	}

	commandBuff.pipelineBarrier(srcStage, dstStage, {}, {}, nullptr, { barrier });

	endSingleTimeCommands(commandBuff);
}


void HelloTriangleApplication::endSingleTimeCommands(vk::raii::CommandBuffer &commandBuffer) {
	commandBuffer.end();
	vk::SubmitInfo submitInfo{ .commandBufferCount = 1, .pCommandBuffers = &*commandBuffer };
	graphicsQueue.submit(submitInfo);
	graphicsQueue.waitIdle();

}


void HelloTriangleApplication::createImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::raii::Image &image, vk::raii::DeviceMemory &deviceMemory) {
	vk::ImageCreateInfo imageInfo { 
		.imageType = vk::ImageType::e2D, 
		.format = format, 
		.extent = {width, height, 1}, 
		.mipLevels = 1, 
		.arrayLayers = 1,
		.samples = vk::SampleCountFlagBits::e1,
		.tiling = tiling, 
		.usage = usage, 
		.sharingMode = vk::SharingMode::eExclusive /*Come back to this, just in case we need to separate the queue family indexes*/
	};

	image = vk::raii::Image(device, imageInfo);

	vk::MemoryRequirements memRequirements{ image.getMemoryRequirements() };
	vk::MemoryAllocateInfo allocInfo{ .allocationSize = memRequirements.size, .memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties) };

	deviceMemory = vk::raii::DeviceMemory(device, allocInfo);


	image.bindMemory(deviceMemory, 0);
}


void HelloTriangleApplication::createTextureImage() {
	int texWidth, texHeight, texChannels;

	stbi_uc* pixels{ stbi_load("E:/Important_Repos/vulkan/vulkantest/extern/assets/viking_room.png" /*yeah find a way to handle paths better, not just for assets but for shaders too, probably would just involve copying the files over with cmake maybe?*/, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha)};
	vk::DeviceSize imageSize{ static_cast<uint64_t>(texWidth) * static_cast<uint64_t>(texHeight) * 4 };

	if (!pixels) {
		throw std::runtime_error("Error wif the textures buddy");
	}

	vk::raii::Buffer stagingBuffer{nullptr};
	vk::raii::DeviceMemory stagingBufferMemory{nullptr};
	
	createBuffer(imageSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);

	void* data{ stagingBufferMemory.mapMemory(0, imageSize) };
	memcpy(data, pixels, imageSize);
	stagingBufferMemory.unmapMemory();

	stbi_image_free(pixels);

	createImage(texWidth, texHeight, vk::Format::eR8G8B8A8Srgb, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal, textureImage, textureImageMemory);

	transitionImageLayout(textureImage, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);

	copyBufferToImage(stagingBuffer, textureImage, texWidth, texHeight);

	transitionImageLayout(textureImage, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

}

void HelloTriangleApplication::createTextureImageView() {
	vk::ImageViewCreateInfo imageInfo{ .image = textureImage, .viewType = vk::ImageViewType::e2D, .format = vk::Format::eR8G8B8A8Srgb, .subresourceRange = {vk::ImageAspectFlagBits::eColor, 0 , 1, 0, 1} };

	textureImageView = vk::raii::ImageView(device, imageInfo);

}

vk::raii::ImageView HelloTriangleApplication::createImageViewHelper(vk::raii::Image &image, vk::Format const &format, vk::ImageAspectFlags aspectFlags) {
	vk::ImageViewCreateInfo info{ .image = image, .viewType = vk::ImageViewType::e2D, .format = format, .subresourceRange = {aspectFlags, 0, 1, 0, 1} };

	return vk::raii::ImageView(device, info);
}

void HelloTriangleApplication::createTextureSampler() {
	
	vk::PhysicalDeviceProperties properties = physicalDevice.getProperties();

	vk::SamplerCreateInfo samplerInfo{ .magFilter = vk::Filter::eLinear, .minFilter = vk::Filter::eLinear, .mipmapMode = vk::SamplerMipmapMode::eLinear, .addressModeU = vk::SamplerAddressMode::eRepeat, .addressModeV = vk::SamplerAddressMode::eRepeat, .anisotropyEnable = vk::True, .maxAnisotropy = properties.limits.maxSamplerAnisotropy, .compareEnable = vk::False, .compareOp = vk::CompareOp::eAlways };

	samplerInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;

	samplerInfo.unnormalizedCoordinates = vk::False;

	samplerInfo.mipLodBias = 0.f;
	samplerInfo.minLod = 0.f;
	samplerInfo.maxLod = 0.f;

	textureSampler = vk::raii::Sampler(device, samplerInfo);

}
#include "HelloTriangleApplication.hpp"

void HelloTriangleApplication::copyBuffer(vk::raii::Buffer &stagingBuffer, vk::raii::Buffer &vertexBuffer, vk::DeviceSize size) {
	vk::CommandBufferAllocateInfo allocateInfo{ .commandPool = commandPool, .level = vk::CommandBufferLevel::ePrimary, .commandBufferCount = 1 };
	vk::raii::CommandBuffer commandBuff = std::move(vk::raii::CommandBuffers(device, allocateInfo).front());
	commandBuff.begin(vk::CommandBufferBeginInfo{ .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit });

	commandBuff.copyBuffer(stagingBuffer, vertexBuffer, vk::BufferCopy(0, 0, size));

	commandBuff.end();

	graphicsQueue.submit(vk::SubmitInfo{ .commandBufferCount = 1, .pCommandBuffers = &*commandBuff }, nullptr);

	graphicsQueue.waitIdle();
}

void HelloTriangleApplication::createVertexBuffer() {

	vk::DeviceSize bufferSize{ sizeof(vertices[0]) * vertices.size() };

	vk::raii::Buffer stagingBuffer{ nullptr };

	vk::raii::DeviceMemory stagingBufferMemory{ nullptr };

	createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);

	void* stagingMemory{ stagingBufferMemory.mapMemory(0, bufferSize) };

	memcpy(stagingMemory, vertices.data(), bufferSize);

	stagingBufferMemory.unmapMemory();


	createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, vertexBuffer, vertexBufferMemory);


	copyBuffer(stagingBuffer, vertexBuffer, bufferSize);


}

void HelloTriangleApplication::createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags props, vk::raii::Buffer &buffer, vk::raii::DeviceMemory& bufferMemory) {
	
	vk::BufferCreateInfo bufferInfo{ .size = size, .usage = usage, .sharingMode = vk::SharingMode::eExclusive };
	buffer = vk::raii::Buffer(device, bufferInfo);

	vk::MemoryRequirements memRequirements{ buffer.getMemoryRequirements() }; /*Get mem reqs of buffer, find the appropriate memory in the device*/
	vk::MemoryAllocateInfo allocInfo{ .allocationSize = memRequirements.size, .memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, props) /*Get the memory type that satisfies the buffer in the previous step*/};

	bufferMemory = vk::raii::DeviceMemory(device, allocInfo);

	buffer.bindMemory(*bufferMemory, 0);
	
}
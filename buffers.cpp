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

	vk::DeviceSize bufferSize{ sizeof(gameObjects.vertices[0]) * gameObjects.vertices.size() };

	vk::raii::Buffer stagingBuffer{ nullptr };

	vk::raii::DeviceMemory stagingBufferMemory{ nullptr };

	createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);

	void* stagingMemory{ stagingBufferMemory.mapMemory(0, bufferSize) };

	memcpy(stagingMemory, gameObjects.vertices.data(), bufferSize);

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

void HelloTriangleApplication::createIndexBuffer() {

	vk::DeviceSize idxBuffSize{ gameObjects.indices.size() * sizeof(gameObjects.indices[0]) };
	
	vk::raii::Buffer stageIndexBuff{ nullptr };
	vk::raii::DeviceMemory stageIndexMemoryBuff{ nullptr };

	createBuffer(idxBuffSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible, stageIndexBuff, stageIndexMemoryBuff);

	void* data{ stageIndexMemoryBuff.mapMemory(0, idxBuffSize) };

	memcpy(data, gameObjects.indices.data(), idxBuffSize);

	stageIndexMemoryBuff.unmapMemory();

	createBuffer(idxBuffSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, indexBuffer, indexBufferMemory);

	copyBuffer(stageIndexBuff, indexBuffer, idxBuffSize);

}

void HelloTriangleApplication::createUniformBuffers() {
	uniformBuffer.clear();
	uniformBufferMemory.clear();
	mappedData.clear();

	for (std::size_t i{}; i < MAX_FRAMES_IN_FLIGHT; ++i) {
		vk::DeviceSize size{ sizeof(UniformBufferObject) };
		vk::raii::Buffer stageBuff{ nullptr };
		vk::raii::DeviceMemory stageMemoryBuff{ nullptr };

		vk::DeviceSize size2{ sizeof(GameObjectInfo) * gameObjects.vertices.size() };
		vk::raii::Buffer stageBuff2{ nullptr };
		vk::raii::DeviceMemory stageMemoryBuff2{ nullptr };

		createBuffer(size, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stageBuff, stageMemoryBuff);
		uniformBuffer.emplace_back(std::move(stageBuff));
		uniformBufferMemory.emplace_back(std::move(stageMemoryBuff));
		mappedData.emplace_back(uniformBufferMemory[i].mapMemory(0, size));

		createBuffer(size2, vk::BufferUsageFlagBits::eStorageBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stageBuff2, stageMemoryBuff2);
		uniformBuffer2.emplace_back(std::move(stageBuff2));
		uniformBufferMemory2.emplace_back(std::move(stageMemoryBuff2));
		mappedData2.emplace_back(uniformBufferMemory2[i].mapMemory(0, size2));
	}
}

void HelloTriangleApplication::createDescriptorPool() {
	std::array<vk::DescriptorPoolSize, 2> poolSize{
		vk::DescriptorPoolSize(vk::DescriptorType::eUniformBuffer, MAX_FRAMES_IN_FLIGHT),
		vk::DescriptorPoolSize(vk::DescriptorType::eStorageBuffer, MAX_FRAMES_IN_FLIGHT)
	};
	vk::DescriptorPoolCreateInfo layoutInfo{ .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, .maxSets = MAX_FRAMES_IN_FLIGHT, .poolSizeCount = 2, .pPoolSizes = poolSize.data() };
	descriptorPool = vk::raii::DescriptorPool(device, layoutInfo);
}

void HelloTriangleApplication::createDescriptorSets() {
	std::vector<vk::DescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, *descriptorSetLayout);
	vk::DescriptorSetAllocateInfo allocInfo{ .descriptorPool = descriptorPool, .descriptorSetCount = static_cast<uint32_t>(layouts.size()), .pSetLayouts = layouts.data() };

	descriptorSets.clear();
	descriptorSets = device.allocateDescriptorSets(allocInfo);

	for (int i{}; i < MAX_FRAMES_IN_FLIGHT; ++i) {
		vk::DescriptorBufferInfo bufferInfo{ .buffer = uniformBuffer[i], .offset = 0, .range = sizeof(UniformBufferObject) };
		vk::DescriptorBufferInfo bufferInfo2{ .buffer = uniformBuffer2[i], .offset = 0, .range = sizeof(GameObjectInfo) * gameObjects.vertices.size() };
		vk::WriteDescriptorSet descriptorWrite[2]{ 
			{.dstSet = descriptorSets[i], .dstBinding = 0, .dstArrayElement = 0, .descriptorCount = 1, .descriptorType = vk::DescriptorType::eUniformBuffer, .pBufferInfo = &bufferInfo },
			{.dstSet = descriptorSets[i], .dstBinding = 1, .dstArrayElement = 0, .descriptorCount = 1, .descriptorType = vk::DescriptorType::eStorageBuffer, .pBufferInfo = &bufferInfo2 }
		};


		
		device.updateDescriptorSets(descriptorWrite, {});
	}
}
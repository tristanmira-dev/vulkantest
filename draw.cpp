#include "HelloTriangleApplication.hpp"
#include "commonMath.hpp"
#include <iostream>

/*

Wait for the previous frame to finish

Acquire an image from the swap chain

Record a command buffer which draws the scene onto that image

Submit the recorded command buffer

Present the swap chain image

*/
void HelloTriangleApplication::drawFrame() {


	auto fenceRes{ device.waitForFences(*inFlightFences[frameIdx], vk::True, UINT32_MAX) };

	if (fenceRes != vk::Result::eSuccess) {
		throw std::runtime_error("Failed to wait for fence!");
	}

	auto [result, imageIndex] = swapChain.acquireNextImage(UINT32_MAX, *presentCompleteSemaphore[frameIdx] /*Using this as a signal*/, nullptr);

	if (result == vk::Result::eErrorOutOfDateKHR /* Usually happens after a window resize. */ ) {
		recreateSwapChain(); 
		return;
	} if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR) {
		throw("Failed to get swapchain image!");
	}

	commandBuffers[frameIdx].reset();

	device.resetFences(*inFlightFences[frameIdx]);

	updateUniformBuffer(frameIdx);

	recordCommandBuffer(imageIndex);

	vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput); /*only blocks when the gpu needs to write pixels to the image*/
	const vk::SubmitInfo submitInfo{ .waitSemaphoreCount = 1, .pWaitSemaphores = &*presentCompleteSemaphore[frameIdx], .pWaitDstStageMask = &waitDestinationStageMask, .commandBufferCount = 1, .pCommandBuffers = &*commandBuffers[frameIdx], .signalSemaphoreCount = 1, .pSignalSemaphores = &*renderFinishedSemaphore[imageIndex]};
	graphicsQueue.submit(submitInfo, *inFlightFences[frameIdx]);


	const vk::PresentInfoKHR presentInfoKHR{ .waitSemaphoreCount = 1, .pWaitSemaphores = &*renderFinishedSemaphore[imageIndex], .swapchainCount = 1, .pSwapchains = &*swapChain, .pImageIndices = &imageIndex};
	result = graphicsQueue.presentKHR(presentInfoKHR);
	if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR || frameBufferResized) {
		frameBufferResized = false;
		recreateSwapChain();
	}

	
	frameIdx = (frameIdx + 1) % MAX_FRAMES_IN_FLIGHT;
	
}

/*frame 0
fence wait frameIdx 0
reset fence frameIdx 0
get image [0]
reset command buffer
recordCommandBuffer imageIdx[0]
submit command buffer

*/


void HelloTriangleApplication::createSyncObjects() {


	for (size_t i{}; i < swapChainImages.size(); ++i) {
		renderFinishedSemaphore.emplace_back(vk::raii::Semaphore(device, vk::SemaphoreCreateInfo{}));
	}

	for (int i{}; i < MAX_FRAMES_IN_FLIGHT; ++i) {
		inFlightFences.emplace_back(vk::raii::Fence(device, vk::FenceCreateInfo{ .flags = vk::FenceCreateFlagBits::eSignaled }));
		presentCompleteSemaphore.emplace_back(vk::raii::Semaphore(device, vk::SemaphoreCreateInfo{}));

	}
	
}

float currentAngle{50.f};

void HelloTriangleApplication::updateUniformBuffer(uint32_t currentImg) {
	struct Cam {
		glm::vec3 pos{ 0.f, 3.f, -9.f };
	};

	Cam cam{};

	float rotationSpeed{ 1.f };
	UniformBufferObject ubo{};
	ubo.model = glm::rotate(glm::mat4(1.f), currentAngle += deltaTime * rotationSpeed, glm::vec3(0.f, 0.f, 1.f));

	glm::mat4 camTranslate{ glm::identity<glm::mat4>() };
	camTranslate[3][0] = -cam.pos.x;
	camTranslate[3][1] = -cam.pos.y;
	camTranslate[3][2] = -cam.pos.z;

	ubo.view = camTranslate;

	ubo.projection = projection(window, 45.f, 0.1f, 10.f);

	memcpy(mappedData[currentImg], &ubo, sizeof(ubo));
}

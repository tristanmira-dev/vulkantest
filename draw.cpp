#include "HelloTriangleApplication.hpp"

#include <iostream>

/*

Wait for the previous frame to finish

Acquire an image from the swap chain

Record a command buffer which draws the scene onto that image

Submit the recorded command buffer

Present the swap chain image

*/
void HelloTriangleApplication::drawFrame() {

	int secondBufferFrameIdx = frameIdx + 2;


	device.waitForFences(*inFlightFences[frameIdx], vk::True, UINT32_MAX);
	device.resetFences(*inFlightFences[frameIdx]);

	auto [result, imageIndex] = swapChain.acquireNextImage(UINT32_MAX, *presentCompleteSemaphore[frameIdx] /*Using this as a signal*/, nullptr);

	commandBuffers[frameIdx].reset();
	recordCommandBuffer(imageIndex);

	vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput); /*only blocks when the gpu needs to write pixels to the image*/
	const vk::SubmitInfo submitInfo{ .waitSemaphoreCount = 1, .pWaitSemaphores = &*presentCompleteSemaphore[frameIdx], .pWaitDstStageMask = &waitDestinationStageMask, .commandBufferCount = 1, .pCommandBuffers = &*commandBuffers[frameIdx], .signalSemaphoreCount = 1, .pSignalSemaphores = &*presentCompleteSemaphore[secondBufferFrameIdx]};
	graphicsQueue.submit(submitInfo, nullptr);

	commandBuffers[secondBufferFrameIdx].reset();
	recordCommandBuffer2(imageIndex);
	const vk::SubmitInfo submitInfo2{ .waitSemaphoreCount = 1, .pWaitSemaphores = &*presentCompleteSemaphore[secondBufferFrameIdx], . pWaitDstStageMask = &waitDestinationStageMask, .commandBufferCount = 1, .pCommandBuffers = &*commandBuffers[secondBufferFrameIdx], .signalSemaphoreCount = 1, .pSignalSemaphores = &*renderFinishedSemaphore[imageIndex] };
	graphicsQueue.submit(submitInfo2, *inFlightFences[frameIdx]);

	if (result != vk::Result::eSuccess) {
		throw std::runtime_error("failed to wait for fence!");
	}



	const vk::PresentInfoKHR presentInfoKHR{ .waitSemaphoreCount = 1, .pWaitSemaphores = &*renderFinishedSemaphore[imageIndex], .swapchainCount = 1, .pSwapchains = &*swapChain, .pImageIndices = &imageIndex};
	result = graphicsQueue.presentKHR(presentInfoKHR);
	
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

	for (int i{}; i < MAX_FRAMES_IN_FLIGHT * 2; ++i) {
		inFlightFences.emplace_back(vk::raii::Fence(device, vk::FenceCreateInfo{ .flags = vk::FenceCreateFlagBits::eSignaled }));
		presentCompleteSemaphore.emplace_back(vk::raii::Semaphore(device, vk::SemaphoreCreateInfo{}));

	}


	
}

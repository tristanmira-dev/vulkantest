#include "HelloTriangleApplication.hpp"

void HelloTriangleApplication::createCommandPool() {
	vk::CommandPoolCreateInfo poolInfo{.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer, .queueFamilyIndex = queueIdx};
	commandPool = vk::raii::CommandPool(device, poolInfo);
}

void HelloTriangleApplication::createCommandBuffer() {
	vk::CommandBufferAllocateInfo allocInfo{ .commandPool = commandPool, .level = vk::CommandBufferLevel::ePrimary, .commandBufferCount = 1 };
	
	commandBuffer = std::move(vk::raii::CommandBuffers(device, allocInfo).front());

}

void HelloTriangleApplication::recordCommandBuffer(uint32_t imageIdx) {
	/*
	imageIndex — which swapchain image you're transitioning
	eUndefined — old layout. "I don't care what it was before, throw away the contents"
	eColorAttachmentOptimal — new layout. "Rearrange it for drawing to"
	{} (srcAccessMask) — no access to wait on. Nothing was using this image before
	eColorAttachmentWrite — the next thing doing is writing color to it
	eColorAttachmentOutput (srcStage) — wait for the color output stage to be free
	eColorAttachmentOutput (dstStage) — the stage that will use it next is also color output
	*/
	transition_image_layout(imageIdx, vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal, {}, vk::AccessFlagBits2::eColorAttachmentWrite, vk::PipelineStageFlagBits2::eColorAttachmentOutput, vk::PipelineStageFlagBits2::eColorAttachmentOutput);
	vk::ClearValue clearColor{ vk::ClearColorValue(0.f,0.f,0.f,1.f) };

	vk::RenderingAttachmentInfo attachmentInfo{
		.imageView = swapChainImageViews[imageIdx],
		.imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
		.loadOp = vk::AttachmentLoadOp::eClear,
		.storeOp = vk::AttachmentStoreOp::eStore,
		.clearValue = clearColor
	};

	/*renderArea — draw over the whole swapchain (starting at 0,0, full extent)
	layerCount — 1, you're not doing stereoscopic/VR stuff
	colorAttachmentCount + pColorAttachments — points to that attachment info above*/
	vk::RenderingInfo renderingInfo{
		.renderArea = {},
		.layerCount = 1,
		.colorAttachmentCount = 1,
		.pColorAttachments = &attachmentInfo
	};


}
/*

srcStageMask — which pipeline stage was using this image before (e.g. color output, fragment shader)
srcAccessMask — what kind of memory access was happening before (e.g. write, read)
dstStageMask — which pipeline stage will use it next
dstAccessMask — what kind of memory access will happen next
oldLayout — current image layout
newLayout — layout you want it to transition to
srcQueueFamilyIndex / dstQueueFamilyIndex — for transferring image ownership between queue families (e.g. graphics → compute). IGNORED means "same queue, don't care"
image — which image you're transitioning
subresourceRange — which part of the image:
aspectMask — color data (not depth/stencil)
baseMipLevel / levelCount — which mip levels (you have 1, starting at 0)
baseArrayLayer / layerCount — which array layers (you have 1, starting at 0). Array layers are for things like cubemaps which have 6 faces in one image

*/
void HelloTriangleApplication::transition_image_layout(uint32_t imageIdx, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::AccessFlags2 srcAccessMask, vk::AccessFlags2 dstAccessMask, vk::PipelineStageFlags2 srcStageMask, vk::PipelineStageFlags2 dstStageMask) {
	vk::ImageMemoryBarrier2 imageTransitionAndWait{
		.srcStageMask = srcStageMask,
		.srcAccessMask = srcAccessMask,
		.dstStageMask = dstStageMask,
		.dstAccessMask = dstAccessMask,
		.oldLayout = oldLayout,
		.newLayout = newLayout,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = swapChainImages[imageIdx],
		.subresourceRange = {
			.aspectMask = vk::ImageAspectFlagBits::eColor,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		}
	};


	vk::DependencyInfo dependencyInfo{
		.dependencyFlags = {},
		.imageMemoryBarrierCount = 1,
		.pImageMemoryBarriers = &imageTransitionAndWait /* "everything before this barrier must finish before anything after it can start.", The layout transition is the barrier. It's the wall itself. */
	};

	commandBuffer.pipelineBarrier2(dependencyInfo);
}

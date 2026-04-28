#include "HelloTriangleApplication.hpp"

void HelloTriangleApplication::createCommandPool() {
	vk::CommandPoolCreateInfo poolInfo{.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer, .queueFamilyIndex = queueIdx};
	commandPool = vk::raii::CommandPool(device, poolInfo);
}

void HelloTriangleApplication::createCommandBuffer() {
	commandBuffers.clear();
	vk::CommandBufferAllocateInfo allocInfo{ .commandPool = commandPool, .level = vk::CommandBufferLevel::ePrimary, .commandBufferCount = MAX_FRAMES_IN_FLIGHT };
	
	commandBuffers = vk::raii::CommandBuffers(device, allocInfo);

}


void HelloTriangleApplication::recordCommandBuffer(uint32_t imageIdx) {

	auto &commandBuffer{commandBuffers[frameIdx]};

	commandBuffer.begin({});

	/*
	imageIndex — which swapchain image you're transitioning
	eUndefined — old layout. "I don't care what it was before, throw away the contents"
	eColorAttachmentOptimal — new layout. "Rearrange it for drawing to"
	{} (srcAccessMask) — no access to wait on. Nothing was using this image before
	eColorAttachmentWrite — the next thing doing is writing color to it
	eColorAttachmentOutput (srcStage) — wait for the color output stage to be free
	eColorAttachmentOutput (dstStage) — the stage that will use it next is also color output
	*/
	transition_image_layout(swapChainImages[imageIdx], vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal, {}, vk::AccessFlagBits2::eColorAttachmentWrite, vk::PipelineStageFlagBits2::eColorAttachmentOutput, vk::PipelineStageFlagBits2::eColorAttachmentOutput, vk::ImageAspectFlagBits::eColor);
	transition_image_layout(
		*depthImage,
		vk::ImageLayout::eUndefined,
		vk::ImageLayout::eDepthAttachmentOptimal,
		vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
		vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
		vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests,
		vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests,
		vk::ImageAspectFlagBits::eDepth
	);

	vk::ClearValue clearColor{ vk::ClearColorValue(0.f,0.f,0.f,1.f) };
	vk::ClearDepthStencilValue depthClearValue{ .depth = 1.f, .stencil = 0 };

	/*imageView — which swapchain image view to render to
	imageLayout — the layout it's in (you just transitioned it to COLOR_ATTACHMENT_OPTIMAL)
	loadOp — what to do before rendering. Clear = wipe it to black first
	storeOp — what to do after rendering. Store = keep the pixels (you need them for presenting)
	clearValue — the black color (RGBA 0,0,0,1)*/
	vk::RenderingAttachmentInfo attachmentInfo{
		.imageView = swapChainImageViews[imageIdx],
		.imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
		.loadOp = vk::AttachmentLoadOp::eClear,
		.storeOp = vk::AttachmentStoreOp::eStore,
		.clearValue = clearColor
	};

	vk::RenderingAttachmentInfo depthAttachmentInfo{
		.imageView = depthImageView,
		.imageLayout = vk::ImageLayout::eDepthAttachmentOptimal,
		.loadOp = vk::AttachmentLoadOp::eClear,
		.storeOp = vk::AttachmentStoreOp::eDontCare,
		.clearValue = depthClearValue
	};

	/*"color attachment" = the image where your pixel colors end up. Your swapchain image is the color attachment.*/

	/*renderArea — draw over the whole swapchain (starting at 0,0, full extent)
	layerCount — 1, you're not doing stereoscopic/VR stuff
	colorAttachmentCount + pColorAttachments — points to that attachment info above*/
	vk::RenderingInfo renderingInfo{
		.renderArea = {.offset = {0, 0}, .extent = swapChainExtent},
		.layerCount = 1,
		.colorAttachmentCount = 1,
		.pColorAttachments = &attachmentInfo,
		.pDepthAttachment = &depthAttachmentInfo
	};

	commandBuffer.beginRendering(renderingInfo);

	commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);


	commandBuffer.bindVertexBuffers(0, *vertexBuffer, { 0 });
	commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, *descriptorSets[frameIdx], nullptr);
	commandBuffer.bindIndexBuffer(*indexBuffer, 0, vk::IndexType::eUint32);

	commandBuffer.setViewport(0, vk::Viewport(0.f, 0.f, static_cast<float>(swapChainExtent.width), static_cast<float>(swapChainExtent.height), 0.f, 1.f));
	commandBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), swapChainExtent));


	size_t totalGameObjsP{ gameObjects.gameObjInfoCollection.size() };
	for (size_t i{}; i < totalGameObjsP; ++i) {
		commandBuffer.pushConstants<int>(pipelineLayout, vk::ShaderStageFlagBits::eVertex, 0, static_cast<int>(i));
		commandBuffer.drawIndexed(gameObjects.verticesInformation[gameObjects.meshIdx[i]].totalIndices, 1,  gameObjects.verticesInformation[gameObjects.meshIdx[i]].start - gameObjects.indices.begin(), 0, 0);
	}


	commandBuffer.endRendering();

	/*
	
		UNDEFINED → COLOR_ATTACHMENT_OPTIMAL

		"I don't care what state this image was in, just make it ready for drawing"
		Happens before beginRendering
		COLOR_ATTACHMENT_OPTIMAL (during rendering)

		GPU is actively writing pixels to it — your triangle gets drawn here
		COLOR_ATTACHMENT_OPTIMAL → PRESENT_SRC_KHR

		"Done drawing, rearrange it for showing on screen"
		Happens after endRendering
		PRESENT_SRC_KHR (presenting)

		Image gets sent to the display
	
	*/

	transition_image_layout(swapChainImages[imageIdx], vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::ePresentSrcKHR, vk::AccessFlagBits2::eColorAttachmentWrite, {}, vk::PipelineStageFlagBits2::eColorAttachmentOutput, vk::PipelineStageFlagBits2::eBottomOfPipe, vk::ImageAspectFlagBits::eColor);

	commandBuffer.end();
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
void HelloTriangleApplication::transition_image_layout(vk::Image const &image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::AccessFlags2 srcAccessMask, vk::AccessFlags2 dstAccessMask, vk::PipelineStageFlags2 srcStageMask, vk::PipelineStageFlags2 dstStageMask, vk::ImageAspectFlags aspectFlag) {
	vk::ImageMemoryBarrier2 imageTransitionAndWait{
		.srcStageMask = srcStageMask,
		.srcAccessMask = srcAccessMask,
		.dstStageMask = dstStageMask,
		.dstAccessMask = dstAccessMask,
		.oldLayout = oldLayout,
		.newLayout = newLayout,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = image,
		.subresourceRange = {
			.aspectMask = aspectFlag,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		}
	};


	vk::DependencyInfo dependencyInfo{
		.dependencyFlags = {},
		.imageMemoryBarrierCount = 1,
		.pImageMemoryBarriers = &imageTransitionAndWait /* "everything before this barrier must finish before anything after it can start.", The layout transition is the barrier */
	};

	commandBuffers[frameIdx].pipelineBarrier2(dependencyInfo);
}

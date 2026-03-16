#include "HelloTriangleApplication.hpp"
#include "readShaders.hpp"
#include <iostream>
#include <vulkan/vulkan_raii.hpp>

vk::raii::ShaderModule HelloTriangleApplication::createShaderModule(std::vector<char> const &shader) const {
	vk::ShaderModuleCreateInfo createInfo{ .codeSize = shader.size() * sizeof(char), .pCode = reinterpret_cast<const uint32_t*>(shader.data()) };
	return vk::raii::ShaderModule { device, createInfo };
}


void HelloTriangleApplication::createGraphicsPipeline() {

	/*SHADERS---------------------*/
	vk::raii::ShaderModule shaderModule{createShaderModule(readFile(std::string{"../shaders/slang.spv"}))}; /*note, probably need to find a better way to represent these dirs via cmake*/

	vk::PipelineShaderStageCreateInfo vertShaderStageInfo{ .stage = vk::ShaderStageFlagBits::eVertex, .module = shaderModule, .pName = "vertMain" };
	vk::PipelineShaderStageCreateInfo fragShaderStageInfo{ .stage = vk::ShaderStageFlagBits::eFragment, .module = shaderModule, .pName = "fragMain" };

	vk::PipelineShaderStageCreateInfo shadersStage[] = { vertShaderStageInfo, fragShaderStageInfo };



	/*DYNAMIC STATES---------------*/
	std::vector<vk::DynamicState> dynamicStates{vk::DynamicState::eViewport, vk::DynamicState::eScissor}; //This will cause the configuration of these values to be ignored, and you will be able (and required) to specify the data at drawing time
	vk::PipelineDynamicStateCreateInfo dynamicState{ .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()), .pDynamicStates = dynamicStates.data() };



	/*Vertex input----------------*/
	vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};

	/*Input Assembly(Type of geometry to draw)*/
	vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo{ .topology = vk::PrimitiveTopology::eTriangleList };

	/*Viewports and Scissors*/
		/*Viewport = where to draw
		Scissor = what to keep*/
		/*insert here if you want a static, unchanging viewport or scissor 
			vk::PipelineViewportStateCreateInfo viewportState(
				.viewportCount = 1,
				.pViewports = &viewport,
				.scissorCount = 1,
				.pScissors = &scissor
			);
		*/
	vk::PipelineViewportStateCreateInfo viewportState{ .viewportCount = 1, .scissorCount = 1 };

	/*Independent of how you set them, it’s possible to use multiple viewports and scissor rectangles on some graphics cards, so the structure members reference an array of them. Using multiple requires enabling a GPU feature (see logical device creation).*/


	/*Rasterizer*/
	vk::PipelineRasterizationStateCreateInfo rasterizer{ .depthClampEnable = vk::False, .rasterizerDiscardEnable = vk::False, .polygonMode = vk::PolygonMode::eFill, .cullMode = vk::CullModeFlagBits::eBack,
		.frontFace = vk::FrontFace::eClockwise, .depthBiasEnable = vk::False, .depthBiasSlopeFactor = 1.f, .lineWidth = 1.f
	};

	/*Multisampling*/
	vk::PipelineMultisampleStateCreateInfo multisampling{ .rasterizationSamples = vk::SampleCountFlagBits::e1, .sampleShadingEnable = vk::False };


	/*Depth and stencil testing*/
	/*VkPipelineDepthStencilStateCreateInfo. We don’t have one right now, so we can simply pass a nullptr instead of a pointer to such a struct. We’ll get back to it in the depth buffering chapter.*/

	/*Color Blending*/
	/*color blending is essentially just for either using whats already on the framebuffer and blending it with the fragment shader output or just taking the framebuffer output directly*/
	vk::PipelineColorBlendAttachmentState colorBlendAttachment{.blendEnable = vk::False, .colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA };
	vk::PipelineColorBlendStateCreateInfo colorBlending{ .logicOpEnable = vk::False, .logicOp = vk::LogicOp::eCopy, .attachmentCount = 1, .pAttachments = &colorBlendAttachment };


	/*Pipeline Layout*/
	vk::PipelineLayoutCreateInfo layoutCreateInfo{ .setLayoutCount = 0, .pushConstantRangeCount = 0 }; /*"my shaders don't use any uniforms or push constants right now."*/
	pipelineLayout = vk::raii::PipelineLayout(device, layoutCreateInfo);

	/*Rendering, Pipeline creation*/
	vk::StructureChain<vk::GraphicsPipelineCreateInfo, vk::PipelineRenderingCreateInfo> chain{
		{
			.stageCount = 2, .pStages = shadersStage, .pVertexInputState = &vertexInputInfo, .pInputAssemblyState = &inputAssemblyInfo,
			.pViewportState = &viewportState, .pRasterizationState = &rasterizer, .pMultisampleState = &multisampling,
			.pColorBlendState = &colorBlending,
			.pDynamicState = &dynamicState,
			.layout = pipelineLayout,
			.renderPass = nullptr /*Note that the renderPass parameter is set to nullptr because we’re using dynamic rendering instead of a traditional render pass.*/
		},
		{.colorAttachmentCount = 1, .pColorAttachmentFormats = &swapChainSurfaceFormat.format}
	};

	/*
	

		Shader stages: the shader modules that define the functionality of the programmable stages of the graphics pipeline

		Fixed-function state: all the structures that define the fixed-function stages of the pipeline, like input assembly, rasterizer, viewport and color blending

		Pipeline layout: the uniform and push values referenced by the shader that can be updated at draw time

		Dynamic rendering: the formats of the attachments that will be used during rendering

	
	*/

	pipeline = vk::raii::Pipeline(device, nullptr, chain.get<vk::GraphicsPipelineCreateInfo>());


	std::cout << "Pipeline created\n";


}
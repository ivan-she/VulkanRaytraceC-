#include "app.hpp"
#include <stdexcept>
namespace rt {


	App::App()
	{
		createPipelineLayout();
		createPipeline();
		createCommadBuffers();
	}

	App::~App()
	{
		vkDestroyPipelineLayout(rtDevice.device(), pipelineLayout, nullptr);
	}

	void App::run(){
		while (!rtWindow.shouldClose())
		{
			glfwPollEvents();
		}
	}

	void App::createPipelineLayout()
	{
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;
		if (vkCreatePipelineLayout(rtDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed To create pipeline layout!");
		}

	}

	void App::createPipeline() 
	{
		auto pipelineConfig = RtPipeline::defaultPipelineConfigInfo(rtSwapChain.width(), rtSwapChain.height());

		pipelineConfig.renderPass = rtSwapChain.getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		rtPipeline = std::make_unique<RtPipeline>(
			rtDevice,
			"simp_shader.vert.spv",
			"simp_shader.frag.spv",
			pipelineConfig);

	}
	void App::createCommadBuffers() {}
	void App::drawframe() {}


}
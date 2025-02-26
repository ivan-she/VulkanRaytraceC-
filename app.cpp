#include "app.hpp"
#include <stdexcept>
#include <array>
namespace rt {


	App::App()
	{
		loadModels();
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
			drawFrame();
		}

		vkDeviceWaitIdle(rtDevice.device());
	}


	void App::loadModels()
	{
		std::vector<RtModel::Vertex> vertices{
			{{0.0f,-0.5f}},
			{{0.5f,0.5f}},
			{{-0.5f,0.0f}},
		};

		rtModel = std::make_unique<RtModel>(rtDevice, vertices);
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
	void App::createCommadBuffers() 
	{
		commandBuffers.resize(rtSwapChain.imageCount());
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = rtDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(rtDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate command buffers!");
		}

		for (int i = 0; i < commandBuffers.size(); i++)
		{
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

			if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to begin recording command buffer!");
			}

			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = rtSwapChain.getRenderPass();
			renderPassInfo.framebuffer = rtSwapChain.getFrameBuffer(i);

			renderPassInfo.renderArea.offset = { 0,0 };
			renderPassInfo.renderArea.extent = rtSwapChain.getSwapChainExtent();

			std::array<VkClearValue, 2> clearValues{};
			clearValues[0].color = { 0.1f,0.1f,0.1f,1.0f };
			clearValues[1].depthStencil = { 1.0f, 0 };

			renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassInfo.pClearValues = clearValues.data();

			vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			rtPipeline->bind(commandBuffers[i]);

			rtModel->bind(commandBuffers[i]);
			rtModel->draw(commandBuffers[i]);


			vkCmdEndRenderPass(commandBuffers[i]);
			if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to record command buffer!");
			}

		}


	}
	void App::drawFrame() 
	{
		uint32_t imageIndex;
		auto result = rtSwapChain.acquireNextImage(&imageIndex);

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) 
		{
			throw std::runtime_error("Failed to acquire swapchain image");
		}

		result = rtSwapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);

		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to present swapchain image!");
		}
	}


}
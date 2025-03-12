#include "app.hpp"
#include <stdexcept>
#include <array>
namespace rt {


	App::App()
	{
		loadModels();
		createPipelineLayout();
		recreateSwapChain();
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
			{{0.0f,-0.5f}, { 1.0f,0.0f,0.0f }},
			{{0.5f,0.5f}, { 0.0f,0.0f,1.0f }},
			{{-0.5f,0.5f}, { 0.0f,1.0f,0.0f }}
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
		assert(rtSwapChain != nullptr && "Cannot create pipeline before swapchain");
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipelinelayout");

		PipelineConfigInfo pipelineConfig{};

		RtPipeline::defaultPipelineConfigInfo(pipelineConfig);

		pipelineConfig.renderPass = rtSwapChain->getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		rtPipeline = std::make_unique<RtPipeline>(
			rtDevice,
			"simp_shader.vert.spv",
			"simp_shader.frag.spv",
			pipelineConfig);

	}

	void App::recreateSwapChain()
	{
		auto extent = rtWindow.getExtent();
		while (extent.width == 0 || extent.height == 0)
		{
			extent = rtWindow.getExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(rtDevice.device());


		if (rtSwapChain = nullptr)
		{
			rtSwapChain = std::make_unique<RtSwapChain>(rtDevice, extent);
		}
		else
		{
			rtSwapChain = std::make_unique<RtSwapChain>(rtDevice, extent, std::move(rtSwapChain));
			if (rtSwapChain->imageCount() != commandBuffers.size())
			{
				freeCommandBuffers();
				createCommadBuffers();
			}

		}

		createPipeline();
	}

	void App::createCommadBuffers() 
	{
		commandBuffers.resize(rtSwapChain->imageCount());
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = rtDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(rtDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate command buffers!");
		}

	}
	void App::freeCommandBuffers()
	{
		vkFreeCommandBuffers(rtDevice.device(), rtDevice.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
		commandBuffers.clear();
	}

	void App::recordCommandBuffer(int imageIndex)
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = rtSwapChain->getRenderPass();
		renderPassInfo.framebuffer = rtSwapChain->getFrameBuffer(imageIndex);

		renderPassInfo.renderArea.offset = { 0,0 };
		renderPassInfo.renderArea.extent = rtSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.1f,0.1f,0.1f,1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(rtSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(rtSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0,0},rtSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
		vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);
	

		rtPipeline->bind(commandBuffers[imageIndex]);
		rtModel->bind(commandBuffers[imageIndex]);
		rtModel->draw(commandBuffers[imageIndex]);


		vkCmdEndRenderPass(commandBuffers[imageIndex]);
		if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to record command buffer!");
		}
	}

	void App::drawFrame() 
	{
		uint32_t imageIndex;
		auto result = rtSwapChain->acquireNextImage(&imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapChain();
			return;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) 
		{
			throw std::runtime_error("Failed to acquire swapchain image");
		}

		recordCommandBuffer(imageIndex);

		result = rtSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || rtWindow.wasWindowResized())
		{
			rtWindow.resetWindowResizedFlag();
			recreateSwapChain();
			return;
		}

		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to present swapchain image!");
		}
	}


}
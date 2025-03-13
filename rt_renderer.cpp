#include "rt_renderer.hpp"
#include <stdexcept>
#include <array>

#include <iostream>


namespace rt {

	

	RtRenderer::RtRenderer(RtWindow& window, RtDevice& device) : rtWindow{ window }, rtDevice{device}
	{

		recreateSwapChain();
		createCommadBuffers();
	}

	RtRenderer::~RtRenderer()
	{
		freeCommandBuffers();
	}

	void RtRenderer::recreateSwapChain()
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
			std::shared_ptr<RtSwapChain> oldSwapChain = std::move(rtSwapChain);
			rtSwapChain = std::make_unique<RtSwapChain>(rtDevice, extent, oldSwapChain);

			if (!oldSwapChain->compareSwapFormats(*rtSwapChain.get()))
			{
				throw std::runtime_error("Swap chain image or deph format has changed!");
			}



		}

		//
	}



	void RtRenderer::createCommadBuffers()
	{
		commandBuffers.resize(RtSwapChain::MAX_FRAMES_IN_FLIGHT);
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
	void RtRenderer::freeCommandBuffers()
	{
		vkFreeCommandBuffers(rtDevice.device(), rtDevice.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
		commandBuffers.clear();
	}



	VkCommandBuffer RtRenderer::beginFrame()
	{
		assert(!isFrameStarted && "Can't call BeginFrame() While already in progress");
		
		auto result = rtSwapChain->acquireNextImage(&currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("Failed to acquire swapchain image");
		}
		isFrameStarted = true;

		auto commandBuffer = getCurrentCommandBuffer();
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to begin recording command buffer!");
		}
		return commandBuffer;
	}
	void RtRenderer::endFrame()
	{
		assert(isFrameStarted && "Can't call EndFrame() while frame is not in Progress");
		auto commandBuffer = getCurrentCommandBuffer();
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to record command buffer!");
		}

		auto result = rtSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || rtWindow.wasWindowResized())
		{
			rtWindow.resetWindowResizedFlag();
			recreateSwapChain();
		}else if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to present swapchain image!");
		}
		isFrameStarted = false;

		currentFrameIndex = (currentFrameIndex + 1) % RtSwapChain::MAX_FRAMES_IN_FLIGHT;
	}

	void RtRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(isFrameStarted && "Can't call BeginSwapChainRenderPass() if frame is not in progress");
		assert(commandBuffer == getCurrentCommandBuffer() && "Can't beginRenderPass() on command buffer from diffrent name");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = rtSwapChain->getRenderPass();
		renderPassInfo.framebuffer = rtSwapChain->getFrameBuffer(currentImageIndex);

		renderPassInfo.renderArea.offset = { 0,0 };
		renderPassInfo.renderArea.extent = rtSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f,0.01f,0.01f,1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(rtSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(rtSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0,0},rtSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}
	void RtRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(isFrameStarted && "Can't call EndSwapChainRenderPass() if frame is not in progress");
		assert(commandBuffer == getCurrentCommandBuffer() && "Can't endRenderPass() on command buffer from diffrent name");


		vkCmdEndRenderPass(commandBuffer);
	}


}
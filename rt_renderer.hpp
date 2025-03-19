#pragma once

#include "rt_window.hpp"

#include "rt_device.hpp"

#include "rt_swap_chain.hpp"

#include <cassert>
#include <memory>
#include <vector>

namespace rt {
	class RtRenderer {
	public:
		RtRenderer(RtWindow &window,RtDevice &device);
		~RtRenderer();

		RtRenderer(const RtRenderer &) = delete;
		RtRenderer& operator=(const RtRenderer &) = delete;

		VkRenderPass getSwapChainRenderPass() const { return rtSwapChain->getRenderPass(); }
		float getAspectRation() const { return rtSwapChain->extentAspectRatio(); }

		bool isFrameInProgress() const { return isFrameStarted; }

		VkCommandBuffer getCurrentCommandBuffer() const
		{
			assert(isFrameStarted && "Cannot Get Command Buffer when frame is not in progress");
			return commandBuffers[currentFrameIndex];
		}

		int getFrameIndex() const 
		{
			assert(isFrameStarted && "Cannot GetFrameIndex() when frame is not in progress");
			return currentFrameIndex;
		}

		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);


	private:


		void createCommadBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();

		RtWindow& rtWindow;
		RtDevice& rtDevice;
		std::unique_ptr<RtSwapChain> rtSwapChain;

		std::vector<VkCommandBuffer> commandBuffers;

		uint32_t currentImageIndex;
		int currentFrameIndex{0};

		bool isFrameStarted{false};
	};
}
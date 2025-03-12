#pragma once

#include "rt_window.hpp"
#include "rt_pipeline.hpp"
#include "rt_device.hpp"
#include "rt_swap_chain.hpp"
#include "rt_model.hpp"

#include <memory>
#include <vector>

namespace rt {
	class App {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		App();
		~App();

		App(const App&) = delete;
		App& operator=(const App&) = delete;

		void run();

	private:
		void loadModels();
		void createPipelineLayout();
		void createPipeline();
		void createCommadBuffers();
		void drawFrame();
		void recreateSwapChain();
		void recordCommandBuffer(int imageIndex);

		RtWindow rtWindow{ WIDTH,HEIGHT,"Hello ray trace!" };
		RtDevice rtDevice{rtWindow};
		std::unique_ptr<RtSwapChain> rtSwapChain;

		std::unique_ptr<RtPipeline> rtPipeline;
		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;
		std::unique_ptr<RtModel> rtModel;

	};
}
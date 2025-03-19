#pragma once

#include "rt_camera.hpp"
#include "rt_pipeline.hpp"
#include "rt_device.hpp"
#include "game_object.hpp"



#include <memory>
#include <vector>

namespace rt {
	class RenderSystem {
	public:


		RenderSystem(RtDevice &device, VkRenderPass renderPass);
		~RenderSystem();

		RenderSystem(const RenderSystem&) = delete;
		RenderSystem& operator=(const RenderSystem&) = delete;


		void renderObjects(VkCommandBuffer commandBuffer, std::vector<RtObject> &objects, const RtCamera &camera);

	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);






		RtDevice &rtDevice;

		std::unique_ptr<RtPipeline> rtPipeline;
		VkPipelineLayout pipelineLayout;

	};
}
#include "render_system.hpp"
#include <stdexcept>
#include <array>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <iostream>


namespace rt {

	struct SimplePushConstantData
	{
		glm::mat4 transform{ 1.f };
		alignas(16) glm::vec3 color;
	};


	RenderSystem::RenderSystem(RtDevice& device, VkRenderPass renderPass) : rtDevice{device}
	{
		createPipelineLayout();
		createPipeline(renderPass);

	}

	RenderSystem::~RenderSystem()
	{
		vkDestroyPipelineLayout(rtDevice.device(), pipelineLayout, nullptr);
	}



	void RenderSystem::createPipelineLayout()
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);



		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
		if (vkCreatePipelineLayout(rtDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed To create pipeline layout!");
		}

	}

	void RenderSystem::createPipeline(VkRenderPass renderPass)
	{

		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipelinelayout");

		PipelineConfigInfo pipelineConfig{};

		RtPipeline::defaultPipelineConfigInfo(pipelineConfig);

		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		rtPipeline = std::make_unique<RtPipeline>(
			rtDevice,
			"simp_shader.vert.spv",
			"simp_shader.frag.spv",
			pipelineConfig);

	}

	void RenderSystem::renderObjects(VkCommandBuffer commandBuffer, std::vector<RtObject>& objects, const RtCamera& camera)
	{


		rtPipeline->bind(commandBuffer);
		auto projectionView = camera.getProjection() * camera.getView();

		for (auto& obj : objects)
		{


			SimplePushConstantData push{};
			push.color = obj.color;
			push.transform = projectionView * obj.transform.mat4();
			vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
			obj.model->bind(commandBuffer);
			obj.model->draw(commandBuffer);
		}
	}

}
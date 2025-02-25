#pragma

#include "rt_device.hpp"

#include <string>
#include <vector>

namespace rt 
{

	struct PipelineConfigInfo {
		VkViewport viewport;
		VkRect2D scissor;

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};

	class RtPipeline {
	public:
		RtPipeline(
			RtDevice &device,
			const std::string& vertFilepath, 
			const std::string& fragFilepath, 
			const PipelineConfigInfo& configInfo);
		~RtPipeline();

		RtPipeline(const RtPipeline&) = delete;
		void operator=(const RtPipeline&) = delete;


		void bind(VkCommandBuffer commandBuffer);


		static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);


	private:
		static std::vector<char> readFile(const std::string& filepath);

		void createGraphicsPipeline(
			const std::string& vertFilepath,
			const std::string& fragFilepath,
			const PipelineConfigInfo& configInfo);

		void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

		RtDevice& rtDevice;
		VkPipeline graphicsPipeline;
		VkShaderModule vertShaderModule;
		VkShaderModule fragShadereModule;

	};
}
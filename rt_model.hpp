#pragma once

#include "rt_device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>

namespace rt{

	class RtModel {
	public:

		struct Vertex
		{
			glm::vec2 position;
			glm::vec3 color;
			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttrivuteDescriptions();
		};

		RtModel(RtDevice &device, const std::vector<Vertex> vertices);
		~RtModel();

		RtModel(const RtModel&) = delete;
		RtModel& operator=(const RtModel&) = delete;

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

	private:

		void createVertexBuffers(const std::vector<Vertex> vertices );
		RtDevice& rtDevice;
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		uint32_t vertexCount;
	};
}
#pragma once

#include "rt_device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <memory>
#include <vector>

namespace rt{

	class RtModel {
	public:

		struct Vertex
		{
			glm::vec3 position;
			glm::vec3 color;
			glm::vec3 normal{};
			glm::vec2 uv{};


			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttrivuteDescriptions();
		};

		struct Builder {
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};

			void loadModel(const std::string& filepath);

		};

		RtModel(RtDevice &device, const RtModel::Builder &builder);
		~RtModel();

		RtModel(const RtModel&) = delete;
		RtModel& operator=(const RtModel&) = delete;

		static std::unique_ptr<RtModel> creatModelFromFile(RtDevice &device, const std::string& filepath);


		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

	private:

		void createVertexBuffers(const std::vector<Vertex> vertices );
		void createIndexBuffers(const std::vector<uint32_t> indices);

		RtDevice& rtDevice;


		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		uint32_t vertexCount;

		bool hasIndexBuffer = false;
		VkBuffer indexBuffer;
		VkDeviceMemory indexBufferMemory;
		uint32_t indexCount;
	};
}
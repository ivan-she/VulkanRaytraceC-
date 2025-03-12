#include "rt_model.hpp"

#include <cassert>
#include <cstring>

namespace rt {

	RtModel::RtModel(RtDevice& device, const std::vector<Vertex> vertices) : rtDevice{device} {
		createVertexBuffers(vertices);
	}
	RtModel::~RtModel() {
		vkDestroyBuffer(rtDevice.device(), vertexBuffer, nullptr);
		vkFreeMemory(rtDevice.device(), vertexBufferMemory, nullptr);
	}

	void RtModel::createVertexBuffers(const std::vector<Vertex> vertices) {
		vertexCount = static_cast<uint32_t>(vertices.size());
		assert(vertexCount >= 3 && "Vertex count must be atleast 3!");
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
		rtDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			vertexBuffer,
			vertexBufferMemory
		);

		void* data;
		vkMapMemory(rtDevice.device(), vertexBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(rtDevice.device(), vertexBufferMemory);

	}

	void RtModel::draw(VkCommandBuffer commandBuffer) 
	{
		vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
	}

	void RtModel::bind(VkCommandBuffer commandBuffer)
	{
		VkBuffer buffers[] = { vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

	}

	std::vector<VkVertexInputBindingDescription> RtModel::Vertex::getBindingDescriptions() {
		return { {0,sizeof(Vertex),VK_VERTEX_INPUT_RATE_VERTEX}};

	}

	std::vector<VkVertexInputAttributeDescription> RtModel::Vertex::getAttrivuteDescriptions() {
		return { {0,0,VK_FORMAT_R32G32_SFLOAT,offsetof(Vertex,position)},{1,0,VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex,color)}};
	}
}
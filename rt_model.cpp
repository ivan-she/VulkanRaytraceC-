#include "rt_model.hpp"

#define TINYOBJLOADER_IMPLEMENTATION

#include <tiny_obj_loader.h>
#include <cassert>
#include <cstring>
#include <iostream>

namespace rt {

	RtModel::RtModel(RtDevice& device, const RtModel::Builder &builder) : rtDevice{device} {
		createVertexBuffers(builder.vertices);
		createIndexBuffers(builder.indices);
	}
	RtModel::~RtModel() {
		vkDestroyBuffer(rtDevice.device(), vertexBuffer, nullptr);
		vkFreeMemory(rtDevice.device(), vertexBufferMemory, nullptr);

		if (hasIndexBuffer)
		{
			vkDestroyBuffer(rtDevice.device(), indexBuffer, nullptr);
			vkFreeMemory(rtDevice.device(), indexBufferMemory, nullptr);
		}
	}

	std::unique_ptr<RtModel> RtModel::creatModelFromFile(RtDevice& device, const std::string& filepath) 
	{
		Builder builder{};
		builder.loadModel(filepath);
		std::cout << "Vertex Count:" << builder.vertices.size() << "\n";
		return std::make_unique<RtModel>(device, builder);
	}

	void RtModel::createVertexBuffers(const std::vector<Vertex> vertices) {
		vertexCount = static_cast<uint32_t>(vertices.size());
		assert(vertexCount >= 3 && "Vertex count must be atleast 3!");
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		rtDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer,
			stagingBufferMemory
		);

		void* data;
		vkMapMemory(rtDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(rtDevice.device(), stagingBufferMemory);

		rtDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			vertexBuffer,
			vertexBufferMemory);

		rtDevice.copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

		vkDestroyBuffer(rtDevice.device(), stagingBuffer, nullptr);
		vkFreeMemory(rtDevice.device(), stagingBufferMemory, nullptr);

	}

	void RtModel::createIndexBuffers(const std::vector<uint32_t> indecis) {
		indexCount = static_cast<uint32_t>(indecis.size());
		hasIndexBuffer = indexCount > 0;
		if (!hasIndexBuffer)
		{
			return;
		}
		VkDeviceSize bufferSize = sizeof(indecis[0]) * indexCount;
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		rtDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer,
			stagingBufferMemory
		);

		void* data;
		vkMapMemory(rtDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, indecis.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(rtDevice.device(), stagingBufferMemory);

		rtDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			indexBuffer,
			indexBufferMemory);

		rtDevice.copyBuffer(stagingBuffer, indexBuffer, bufferSize);

		vkDestroyBuffer(rtDevice.device(), stagingBuffer, nullptr);
		vkFreeMemory(rtDevice.device(), stagingBufferMemory, nullptr);

	}

	void RtModel::draw(VkCommandBuffer commandBuffer) 
	{
		if (hasIndexBuffer)
		{
			vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
		}
		else {
			vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
		}
	}

	void RtModel::bind(VkCommandBuffer commandBuffer)
	{
		VkBuffer buffers[] = { vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
		if (hasIndexBuffer)
		{
			vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		}
	}

	std::vector<VkVertexInputBindingDescription> RtModel::Vertex::getBindingDescriptions() {
		return { {0,sizeof(Vertex),VK_VERTEX_INPUT_RATE_VERTEX}};

	}

	std::vector<VkVertexInputAttributeDescription> RtModel::Vertex::getAttrivuteDescriptions() {
		return { {0,0,VK_FORMAT_R32G32B32_SFLOAT,offsetof(Vertex,position)},{1,0,VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex,color)}};
	}

	void RtModel::Builder::loadModel(const std::string& filepath)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;

		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib,&shapes, &materials, &warn,&err,filepath.c_str()))
		{
			throw std::runtime_error(warn + err);
		}
		vertices.clear();
		indices.clear();

		for (const auto &shape : shapes)
		{
			for (const auto &index : shape.mesh.indices)
			{
				Vertex vertex{};

				if (index.vertex_index >= 0)
				{
					vertex.position = { attrib.vertices[3 * index.vertex_index + 0],attrib.vertices[3 * index.vertex_index + 1],attrib.vertices[3 * index.vertex_index + 2] };

					auto colorIndex = 3 * index.vertex_index + 2;
					if (colorIndex < attrib.colors.size())
					{
						vertex.color = { attrib.colors[colorIndex -2],attrib.colors[colorIndex - 1],attrib.colors[colorIndex -0] };
					}
					else
					{
						vertex.color = { 1.f,1.f,1.f };
					}

				}

				if (index.normal_index >= 0)
				{
					vertex.normal = { attrib.normals[3 * index.normal_index + 0],attrib.normals[3 * index.normal_index + 1],attrib.normals[3 * index.normal_index + 2] };
				}

				if (index.texcoord_index >= 0)
				{
					vertex.uv = { attrib.texcoords[2 * index.texcoord_index + 0], attrib.texcoords[2 * index.texcoord_index + 1] };
				}

				vertices.push_back(vertex);
			}
		}

	}

}
#pragma once

#include "rt_camera.hpp"

#include "vulkan/vulkan.h"

namespace rt 
{
	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		RtCamera &camera;
		VkDescriptorSet globalDescriptorSet;


	};



}
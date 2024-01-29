#pragma once

#include "vcu_camera.hpp"

// lib
#include <vulkan/vulkan.h>

namespace vcu {
	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		VcuCamera &camera;
		VkDescriptorSet globalDescriptorSet;
	};
}
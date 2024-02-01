#pragma once

#include "vcu_camera.hpp"
#include "vcu_game_object.hpp"

// lib
#include <vulkan/vulkan.h>

namespace vcu {
	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		VcuCamera &camera;
		VkDescriptorSet globalDescriptorSet;
		VcuGameObject::Map &gameObjects;
	};
}
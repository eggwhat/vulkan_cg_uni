#pragma once

#include "vcu_camera.hpp"
#include "vcu_game_object.hpp"

// lib
#include <vulkan/vulkan.h>

namespace vcu {

	#define MAX_LIGHTS 10

	struct PointLight {
		glm::vec4 position{}; // ignore w
		glm::vec4 color{}; // w is intensity
	};

	struct GlobalUbo {
		glm::mat4 projection{ 1.f };
		glm::mat4 view{ 1.f };
		glm::mat4 inverseView{ 1.f };	
		glm::vec4 ambientLightColor{ 0.988f, 0.933f, 0.655 , 0.02f }; // w is intensity
		PointLight pointLights[MAX_LIGHTS];
		int numLights;
		bool fogEnabled;
		glm::vec2 movingLightIndices;
	};

	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		VcuCamera &camera;
		VkDescriptorSet globalDescriptorSet;
		VcuGameObject::Map &gameObjects;
	};
}
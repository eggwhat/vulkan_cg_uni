#pragma once

#include "vcu_camera.hpp"
#include "vcu_pipeline.hpp"
#include "vcu_game_object.hpp"
#include "vcu_device.hpp"

// std
#include <memory>
#include <vector>

namespace vcu {
	class SimpleRenderSystem {
	public:
		SimpleRenderSystem(VcuDevice &device, VkRenderPass renderPass);
		~SimpleRenderSystem();
		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

		void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<VcuGameObject>& gameObjects, const VcuCamera& camera);

	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);

		VcuDevice& vcuDevice;

		std::unique_ptr<VcuPipeline> vcuPipeline;
		VkPipelineLayout pipelineLayout;
	};
} // namespace vcu


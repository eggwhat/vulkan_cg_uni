#pragma once

#include "../src/vcu_camera.hpp"
#include "../src/vcu_pipeline.hpp"
#include "../src/vcu_game_object.hpp"
#include "../src/vcu_device.hpp"
#include "../src/vcu_frame_info.hpp"

// std
#include <memory>
#include <vector>

namespace vcu {
	class PointLightSystem {
	public:
		PointLightSystem(VcuDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~PointLightSystem();
		PointLightSystem(const PointLightSystem&) = delete;
		PointLightSystem& operator=(const PointLightSystem&) = delete;

		void update(FrameInfo &frameInfo, GlobalUbo &ubo, glm::vec3 translation);
		void render(FrameInfo &frameInfo);

	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		VcuDevice& vcuDevice;

		std::unique_ptr<VcuPipeline> vcuPipeline;
		VkPipelineLayout pipelineLayout;
	};
} // namespace vcu


#pragma once

#include "../vcu_camera.hpp"
#include "../vcu_pipeline.hpp"
#include "../vcu_game_object.hpp"
#include "../vcu_device.hpp"
#include "../vcu_frame_info.hpp"

// std
#include <memory>
#include <vector>

namespace vcu {
	class MovingRenderSystem {
	public:
		MovingRenderSystem(VcuDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout,
			const std::string& vertexShaderFile, const std::string& fragmentShaderFile);
		~MovingRenderSystem();
		MovingRenderSystem(const MovingRenderSystem&) = delete;
		MovingRenderSystem& operator=(const MovingRenderSystem&) = delete;

		void render(FrameInfo &frameInfo);
		glm::vec3 update(FrameInfo& frameInfo, GlobalUbo& ubo);

	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass, const std::string& vertexShaderFile, const std::string& fragmentShaderFile);

		VcuDevice& vcuDevice;

		std::unique_ptr<VcuPipeline> vcuPipeline;
		VkPipelineLayout pipelineLayout;
	};
} // namespace vcu


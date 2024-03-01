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
	class MovingRenderSystem {
	public:
		MovingRenderSystem(VcuDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout,
			const std::string& vertexShaderFile, const std::string& fragmentShaderFile);
		~MovingRenderSystem();
		MovingRenderSystem(const MovingRenderSystem&) = delete;
		MovingRenderSystem& operator=(const MovingRenderSystem&) = delete;

		void render(FrameInfo &frameInfo);
		void update(FrameInfo& frameInfo, GlobalUbo& ubo, glm::vec3& translation, glm::vec3& rotation);

	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass, const std::string& vertexShaderFile, const std::string& fragmentShaderFile);

		VcuDevice& vcuDevice;

		std::unique_ptr<VcuPipeline> vcuPipeline;
		VkPipelineLayout pipelineLayout;
	};
} // namespace vcu


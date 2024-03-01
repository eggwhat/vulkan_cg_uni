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
	class WoodRenderSystem {
	public:
		WoodRenderSystem(VcuDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout,
			const std::string& vertexShaderFile, const std::string& fragmentShaderFile);
		~WoodRenderSystem();
		WoodRenderSystem(const WoodRenderSystem&) = delete;
		WoodRenderSystem& operator=(const WoodRenderSystem&) = delete;

		void renderGameObjects(FrameInfo &frameInfo);

	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass, const std::string& vertexShaderFile, const std::string& fragmentShaderFile);

		VcuDevice& vcuDevice;

		std::unique_ptr<VcuPipeline> vcuPipeline;
		VkPipelineLayout pipelineLayout;
	};
} // namespace vcu


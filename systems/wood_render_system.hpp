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


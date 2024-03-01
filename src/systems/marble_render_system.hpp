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
	class MarbleRenderSystem {
	public:
		MarbleRenderSystem(VcuDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout,
			const std::string& vertexShaderFile, const std::string& fragmentShaderFile);
		~MarbleRenderSystem();
		MarbleRenderSystem(const MarbleRenderSystem&) = delete;
		MarbleRenderSystem& operator=(const MarbleRenderSystem&) = delete;

		void renderGameObjects(FrameInfo &frameInfo);

	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass, const std::string& vertexShaderFile, const std::string& fragmentShaderFile);

		VcuDevice& vcuDevice;

		std::unique_ptr<VcuPipeline> vcuPipeline;
		VkPipelineLayout pipelineLayout;
	};
} // namespace vcu


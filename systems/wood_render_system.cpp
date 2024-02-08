#include "wood_render_system.hpp"

// libs
#define GLFW_FORCE_RADIANS
#define GLFW_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <stdexcept>
#include <cassert>
#include <array>

namespace vcu {

	struct SimplePushConstantData {
		glm::mat4 modelMatrix{ 1.f };
		glm::mat4 normalMatrix{ 1.f };		
	};

	WoodRenderSystem::WoodRenderSystem(VcuDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout,
		const std::string& vertexShaderFile, const std::string& fragmentShaderFile)
		: vcuDevice{device} {
		createPipelineLayout(globalSetLayout);
		createPipeline(renderPass, vertexShaderFile, fragmentShaderFile);
	}

	WoodRenderSystem::~WoodRenderSystem() {
		vkDestroyPipelineLayout(vcuDevice.device(), pipelineLayout, nullptr);
	}

	void WoodRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts = { globalSetLayout };

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
		if (vkCreatePipelineLayout(vcuDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
			VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	void WoodRenderSystem::createPipeline(VkRenderPass renderPass, const std::string& vertexShaderFile, const std::string& fragmentShaderFile) {
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		VcuPipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		vcuPipeline = std::make_unique<VcuPipeline>(
			vcuDevice,
			"shaders/" + vertexShaderFile,
			"shaders/" + fragmentShaderFile,
			pipelineConfig
		);
	}

	void WoodRenderSystem::renderGameObjects(FrameInfo &frameInfo) {
		vcuPipeline->bind(frameInfo.commandBuffer);

		vkCmdBindDescriptorSets(
			frameInfo.commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipelineLayout,
			0,
			1,
			&frameInfo.globalDescriptorSet,
			0,
			nullptr);	

		for (auto& kv : frameInfo.gameObjects) {
			auto& obj = kv.second;
			if (obj.model == nullptr || obj.type != 2) continue;
			SimplePushConstantData push{};
			push.modelMatrix = obj.transform.mat4();
			push.normalMatrix = obj.transform.normalMatrix();

			vkCmdPushConstants(
				frameInfo.commandBuffer,
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(SimplePushConstantData),
				&push);
			obj.model->bind(frameInfo.commandBuffer);
			obj.model->draw(frameInfo.commandBuffer);
		}
	}
}
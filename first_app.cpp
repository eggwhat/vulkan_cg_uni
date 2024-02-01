#include "first_app.hpp"

#include "keyboard_movement_controller.hpp"
#include "vcu_buffer.hpp"
#include "vcu_camera.hpp"
#include "simple_render_system.hpp"

//#define MAX_FRAME_TIME 0.5f

// libs
#define GLFW_FORCE_RADIANS
#define GLFW_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <stdexcept>
#include <chrono>
#include <cassert>
#include <array>
#include <numeric>

namespace vcu {

	struct GlobalUbo {
		glm::mat4 projectionView{1.f};
		glm::vec4 ambientLightColor{1.f, 1.f, 1.f, .02f}; // w is intensity
		glm::vec3 lightPosition{ -1.f };
		alignas(16) glm::vec4 lightColor{ 1.f }; // w is light intensity
	};

	FirstApp::FirstApp() {
		globalPool = VcuDescriptorPool::Builder(vcuDevice)
			.setMaxSets(VcuSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VcuSwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();
		loadGameObjects();
	}

	FirstApp::~FirstApp() {}

	void FirstApp::run() {
		
		std::vector<std::unique_ptr<VcuBuffer>> uboBuffers(VcuSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < uboBuffers.size(); i++) {
			uboBuffers[i] = std::make_unique<VcuBuffer>(
				vcuDevice, 
				sizeof(GlobalUbo), 
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT); 
			uboBuffers[i]->map();
		}

		auto globalSetLayout = VcuDescriptorSetLayout::Builder(vcuDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.build();

		std::vector<VkDescriptorSet> globalDescriptorSets(VcuSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++) {
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			VcuDescriptorWriter(*globalSetLayout, *globalPool)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescriptorSets[i]);
		}

		SimpleRenderSystem simpleRenderSystem{ vcuDevice, vcuRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};
        VcuCamera camera{};
    
        auto viewerObject = VcuGameObject::createGameObject();
		viewerObject.transform.translation.z = -2.5f;
        KeyboardMovementController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();

		while (!vcuWindow.shouldClose()) {
			glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            //frameTime = glm::min(frameTime, MAX_FRAME_TIME);

            cameraController.moveInPlaneXZ(vcuWindow.getGLFWwindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            float aspect = vcuRenderer.getAspectRatio();
            camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);

			if (auto commandBuffer = vcuRenderer.beginFrame()) {
				int frameIndex = vcuRenderer.getFrameIndex();
				FrameInfo frameInfo{
					frameIndex,
					frameTime,
					commandBuffer,
					camera,
					globalDescriptorSets[frameIndex],
					gameObjects
				};

				// update
				GlobalUbo ubo{};
				ubo.projectionView = camera.getProjection() * camera.getView();
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				// render
				vcuRenderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(frameInfo);
				vcuRenderer.endSwapChainRenderPass(commandBuffer);
				vcuRenderer.endFrame();
			}
		}

		vkDeviceWaitIdle(vcuDevice.device());
	}

	void FirstApp::loadGameObjects() {
		std::shared_ptr<VcuModel> vcuModel = VcuModel::createModelFromFile(vcuDevice, "models/flat_vase.obj");	

        auto flatVase = VcuGameObject::createGameObject();
		flatVase.model = vcuModel;
		flatVase.transform.translation = { -.5f, .5f, 0.f };
		flatVase.transform.scale = glm::vec3{ 3.f, 1.5f, 3.f };
		gameObjects.emplace(flatVase.getId(), std::move(flatVase));

		vcuModel = VcuModel::createModelFromFile(vcuDevice, "models/smooth_vase.obj");
		auto smoothVase = VcuGameObject::createGameObject();
		smoothVase.model = vcuModel;
		smoothVase.transform.translation = { .5f, .5f, 0.f };
		smoothVase.transform.scale = glm::vec3{ 3.f, 1.5f, 3.f };
		gameObjects.emplace(smoothVase.getId(), std::move(smoothVase));

		vcuModel = VcuModel::createModelFromFile(vcuDevice, "models/quad.obj");
		auto floor = VcuGameObject::createGameObject();
		floor.model = vcuModel;
		floor.transform.translation = { 0.f, .5f, 0.f };
		floor.transform.scale = glm::vec3{ 3.f, 1.f, 3.f };
		gameObjects.emplace(floor.getId(), std::move(floor));
	}
}
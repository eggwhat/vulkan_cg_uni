#include "first_app.hpp"

#include "keyboard_movement_controller.hpp"
#include "vcu_buffer.hpp"
#include "vcu_camera.hpp"
#include "systems/simple_render_system.hpp"
#include "systems/point_light_system.hpp"

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
		PointLightSystem pointLightSystem{ vcuDevice, vcuRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
        VcuCamera camera{};
    
        auto viewerObject = VcuGameObject::createGameObject();
		viewerObject.transform.translation.z = -2.5f;
        KeyboardMovementController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();
		auto lastCameraModeChangeTime = currentTime;

		while (!vcuWindow.shouldClose()) {
			glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            //frameTime = glm::min(frameTime, MAX_FRAME_TIME);
			auto window = vcuWindow.getGLFWwindow();

			if (glfwGetKey(window, cameraController.keys.cameraModeChange) == GLFW_PRESS &&
				std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastCameraModeChangeTime) > std::chrono::seconds(1)) {
				cameraMode = (cameraMode + 1) % CAMERA_MODES;
				lastCameraModeChangeTime = currentTime;
			}

            cameraController.moveInPlaneXZ(window, frameTime, viewerObject, cameraMode);
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
				ubo.projection = camera.getProjection();
				ubo.view = camera.getView();
				ubo.inverseView = camera.getInverseView();
				pointLightSystem.update(frameInfo, ubo);
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				// render
				vcuRenderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(frameInfo);
				pointLightSystem.render(frameInfo);
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

		/*{
			auto pointLight = VcuGameObject::makePointLight(0.2f);
			gameObjects.emplace(pointLight.getId(), std::move(pointLight));
		}*/

		std::vector<glm::vec3> lightColors{
		 {1.f, .1f, .1f},
		 {.1f, .1f, 1.f},
		 {.1f, 1.f, .1f},
		 {1.f, 1.f, .1f},
		 {.1f, 1.f, 1.f},
		 {1.f, 1.f, 1.f}  //
		};

		for (int i = 0; i < lightColors.size(); i++) {
			auto pointLight = VcuGameObject::makePointLight(0.2f);
			pointLight.color = lightColors[i];
			auto rotateLight = glm::rotate(glm::mat4(1.f), (i * glm::two_pi<float>() / lightColors.size()),
				{0.f, -1.f, 0.f});

			pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.f, -1.f, -1.f, 1.f));
			gameObjects.emplace(pointLight.getId(), std::move(pointLight));
		}
	}
}
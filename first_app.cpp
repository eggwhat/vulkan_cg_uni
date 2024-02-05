#include "first_app.hpp"

#include "keyboard_movement_controller.hpp"
#include "vcu_buffer.hpp"
#include "vcu_camera.hpp"
#include "vcu_texture.hpp"
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
#include <vector>

namespace vcu {

	FirstApp::FirstApp() {
		globalPool = VcuDescriptorPool::Builder(vcuDevice)
			.setMaxSets(VcuSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VcuSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VcuSwapChain::MAX_FRAMES_IN_FLIGHT)
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
			.addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.build();

		Texture texture = Texture(vcuDevice, "textures/wood.png");

		VkDescriptorImageInfo imageInfo = {};
		imageInfo.sampler = texture.getSampler();
		imageInfo.imageView = texture.getImageView();
		imageInfo.imageLayout = texture.getImageLayout();

		std::vector<VkDescriptorSet> globalDescriptorSets(VcuSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++) {
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			VcuDescriptorWriter(*globalSetLayout, *globalPool)
				.writeBuffer(0, &bufferInfo)
				.writeImage(1, &imageInfo)
				.build(globalDescriptorSets[i]);
		}

		auto renderSystems = std::vector<std::unique_ptr<SimpleRenderSystem>>();
		renderSystems.push_back(std::move(std::make_unique<SimpleRenderSystem>(vcuDevice, vcuRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout(),"simple_shader.vert.spv","simple_shader.frag.spv")));
		renderSystems.push_back(std::move(std::make_unique<SimpleRenderSystem>(vcuDevice, vcuRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout(),"flat_shader.vert.spv","flat_shader.frag.spv")));
		renderSystems.push_back(std::move(std::make_unique<SimpleRenderSystem>(vcuDevice, vcuRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout(),"gourard_shader.vert.spv","gourard_shader.frag.spv")));
		PointLightSystem pointLightSystem{ vcuDevice, vcuRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
        VcuCamera camera{};
    
        auto viewerObject = VcuGameObject::createGameObject();
		viewerObject.transform.translation.z = -2.5f;
        KeyboardMovementController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();
		auto lastCameraModeChangeTime = currentTime;
		auto lastShaderModeChangeTime = currentTime;
		auto lastFogChangeTime = currentTime;

		while (!vcuWindow.shouldClose()) {
			glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            //frameTime = glm::min(frameTime, MAX_FRAME_TIME);
			auto window = vcuWindow.getGLFWwindow();

			if (glfwGetKey(window, cameraController.keys.cameraModeChange) == GLFW_PRESS &&
				std::chrono::duration_cast<std::chrono::microseconds>(currentTime - lastCameraModeChangeTime) > std::chrono::milliseconds(500)) {
				cameraMode = (cameraMode + 1) % CAMERA_MODES;
				lastCameraModeChangeTime = currentTime;
			}

			if (glfwGetKey(window, cameraController.keys.shaderModeChange) == GLFW_PRESS &&
				std::chrono::duration_cast<std::chrono::microseconds>(currentTime - lastShaderModeChangeTime) > std::chrono::milliseconds(500)) {
				shaderMode = (shaderMode + 1) % SHADERS;
				lastShaderModeChangeTime = currentTime;
			}

			if (glfwGetKey(window, cameraController.keys.fogChange) == GLFW_PRESS &&
				std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastFogChangeTime) > std::chrono::milliseconds(500)) {
				fogEnabled = !fogEnabled;
				lastFogChangeTime = currentTime;
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
				ubo.fogEnabled = fogEnabled;
				pointLightSystem.update(frameInfo, ubo);
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				// render
				vcuRenderer.beginSwapChainRenderPass(commandBuffer);
				renderSystems[shaderMode]->renderGameObjects(frameInfo);
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

		vcuModel = VcuModel::createModelFromFile(vcuDevice, "models/floor_mat.obj");
		auto floor = VcuGameObject::createGameObject();
		floor.model = vcuModel;
		floor.transform.translation = { 0.f, .5f, 0.f };
		floor.transform.rotation = glm::radians(glm::vec3{ -90.f, 0.f, 0.f });
		floor.transform.scale = glm::vec3{ .4f, .4f, .4f };
		gameObjects.emplace(floor.getId(), std::move(floor));

		/*{
			auto pointLight = VcuGameObject::makePointLight(0.2f);
			pointLight.transform.translation = { 0.f, -0.5f, 0.f };
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
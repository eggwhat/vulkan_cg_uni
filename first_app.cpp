#include "first_app.hpp"

#include "keyboard_movement_controller.hpp"
#include "vcu_buffer.hpp"
#include "vcu_camera.hpp"
#include "vcu_texture.hpp"
#include "systems/simple_render_system.hpp"
#include "systems/point_light_system.hpp"
#include "systems/moving_render_system.hpp"
#include "systems/wood_render_system.hpp"
#include "systems/no_txt_render_system.hpp"
#include "systems/marble_render_system.hpp"

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
			.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VcuSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VcuSwapChain::MAX_FRAMES_IN_FLIGHT)
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
			.addBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.addBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.addBinding(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.build();

		Texture texture = Texture(vcuDevice, "textures/road.png");

		VkDescriptorImageInfo imageInfo = {};
		imageInfo.sampler = texture.getSampler();
		imageInfo.imageView = texture.getImageView();
		imageInfo.imageLayout = texture.getImageLayout();

		Texture road = Texture(vcuDevice, "textures/metal_plate3.jpg");

		VkDescriptorImageInfo roadImageInfo = {};
		roadImageInfo.sampler = road.getSampler();
		roadImageInfo.imageView = road.getImageView();
		roadImageInfo.imageLayout = road.getImageLayout();

		Texture table = Texture(vcuDevice, "textures/table.jpg");

		VkDescriptorImageInfo tableImageInfo = {};
		tableImageInfo.sampler = table.getSampler();
		tableImageInfo.imageView = table.getImageView();
		tableImageInfo.imageLayout = table.getImageLayout();
		
		Texture marble = Texture(vcuDevice, "textures/marble2.jpg");

		VkDescriptorImageInfo marbleImageInfo = {};
		marbleImageInfo.sampler = marble.getSampler();
		marbleImageInfo.imageView = marble.getImageView();
		marbleImageInfo.imageLayout = marble.getImageLayout();

		std::vector<VkDescriptorSet> globalDescriptorSets(VcuSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++) {
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			VcuDescriptorWriter(*globalSetLayout, *globalPool)
				.writeBuffer(0, &bufferInfo)
				.writeImage(1, &imageInfo)
				.writeImage(2, &roadImageInfo)
				.writeImage(3, &tableImageInfo)
				.writeImage(4, &marbleImageInfo)
				.build(globalDescriptorSets[i]);
		}

		auto renderSystems = std::vector<std::unique_ptr<SimpleRenderSystem>>();
		renderSystems.push_back(std::move(std::make_unique<SimpleRenderSystem>(vcuDevice, vcuRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout(),"simple_shader.vert.spv","simple_shader.frag.spv")));
		renderSystems.push_back(std::move(std::make_unique<SimpleRenderSystem>(vcuDevice, vcuRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout(),"flat_shader.vert.spv","flat_shader.frag.spv")));
		renderSystems.push_back(std::move(std::make_unique<SimpleRenderSystem>(vcuDevice, vcuRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout(),"gourard_shader.vert.spv","gourard_shader.frag.spv")));
		PointLightSystem pointLightSystem{ vcuDevice, vcuRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		auto movingRenderSystems = std::vector<std::unique_ptr<MovingRenderSystem>>();
		movingRenderSystems.push_back(std::move(std::make_unique<MovingRenderSystem>(vcuDevice, vcuRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout(), "mv_simple_shader.vert.spv", "mv_simple_shader.frag.spv")));
		movingRenderSystems.push_back(std::move(std::make_unique<MovingRenderSystem>(vcuDevice, vcuRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout(), "mv_flat_shader.vert.spv", "mv_flat_shader.frag.spv")));
		movingRenderSystems.push_back(std::move(std::make_unique<MovingRenderSystem>(vcuDevice, vcuRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout(), "mv_gourard_shader.vert.spv", "mv_gourard_shader.frag.spv")));
		auto woodRenderSystems = std::vector<std::unique_ptr<WoodRenderSystem>>();
		woodRenderSystems.push_back(std::move(std::make_unique<WoodRenderSystem>(vcuDevice, vcuRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout(), "wd_simple_shader.vert.spv", "wd_simple_shader.frag.spv")));
		woodRenderSystems.push_back(std::move(std::make_unique<WoodRenderSystem>(vcuDevice, vcuRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout(), "wd_flat_shader.vert.spv", "wd_flat_shader.frag.spv")));
		woodRenderSystems.push_back(std::move(std::make_unique<WoodRenderSystem>(vcuDevice, vcuRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout(), "wd_gourard_shader.vert.spv", "wd_gourard_shader.frag.spv")));
		auto noTxtRenderSystem = std::vector<std::unique_ptr<NoTxtRenderSystem>>();
		noTxtRenderSystem.push_back(std::move(std::make_unique<NoTxtRenderSystem>(vcuDevice, vcuRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout(), "no_txt_phong_shader.vert.spv", "no_txt_phong_shader.frag.spv")));
		noTxtRenderSystem.push_back(std::move(std::make_unique<NoTxtRenderSystem>(vcuDevice, vcuRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout(), "no_txt_flat_shader.vert.spv", "no_txt_flat_shader.frag.spv")));
		noTxtRenderSystem.push_back(std::move(std::make_unique<NoTxtRenderSystem>(vcuDevice, vcuRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout(), "no_txt_gourard_shader.vert.spv", "no_txt_gourard_shader.frag.spv")));
		auto marbleRenderSystem = std::vector<std::unique_ptr<MarbleRenderSystem>>();
		marbleRenderSystem.push_back(std::move(std::make_unique<MarbleRenderSystem>(vcuDevice, vcuRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout(), "mrb_phong_shader.vert.spv", "mrb_phong_shader.frag.spv")));
		marbleRenderSystem.push_back(std::move(std::make_unique<MarbleRenderSystem>(vcuDevice, vcuRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout(), "mrb_flat_shader.vert.spv", "mrb_flat_shader.frag.spv")));
		marbleRenderSystem.push_back(std::move(std::make_unique<MarbleRenderSystem>(vcuDevice, vcuRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout(), "mrb_gourard_shader.vert.spv", "mrb_gourard_shader.frag.spv")));
        VcuCamera camera{};
    
        auto viewerObject = VcuGameObject::createGameObject();
		viewerObject.transform.translation.z = -2.5f;
        KeyboardMovementController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();
		auto lastCameraModeChangeTime = currentTime;
		auto lastShaderModeChangeTime = currentTime;
		auto lastFogChangeTime = currentTime;
		auto lastNightModeChangeTime = currentTime;
		auto movingObjectTranslation = glm::vec3{ 0.f, 0.f, 0.f };
		auto movingObjectRotation = glm::vec3{ 0.f, 0.f, 0.f };
		std::vector<glm::vec4> ambientLight{{ 1.0f, 1.0f, 1.0, 0.2f }, { 1.f, 1.f, 1.f, 0.02f } };

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

			if (glfwGetKey(window, cameraController.keys.nightModeChange) == GLFW_PRESS &&
				std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastNightModeChangeTime) > std::chrono::milliseconds(500)) {
				nightMode = !nightMode;
				lastNightModeChangeTime = currentTime;
			}

            cameraController.moveInPlaneXZ(window, frameTime, viewerObject, cameraMode, movingObjectTranslation, movingObjectRotation);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

			std::vector<std::string> fr = { " -- Camera mode: ", cameraModeNames[cameraMode], " -- Shading mode: ", shadingModeNames[shaderMode],
				" -- ", fogModeNames[fogEnabled ? 1 : 0], " -- Lighting mode: ", nightModeNames[nightMode ? 1 : 0]};
			char rendererInfo[512] = { 0 };
			std::accumulate(fr.begin(), fr.end(), std::string()).copy(rendererInfo, 512);
			glfwSetWindowTitle(window, rendererInfo);

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
				ubo.ambientLightColor = nightMode ? ambientLight[1] : ambientLight[0];
				ubo.movingLightIndices = glm::vec2{0,1};
				movingRenderSystems[shaderMode]->update(frameInfo, ubo, movingObjectTranslation, movingObjectRotation);
				pointLightSystem.update(frameInfo, ubo, movingObjectTranslation);
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				// render
				vcuRenderer.beginSwapChainRenderPass(commandBuffer);
				renderSystems[shaderMode]->renderGameObjects(frameInfo);
				woodRenderSystems[shaderMode]->renderGameObjects(frameInfo);
				noTxtRenderSystem[shaderMode]->renderGameObjects(frameInfo);
				marbleRenderSystem[shaderMode]->renderGameObjects(frameInfo);
				pointLightSystem.render(frameInfo);
				movingRenderSystems[shaderMode]->render(frameInfo);
				vcuRenderer.endSwapChainRenderPass(commandBuffer);
				vcuRenderer.endFrame();
			}
		}

		vkDeviceWaitIdle(vcuDevice.device());
	}

	void FirstApp::loadGameObjects() {
		std::shared_ptr<VcuModel> vcuModel = VcuModel::createModelFromFile(vcuDevice, "models/smooth_vase.obj");

		auto flatVase = VcuGameObject::createGameObject();
		flatVase.model = vcuModel;
		flatVase.transform.translation = { -7.5f, .5f, 0.f };
		flatVase.transform.scale = glm::vec3{ 20.f, 15.5f, 20.f };
		flatVase.type = 5;
		gameObjects.emplace(flatVase.getId(), std::move(flatVase));


		/*std::shared_ptr<VcuModel> vcuModel = VcuModel::createModelFromFile(vcuDevice, "models/Chess.obj");
		auto floor = VcuGameObject::createGameObject();
		floor.model = vcuModel;
		floor.transform.translation = { 0.f, .5f, 0.f };
		floor.transform.rotation = glm::vec3{ glm::radians(-180.f), 0.f, 0.f };
		floor.transform.scale = glm::vec3{ 3.0f, 3.0f, 3.0f };
		gameObjects.emplace(floor.getId(), std::move(floor));*/

		vcuModel = VcuModel::createModelFromFile(vcuDevice, "models/table.obj");
		auto table = VcuGameObject::makeWoodObject();
		table.model = vcuModel;
		table.transform.translation = { 0.f, 16.0f, 0.f };
		table.transform.rotation = glm::vec3{ glm::radians(-180.f), 0.f, 0.f };
		table.transform.scale = glm::vec3{ 18.0f, 18.0f, 18.0f };
		gameObjects.emplace(table.getId(), std::move(table)); 

		vcuModel = VcuModel::createModelFromFile(vcuDevice, "models/uh60.obj");
		auto helicopter = VcuGameObject::makeMovingObject();
		helicopter.model = vcuModel;
		helicopter.transform.translation = { 0.f, -10.5f, 0.f };
		helicopter.transform.rotation = glm::vec3{ glm::radians(-180.f), 0.f, 0.f };
		helicopter.transform.scale = glm::vec3{ 0.04f, 0.04f, 0.04f };
		gameObjects.emplace(helicopter.getId(), std::move(helicopter));

		auto pointLightLeft = VcuGameObject::makePointLight(2.2f, 0.05f, { 0.5f, 1.f, 1.f }, 1);
		pointLightLeft.transform.translation = { -0.4f, -11.0f, -2.8f };
		gameObjects.emplace(pointLightLeft.getId(), std::move(pointLightLeft));

		auto pointLightRight = VcuGameObject::makePointLight(2.2f, 0.05f, { 0.5f, 1.f, 1.f }, 1);
		pointLightRight.transform.translation = { 0.4f, -11.0f, -2.8f };
		gameObjects.emplace(pointLightRight.getId(), std::move(pointLightRight));

		{
			auto pointLight = VcuGameObject::makePointLight(2.9f, 0.5f, glm::vec3{0.2f, 0.2f ,1.f}, 2);
			pointLight.transform.translation = { 10.5f, -1.5f, 0.f };
			gameObjects.emplace(pointLight.getId(), std::move(pointLight));
		}

		vcuModel = VcuModel::createModelBezier(vcuDevice);
		auto bezierModel = VcuGameObject::createGameObject();
		bezierModel.model = vcuModel;
		bezierModel.transform.translation = { 10.5f, 0.5f, 0.f };
		bezierModel.transform.scale = glm::vec3{ 6.f, 5.f, 4.f };
		bezierModel.transform.rotation = glm::vec3{ 1.5f, 0.f, 0.f };
		bezierModel.type = 3;
		gameObjects.emplace(bezierModel.getId(), std::move(bezierModel));

		std::vector<glm::vec3> lightColors{
		 {1.f, .1f, .1f},
		 {.1f, .1f, 1.f},
		 {.1f, 1.f, .1f},
		 {1.f, 1.f, .1f},
		 {.1f, 1.f, 1.f},
		 {1.f, 1.f, 1.f}  //
		};

		for (int i = 0; i < lightColors.size(); i++) {
			auto pointLight = VcuGameObject::makePointLight(0.4f, 0.2f);
			pointLight.color = lightColors[i];
			auto rotateLight = glm::rotate(glm::mat4(1.f), (i * glm::two_pi<float>() / lightColors.size()),
				{0.f, -1.f, 0.f});

			pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-2.0f, -3.5f, -2.0f, 1.f));
			gameObjects.emplace(pointLight.getId(), std::move(pointLight));
		}
	}
}
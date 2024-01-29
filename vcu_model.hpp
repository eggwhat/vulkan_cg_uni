#pragma once

#include "vcu_device.hpp"
#include "vcu_buffer.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

// std
#include <memory>
#include <vector>

namespace vcu {
	class VcuModel {
	public:
		struct Vertex {
			glm::vec3 position{};
			glm::vec3 color{};
			glm::vec3 normal{};
			glm::vec2 uv{};

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

			bool operator==(const Vertex& other) const {
				return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
			}
		};

		struct Builder {
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};

			void loadModel(const std::string& filename);
		};

		VcuModel(VcuDevice& device, const VcuModel::Builder &builder);
		~VcuModel();

		static std::unique_ptr<VcuModel> createModelFromFile(VcuDevice& device, const std::string& filepath);

		VcuModel(const VcuModel&) = delete;
		VcuModel& operator=(const VcuModel&) = delete;

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

	private:
		void createVertexBuffers(const std::vector<Vertex> &vertices);
		void createIndexBuffer(const std::vector<uint32_t> &indices);

		VcuDevice& vcuDevice;

		std::unique_ptr<VcuBuffer> vertexBuffer;
		uint32_t vertexCount;

		bool hasIndexBuffer = false;
		std::unique_ptr<VcuBuffer> indexBuffer;
		uint32_t indexCount;
	};
}
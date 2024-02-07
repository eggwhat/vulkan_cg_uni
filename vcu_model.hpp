#pragma once

#include <vulkan/vulkan_core.h>
#include "vcu_device.hpp"
#include "vcu_buffer.hpp"
#include "vcu_texture.hpp"
#include "vcu_descriptors.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

// std
#include <memory>
#include <vector>
#include <filesystem>

namespace vcu {
	/*class VcuModel {
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
	};*/

    class VcuModel {
    public:
        struct Material {
            std::shared_ptr<Texture> albedoTexture;
            std::shared_ptr<Texture> normalTexture;
            std::shared_ptr<Texture> metallicRoughnessTexture;
            VkDescriptorSet descriptorSet;
        };

        struct Primitive {
            uint32_t firstIndex;
            uint32_t firstVertex;
            uint32_t indexCount;
            uint32_t vertexCount;
            Material material;
        };

        struct Vertex {
            glm::vec3 position{};
            glm::vec3 color{};
            glm::vec3 normal{};
            glm::vec4 tangent{};
            glm::vec2 uv{};

            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();

            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

            bool operator==(const Vertex& other) const {
                return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
            }
        };

        VcuModel(VcuDevice& device, const std::string& filepath, VcuDescriptorSetLayout& setLayout, VcuDescriptorPool& pool);
        ~VcuModel();


        void bind(VkCommandBuffer commandBuffer);
        void draw(VkCommandBuffer commandBuffer, VkDescriptorSet globalDescriptorSet, VkPipelineLayout pipelineLayout);

    private:
        void createVertexBuffers(const std::vector<Vertex>& vertices);

        void createIndexBuffers(const std::vector<uint32_t>& indices);

        std::unique_ptr<VcuBuffer> vertexBuffer;

        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        std::vector<Primitive> primitives;
        std::vector<std::shared_ptr<Texture>> images;

        bool hasIndexBuffer = false;
        std::unique_ptr<VcuBuffer> indexBuffer;
        VcuDevice& vcuDevice;
    };
}
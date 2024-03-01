#pragma once

#include "vcu_device.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace vcu {

    class VcuDescriptorSetLayout {
    public:
        class Builder {
        public:
            Builder(VcuDevice& vcuDevice) : vcuDevice{ vcuDevice } {}

            Builder& addBinding(
                uint32_t binding,
                VkDescriptorType descriptorType,
                VkShaderStageFlags stageFlags,
                uint32_t count = 1);
            std::unique_ptr<VcuDescriptorSetLayout> build() const;

        private:
            VcuDevice& vcuDevice;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
        };

        VcuDescriptorSetLayout(
            VcuDevice& lveDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
        ~VcuDescriptorSetLayout();
        VcuDescriptorSetLayout(const VcuDescriptorSetLayout&) = delete;
        VcuDescriptorSetLayout& operator=(const VcuDescriptorSetLayout&) = delete;

        VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

    private:
        VcuDevice& vcuDevice;
        VkDescriptorSetLayout descriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

        friend class VcuDescriptorWriter;
    };


    class VcuDescriptorPool {
    public:
        class Builder {
        public:
            Builder(VcuDevice& vcuDevice) : vcuDevice{ vcuDevice } {}

            Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
            Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
            Builder& setMaxSets(uint32_t count);
            std::unique_ptr<VcuDescriptorPool> build() const;

        private:
            VcuDevice& vcuDevice;
            std::vector<VkDescriptorPoolSize> poolSizes{};
            uint32_t maxSets = 1000;
            VkDescriptorPoolCreateFlags poolFlags = 0;
        };

        VcuDescriptorPool(
            VcuDevice&  vcuDevice,
            uint32_t maxSets,
            VkDescriptorPoolCreateFlags poolFlags,
            const std::vector<VkDescriptorPoolSize>& poolSizes);
        ~VcuDescriptorPool();
        VcuDescriptorPool(const VcuDescriptorPool&) = delete;
        VcuDescriptorPool& operator=(const VcuDescriptorPool&) = delete;

        bool allocateDescriptorSet(
            const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

        void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

        void resetPool();

    private:
        VcuDevice& vcuDevice;
        VkDescriptorPool descriptorPool;

        friend class VcuDescriptorWriter;
    };

    class VcuDescriptorWriter {
    public:
        VcuDescriptorWriter(VcuDescriptorSetLayout& setLayout, VcuDescriptorPool& pool);

        VcuDescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
        VcuDescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

        bool build(VkDescriptorSet& set);
        void overwrite(VkDescriptorSet& set);

    private:
        VcuDescriptorSetLayout& setLayout;
        VcuDescriptorPool& pool;
        std::vector<VkWriteDescriptorSet> writes;
    };
}
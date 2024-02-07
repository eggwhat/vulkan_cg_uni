#pragma once

#include "vcu_device.hpp"
#include <string.h>
#include <vulkan/vulkan_core.h>

namespace vcu {
    class Texture {
    public:
        Texture(VcuDevice& device, const std::string& filepath);
        ~Texture();

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;
        Texture(Texture&&) = delete;
        Texture& operator=(Texture&&) = delete;

        VkSampler getSampler() { return sampler; }
        VkImageView getImageView() { return imageView; }
        VkImageLayout getImageLayout() { return imageLayout; }
    private:
        void transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout);
        void generateMipmaps();

        int width, height, mipLevels;

        VcuDevice& vcuDevice;
        VkImage image;
        VkDeviceMemory imageMemory;
        VkImageView imageView;
        VkSampler sampler;
        VkFormat imageFormat;
        VkImageLayout imageLayout;
    };
}
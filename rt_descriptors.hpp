#pragma once

#include "rt_device.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace rt {

    class RtDescriptorSetLayout {
    public:
        class Builder {
        public:
            Builder(RtDevice& rtDevice) : rtDevice{ rtDevice } {}

            Builder& addBinding(
                uint32_t binding,
                VkDescriptorType descriptorType,
                VkShaderStageFlags stageFlags,
                uint32_t count = 1);
            std::unique_ptr<RtDescriptorSetLayout> build() const;

        private:
            RtDevice& rtDevice;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
        };

        RtDescriptorSetLayout(
            RtDevice& rtDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
        ~RtDescriptorSetLayout();
        RtDescriptorSetLayout(const RtDescriptorSetLayout&) = delete;
        RtDescriptorSetLayout& operator=(const RtDescriptorSetLayout&) = delete;

        VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

    private:
        RtDevice& rtDevice;
        VkDescriptorSetLayout descriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

        friend class RtDescriptorWriter;
    };

    class RtDescriptorPool {
    public:
        class Builder {
        public:
            Builder(RtDevice& rtDevice) : rtDevice{ rtDevice } {}

            Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
            Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
            Builder& setMaxSets(uint32_t count);
            std::unique_ptr<RtDescriptorPool> build() const;

        private:
            RtDevice& rtDevice;
            std::vector<VkDescriptorPoolSize> poolSizes{};
            uint32_t maxSets = 1000;
            VkDescriptorPoolCreateFlags poolFlags = 0;
        };

        RtDescriptorPool(
            RtDevice& rtDevice,
            uint32_t maxSets,
            VkDescriptorPoolCreateFlags poolFlags,
            const std::vector<VkDescriptorPoolSize>& poolSizes);
        ~RtDescriptorPool();
        RtDescriptorPool(const RtDescriptorPool&) = delete;
        RtDescriptorPool& operator=(const RtDescriptorPool&) = delete;

        bool allocateDescriptor(
            const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

        void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

        void resetPool();

    private:
        RtDevice& rtDevice;
        VkDescriptorPool descriptorPool;

        friend class RtDescriptorWriter;
    };

    class RtDescriptorWriter {
    public:
        RtDescriptorWriter(RtDescriptorSetLayout& setLayout, RtDescriptorPool& pool);

        RtDescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
        RtDescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

        bool build(VkDescriptorSet& set);
        void overwrite(VkDescriptorSet& set);

    private:
        RtDescriptorSetLayout& setLayout;
        RtDescriptorPool& pool;
        std::vector<VkWriteDescriptorSet> writes;
    };

}
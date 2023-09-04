#pragma once
#include <vulkan/vulkan.h>
#include <vector>

#define MAX_SETS_PER_POOL 100

namespace GfxVk
{
    namespace Shading
    {
        struct PoolWrapper
        {
            uint32_t setsAssigned = 0;
            uint32_t poolId;
            VkDescriptorPoolSize* poolSize;
            uint32_t poolSizeCount;
            VkDescriptorPool* pool;
        };

        class VkDescriptorPoolFactory
        {

        private:
            VkDescriptorPoolFactory() {}
            VkDescriptorPoolFactory(VkDescriptorPoolFactory const&) {}
            VkDescriptorPoolFactory const& operator= (VkDescriptorPoolFactory const&) {}

            static VkDescriptorPoolFactory* instance;

            uint32_t idCounter = 0;
            uint32_t GetId();
            uint32_t defaultPoolId = 0;

            std::vector<PoolWrapper> poolWrapperList;
            std::vector<VkDescriptorSet*> descriptorSetList;

        public:
            void Init();
            void DeInit();
            void Update();
            static VkDescriptorPoolFactory* GetInstance();
            ~VkDescriptorPoolFactory();

            const uint32_t CreateDescritorPool();
            VkDescriptorPool* GetDescriptorPool(const uint32_t& id);
            VkDescriptorPool* GetDescriptorPool();
            void StoreDescriptorSet(VkDescriptorSet* set);
        };
    }
}
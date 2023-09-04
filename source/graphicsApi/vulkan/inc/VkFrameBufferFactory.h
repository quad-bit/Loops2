#pragma once
#include <vulkan/vulkan.h>
#include <Utility/RenderingWrappers/RenderingWrapper.h>
#include <vector>

namespace GfxVk
{
    namespace Framebuffer
    {
        class FrameBufferWrapper
        {
        public:
            uint32_t id;
            VkFramebuffer fbo;
        };

        class VkFrameBufferFactory
        {
        private:
            VkFrameBufferFactory() {}
            VkFrameBufferFactory(VkFrameBufferFactory const&) {}
            VkFrameBufferFactory const& operator= (VkFrameBufferFactory const&) {}

            static VkFrameBufferFactory* instance;
            static uint32_t fboId;

            std::vector<FrameBufferWrapper*> fboList;
            uint32_t GetId();

        public:
            void Init();
            void DeInit();
            void Update();
            static VkFrameBufferFactory* GetInstance();
            ~VkFrameBufferFactory();

            void CreateFrameBuffer(uint32_t numFrameBuffers, VkImageView* pImageView, uint32_t viewsPerFB,
                VkRenderPass* renderPass, uint32_t width, uint32_t height, uint32_t* ids);
            void DestroyFrameBuffer(uint32_t id);
            void DestroyFrameBuffer(uint32_t* ids, uint32_t count);
            VkFramebuffer* GetFrameBuffer(uint32_t id);
        };
    }
}
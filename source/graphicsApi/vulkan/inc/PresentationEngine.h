#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <Utility/RenderingWrappers/RenderingWrapper.h>

namespace Core
{
    namespace Wrappers
    {
        struct AttachmentInfo;
    }
    class Settings;
}

namespace GfxVk
{
    namespace Utility
    {
        class PresentationEngine
        {
        private:
            PresentationEngine() {};
            PresentationEngine(PresentationEngine const&) = delete;
            PresentationEngine const& operator= (PresentationEngine const&) = delete;

            static PresentationEngine* instance;

            VkSurfaceKHR surfaceObj;
            VkSurfaceFormatKHR surfaceFormat;
            uint32_t m_swapchainImageCount;
            VkSurfaceCapabilitiesKHR surfaceCapabilities = {};
            VkSwapchainKHR swapchainObj;
            VkPresentModeKHR presentMode;

            std::vector<VkImage> m_swapChainImageList;
            std::vector<VkImageView> m_swapChainImageViewList;

            uint32_t activeSwapchainImageID;
            uint32_t m_presentationQueueId;
            void ChangeImageLayout();

        public:
            void Init(VkSurfaceKHR surfaceObj, VkSurfaceFormatKHR surfaceFormat, uint32_t& swapbufferCount, uint32_t presentationQueueId);
            void CreateSwapChain(VkSwapchainCreateInfoKHR swapChainCreateInfo);
            void CreateSwapChain(Core::Wrappers::ImageCreateInfo swapChainCreateInfo);

            std::vector<VkImage>* CreateSwapchainImage(Core::Wrappers::AttachmentInfo* info, uint32_t count);
            std::vector<VkImageView>* CreateSwapchainImageViews(Core::Wrappers::AttachmentInfo* info, uint32_t count);

            void CreateSwapchainImages();
            void CreateSwapchainImageViews(const std::vector<VkImageViewCreateInfo>& info);

            void DestroySwapChain();
            //deprecated.
            void DestroySwapChainImageView();

            void DeInit();
            void Update();
            static PresentationEngine* GetInstance();
            ~PresentationEngine();

            uint32_t VkGetAvailableSwapChainId(VkFence* fence, VkSemaphore* semaphore);
            uint32_t VkGetAvailableSwapChainId(const VkFence& fence, const VkSemaphore& semaphore);
            VkSwapchainKHR* GetSwapchain() { return &swapchainObj; }

            void PresentSwapchainImage(VkPresentInfoKHR* info, VkQueue* presentationQueue);
            void PresentSwapchainImage(VkPresentInfoKHR& info, const VkQueue& presentationQueue);
            const VkImage& GetSwapchainImage(uint32_t swapbufferIndex);
            const VkImageView& GetSwapchainImageView(uint32_t swapbufferIndex);
        };
    }
}
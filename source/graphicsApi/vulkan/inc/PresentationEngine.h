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

            std::vector<VkImage> swapChainImageList;
            std::vector<VkImageView> swapChainImageViewList;

            //vector<VkImage *> swapChainImageList;
            //vector<VkImageView *> swapChainImageViewList;

            uint32_t activeSwapchainImageID;
        public:
            void Init(VkSurfaceKHR surfaceObj, VkSurfaceFormatKHR surfaceFormat, uint32_t& swapbufferCount);
            void CreateSwapChain(VkSwapchainCreateInfoKHR swapChainCreateInfo);
            void CreateSwapChain(Core::Wrappers::ImageInfo swapChainCreateInfo);

            std::vector<VkImage>* CreateSwapchainImage(Core::Wrappers::AttachmentInfo* info, uint32_t count);
            std::vector<VkImageView>* CreateSwapchainImageViews(Core::Wrappers::AttachmentInfo* info, uint32_t count);

            std::vector<VkImage> CreateSwapchainImages(const VkImageCreateInfo& info, uint32_t count);
            std::vector<VkImageView> CreateSwapchainImageViews(VkImageViewCreateInfo info, VkImage* images, uint32_t count);

            void DestroySwapChain();
            //deprecated.
            void DestroySwapChainImageView();

            void DeInit();
            void Update();
            static PresentationEngine* GetInstance();
            ~PresentationEngine();

            uint32_t VkGetAvailableSwapChainId(VkFence* fence, VkSemaphore* semaphore);
            VkSwapchainKHR* GetSwapchain() { return &swapchainObj; }

            void PresentSwapchainImage(VkPresentInfoKHR* info, VkQueue* presentationQueue);
        };
    }
}
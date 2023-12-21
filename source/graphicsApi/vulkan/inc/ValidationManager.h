#pragma once
#include <vector>
#include <iostream>
#include <sstream>
#include <vulkan/vulkan.h>


namespace GfxVk
{
    namespace Utility
    {
        class ValidationManager
        {
        private:
            VkInstance* vulkanInstanceRef;
            VkAllocationCallbacks* pAllocatorRef;

            void AddRequiredPlatformInstanceExtensions(std::vector<const char*>* instance_extensions);

        public:

            std::vector<const char*>        deviceLayerNameList;
            std::vector<const char*>        deviceExtensionNameList;
            std::vector<const char*>        instanceLayerNameList;
            std::vector<const char*>        instanceExtensionNameList;

            //VkDebugReportCallbackEXT       debugReport;// = nullptr;
            //VkDebugReportCallbackCreateInfoEXT VkDebugReportCallbackCreateInfoEXTObj = {};

            VkDebugUtilsMessengerCreateInfoEXT dbg_messenger_create_info;
            VkDebugUtilsMessengerEXT dbg_messenger;
            PFN_vkCreateDebugUtilsMessengerEXT CreateDebugUtilsMessengerEXT;
            PFN_vkDestroyDebugUtilsMessengerEXT DestroyDebugUtilsMessengerEXT;


            ValidationManager();
            ~ValidationManager();

            void SetupDebug();
            void InitDebug(VkInstance* vulkanInstance, VkAllocationCallbacks* pAllocator);
            void DeinitDebug();

            void SetupLayersAndExtensions();
        };
    }
}

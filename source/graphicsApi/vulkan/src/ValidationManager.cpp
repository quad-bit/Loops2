#include "ValidationManager.h"
#include "utility/VulkanSettings.h"
#include "Platform/Assertion.h"
#include "PlatformSettings.h"
#include "utility/VulkanUtility.h"

void GfxVk::Utility::ValidationManager::AddRequiredPlatformInstanceExtensions(std::vector<const char*>* instance_extensions)
{
#if defined(GLFW_ENABLED)
    uint32_t instance_extension_count = 0;
    const char ** instance_extensions_buffer = glfwGetRequiredInstanceExtensions(&instance_extension_count);
    for (uint32_t i = 0; i < instance_extension_count; i++) 
    {
        instance_extensions->push_back(instance_extensions_buffer[i]);
    }

#elif VK_USE_PLATFORM_WIN32_KHR
    instance_extensions->push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif VK_USE_PLATFORM_XCB_KHR
    instance_extensions->push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#endif
}

GfxVk::Utility::ValidationManager::ValidationManager()
{
    SetupLayersAndExtensions();
    SetupDebug();
}

GfxVk::Utility::ValidationManager::~ValidationManager()
{
    vulkanInstanceRef = nullptr;
    pAllocatorRef = nullptr;
}

//================================================  DEBUGGING

void GfxVk::Utility::ValidationManager::SetupDebug()
{
#if BUILD_ENABLE_VULKAN_DEBUG

    instanceLayerNameList.push_back("VK_LAYER_KHRONOS_validation");
    //instanceLayerNameList.push_back("VK_LAYER_LUNARG_api_dump");
    //instanceLayerNameList.push_back("VK_LAYER_RENDERDOC_Capture");
    instanceExtensionNameList.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    instanceExtensionNameList.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
}

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugReportFlagsEXT           flags,
    VkDebugReportObjectTypeEXT      obj_type,
    uint64_t                        src_obj,
    size_t                          location,
    int32_t                         msg_code,
    const char *                    layer_prefix,
    const char *                    msg,
    void *                          user_data
)
{
    std::ostringstream stream;
    stream << "VKDBG: ";
    if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT) {
        stream << "INFO: ";
    }
    if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT) {
        stream << "WARNING: ";
    }
    if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) {
        stream << "PERFORMANCE: ";
    }
    if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
        stream << "ERROR: ";
    }
    if (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT) {
        stream << "DEBUG: ";
    }
    stream << "@[" << layer_prefix << "]: ";
    stream << msg << std::endl;
    std::cout << stream.str();

#if defined( _WIN32 )
    if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) 
    {
        std::string message = stream.str().c_str();
        std::wstring stemp = std::wstring(message.begin(), message.end());
        MessageBox(NULL, stemp.c_str(), L"Vulkan Error!", 0);
    }
#endif

    return false;
}


PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXTObj = nullptr;
PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXTObj = nullptr;

void GfxVk::Utility::ValidationManager::InitDebug(VkInstance * vulkanInstance, VkAllocationCallbacks* pAllocator)
{
#if BUILD_ENABLE_VULKAN_DEBUG
    vulkanInstanceRef = vulkanInstance;
    pAllocatorRef = pAllocator;

    vkCreateDebugReportCallbackEXTObj = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(*vulkanInstanceRef, "vkCreateDebugReportCallbackEXT");
    vkDestroyDebugReportCallbackEXTObj = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(*vulkanInstanceRef, "vkDestroyDebugReportCallbackEXT");

    if (vkDestroyDebugReportCallbackEXTObj == nullptr || vkCreateDebugReportCallbackEXTObj == nullptr)
    {
        ASSERT_MSG_DEBUG(0, "Extension issue ");
        std::exit(-1);
    }


    VkDebugReportCallbackCreateInfoEXTObj.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
    VkDebugReportCallbackCreateInfoEXTObj.flags = VK_DEBUG_REPORT_INFORMATION_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT |
        VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_DEBUG_BIT_EXT | 0;
    VkDebugReportCallbackCreateInfoEXTObj.pfnCallback = debugCallback;

    vkCreateDebugReportCallbackEXTObj(*vulkanInstanceRef, &VkDebugReportCallbackCreateInfoEXTObj, pAllocatorRef, &debugReport);
#endif
}

void GfxVk::Utility::ValidationManager::DeinitDebug()
{
#if BUILD_ENABLE_VULKAN_DEBUG
    vkDestroyDebugReportCallbackEXTObj(*vulkanInstanceRef, debugReport, pAllocatorRef);
    //debugReport;// = nullptr;
#endif
}

void GfxVk::Utility::ValidationManager::SetupLayersAndExtensions()
{
    instanceExtensionNameList.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    deviceExtensionNameList.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    AddRequiredPlatformInstanceExtensions(&instanceExtensionNameList);
}

#define NOMINMAX

#include "VulkanManager.h"
#include "ValidationManager.h"
#include "utility/VulkanUtility.h"
#include "utility/VulkanMemoryManager.h"
#include "VkQueueFactory.h"
//#include "VkCommandBufferFactory.h"
#include "synchronisation/VkSynchroniserFactory.h"
#include "pipeline/VulkanGraphicsPipelineFactory.h"
#include <Settings.h>
#include <PlatformSettings.h>
#include <CorePrecompiled.h>
#include "PresentationEngine.h"
#include "VkFrameBufferFactory.h"
#include "VkRenderPassFactory.h"
#include "utility/VkImageFactory.h"
#include "shading/VkShaderFactory.h"
#include "shading/VkDescriptorPoolFactory.h"
#include "shading/VkShaderResourceManager.h"
#include "shading/VkBufferFactory.h"
#include "VkSamplerFactory.h"
#include "utility/VkRenderingUnwrapper.h"
#include <algorithm>

GfxVk::Utility::VulkanManager::VulkanManager(const Core::WindowSettings& windowSettings):
    m_windowSettings{windowSettings}
{
    m_validationManagerObj = new ValidationManager();
}

void GfxVk::Utility::VulkanManager::CreateInstance()
{
    VkApplicationInfo appInfo{};
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pApplicationName = "Random Game";
    appInfo.pEngineName = "Loops";
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo createInfoObj{};
    createInfoObj.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfoObj.pApplicationInfo = &appInfo;
    createInfoObj.enabledExtensionCount = (uint32_t)m_validationManagerObj->instanceExtensionNameList.size();
    createInfoObj.enabledLayerCount = (uint32_t)m_validationManagerObj->instanceLayerNameList.size();
    createInfoObj.pNext = &(m_validationManagerObj->dbg_messenger_create_info);
    createInfoObj.ppEnabledExtensionNames = m_validationManagerObj->instanceExtensionNameList.data();
    createInfoObj.ppEnabledLayerNames = m_validationManagerObj->instanceLayerNameList.data();

    ErrorCheck(vkCreateInstance(&createInfoObj, GfxVk::Utility::VulkanSettings::pAllocator, &DeviceInfo::m_instanceObj));
}

void GfxVk::Utility::VulkanManager::CreateLogicalDevice()
{
    if(DeviceInfo::m_physicalDeviceFeatures.sampleRateShading)
        DeviceInfo::m_enabledPhysicalDeviceFeatures.sampleRateShading = VK_TRUE;

    VkPhysicalDeviceDynamicRenderingFeatures dynamic_rendering_feature{};
    dynamic_rendering_feature.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
    dynamic_rendering_feature.dynamicRendering = VK_TRUE;

    VkPhysicalDeviceSynchronization2Features sync2 = {};
    sync2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
    sync2.synchronization2 = VK_TRUE;
    sync2.pNext = &dynamic_rendering_feature;
    
    VkPhysicalDeviceFeatures2 physicalFeatures2 = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
    physicalFeatures2.pNext = &sync2;
    vkGetPhysicalDeviceFeatures2(DeviceInfo::GetPhysicalDevice(), &physicalFeatures2);

    std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfoList = VkQueueFactory::GetInstance()->FindQueue();

    VkDeviceCreateInfo vkDeviceCreateInfoObj{};
    vkDeviceCreateInfoObj.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    vkDeviceCreateInfoObj.queueCreateInfoCount = (uint32_t)deviceQueueCreateInfoList.size();
    vkDeviceCreateInfoObj.pQueueCreateInfos = deviceQueueCreateInfoList.data();
    vkDeviceCreateInfoObj.enabledExtensionCount = (uint32_t)m_validationManagerObj->deviceExtensionNameList.size();
    vkDeviceCreateInfoObj.enabledLayerCount = 0;
    //vkDeviceCreateInfoObj.pEnabledFeatures = &GfxVk::Utility::VulkanDeviceInfo::m_enabledPhysicalDeviceFeatures;
    vkDeviceCreateInfoObj.ppEnabledExtensionNames = m_validationManagerObj->deviceExtensionNameList.data();
    vkDeviceCreateInfoObj.ppEnabledLayerNames = nullptr;
    vkDeviceCreateInfoObj.pNext = &physicalFeatures2;

    ErrorCheck(vkCreateDevice(DeviceInfo::m_physicalDeviceObj, &vkDeviceCreateInfoObj, GfxVk::Utility::VulkanSettings::pAllocator, &DeviceInfo::m_logicalDeviceObj));
}


void GfxVk::Utility::VulkanManager::CreateSurface(GLFWwindow * glfwWindow)
{
#if defined(GLFW_ENABLED)
    if (VK_SUCCESS != glfwCreateWindowSurface(DeviceInfo::m_instanceObj, glfwWindow, GfxVk::Utility::VulkanSettings::pAllocator, &DeviceInfo::m_surface))
    {
        glfwTerminate();
        ASSERT_MSG_DEBUG(0, "GLFW could not create the window surface.");
        std::exit(-1);
    }
#else
    ASSERT_MSG(0, "GLFW not getting used, need to implement OS specific implementation");
    std::exit(-1);
#endif

    VkBool32 WSI_supported = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(DeviceInfo::m_physicalDeviceObj, 
        VkQueueFactory::GetInstance()->GetGraphicsQueueFamilyIndex(), 
        DeviceInfo::m_surface, &WSI_supported);
    if (!WSI_supported) 
    {
        ASSERT_MSG_DEBUG(0, "WSI not supported");
        std::exit(-1);
    }

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(DeviceInfo::m_physicalDeviceObj, DeviceInfo::m_surface, &DeviceInfo::m_surfaceCapabilities);

    if (DeviceInfo::m_surfaceCapabilities.currentExtent.width < UINT32_MAX)
    {
        DeviceInfo::m_surfaceWidth = DeviceInfo::m_surfaceCapabilities.currentExtent.width;
        DeviceInfo::m_surfaceHeight = DeviceInfo::m_surfaceCapabilities.currentExtent.height;
    }

    {
        uint32_t format_count = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(DeviceInfo::m_physicalDeviceObj, DeviceInfo::m_surface, &format_count, nullptr);
        if (format_count == 0) 
        {
            ASSERT_MSG_DEBUG(0, "Surface formats missing.");
            std::exit(-1);
        }
        std::vector<VkSurfaceFormatKHR> formats(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(DeviceInfo::m_physicalDeviceObj, DeviceInfo::m_surface, &format_count, formats.data());
        if (formats[0].format == VK_FORMAT_UNDEFINED) 
        {
            DeviceInfo::m_surfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
            DeviceInfo::m_surfaceFormat.colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
        }
        else 
        {
            DeviceInfo::m_surfaceFormat = formats[0];
        }
    }
}

void GfxVk::Utility::VulkanManager::GetMaxUsableVKSampleCount()
{
    auto colorCount = DeviceInfo::m_physicalDeviceProps.limits.framebufferColorSampleCounts;
    auto depthCount = DeviceInfo::m_physicalDeviceProps.limits.framebufferDepthSampleCounts;
    VkSampleCountFlags counts =  std::min(colorCount, depthCount);

    if (counts & VK_SAMPLE_COUNT_64_BIT) { DeviceInfo::m_maxUsableSampleCount = VK_SAMPLE_COUNT_64_BIT; return; }
    if (counts & VK_SAMPLE_COUNT_32_BIT) { DeviceInfo::m_maxUsableSampleCount = VK_SAMPLE_COUNT_32_BIT; return; }
    if (counts & VK_SAMPLE_COUNT_16_BIT) { DeviceInfo::m_maxUsableSampleCount = VK_SAMPLE_COUNT_16_BIT; return;    }
    if (counts & VK_SAMPLE_COUNT_8_BIT) { DeviceInfo::m_maxUsableSampleCount = VK_SAMPLE_COUNT_8_BIT; return;}
    if (counts & VK_SAMPLE_COUNT_4_BIT) { DeviceInfo::m_maxUsableSampleCount = VK_SAMPLE_COUNT_4_BIT; return;}
    if (counts & VK_SAMPLE_COUNT_2_BIT) { DeviceInfo::m_maxUsableSampleCount = VK_SAMPLE_COUNT_2_BIT; return;}
    DeviceInfo::m_maxUsableSampleCount = VK_SAMPLE_COUNT_1_BIT;
}

//Core::Enums::Samples GfxVk::Utility::VulkanManager::GetMaxUsableSampleCount()
//{
//    VkSampleCountFlags counts = GetMaxUsableVKSampleCount();
//    
//    if (DeviceInfo::m_physicalDeviceProps.limits.framebufferColorSampleCounts < counts ||
//        DeviceInfo::m_physicalDeviceProps.limits.framebufferDepthSampleCounts < counts )
//    {
//        ASSERT_MSG_DEBUG(0, "sample counts not valid");
//        counts = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
//    }
//
//    Core::Enums::Samples samples = Unwrap::UnWrapSampleCount(counts);
//    return samples;
//}

void GfxVk::Utility::VulkanManager::GetPhysicalDevice()
{
    uint32_t count = 0;
    vkEnumeratePhysicalDevices(DeviceInfo::m_instanceObj, &count, nullptr);
    std::vector<VkPhysicalDevice> deviceList(count);
    vkEnumeratePhysicalDevices(DeviceInfo::m_instanceObj, &count, deviceList.data());

    VkPhysicalDevice discreteGpu = VK_NULL_HANDLE;
    VkPhysicalDevice integratedGpu = VK_NULL_HANDLE;
    for (auto dev : deviceList)
    {
        VkPhysicalDeviceProperties deviceProp = {};
        vkGetPhysicalDeviceProperties(dev, &deviceProp);

        if (deviceProp.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) // deviceProp.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU ||
        {
            //DeviceInfo::m_physicalDeviceObj = dev;
            //CoreObjects::physicalDeviceObj = &DeviceInfo::m_physicalDeviceObj;
            discreteGpu = dev;
            break;
        }

        if (deviceProp.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
        {
            //DeviceInfo::m_physicalDeviceObj = dev;
            //CoreObjects::physicalDeviceObj = &DeviceInfo::m_physicalDeviceObj;
            integratedGpu = dev;
        }
    }

    if (discreteGpu != VK_NULL_HANDLE)
    {
        DeviceInfo::m_physicalDeviceObj = discreteGpu;
    }
    else if (integratedGpu!= VK_NULL_HANDLE)
    {
        DeviceInfo::m_physicalDeviceObj = integratedGpu;
    }

    if (DeviceInfo::m_physicalDeviceObj == VK_NULL_HANDLE)
    {
        ASSERT_MSG_DEBUG(0, "gpu required");
        std::exit(-1);
    }

    vkGetPhysicalDeviceMemoryProperties(DeviceInfo::m_physicalDeviceObj, &DeviceInfo::m_physicalDeviceMemProps);
    vkGetPhysicalDeviceProperties(DeviceInfo::m_physicalDeviceObj, &DeviceInfo::m_physicalDeviceProps);
    vkGetPhysicalDeviceFeatures(DeviceInfo::m_physicalDeviceObj, &DeviceInfo::m_physicalDeviceFeatures);
}

/*
void GfxVk::Utility::VulkanManager::Init()
{
    
    PLOGD << "VulkanManager init";

    CreateInstance();
    GetPhysicalDevice();
    m_validationManagerObj->InitDebug(&DeviceInfo::m_instanceObj, GfxVk::Utility::VulkanSettings::pAllocator);
    VkQueueFactory::GetInstance()->Init();
    CreateLogicalDevice();

    //TODO : expose queue creation. This function is deprecated.DONE 

    VkQueueFactory::GetInstance()->CreateGraphicsQueues(&CoreObjects::renderQueueId, 1);
    QueuePurpose * renderPurpose = new QueuePurpose{ QueuePurpose::RENDER };
    VkQueueFactory::GetInstance()->SetQueuePurpose(renderPurpose, QueueType::GRAPHICS, CoreObjects::renderQueueId);
    
    VkQueueFactory::GetInstance()->CreateGraphicsQueues(&CoreObjects::presentationQueuedId, 1);
    QueuePurpose * presentPurpose = new QueuePurpose{ QueuePurpose::PRESENT };
    VkQueueFactory::GetInstance()->SetQueuePurpose(presentPurpose, QueueType::GRAPHICS, CoreObjects::presentationQueuedId);

    VkQueueFactory::GetInstance()->CreateComputeQueues(&CoreObjects::computeQueueId, 1);
    VkQueueFactory::GetInstance()->CreateTransferQueues(&CoreObjects::transferQueueId, 1);

    VulkanMemoryManager::GetSingleton()->Init(DeviceInfo::m_physicalDeviceMemProps);
    VkCommandBufferFactory::GetInstance()->Init();
    VkSynchroniserFactory::GetInstance()->Init();
}
*/

void GfxVk::Utility::VulkanManager::Init(std::vector<Core::Wrappers::QueueWrapper>& queueRequirement, uint32_t& renderQueueId, uint32_t& presentationQueueId, uint32_t& computeQueueId, uint32_t& transferQueueId)
{
    PLOGD << "VulkanManager init";

    CreateInstance();
    GetPhysicalDevice();
    m_validationManagerObj->InitDebug(&DeviceInfo::m_instanceObj, GfxVk::Utility::VulkanSettings::pAllocator);
    GfxVk::Utility::VkQueueFactory::GetInstance()->Init();
    CreateLogicalDevice();

    auto CreateQueues = [&]() 
        {
            for (uint32_t i = 0; i < queueRequirement.size(); i++)
            {
                if (queueRequirement[i].queueType == Core::Enums::PipelineType::GRAPHICS)
                {
                    if (queueRequirement[i].purpose == Core::Enums::QueueType::PRESENT)
                    {
                        GfxVk::Utility::VkQueueFactory::GetInstance()->CreateGraphicsQueues(&queueRequirement[i].queueId, 1);
                        presentationQueueId = queueRequirement[i].queueId;
                    }

                    if (queueRequirement[i].purpose == Core::Enums::QueueType::RENDER)
                    {
                        GfxVk::Utility::VkQueueFactory::GetInstance()->CreateGraphicsQueues(&queueRequirement[i].queueId, 1);
                        renderQueueId = queueRequirement[i].queueId;
                    }
                }

                if (queueRequirement[i].queueType == Core::Enums::PipelineType::COMPUTE)
                {
                    GfxVk::Utility::VkQueueFactory::GetInstance()->CreateComputeQueues(&queueRequirement[i].queueId, 1);
                    computeQueueId = queueRequirement[i].queueId;
                }

                if (queueRequirement[i].queueType == Core::Enums::PipelineType::TRANSFER)
                {
                    GfxVk::Utility::VkQueueFactory::GetInstance()->CreateTransferQueues(&queueRequirement[i].queueId, 1);
                    transferQueueId = queueRequirement[i].queueId;
                }

                queueRequirement[i].queueFamilyId = GfxVk::Utility::VkQueueFactory::GetInstance()->GetQueueFamilyIndex(queueRequirement[i].queueType, queueRequirement[i].queueId);
                GfxVk::Utility::VkQueueFactory::GetInstance()->SetQueuePurpose(queueRequirement[i].purpose, queueRequirement[i].queueType, queueRequirement[i].queueId);
            }
        };

    CreateQueues();
    GetMaxUsableVKSampleCount();
}

void GfxVk::Utility::VulkanManager::InitializeFactories()
{
    //GfxVk::Utility::PresentationEngine::GetInstance()->Init(GfxVk::Utility::VulkanManager::GetInstance()->GetSurface(), GfxVk::Utility::VulkanManager::GetInstance()->GetSurfaceFormat());
    /*GfxVk::Framebuffer::VkAttachmentFactory::GetInstance()->Init();
    GfxVk::Renderpass::VkRenderPassFactory::GetInstance()->Init();
    GfxVk::Framebuffer::VkFrameBufferFactory::GetInstance()->Init();

    GfxVk::CommandPool::VkCommandBufferFactory::GetInstance()->Init();
    GfxVk::Sync::VkSynchroniserFactory::GetInstance()->Init();
    GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::GetInstance()->Init();
    GfxVk::Shading::VkShaderFactory::GetInstance()->Init();
    GfxVk::Shading::VkDescriptorPoolFactory::GetInstance()->Init();
    GfxVk::Shading::VkBufferFactory::GetInstance()->Init();
    GfxVk::Shading::VkShaderResourceManager::GetInstance()->Init();
    GfxVk::Shading::VkSamplerFactory::GetInstance()->Init();*/
}

void GfxVk::Utility::VulkanManager::DeInit()
{
    PLOGD << "VulkanManager Deinit";
    /*
    GfxVk::Shading::VkSamplerFactory::GetInstance()->DeInit();
    delete GfxVk::Shading::VkSamplerFactory::GetInstance();

    GfxVk::Shading::VkShaderResourceManager::GetInstance()->DeInit();
    delete GfxVk::Shading::VkShaderResourceManager::GetInstance();

    GfxVk::Shading::VkDescriptorPoolFactory::GetInstance()->DeInit();
    delete GfxVk::Shading::VkDescriptorPoolFactory::GetInstance();

    GfxVk::Shading::VkBufferFactory::GetInstance()->DeInit();
    delete GfxVk::Shading::VkBufferFactory::GetInstance();

    GfxVk::Shading::VkShaderFactory::GetInstance()->DeInit();
    delete GfxVk::Shading::VkShaderFactory::GetInstance();

    GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::GetInstance()->DeInit();
    delete GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::GetInstance();

    GfxVk::Sync::VkSynchroniserFactory::GetInstance()->DeInit();
    delete GfxVk::Sync::VkSynchroniserFactory::GetInstance();

    GfxVk::CommandPool::VkCommandBufferFactory::GetInstance()->DeInit();
    delete GfxVk::CommandPool::VkCommandBufferFactory::GetInstance();

    GfxVk::Framebuffer::VkFrameBufferFactory::GetInstance()->DeInit();
    delete GfxVk::Framebuffer::VkFrameBufferFactory::GetInstance();

    GfxVk::Renderpass::VkRenderPassFactory::GetInstance()->DeInit();
    delete GfxVk::Renderpass::VkRenderPassFactory::GetInstance();

    GfxVk::Framebuffer::VkAttachmentFactory::GetInstance()->DeInit();
    delete GfxVk::Framebuffer::VkAttachmentFactory::GetInstance();

    GfxVk::Utility::PresentationEngine::GetInstance()->DeInit();
    delete GfxVk::Utility::PresentationEngine::GetInstance();

    GfxVk::Utility::VulkanMemoryManager::GetSingleton()->DeInit();
    delete GfxVk::Utility::VulkanMemoryManager::GetSingleton();
    */

    vkDestroySurfaceKHR(DeviceInfo::m_instanceObj, DeviceInfo::m_surface, GfxVk::Utility::VulkanSettings::pAllocator);
    vkDestroyDevice(DeviceInfo::m_logicalDeviceObj, GfxVk::Utility::VulkanSettings::pAllocator);
    m_validationManagerObj->DeinitDebug();

    GfxVk::Utility::VkQueueFactory::GetInstance()->DeInit();
    delete GfxVk::Utility::VkQueueFactory::GetInstance();

    vkDestroyInstance(DeviceInfo::m_instanceObj, GfxVk::Utility::VulkanSettings::pAllocator);
}

void GfxVk::Utility::VulkanManager::Update()
{

}

GfxVk::Utility::VulkanManager::~VulkanManager()
{
    delete m_validationManagerObj;
}

#define NOMINMAX

#include "VulkanManager.h"
#include "ValidationManager.h"
#include "VulkanUtility.h"
#include "VulkanMemoryManager.h"
#include "VkQueueFactory.h"
#include "VkCommandBufferFactory.h"
#include "VkSynchroniserFactory.h"
#include "VulkanGraphicsPipelineFactory.h"
#include <Settings.h>
#include <PlatformSettings.h>
#include <CorePrecompiled.h>
#include "PresentationEngine.h"
#include "VkFrameBufferFactory.h"
#include "VkRenderPassFactory.h"
#include "VkAttachmentFactory.h"
#include "VkShaderFactory.h"
#include "VkDescriptorPoolFactory.h"
#include "VkShaderResourceManager.h"
#include "VkBufferFactory.h"
#include "VkSamplerFactory.h"
#include "VkRenderingUnwrapper.h"
#include <algorithm>

//using namespace std;

VulkanManager* VulkanManager::instance = nullptr;

VulkanManager::VulkanManager()
{
    validationManagerObj = new ValidationManager();
}

void VulkanManager::CreateInstance()
{
    VkApplicationInfo appInfo{};
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pApplicationName = "Random Game";
    appInfo.pEngineName = "Loops";
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfoObj{};
    createInfoObj.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfoObj.pApplicationInfo = &appInfo;
    createInfoObj.enabledExtensionCount = (uint32_t)validationManagerObj->instanceExtensionNameList.size();
    createInfoObj.enabledLayerCount = (uint32_t)validationManagerObj->instanceLayerNameList.size();
    createInfoObj.pNext = &(validationManagerObj->VkDebugReportCallbackCreateInfoEXTObj);
    createInfoObj.ppEnabledExtensionNames = validationManagerObj->instanceExtensionNameList.data();
    createInfoObj.ppEnabledLayerNames = validationManagerObj->instanceLayerNameList.data();

    ErrorCheck(vkCreateInstance(&createInfoObj, pAllocator, &vkInstanceObj));

    CoreObjects::instanceObj = &vkInstanceObj;
}

void VulkanManager::CreateLogicalDevice()
{
    if(IsSampleRateShadingAvailable())
        enabledPhysicalDeviceFeatures.sampleRateShading = VK_TRUE;

    std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfoList = VkQueueFactory::GetInstance()->FindQueue();

    VkDeviceCreateInfo vkDeviceCreateInfoObj{};
    vkDeviceCreateInfoObj.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    vkDeviceCreateInfoObj.queueCreateInfoCount = (uint32_t)deviceQueueCreateInfoList.size();
    vkDeviceCreateInfoObj.pQueueCreateInfos = deviceQueueCreateInfoList.data();
    vkDeviceCreateInfoObj.enabledExtensionCount = (uint32_t)validationManagerObj->deviceExtensionNameList.size();
    vkDeviceCreateInfoObj.enabledLayerCount = 0;
    vkDeviceCreateInfoObj.pEnabledFeatures = &enabledPhysicalDeviceFeatures;
    vkDeviceCreateInfoObj.ppEnabledExtensionNames = validationManagerObj->deviceExtensionNameList.data();
    vkDeviceCreateInfoObj.ppEnabledLayerNames = nullptr;

    ErrorCheck(vkCreateDevice(vkPhysicalDeviceObj, &vkDeviceCreateInfoObj, pAllocator, &vkLogicalDeviceObj));

    CoreObjects::logicalDeviceObj = &vkLogicalDeviceObj;
}


void VulkanManager::CreateSurface(GLFWwindow * glfwWindow)
{
#if defined(GLFW_ENABLED)
    if (VK_SUCCESS != glfwCreateWindowSurface(vkInstanceObj, glfwWindow, nullptr, &surface)) 
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
    vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDeviceObj, 
        VkQueueFactory::GetInstance()->GetGraphicsQueueFamilyIndex(), 
        surface, &WSI_supported);
    if (!WSI_supported) 
    {
        ASSERT_MSG_DEBUG(0, "WSI not supported");
        std::exit(-1);
    }

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkPhysicalDeviceObj, surface, &surfaceCapabilities);

    if (surfaceCapabilities.currentExtent.width < UINT32_MAX)
    {
        surfaceWidth = surfaceCapabilities.currentExtent.width;
        surfaceHeight = surfaceCapabilities.currentExtent.height;
    }

    {
        uint32_t format_count = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDeviceObj, surface, &format_count, nullptr);
        if (format_count == 0) 
        {
            ASSERT_MSG_DEBUG(0, "Surface formats missing.");
            std::exit(-1);
        }
        std::vector<VkSurfaceFormatKHR> formats(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDeviceObj, surface, &format_count, formats.data());
        if (formats[0].format == VK_FORMAT_UNDEFINED) 
        {
            surfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
            surfaceFormat.colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
        }
        else 
        {
            surfaceFormat = formats[0];
        }
    }
}

bool VulkanManager::IsSampleRateShadingAvailable()
{
    if (physicalDeviceFeatures.sampleRateShading)
    {
        return true;
    }
    
    return false;
}

VkSampleCountFlagBits VulkanManager::GetMaxUsableVKSampleCount()
{
    VkSampleCountFlags counts =  std::min(physicalDeviceProps.limits.framebufferColorSampleCounts, physicalDeviceProps.limits.framebufferDepthSampleCounts);
    if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
    if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
    if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
    if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
    if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
    if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }
    return VK_SAMPLE_COUNT_1_BIT;
}

Samples VulkanManager::GetMaxUsableSampleCount()
{
    VkSampleCountFlags counts = GetMaxUsableVKSampleCount();
    
    if (physicalDeviceProps.limits.framebufferColorSampleCounts < counts ||
        physicalDeviceProps.limits.framebufferDepthSampleCounts < counts )
    {
        ASSERT_MSG_DEBUG(0, "sample counts not valid");
        counts = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
    }

    Samples samples = VulkanUnwrap::UnWrapSampleCount(counts);
    return samples;
}

void VulkanManager::GetPhysicalDevice()
{
    uint32_t count = 0;
    vkEnumeratePhysicalDevices(vkInstanceObj, &count, nullptr);
    std::vector<VkPhysicalDevice> deviceList(count);
    vkEnumeratePhysicalDevices(vkInstanceObj, &count, deviceList.data());

    VkPhysicalDevice discreteGpu = VK_NULL_HANDLE;
    VkPhysicalDevice integratedGpu = VK_NULL_HANDLE;
    for (auto dev : deviceList)
    {
        VkPhysicalDeviceProperties deviceProp = {};
        vkGetPhysicalDeviceProperties(dev, &deviceProp);

        if (deviceProp.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) // deviceProp.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU ||
        {
            //vkPhysicalDeviceObj = dev;
            //CoreObjects::physicalDeviceObj = &vkPhysicalDeviceObj;
            discreteGpu = dev;
            break;
        }

        if (deviceProp.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
        {
            //vkPhysicalDeviceObj = dev;
            //CoreObjects::physicalDeviceObj = &vkPhysicalDeviceObj;
            integratedGpu = dev;
        }
    }

    if (discreteGpu != VK_NULL_HANDLE)
    {
        vkPhysicalDeviceObj = discreteGpu;
        CoreObjects::physicalDeviceObj = &vkPhysicalDeviceObj;
    }
    else if (integratedGpu!= VK_NULL_HANDLE)
    {
        vkPhysicalDeviceObj = integratedGpu;
        CoreObjects::physicalDeviceObj = &vkPhysicalDeviceObj;
    }

    if (vkPhysicalDeviceObj == VK_NULL_HANDLE)
    {
        ASSERT_MSG_DEBUG(0, "gpu required");
        std::exit(-1);
    }

    vkGetPhysicalDeviceMemoryProperties(vkPhysicalDeviceObj, &physicalDeviceMemProps);
    vkGetPhysicalDeviceProperties(vkPhysicalDeviceObj, &physicalDeviceProps);
    vkGetPhysicalDeviceFeatures(vkPhysicalDeviceObj, &physicalDeviceFeatures);
}

void VulkanManager::Init()
{
    /*
    PLOGD << "VulkanManager init";

    CreateInstance();
    GetPhysicalDevice();
    validationManagerObj->InitDebug(&vkInstanceObj, pAllocator);
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

    VulkanMemoryManager::GetSingleton()->Init(physicalDeviceMemProps);
    VkCommandBufferFactory::GetInstance()->Init();
    VkSynchroniserFactory::GetInstance()->Init();
    */
}

void VulkanManager::Init(QueueWrapper * queueRequirement, const uint32_t & count)
{
    PLOGD << "VulkanManager init";

    CreateInstance();
    GetPhysicalDevice();
    validationManagerObj->InitDebug(&vkInstanceObj, pAllocator);
    VkQueueFactory::GetInstance()->Init();
    CreateLogicalDevice();

    //TODO : expose queue creation.

    for (uint32_t i = 0; i < count; i++)
    {
        if (*queueRequirement[i].queueType == QueueType::GRAPHICS)
        {
            if (*queueRequirement[i].purpose == QueuePurpose::PRESENT)
            {
                VkQueueFactory::GetInstance()->CreateGraphicsQueues(&queueRequirement[i].queueId, 1);
                CoreObjects::presentationQueuedId = queueRequirement[i].queueId;
            }

            if (*queueRequirement[i].purpose == QueuePurpose::RENDER)
            {
                VkQueueFactory::GetInstance()->CreateGraphicsQueues(&queueRequirement[i].queueId, 1);
                CoreObjects::renderQueueId = queueRequirement[i].queueId;
            }
        }

        if (*queueRequirement[i].queueType == QueueType::COMPUTE)
        {
            VkQueueFactory::GetInstance()->CreateComputeQueues(&queueRequirement[i].queueId, 1);
            CoreObjects::computeQueueId = queueRequirement[i].queueId;
        }

        if (*queueRequirement[i].queueType == QueueType::TRANSFER)
        {
            VkQueueFactory::GetInstance()->CreateTransferQueues(&queueRequirement[i].queueId, 1);
            CoreObjects::transferQueueId = queueRequirement[i].queueId;
        }

        queueRequirement[i].queueFamilyId = VkQueueFactory::GetInstance()->GetQueueFamilyIndex(*queueRequirement[i].queueType, queueRequirement[i].queueId);
        VkQueueFactory::GetInstance()->SetQueuePurpose(queueRequirement[i].purpose, *queueRequirement[i].queueType, queueRequirement[i].queueId);
    }

    VulkanMemoryManager::GetSingleton()->Init(physicalDeviceMemProps);
    /*VkCommandBufferFactory::GetInstance()->Init();
    VkSynchroniserFactory::GetInstance()->Init();
    VulkanGraphicsPipelineFactory::GetInstance()->Init();*/
}

void VulkanManager::InitializeFactories()
{
    PresentationEngine::GetInstance()->Init(VulkanManager::GetInstance()->GetSurface(), VulkanManager::GetInstance()->GetSurfaceFormat());
    VkAttachmentFactory::GetInstance()->Init();
    VkRenderPassFactory::GetInstance()->Init();
    VkFrameBufferFactory::GetInstance()->Init();

    VkCommandBufferFactory::GetInstance()->Init();
    VkSynchroniserFactory::GetInstance()->Init();
    VulkanGraphicsPipelineFactory::GetInstance()->Init();
    VkShaderFactory::GetInstance()->Init();
    VkDescriptorPoolFactory::GetInstance()->Init();
    VkBufferFactory::GetInstance()->Init();
    VkShaderResourceManager::GetInstance()->Init();
    VkSamplerFactory::GetInstance()->Init();
}

void VulkanManager::DeInit()
{
    PLOGD << "VulkanManager Deinit";

    VkSamplerFactory::GetInstance()->DeInit();
    delete VkSamplerFactory::GetInstance();
    

    VkShaderResourceManager::GetInstance()->DeInit();
    delete VkShaderResourceManager::GetInstance();

    VkDescriptorPoolFactory::GetInstance()->DeInit();
    delete VkDescriptorPoolFactory::GetInstance();

    VkBufferFactory::GetInstance()->DeInit();
    delete VkBufferFactory::GetInstance();

    VkShaderFactory::GetInstance()->DeInit();
    delete VkShaderFactory::GetInstance();

    VulkanGraphicsPipelineFactory::GetInstance()->DeInit();
    delete VulkanGraphicsPipelineFactory::GetInstance();

    VkSynchroniserFactory::GetInstance()->DeInit();
    delete VkSynchroniserFactory::GetInstance();

    VkCommandBufferFactory::GetInstance()->DeInit();
    delete VkCommandBufferFactory::GetInstance();

    VkFrameBufferFactory::GetInstance()->DeInit();
    delete VkFrameBufferFactory::GetInstance();

    VkRenderPassFactory::GetInstance()->DeInit();
    delete VkRenderPassFactory::GetInstance();

    VkAttachmentFactory::GetInstance()->DeInit();
    delete VkAttachmentFactory::GetInstance();

    PresentationEngine::GetInstance()->DeInit();
    delete PresentationEngine::GetInstance();

    VulkanMemoryManager::GetSingleton()->DeInit();
    delete VulkanMemoryManager::GetSingleton();

    vkDestroySurfaceKHR(vkInstanceObj, surface, pAllocator);
    vkDestroyDevice(vkLogicalDeviceObj, pAllocator);
    validationManagerObj->DeinitDebug();

    VkQueueFactory::GetInstance()->DeInit();
    delete VkQueueFactory::GetInstance();

    vkDestroyInstance(vkInstanceObj, pAllocator);
}

void VulkanManager::Update()
{

}

VulkanManager * VulkanManager::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new VulkanManager();
    }
    return instance;
}

VulkanManager::~VulkanManager()
{
    delete validationManagerObj;
}

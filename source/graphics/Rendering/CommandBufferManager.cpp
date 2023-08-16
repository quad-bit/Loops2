
#include "CommandBufferManager.h"

#if (RENDERING_API == VULKAN)

#include "VulkanInterface.h"

CommandBufferManager<VulkanInterface>* CommandBufferManager<VulkanInterface>::instance = nullptr;

CommandBufferManager<VulkanInterface> * CommandBufferManager<VulkanInterface>::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new CommandBufferManager();
    }
    return instance;
}
#elif (RENDERING_API == DX)

#include "DxInterface.h"

CommandBufferManager<DxInterface>* CommandBufferManager<DxInterface>::instance = nullptr;
CommandBufferManager<DxInterface> * CommandBufferManager<DxInterface>::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new CommandBufferManager();
    }
    return instance;
}

#endif

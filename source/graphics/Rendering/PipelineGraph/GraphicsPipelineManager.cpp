#include "GraphicsPipelineManager.h"
#include "Settings.h"
#include "DrawCommandBuffer.h"

#if (RENDERING_API == VULKAN)

#include "VulkanInterface.h"

GraphicsPipelineManager<VulkanInterface>* GraphicsPipelineManager<VulkanInterface>::instance = nullptr;

GraphicsPipelineManager<VulkanInterface> * GraphicsPipelineManager<VulkanInterface>::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new GraphicsPipelineManager();
    }
    return instance;
}
#elif (RENDERING_API == DX)

#include "DxInterface.h"

GraphicsPipelineManager<DxInterface>* GraphicsPipelineManager<DxInterface>::instance = nullptr;
GraphicsPipelineManager<DxInterface> * GraphicsPipelineManager<DxInterface>::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new GraphicsPipelineManager();
    }
    return instance;
}

#endif

void PipelineDrawNode::Entry()
{
    DrawGraphUtil::pipelineLayoutId = this->pipelineLayoutId;
    
    //vkCmdBindPipeline(commandBufferObj, VK_PIPELINE_BIND_POINT_GRAPHICS, objectPipeline->GetObjectPipeline()); 
    PipelineType type = PipelineType::GRAPHICS;
    dcb->BindPipeline(&type, this->pipelineId);
}

void PipelineDrawNode::Exit()
{

}
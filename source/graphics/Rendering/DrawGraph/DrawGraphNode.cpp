#include "DrawGraphNode.h"
#include "DrawCommandBuffer.h"

uint32_t DrawGraphUtil::pipelineLayoutId = 0 ;
std::vector<uint32_t> DrawGraphUtil::descriptorIdList;
uint32_t DrawGraphUtil::setOffset = 0;

DrawCommandBuffer<ApiInterface> * DrawGraphNode::dcb = nullptr;

void DrawGraphNode::Exit()
{
    DrawGraphUtil::setOffset = (uint32_t)DrawGraphUtil::descriptorIdList.size() - 1;
    DrawGraphUtil::descriptorIdList.pop_back();
}

void TransformNode::Entry()
{
    PipelineType bindPoint = PipelineType::GRAPHICS;

    // Binding the descriptor set for transform
    DescriptorSetBindingInfo info = {};
    info.descriptorSetId = (this->descriptorSetIds[Settings::currentFrameInFlight]);
    info.firstSet = DrawGraphUtil::setOffset;
    info.dynamicOffsetCount = 0;
    info.pDynamicOffsets = nullptr;
    info.pipelineBindPoint = &bindPoint;
    info.pipelineLayoutId = DrawGraphUtil::pipelineLayoutId;

    DrawGraphUtil::descriptorIdList.push_back(info.descriptorSetId);

    info.descriptorSetIds = (DrawGraphUtil::descriptorIdList);
    dcb->BindDescriptorSets(&info);
}

void MeshNode::Entry()
{
    VertexBufferBindingInfo info = {};
    info.bindingCount = 1;
    info.firstBinding = 0;
    info.bufferIds = this->bufferIds;
    info.pOffsets = this->pOffsets;
    dcb->BindVertexBuffers(&info);

    if (isIndexed)
    {
        IndexBufferBindingInfo indInfo = {};
        indInfo.bufferId = this->indexBufferId;
        indInfo.indexType = IndexType::INDEX_TYPE_UINT32;
        indInfo.offset = 0;
        dcb->BindIndexBuffer(&indInfo);
    }
}

void MeshNode::Exit()
{

}

void IndexedDrawNode::Entry()
{
    dcb->DrawIndex(&this->info);
}

void IndexedDrawNode::Exit()
{

}

void DrawArrayDrawNode::Entry()
{
    dcb->Draw(&this->info);
}

void DrawArrayDrawNode::Exit()
{
}

void LightDrawNode::Entry()
{
    // Binding the descriptor set for Light
    DescriptorSetBindingInfo info = {};
    info.descriptorSetId = this->descriptorIds[Settings::currentFrameInFlight];
    DrawGraphUtil::descriptorIdList.push_back(info.descriptorSetId);
}


void CameraDrawNode::Entry()
{
    PipelineType bindPoint = PipelineType::GRAPHICS;

    // Binding the descriptor set for Camera
    DescriptorSetBindingInfo info = {};
    //info.descriptorSetIds.push_back( this->descriptorIds[Settings::currentFrameInFlight]);
    //info.dynamicOffsetCount = 0;
    //info.firstSet = 0;
    //info.pDynamicOffsets = nullptr;
    //info.pipelineBindPoint = &bindPoint;
    //info.pipelineLayoutId = DrawGraphUtil::pipelineLayoutId;
    info.descriptorSetId = this->descriptorIds[Settings::currentFrameInFlight];
    DrawGraphUtil::descriptorIdList.push_back(info.descriptorSetId);
    //dcb->BindDescriptorSets(&info);
}

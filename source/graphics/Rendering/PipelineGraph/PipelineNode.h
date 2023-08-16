/*
#pragma once

#include "RenderingWrapper.h"

template <typename T>
struct PipelineNode
{
    T * apiInterface;
    std::vector<uint32_t> meshIdList;
    PipelineStates pipeLineState;
    PipelineNode(T * obj)
    {
        apiInterface = obj;
    }
    virtual void Execute() = 0;
};

template<typename T>
struct VertexInputNode : public PipelineNode<T>
{

    virtual void Execute()
    {
        //apiInterface->InitiateGraphicsPipelineCreation(mesh->meshId, attribInfo, attribCount, bindingInfo, numVertexBufferPerMesh);
    }
};
*/
#include "CorePrecompiled.h"

#pragma once
#include "Utility/RenderingWrappers/RenderingWrapper.h"
#include "ECS/Components/Shader.h"
#include <map>
#include "ECS/Events/EventBus.h"
#include "Container/BitArray.h"
#include "ECS/Events/Event.h"

struct GraphTraversalEvent : public Event
{
    
};

#define PRINT_STATE 1

namespace PipelineUtil
{
    extern std::map<PipelineStates, uint32_t> stateToIdMap;
    extern std::vector<uint32_t> pipelineStateMeshList;
    extern std::vector<SetWrapper *>  setsPerPipeline;
    extern uint32_t pipelineLayoutId;
    extern bool tagValidity;
    extern uint16_t globalTagMask;

    void FillGlobalMeshList(std::vector<uint32_t> & meshList, const PipelineStates & currentState);
    void const CheckTagValidity(const uint16_t & tag);
}

struct StateWrapperBase
{
protected:
    uint32_t id;

public:
    PipelineStates state;
    std::vector<uint32_t> meshIdList;
    
    uint16_t tagMask;

    StateWrapperBase()
    {
        tagMask = (uint16_t)RenderPassTag::None;
    }

    const uint32_t & GetId() const
    {
        return id;
    }

    virtual void Execute() = 0;
    
    // no concept of Entry, Exit, see function FindAllPaths
    // virtual void Exit() = 0;
};

template<typename T>
void AssignId(uint32_t * id)
{
    *id = T::idCounter++;
}

template<typename T>
void DecrementIdCounter()
{
    T::idCounter--;
}

struct VertexInputWrapper : public StateWrapperBase
{
    VertexInputState * inputState;
    static uint32_t idCounter;

    VertexInputWrapper()
    {
        AssignId<VertexInputWrapper>(&this->id);
        state = PipelineStates::VertexInputState;
    }

    virtual void Execute() override
    {
#if PRINT_STATE 
        PLOGD << std::endl << std::endl;
        PLOGD << std::to_string(this->GetId())  + " PipelineStates::VertexInputState " ;
#endif
        PipelineUtil::stateToIdMap.insert(std::pair<PipelineStates, uint32_t>({ PipelineStates::VertexInputState, id }));
        PipelineUtil::FillGlobalMeshList(meshIdList, state);
        PipelineUtil::CheckTagValidity(tagMask);
    }
};

struct InputAssemblyWrapper : public StateWrapperBase
{
    InputAssemblyState * assemblyState;
    static uint32_t idCounter;

    InputAssemblyWrapper()
    {
        AssignId<InputAssemblyWrapper>(&this->id);
        state = PipelineStates::InputAssemblyState;
    }

    virtual void Execute() override
    {
#if PRINT_STATE 
        PLOGD << std::to_string(this->GetId()) + " PipelineStates::InputAssemblyState";
#endif
        PipelineUtil::stateToIdMap.insert(std::pair<PipelineStates, uint32_t>({ PipelineStates::InputAssemblyState, id }));
        PipelineUtil::FillGlobalMeshList(meshIdList, state);
        PipelineUtil::CheckTagValidity(tagMask); 
    }
};

struct ShaderStateWrapper : public StateWrapperBase
{
    Shader * shader;// one shader equivalent to one shader module
    uint32_t shaderCount;
    RenderPassTag tag;
    static uint32_t idCounter;

    ShaderStateWrapper()
    {
        AssignId<ShaderStateWrapper>(&this->id);
        state = PipelineStates::ShaderStage;
    }

    virtual void Execute() override
    {
#if PRINT_STATE
        PLOGD << std::to_string(this->GetId()) + " PipelineStates::ShaderStateWrapper";

        std::string meshIds;
        for (auto id : meshIdList)
        {
            meshIds += (std::to_string(id) + " ");
        }

        //PLOGD << meshIds + " PipelineStates::ShaderStateWrapper";
#endif
        PipelineUtil::stateToIdMap.insert(std::pair<PipelineStates, uint32_t>({ PipelineStates::ShaderStage, id }));
        PipelineUtil::FillGlobalMeshList(meshIdList, state);
        PipelineUtil::CheckTagValidity(tagMask);
    }
};

struct ShaderResourceStateWrapper : public StateWrapperBase
{
    Shader * shader;
    uint32_t shaderCount;
    std::vector<SetWrapper*> resourcesSetList;
    uint32_t pipelineLayoutId;

    static uint32_t idCounter;

    ShaderResourceStateWrapper()
    {
        AssignId<ShaderResourceStateWrapper>(&this->id);
        state = PipelineStates::ShaderResourcesLayout;
    }

    virtual void Execute() override
    {
#if PRINT_STATE
        PLOGD << std::to_string(this->GetId()) + " PipelineStates::ShaderResourcesLayout";
        
        std::string meshIds;
        for (auto id : meshIdList)
        {
            meshIds += (std::to_string(id) + " ");
        }

        //PLOGD << meshIds + " PipelineStates::ShaderResourcesLayout";
#endif
        PipelineUtil::stateToIdMap.insert(std::pair<PipelineStates, uint32_t>({ PipelineStates::ShaderResourcesLayout, id }));
        PipelineUtil::FillGlobalMeshList(meshIdList, state);
        PipelineUtil::CheckTagValidity(tagMask);
        PipelineUtil::setsPerPipeline = resourcesSetList;
        PipelineUtil::pipelineLayoutId = pipelineLayoutId;
    }
};

struct TessellationStateWrapper : public StateWrapperBase
{
    static uint32_t idCounter;
    TessellationState * tessellationState;
    TessellationStateWrapper()
    {
        AssignId<TessellationStateWrapper>(&this->id);
        state = PipelineStates::TessellationState;
    }

    virtual void Execute() override
    {
#if PRINT_STATE
        PLOGD << std::to_string(this->GetId()) + " PipelineStates::Tessellation";
#endif
        PipelineUtil::stateToIdMap.insert(std::pair<PipelineStates, uint32_t>({ PipelineStates::TessellationState, id }));
        PipelineUtil::FillGlobalMeshList(meshIdList, state);
        PipelineUtil::CheckTagValidity(tagMask);
    }
};

struct ColorBlendStateWrapper : public StateWrapperBase
{
    static uint32_t idCounter;
    ColorBlendState * colorBlendState;
    ColorBlendStateWrapper()
    {
        AssignId<ColorBlendStateWrapper >(&this->id);
        state = PipelineStates::ColorBlendState;
    }

    virtual void Execute() override
    {
#if PRINT_STATE
        PLOGD << std::to_string(this->GetId())  + " PipelineStates::Colorblend";
#endif
        PipelineUtil::stateToIdMap.insert(std::pair<PipelineStates, uint32_t>({ PipelineStates::ColorBlendState, id }));
        PipelineUtil::FillGlobalMeshList(meshIdList, state);
        PipelineUtil::CheckTagValidity(tagMask);
    }
};

struct ViewPortStateWrapper : public StateWrapperBase
{
    static uint32_t idCounter;
    ViewportState * viewportState;
    ViewPortStateWrapper()
    {
        AssignId<ViewPortStateWrapper>(&this->id);
        state = PipelineStates::ViewportState;
    }

    virtual void Execute() override
    {
#if PRINT_STATE
        PLOGD << std::to_string(this->GetId())  + " PipelineStates::Viewport";
#endif
        PipelineUtil::stateToIdMap.insert(std::pair<PipelineStates, uint32_t>({ PipelineStates::ViewportState, id }));
        PipelineUtil::FillGlobalMeshList(meshIdList, state);
        PipelineUtil::CheckTagValidity(tagMask);
    }
};

struct RasterizationStateWrapper : public StateWrapperBase
{
    static uint32_t idCounter;
    RasterizationState * rasterState;
    RasterizationStateWrapper()
    {
        AssignId<RasterizationStateWrapper>(&this->id);
        state = PipelineStates::RasterizationState;
    }

    virtual void Execute() override
    {
#if PRINT_STATE
        PLOGD << std::to_string(this->GetId())  + " PipelineStates::Rasterisation";
#endif
        PipelineUtil::stateToIdMap.insert(std::pair<PipelineStates, uint32_t>({ PipelineStates::RasterizationState, id }));
        PipelineUtil::FillGlobalMeshList(meshIdList, state);
        PipelineUtil::CheckTagValidity(tagMask);
    }
};

struct MultiSampleStateWrapper : public StateWrapperBase
{
    static uint32_t idCounter;
    MultiSampleState * multiSampleState;
    MultiSampleStateWrapper()
    {
        AssignId<MultiSampleStateWrapper>(&this->id);
        state = PipelineStates::MultisampleState;
    }

    virtual void Execute() override
    {
#if PRINT_STATE
        PLOGD << std::to_string(this->GetId())  + " PipelineStates::MultiSample";
#endif
        PipelineUtil::stateToIdMap.insert(std::pair<PipelineStates, uint32_t>({ PipelineStates::MultisampleState, id }));
        PipelineUtil::FillGlobalMeshList(meshIdList, state);
        PipelineUtil::CheckTagValidity(tagMask);
    }
};

struct DepthStencilStateWrapper : public StateWrapperBase
{
    static uint32_t idCounter;
    DepthStencilState * depthState;
    DepthStencilStateWrapper()
    {
        AssignId<DepthStencilStateWrapper>(&this->id);
        state = PipelineStates::DepthStencilState;
    }

    virtual void Execute() override
    {
#if PRINT_STATE
        PLOGD << std::to_string(this->GetId())  + " PipelineStates::DepthStencilState";
#endif
        PipelineUtil::stateToIdMap.insert(std::pair<PipelineStates, uint32_t>({ PipelineStates::DepthStencilState, id }));
        PipelineUtil::FillGlobalMeshList(meshIdList, state);
        PipelineUtil::CheckTagValidity(tagMask);
    }
};

struct DynamicStateWrapper : public StateWrapperBase
{
    static uint32_t idCounter;
    DynamicStateList * dynamicStates;
    DynamicStateWrapper()
    {
        AssignId<DynamicStateWrapper>(&this->id);
        state = PipelineStates::DynamicState;
    }

    virtual void Execute() override
    {
#if PRINT_STATE
        PLOGD << std::to_string(this->GetId())  + " PipelineStates::DynamicState";
#endif
        PipelineUtil::stateToIdMap.insert(std::pair<PipelineStates, uint32_t>({ PipelineStates::DynamicState, id }));
        PipelineUtil::FillGlobalMeshList(meshIdList, state);
        PipelineUtil::CheckTagValidity(tagMask);
        GraphTraversalEvent event;
        EventBus::GetInstance()->Publish(&event);
    }
};

#include "CorePrecompiled.h"

#pragma once
#include "Utility/RenderingWrappers/RenderingWrapper.h"
#include "ECS/Components/Shader.h"
#include <map>
#include "ECS/Events/EventBus.h"
#include "Container/BitArray.h"
#include "ECS/Events/Event.h"

struct GraphTraversalEvent : public Core::ECS::Events::Event
{

};

#define PRINT_STATE 1

namespace Core
{
    namespace Utility
    {
        namespace PipelineUtil
        {
            extern std::map<Core::Enums::PipelineStates, uint32_t> stateToIdMap;
            extern std::vector<uint32_t> pipelineStateMeshList;
            extern std::vector<Core::Wrappers::SetWrapper*>  setsPerPipeline;
            extern uint32_t pipelineLayoutId;
            extern bool tagValidity;
            extern uint16_t globalTagMask;

            void FillGlobalMeshList(std::vector<uint32_t>& meshList, const Core::Enums::PipelineStates& currentState);
            void const CheckTagValidity(const uint16_t& tag);
        }

        struct StateWrapperBase
        {
        protected:
            uint32_t id;

        public:
            Core::Enums::PipelineStates state;
            std::vector<uint32_t> meshIdList;

            uint16_t tagMask;

            StateWrapperBase()
            {
                tagMask = (uint16_t)Core::Enums::RenderPassTag::None;
            }

            const uint32_t& GetId() const
            {
                return id;
            }

            virtual void Execute() = 0;

            // no concept of Entry, Exit, see function FindAllPaths
            // virtual void Exit() = 0;
        };

        template<typename T>
        void AssignId(uint32_t* id)
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
            Core::Wrappers::VertexInputState* inputState;
            static uint32_t idCounter;

            VertexInputWrapper()
            {
                AssignId<VertexInputWrapper>(&this->id);
                state = Core::Enums::PipelineStates::VertexInputState;
            }

            virtual void Execute() override
            {
#if PRINT_STATE 
                PLOGD << std::endl << std::endl;
                PLOGD << std::to_string(this->GetId()) + " Core::Enums::PipelineStates::VertexInputState ";
#endif
                PipelineUtil::stateToIdMap.insert(std::pair<Core::Enums::PipelineStates, uint32_t>({ Core::Enums::PipelineStates::VertexInputState, id }));
                PipelineUtil::FillGlobalMeshList(meshIdList, state);
                PipelineUtil::CheckTagValidity(tagMask);
            }
        };

        struct InputAssemblyWrapper : public StateWrapperBase
        {
            Core::Wrappers::InputAssemblyState* assemblyState;
            static uint32_t idCounter;

            InputAssemblyWrapper()
            {
                AssignId<InputAssemblyWrapper>(&this->id);
                state = Core::Enums::PipelineStates::InputAssemblyState;
            }

            virtual void Execute() override
            {
#if PRINT_STATE 
                PLOGD << std::to_string(this->GetId()) + " Core::Enums::PipelineStates::InputAssemblyState";
#endif
                PipelineUtil::stateToIdMap.insert(std::pair<Core::Enums::PipelineStates, uint32_t>({ Core::Enums::PipelineStates::InputAssemblyState, id }));
                PipelineUtil::FillGlobalMeshList(meshIdList, state);
                PipelineUtil::CheckTagValidity(tagMask);
            }
        };

        struct ShaderStateWrapper : public StateWrapperBase
        {
            Shader* shader;// one shader equivalent to one shader module
            uint32_t shaderCount;
            Core::Enums::RenderPassTag tag;
            static uint32_t idCounter;

            ShaderStateWrapper()
            {
                AssignId<ShaderStateWrapper>(&this->id);
                state = Core::Enums::PipelineStates::ShaderStage;
            }

            virtual void Execute() override
            {
#if PRINT_STATE
                PLOGD << std::to_string(this->GetId()) + " Core::Enums::PipelineStates::ShaderStateWrapper";

                std::string meshIds;
                for (auto id : meshIdList)
                {
                    meshIds += (std::to_string(id) + " ");
                }

                //PLOGD << meshIds + " Core::Enums::PipelineStates::ShaderStateWrapper";
#endif
                PipelineUtil::stateToIdMap.insert(std::pair<Core::Enums::PipelineStates, uint32_t>({ Core::Enums::PipelineStates::ShaderStage, id }));
                PipelineUtil::FillGlobalMeshList(meshIdList, state);
                PipelineUtil::CheckTagValidity(tagMask);
            }
        };

        struct ShaderResourceStateWrapper : public StateWrapperBase
        {
            Shader* shader;
            uint32_t shaderCount;
            std::vector<Core::Wrappers::SetWrapper*> resourcesSetList;
            uint32_t pipelineLayoutId;

            static uint32_t idCounter;

            ShaderResourceStateWrapper()
            {
                AssignId<ShaderResourceStateWrapper>(&this->id);
                state = Core::Enums::PipelineStates::ShaderResourcesLayout;
            }

            virtual void Execute() override
            {
#if PRINT_STATE
                PLOGD << std::to_string(this->GetId()) + " Core::Enums::PipelineStates::ShaderResourcesLayout";

                std::string meshIds;
                for (auto id : meshIdList)
                {
                    meshIds += (std::to_string(id) + " ");
                }

                //PLOGD << meshIds + " Core::Enums::PipelineStates::ShaderResourcesLayout";
#endif
                PipelineUtil::stateToIdMap.insert(std::pair<Core::Enums::PipelineStates, uint32_t>({ Core::Enums::PipelineStates::ShaderResourcesLayout, id }));
                PipelineUtil::FillGlobalMeshList(meshIdList, state);
                PipelineUtil::CheckTagValidity(tagMask);
                PipelineUtil::setsPerPipeline = resourcesSetList;
                PipelineUtil::pipelineLayoutId = pipelineLayoutId;
            }
        };

        struct TessellationStateWrapper : public StateWrapperBase
        {
            static uint32_t idCounter;
            Core::Wrappers::TessellationState* tessellationState;
            TessellationStateWrapper()
            {
                AssignId<TessellationStateWrapper>(&this->id);
                state = Core::Enums::PipelineStates::TessellationState;
            }

            virtual void Execute() override
            {
#if PRINT_STATE
                PLOGD << std::to_string(this->GetId()) + " Core::Enums::PipelineStates::Tessellation";
#endif
                PipelineUtil::stateToIdMap.insert(std::pair<Core::Enums::PipelineStates, uint32_t>({ Core::Enums::PipelineStates::TessellationState, id }));
                PipelineUtil::FillGlobalMeshList(meshIdList, state);
                PipelineUtil::CheckTagValidity(tagMask);
            }
        };

        struct ColorBlendStateWrapper : public StateWrapperBase
        {
            static uint32_t idCounter;
            Core::Wrappers::ColorBlendState* colorBlendState;
            ColorBlendStateWrapper()
            {
                AssignId<ColorBlendStateWrapper >(&this->id);
                state = Core::Enums::PipelineStates::ColorBlendState;
            }

            virtual void Execute() override
            {
#if PRINT_STATE
                PLOGD << std::to_string(this->GetId()) + " Core::Enums::PipelineStates::Colorblend";
#endif
                PipelineUtil::stateToIdMap.insert(std::pair<Core::Enums::PipelineStates, uint32_t>({ Core::Enums::PipelineStates::ColorBlendState, id }));
                PipelineUtil::FillGlobalMeshList(meshIdList, state);
                PipelineUtil::CheckTagValidity(tagMask);
            }
        };

        struct ViewPortStateWrapper : public StateWrapperBase
        {
            static uint32_t idCounter;
            Core::Wrappers::ViewportState* viewportState;
            ViewPortStateWrapper()
            {
                AssignId<ViewPortStateWrapper>(&this->id);
                state = Core::Enums::PipelineStates::ViewportState;
            }

            virtual void Execute() override
            {
#if PRINT_STATE
                PLOGD << std::to_string(this->GetId()) + " Core::Enums::PipelineStates::Viewport";
#endif
                PipelineUtil::stateToIdMap.insert(std::pair<Core::Enums::PipelineStates, uint32_t>({ Core::Enums::PipelineStates::ViewportState, id }));
                PipelineUtil::FillGlobalMeshList(meshIdList, state);
                PipelineUtil::CheckTagValidity(tagMask);
            }
        };

        struct RasterizationStateWrapper : public StateWrapperBase
        {
            static uint32_t idCounter;
            Core::Wrappers::RasterizationState* rasterState;
            RasterizationStateWrapper()
            {
                AssignId<RasterizationStateWrapper>(&this->id);
                state = Core::Enums::PipelineStates::RasterizationState;
            }

            virtual void Execute() override
            {
#if PRINT_STATE
                PLOGD << std::to_string(this->GetId()) + " Core::Enums::PipelineStates::Rasterisation";
#endif
                PipelineUtil::stateToIdMap.insert(std::pair<Core::Enums::PipelineStates, uint32_t>({ Core::Enums::PipelineStates::RasterizationState, id }));
                PipelineUtil::FillGlobalMeshList(meshIdList, state);
                PipelineUtil::CheckTagValidity(tagMask);
            }
        };

        struct MultiSampleStateWrapper : public StateWrapperBase
        {
            static uint32_t idCounter;
            Core::Wrappers::MultiSampleState* multiSampleState;
            MultiSampleStateWrapper()
            {
                AssignId<MultiSampleStateWrapper>(&this->id);
                state = Core::Enums::PipelineStates::MultisampleState;
            }

            virtual void Execute() override
            {
#if PRINT_STATE
                PLOGD << std::to_string(this->GetId()) + " Core::Enums::PipelineStates::MultiSample";
#endif
                PipelineUtil::stateToIdMap.insert(std::pair<Core::Enums::PipelineStates, uint32_t>({ Core::Enums::PipelineStates::MultisampleState, id }));
                PipelineUtil::FillGlobalMeshList(meshIdList, state);
                PipelineUtil::CheckTagValidity(tagMask);
            }
        };

        struct DepthStencilStateWrapper : public StateWrapperBase
        {
            static uint32_t idCounter;
            Core::Wrappers::DepthStencilState* depthState;
            DepthStencilStateWrapper()
            {
                AssignId<DepthStencilStateWrapper>(&this->id);
                state = Core::Enums::PipelineStates::DepthStencilState;
            }

            virtual void Execute() override
            {
#if PRINT_STATE
                PLOGD << std::to_string(this->GetId()) + " Core::Enums::PipelineStates::DepthStencilState";
#endif
                PipelineUtil::stateToIdMap.insert(std::pair<Core::Enums::PipelineStates, uint32_t>({ Core::Enums::PipelineStates::DepthStencilState, id }));
                PipelineUtil::FillGlobalMeshList(meshIdList, state);
                PipelineUtil::CheckTagValidity(tagMask);
            }
        };

        struct DynamicStateWrapper : public StateWrapperBase
        {
            static uint32_t idCounter;
            Core::Wrappers::DynamicStateList* dynamicStates;
            DynamicStateWrapper()
            {
                AssignId<DynamicStateWrapper>(&this->id);
                state = Core::Enums::PipelineStates::DynamicState;
            }

            virtual void Execute() override
            {
#if PRINT_STATE
                PLOGD << std::to_string(this->GetId()) + " Core::Enums::PipelineStates::DynamicState";
#endif
                PipelineUtil::stateToIdMap.insert(std::pair<Core::Enums::PipelineStates, uint32_t>({ Core::Enums::PipelineStates::DynamicState, id }));
                PipelineUtil::FillGlobalMeshList(meshIdList, state);
                PipelineUtil::CheckTagValidity(tagMask);
                GraphTraversalEvent event;
                Core::ECS::Events::EventBus::GetInstance()->Publish(&event);
            }
        };
    }
}
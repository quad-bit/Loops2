#include "Utility/PipelineStateWrappers.h"

std::map<Core::Enums::PipelineStates, uint32_t> Core::Utility::PipelineUtil::stateToIdMap;
std::vector<uint32_t> Core::Utility::PipelineUtil::pipelineStateMeshList;
std::vector<Core::Wrappers::SetWrapper *>  Core::Utility::PipelineUtil::setsPerPipeline;
uint32_t Core::Utility::PipelineUtil::pipelineLayoutId;
bool Core::Utility::PipelineUtil::tagValidity = true;
uint16_t Core::Utility::PipelineUtil::globalTagMask = 0;


uint32_t Core::Utility::VertexInputWrapper::idCounter = 0;
uint32_t Core::Utility::InputAssemblyWrapper::idCounter = 0;
uint32_t Core::Utility::ShaderStateWrapper::idCounter = 0;
uint32_t Core::Utility::ShaderResourceStateWrapper::idCounter = 0;
uint32_t Core::Utility::TessellationStateWrapper::idCounter = 0;
uint32_t Core::Utility::DepthStencilStateWrapper::idCounter = 0;
uint32_t Core::Utility::RasterizationStateWrapper::idCounter = 0;
uint32_t Core::Utility::ColorBlendStateWrapper::idCounter = 0;
uint32_t Core::Utility::MultiSampleStateWrapper::idCounter = 0;
uint32_t Core::Utility::DynamicStateWrapper::idCounter = 0;
uint32_t Core::Utility::ViewPortStateWrapper::idCounter = 0;

void Core::Utility::PipelineUtil::FillGlobalMeshList(std::vector<uint32_t>& meshList, const Core::Enums::PipelineStates & currentState)
{
    if (meshList.size() != 0)
    {
        if (meshList.size() > 0)
        {
            // compare the mesh and pipelineStateMeshList
            if (pipelineStateMeshList.size() == 0 && currentState == Core::Enums::PipelineStates::VertexInputState)
            {
                pipelineStateMeshList = meshList;
            }
            else if (pipelineStateMeshList.size() > 0)
            {
                std::vector<uint32_t>::iterator itt;
                std::vector<uint32_t> indicesToBeErased;
                //for(itt = pipelineStateMeshList.begin(); itt != pipelineStateMeshList.end(); itt++)
                for(uint32_t i = 0; i < pipelineStateMeshList.size(); i++)
                {
                    std::vector<uint32_t>::iterator it;
                    it = std::find_if(meshList.begin(), meshList.end(), [&](uint32_t meshId) { return meshId == pipelineStateMeshList[i]; });
                    
                    // if the id in pipelineMeshList is not found in meshList
                    // erase the id from pipelineMeshList 
                    if (it == meshList.end())
                    {
                        //itt = pipelineStateMeshList.erase(itt);
                        indicesToBeErased.push_back(pipelineStateMeshList[i]);
                    }
                    
                }

                for (auto index : indicesToBeErased)
                {
                    std::vector<uint32_t>::iterator it;
                    it = std::find_if(pipelineStateMeshList.begin(), pipelineStateMeshList.end(), [&](uint32_t meshId) { return meshId == index; });

                    if(it != pipelineStateMeshList.end())
                    pipelineStateMeshList.erase(it);
                }
            }
        }
    }
}

void const Core::Utility::PipelineUtil::CheckTagValidity(const uint16_t & tag)
{
    // if pipeline tag mask == 0 skip
    if ((tag == 0 ) || Core::Utility::PipelineUtil::tagValidity == false)
        return;

    // if pipeline tag mask != 0 && global mask == 0 then global <- pipelineMask
    if (tag != 0 && Core::Utility::PipelineUtil::globalTagMask == 0)
    {
        Core::Utility::PipelineUtil::globalTagMask = tag;
        return;
    }

    // if  both != 0 then
    if (tag != 0 && Core::Utility::PipelineUtil::globalTagMask != 0)
    {
        // bitwise AND with global mask == 0 then invalid pipeline else global <- result of AND
        
        uint16_t result = tag & Core::Utility::PipelineUtil::globalTagMask;
        
        if ((result) == 0)
        {
            Core::Utility::PipelineUtil::tagValidity = false;
            Core::Utility::PipelineUtil::globalTagMask = 0;
        }
        else
        {
            Core::Utility::PipelineUtil::globalTagMask = result;
        }
    }
}

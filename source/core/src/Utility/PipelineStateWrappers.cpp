#include "Utility/PipelineStateWrappers.h"

std::map<PipelineStates, uint32_t> PipelineUtil::stateToIdMap;
std::vector<uint32_t> PipelineUtil::pipelineStateMeshList;
std::vector<SetWrapper *>  PipelineUtil::setsPerPipeline;
uint32_t PipelineUtil::pipelineLayoutId;
bool PipelineUtil::tagValidity = true;
uint16_t PipelineUtil::globalTagMask = 0;


uint32_t VertexInputWrapper::idCounter = 0;
uint32_t InputAssemblyWrapper::idCounter = 0;
uint32_t ShaderStateWrapper::idCounter = 0;
uint32_t ShaderResourceStateWrapper::idCounter = 0;
uint32_t TessellationStateWrapper::idCounter = 0;
uint32_t DepthStencilStateWrapper::idCounter = 0;
uint32_t RasterizationStateWrapper::idCounter = 0;
uint32_t ColorBlendStateWrapper::idCounter = 0;
uint32_t MultiSampleStateWrapper::idCounter = 0;
uint32_t DynamicStateWrapper::idCounter = 0;
uint32_t ViewPortStateWrapper::idCounter = 0;

void PipelineUtil::FillGlobalMeshList(std::vector<uint32_t>& meshList, const PipelineStates & currentState)
{
    if (meshList.size() != 0)
    {
        if (meshList.size() > 0)
        {
            // compare the mesh and pipelineStateMeshList
            if (pipelineStateMeshList.size() == 0 && currentState == PipelineStates::VertexInputState)
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

void const PipelineUtil::CheckTagValidity(const uint16_t & tag)
{
    // if pipeline tag mask == 0 skip
    if ((tag == 0 ) || PipelineUtil::tagValidity == false)
        return;

    // if pipeline tag mask != 0 && global mask == 0 then global <- pipelineMask
    if (tag != 0 && PipelineUtil::globalTagMask == 0)
    {
        PipelineUtil::globalTagMask = tag;
        return;
    }

    // if  both != 0 then
    if (tag != 0 && PipelineUtil::globalTagMask != 0)
    {
        // bitwise AND with global mask == 0 then invalid pipeline else global <- result of AND
        
        uint16_t result = tag & PipelineUtil::globalTagMask;
        
        if ((result) == 0)
        {
            PipelineUtil::tagValidity = false;
            PipelineUtil::globalTagMask = 0;
        }
        else
        {
            PipelineUtil::globalTagMask = result;
        }
    }
}

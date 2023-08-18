#include "Utility/ResourceAllocationHelper.h"

std::vector<size_t> Core::Utility::CalculateOffsetsForDescInUniform(const size_t & dataSizePerDescriptor, const GlobalResourceAllocationConfig & allocConfig,
    const GlobalResourceSharingConfig & resourceSharingConfig)
{
    std::vector<size_t> offsets;
    offsets.resize(allocConfig.numDescriptorSets);

    uint32_t index = resourceSharingConfig.allocatedUniformCount % resourceSharingConfig.maxUniformPerResource;

    for (uint32_t i = 0; i < allocConfig.numDescriptorSets; i++)
    {
        offsets[i] = index * allocConfig.numDescriptorSets * dataSizePerDescriptor + dataSizePerDescriptor * i;
    }
    return offsets;
}


bool Core::Utility::IsNewAllocationRequired(const GlobalResourceSharingConfig & resourceSharingConfig)
{
    if (resourceSharingConfig.allocatedUniformCount % resourceSharingConfig.maxUniformPerResource == 0)
        return true;
    else
        return false;
}

size_t Core::Utility::GetDataSizeMeantForSharing(const size_t & sizeSingleDescriptor, const Core::Utility::GlobalResourceAllocationConfig & allocConfig,
    const Core::Utility::GlobalResourceSharingConfig & resourceSharingConfig)
{
    return sizeSingleDescriptor * allocConfig.numDescriptorSets * resourceSharingConfig.maxUniformPerResource;
}
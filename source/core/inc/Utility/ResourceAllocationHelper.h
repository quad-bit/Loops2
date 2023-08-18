#pragma once
#include "ShaderResourceDescription.h"


namespace Core
{
    namespace Utility
    {
        std::vector<size_t> CalculateOffsetsForDescInUniform(const size_t& dataSizePerDescriptor, const GlobalResourceAllocationConfig& allocConfig,
            const GlobalResourceSharingConfig& resourceSharingConfig);

        bool IsNewAllocationRequired(const GlobalResourceSharingConfig& resourceSharingConfig);

        size_t GetDataSizeMeantForSharing(const size_t& sizeSingleDescriptor, const GlobalResourceAllocationConfig& allocConfig,
            const GlobalResourceSharingConfig& resourceSharingConfig);
    }
}
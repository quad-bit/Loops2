#pragma once

#include <glm\glm.hpp>
#include "Utility/RenderingWrappers/RenderingWrapper.h"
#include "ECS/ECS_Helper.h"
#include <string>

namespace Core
{
    namespace Utility
    {
        enum class AllocationMethod
        {
            IMMEDIATE,
            LAZY
        };

        //overall config for the entire uniform list for a binding
        struct GlobalResourceSharingConfig
        {
            uint32_t maxUniformPerResource; // per buffer or memory
            uint32_t allocatedUniformCount;
            //std::vector<uint32_t> uniformIdList;
        };

        struct GlobalResourceAllocationConfig
        {
            uint32_t numDescriptorSets; // should be same for all the bindings in a set
            uint32_t numResources;  // buffers/textures/samplers
            uint32_t numMemories;
        };

        struct BufferLayoutInfo
        {
            // memory sizes
            size_t totalSize;
            size_t dataSizePerDescriptorAligned;
            size_t dataSizePerDescriptor; // sizeOf(Uniform Struct)
            std::vector<size_t> offsetsForEachDescriptor; // offset in memory, for buffer/image sharing
            GlobalResourceSharingConfig sharingConfig;
            GlobalResourceAllocationConfig allocationConfig;
        };

        struct ImageBindingInfo
        {
            std::vector<uint32_t> imageId;
            std::vector<uint32_t> imageViewId;
            std::vector<uint32_t> memoryId;
        };

        struct SamplerBindingInfo
        {
            std::vector<uint32_t> samplerId;
        };

        // many bufs, many mem || 1 buf, many memory || 1 buf, 1 mem
        struct BufferBindingInfo
        {
            std::vector<uint32_t> bufferIdList;
            std::vector<uint32_t> bufferMemoryId;
            BufferLayoutInfo info;
        };

        //represent one binding, might have unique descriptor for each frame in flight
        struct ShaderBindingDescription
        {
            uint32_t uniformId; // not getting used
            uint32_t resParentId; // not getting used
            Core::ECS::COMPONENT_TYPE parentType; // not getting used

            //size_t dataSizePerDescriptorAligned;
            //size_t dataSizePerDescriptor; // sizeOf(Uniform Struct)
            //std::vector<size_t> offsetsForEachDescriptor; // offset in memory, for buffer/image sharing
            //GlobalResourceSharingConfig sharingConfig;
            //GlobalResourceAllocationConfig allocationConfig;

            Core::Enums::DescriptorType resourceType;
            uint32_t set;
            uint32_t binding;
            std::string resourceName;
            uint32_t numElements;
            std::vector<uint32_t> descriptorSetIds;

            SamplerBindingInfo samplerBindingInfo;
            ImageBindingInfo imageBindingInfo;
            BufferBindingInfo bufferBindingInfo;
        };
    }
}



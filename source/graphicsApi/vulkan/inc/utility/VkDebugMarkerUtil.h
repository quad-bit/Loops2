#ifndef GFXVULKAN_VKDEBUGMARKERUTIL_H_
#define GFXVULKAN_VKDEBUGMARKERUTIL_H_

#include <vulkan/vulkan.h>
#include <vector>

namespace GfxVk
{
    namespace DebugMarker
    {
        void SetObjectName(VkObjectType objectType, uint64_t objectHandle, const char* objectName);
        void SetCommandBufferBeginLabel(VkCommandBuffer commandBuffer, const char* labelName, std::vector<float> color);
        void InsertLabelInCommandBuffer(VkCommandBuffer commandBuffer, const char* labelName, std::vector<float> color);
        void SetCommandBufferEndLabel(VkCommandBuffer commandBuffer);
    }
}


#endif // GFXVULKAN_VKDEBUGMARKERUTIL_H_

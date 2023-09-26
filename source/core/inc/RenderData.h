#ifndef CORE_RENDERDATA_H_
#define CORE_RENDERDATA_H_

#include <string>
#include <vector>
#include <map>
#include "Math/MathUtil.h"

namespace Core
{
    namespace Utility
    {
        /// <summary>
        /// Get :
        /// TransformUniform DescriptorSet
        /// Get the distance from camera, for LOD
        /// Get the effect name from the associated Material 
        /// 
        /// CameraUniform DescriptorSet
        /// The main camera might change in some frame or cut scene
        /// Effect name for each camera
        /// 
        /// LightUniform DescriptorSet
        /// 
        /// 
        /// </summary>

        struct CameraData
        {
            std::vector<std::string> m_tag;// Indicating the effect
            glm::vec3 m_cameraPosition;
            uint32_t m_descriptorSetId;
        };

        // Will get collected under the effect type/name set in the material
        struct TransformData
        {
            uint32_t m_descriptorSetId;
            glm::vec3 m_position; // world position, will help with LOD or sorting
        };

        class RenderData
        {
        public:
            std::vector<uint32_t> m_cameraDescriptorSets;
            std::vector<uint32_t> m_transformDescriptorSets;
        };
    }
}

#endif //CORE_RENDERDATA_H_
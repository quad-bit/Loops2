#ifndef CORE_RENDERDATA_H_
#define CORE_RENDERDATA_H_

#include <string>
#include <vector>
#include <map>
#include "Math/MathUtil.h"
#include <Utility/RenderingWrappers/RenderingWrapper.h>
#include <optional>

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
            std::string m_tag;// Indicating the effect. Optional.
            std::vector<Core::Enums::RenderLayers> m_renderLayers;
            glm::vec3 m_cameraPosition;
            uint32_t m_descriptorSetId;
        };

        // Will get collected under the effect type/name set in the material
        struct TransformData
        {
            uint32_t m_descriptorSetId;
            glm::vec3 m_position; // world position, will help with LOD or sorting
            std::vector<Core::Enums::RenderLayers> m_renderLayers;
            std::vector<uint32_t> m_vertexBufferId;
            std::optional<uint32_t> m_indexBufferId;
            // To put it in maps
            //friend bool operator<(const TransformData& l, const TransformData& r) { return l.id < r.id; }
        };

        struct LightData
        {
            uint32_t m_descriptorSetId;
        };

        class RenderData
        {
        public:
            std::vector<CameraData> m_cameraData;
            std::vector<TransformData> m_transformData;
            std::map<std::string, std::vector<TransformData>> m_perEffectTransformData;
            std::vector<LightData> m_lightData;
        };
    }
}

#endif //CORE_RENDERDATA_H_
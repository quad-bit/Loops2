#ifndef CORE_RENDERDATA_H_
#define CORE_RENDERDATA_H_

#include <string>
#include <vector>
#include <map>
#include "Math/MathUtil.h"
#include <Utility/RenderingWrappers/RenderingWrapper.h>
#include <ECS/Components/Material.h>
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
            glm::mat4 m_viewMat, m_projectionMat;
            uint32_t m_descriptorSetId;
            //point to the next resourceSet indicies 
            std::vector<uint32_t> childSetIndicies;
        };

        /// <summary>
        /// Will capture the texture info per object
        /// </summary>
        struct MaterialData
        {
            uint32_t m_materialComponentId;
            // index into transform list for the associated object to be drawn by this material
            std::vector<uint32_t> m_childSetIndicies;
            std::optional<uint32_t> m_descriptorSetId;
            std::vector<uint32_t> m_transformDescriptorList;
            Core::Utility::EffectId m_effectId;
            Core::Utility::TechniqueId m_techniqueId;
        };

        struct PointLightData
        {
            glm::vec3 m_position; // world position, will help with LOD or sorting
            float radius;
        };

        // represent the complete light set (point binding = 0, spot = binding = 1, etc)
        struct LightData
        {
            uint32_t m_descriptorSetId;
            std::vector<PointLightData> m_pointLights;
            std::vector<uint32_t> m_childSetIndicies;
        };

        struct TransformData
        {
            uint32_t m_descriptorSetId;
            glm::vec3 m_position; // world position, will help with LOD or sorting
            glm::mat4 m_modelMat;
            std::vector<Core::Enums::RenderLayers> m_renderLayers;

            uint32_t m_positionBufferId;
            std::optional<uint32_t> m_colorBufferId;
            std::optional<uint32_t> m_normalBufferId;
            std::optional<uint32_t> m_uv0BufferId;
            std::optional<uint32_t> m_tangentBufferId;

            uint32_t m_vertexCount;
            std::optional<uint32_t> m_indexBufferId, m_indexCount;

            // To put it in maps
            //friend bool operator<(const TransformData& l, const TransformData& r) { return l.id < r.id; }
        };

        class RenderData
        {
        public:
            std::vector<CameraData> m_cameraData;
            std::vector<TransformData> m_transformData;
            //std::map<std::string, std::vector<TransformData>> m_perEffectTransformData;
            LightData m_lightData;
            // material component id to materialdata
            std::vector<MaterialData> m_materialData;
            MaterialData m_skyboxData;
        };
    }
}

#endif //CORE_RENDERDATA_H_
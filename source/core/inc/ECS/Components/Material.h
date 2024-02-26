#pragma once

#include "ECS/Component.h"
#include <glm/glm.hpp>
#include <vector>
#include <optional>
#include <string>
#include <Platform/Assertion.h>
#include <Utility/ShaderResourceDescription.h>

namespace Core
{
    namespace Wrappers
    {
        struct SetWrapper;
    }

    namespace ECS
    {
        namespace Components
        {
            /*enum class EffectType
            {
                OPAQUE_E,
                TRANPARENT,
                CUSTOM
            };*/


            class Material;

            class MaterialCreateInfo
            {
            private:
                glm::vec4 m_color;
                Core::Utility::EffectInfo m_effectInfo;
                std::optional<uint32_t> m_baseColorTextureId;
                std::optional<uint32_t> m_baseColorSamplerId;
                std::optional<uint32_t> m_normalTextureId;
                std::string m_materialName;

            public:
                MaterialCreateInfo(
                    const glm::vec4& color,
                    const Core::Utility::EffectInfo& effectInfo,
                    const std::optional<uint32_t>& baseColorTextureId,
                    const std::optional<uint32_t>& baseColorSamplerId,
                    const std::optional<uint32_t>& normalTextureId,
                    const std::string& materialName):
                    m_color(color),
                    m_effectInfo(effectInfo),
                    m_baseColorTextureId(baseColorTextureId),
                    m_baseColorSamplerId(baseColorSamplerId),
                    m_normalTextureId(normalTextureId),
                    m_materialName(materialName)
                {
                };

                friend class Material;
            };

            class Material : public Component<Material>
            {
            public:
                glm::vec4 m_color;
                bool m_enableInstancing = false;
                glm::vec2 m_mainTextureScale;
                glm::vec2 m_mainTextureOffset;

                std::optional<std::string> m_effectName;
                Core::Utility::EffectInfo m_effectInfo;

                std::optional<uint32_t> m_baseColorTextureId;
                std::optional<uint32_t> m_baseColorSamplerId;

                std::optional<uint32_t> m_normalTextureId;
                std::string m_materialName;

                std::vector<uint32_t> m_descriptorSetIds;

                Material(const MaterialCreateInfo& info):
                    m_color(info.m_color),
                    m_effectInfo(info.m_effectInfo),
                    m_baseColorTextureId(info.m_baseColorTextureId),
                    m_baseColorSamplerId(info.m_baseColorSamplerId),
                    m_normalTextureId(info.m_normalTextureId),
                    m_materialName(info.m_materialName)
                {
                    componentType = COMPONENT_TYPE::MATERIAL;
                }
            };
        }
    }
}

// unity ref
/*

color	The main color of the Material.
doubleSidedGI	Gets and sets whether the Double Sided Global Illumination setting is enabled for this material.
enableInstancing	Gets and sets whether GPU instancing is enabled for this material.
globalIlluminationFlags	Defines how the material should interact with lightmaps and lightprobes.
mainTexture	The main texture.
mainTextureOffset	The offset of the main texture.
mainTextureScale	The scale of the main texture.
passCount	How many passes are in this material (Read Only).
renderQueue	Render queue of this material.
shader	The shader used by the material.
shaderKeywords	Additional shader keywords set by this material.

*/
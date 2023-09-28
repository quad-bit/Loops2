#pragma once

#include "ECS/Component.h"
#include <glm/glm.hpp>
#include <vector>
#include <optional>
#include <string>

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
            enum class EffectType
            {
                OPAQUE_E,
                TRANPARENT,
                CUSTOM
            };

            class Material : public Component<Material>
            {
            public:
                glm::vec4 m_color;
                bool m_enableInstancing = false;
                //std::vector<Texture> m_textures; // main color map
                glm::vec2 m_mainTextureScale;
                glm::vec2 m_mainTextureOffset;

                std::optional<std::string> m_effectName;
                EffectType m_effectType;

            
                Material(const std::string& effectName) :
                    m_effectName(effectName)
                {
                    m_effectType = EffectType::CUSTOM;
                    componentType = COMPONENT_TYPE::MATERIAL;
                }

                Material(const EffectType& effectType) :
                    m_effectType(effectType)
                {
                    componentType = COMPONENT_TYPE::MATERIAL;
                }
            };

            //class Material : public Component<Material>
            //{
            //private:
            //    glm::vec4 color;
            //    bool enableInstancing = false;
            //    Texture* mainTexture; // main color map
            //    glm::vec2 mainTextureScale;
            //    glm::vec2 mainTextureOffset;
            //    Texture* textures; // normal, heightmap, gloss map.. etc   
            //    uint32_t numTextures;
            //    Shader* shaders;
            //    uint32_t numShaders;
            //    std::vector<SetWrapper*> resourceLayoutList;
            //    std::vector<uint32_t> meshList;

            //    std::string m_effectName;
            //    MaterialType m_effectType;

            //public:
            //    Material(Shader* shaders, const uint32_t& numShaders)
            //    {
            //        this->shaders = shaders;
            //        this->numShaders = numShaders;
            //        componentType = COMPONENT_TYPE::MATERIAL;
            //    }

            //    Material(const MaterialType& effectType, const std::string& effectName) :
            //        m_effectType(effectType),
            //        m_effectName(effectName)
            //    {
            //        componentType = COMPONENT_TYPE::MATERIAL;
            //    }
            //};
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
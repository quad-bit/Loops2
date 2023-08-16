#pragma once

#include "ECS/Component.h"
#include <glm/glm.hpp>
#include <vector>

struct SetWrapper;
class Texture;
class Shader;

namespace Core
{
    namespace ECS
    {
        namespace Components
        {

            class Material : public Component<Material>
            {
            public:
                glm::vec4 color;
                bool enableInstancing = false;
                Texture* mainTexture; // main color map
                glm::vec2 mainTextureScale;
                glm::vec2 mainTextureOffset;
                Texture* textures; // normal, heightmap, gloss map.. etc   
                uint32_t numTextures;
                Shader* shaders;
                uint32_t numShaders;
                std::vector<SetWrapper*> resourceLayoutList;
                std::vector<uint32_t> meshList;

                Material(Shader* shaders, const uint32_t& numShaders)
                {
                    this->shaders = shaders;
                    this->numShaders = numShaders;
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
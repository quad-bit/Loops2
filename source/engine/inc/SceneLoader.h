#ifndef ENGINE_SCENELOADER_H_
#define ENGINE_SCENELOADER_H_

#include "tiny_gltf.h"

namespace Core
{
    namespace ECS
    {
        class World;
        class EntityHandle;

        namespace Components
        {
            class Material;
        }
    }
}

namespace Engine
{
    namespace Utility
    {
        using gltfMaterialIndex = int;
        class GltfLoader
        {
        private:
            GltfLoader() {};
            GltfLoader(GltfLoader const&) {}
            GltfLoader const& operator= (GltfLoader const&) {}

            Core::ECS::World* m_ecsWorldObj;
            std::vector<Core::ECS::EntityHandle*> m_entityList;
            void LoadNode(const tinygltf::Node& inputNode, const tinygltf::Model& input, Core::ECS::EntityHandle* parent);

            std::map< gltfMaterialIndex, Core::ECS::Components::Material*> m_materialMap;
            void LoadMaterials(const tinygltf::Model& input);
            Core::ECS::Components::Material* GetMaterial(const tinygltf::Model& input, uint32_t materialIndex);

            std::vector<uint32_t> m_imageList;
            uint32_t CreateTexture(const tinygltf::Model& input, int textureIndex, Core::Enums::Format imageFormat);

            // samplers get destroyed automatically
            std::vector<uint32_t> m_samplerList;
            void LoadSamplers(const tinygltf::Model& input);
            uint32_t GetSampler(uint32_t index);

        public:
            GltfLoader(Core::ECS::World* worldObj);

            void LoadGltf(const std::string& assetName, std::vector<Core::ECS::EntityHandle*>& m_entityList);
            ~GltfLoader();
        };
    }
}

#endif // ENGINE_SCENELOADER_H_
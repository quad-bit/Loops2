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
        class GltfLoader;
        class SceneLoadInfo
        {
        private:
            std::string m_filePath;
            float m_scaleFactor;
            bool m_addMeshBounds;
        public:
            SceneLoadInfo() = delete;
            SceneLoadInfo(const std::string& path,
                float scaleFactor, bool addMeshBounds):
                m_filePath(path), m_scaleFactor(scaleFactor), m_addMeshBounds(addMeshBounds)
            {}
            friend GltfLoader;
        };

        using gltfMaterialIndex = int;
        class GltfLoader
        {
        private:
            GltfLoader() = delete;
            GltfLoader(GltfLoader const&) = delete;

            Core::ECS::World* m_ecsWorldObj;
            std::vector<Core::ECS::EntityHandle*> m_entityList;
            void LoadNode(const tinygltf::Node& inputNode, const tinygltf::Model& input, Core::ECS::EntityHandle* parent);

            std::map< gltfMaterialIndex, Core::ECS::Components::Material*> m_materialMap;
            void LoadMaterials(const tinygltf::Model& input);
            Core::ECS::Components::Material* GetMaterial(const tinygltf::Model& input, uint32_t materialIndex);

            //gltf index to internal image index
            std::map<uint32_t, uint32_t> m_imageList;
            void LoadTextures(const tinygltf::Model& input);
            uint32_t GetTexture(int textureIndex);

            // samplers get destroyed automatically
            // gltf sampler index to internal samplerId
            std::map<uint32_t, uint32_t> m_samplerList;
            void LoadSamplers(const tinygltf::Model& input);
            uint32_t GetSampler(uint32_t index);

            const SceneLoadInfo& m_sceneLoadInfo;

        public:
            GltfLoader(Core::ECS::World* worldObj,
                std::vector<Core::ECS::EntityHandle*>& entityList,
                const SceneLoadInfo& sceneLoadInfo);

            void LoadGltf(const std::string& assetPath, std::vector<Core::ECS::EntityHandle*>& m_entityList);
            ~GltfLoader();
        };
    }
}

#endif // ENGINE_SCENELOADER_H_
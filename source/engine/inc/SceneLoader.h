#ifndef ENGINE_SCENELOADER_H_
#define ENGINE_SCENELOADER_H_

#include "tiny_gltf.h"

namespace Core
{
    namespace ECS
    {
        class World;
        class EntityHandle;
    }
}

namespace Engine
{
    namespace Utility
    {
        class GltfLoader
        {
        private:
            GltfLoader() {};
            GltfLoader(GltfLoader const&) {}
            GltfLoader const& operator= (GltfLoader const&) {}

            Core::ECS::World* m_ecsWorldObj;
            std::vector<Core::ECS::EntityHandle*> m_entityList;

            void LoadNode(const tinygltf::Node& inputNode, const tinygltf::Model& input, Core::ECS::EntityHandle* parent);

        public:
            GltfLoader(Core::ECS::World* worldObj);

            void LoadGltf(const std::string& assetName, std::vector<Core::ECS::EntityHandle*>& m_entityList);
            ~GltfLoader();
        };
    }
}

#endif // ENGINE_SCENELOADER_H_
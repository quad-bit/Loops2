#ifndef CORE_SCENELOADER_H_
#define CORE_SCENELOADER_H_



namespace Core
{
    namespace ECS
    {
        class World;
        class EntityHandle;
    }

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

        public:
            GltfLoader(Core::ECS::World* worldObj);

            void LoadGltf(const std::string& gltfPath, std::vector<Core::ECS::EntityHandle*>& m_entityList);
            ~GltfLoader();
        };
    }
}

#endif // CORE_SCENELOADER_H_
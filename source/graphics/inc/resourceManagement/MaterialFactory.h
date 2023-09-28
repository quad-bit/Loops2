#pragma once
#include "ECS/Components/Material.h"
#include <map>
#include "Utility/RenderingWrappers/RenderingWrapper.h"

struct ShaderDescription;
//
//struct MatSetWrapper
//{
//    Material * mat;
//    std::vector<SetWrapper * > wrapperList;
//};

namespace Renderer
{
    namespace ResourceManagement
    {
        class MaterialFactory
        {
        private:
            MaterialFactory() {}
            MaterialFactory(MaterialFactory const&) {}
            MaterialFactory const& operator= (MaterialFactory const&) {}

            static MaterialFactory* instance;

            uint32_t matIdCounter = 0;
            uint32_t GetMatId();
            void DecrementMatCounter();

            std::map<uint32_t, Core::ECS::Components::Material*> idToMaterialMap;
            //std::vector<MatSetWrapper> matSetWrapperList;
            //void CreateSetResources(std::vector<SetWrapper*> setWrapperList);

        public:
            void Init();
            void DeInit();
            void Update();
            static MaterialFactory* GetInstance();
            ~MaterialFactory();

            //deprecated
            /*Material* CreateMaterial(ShaderDescription* shaders, const uint32_t& shaderCount,
                const uint32_t& meshId, const RenderPassTag& tag);

            Material* CreateMaterial(ShaderDescription* shaderDescriptions, const uint32_t& shaderCount,
                const uint32_t& meshId, const uint16_t& tagMask = (uint16_t)RenderPassTag::ColorPass);

            void AddMeshIds(Material* mat, const uint32_t& meshId);

            std::vector<uint32_t> GetMeshList(SetWrapper* setwrapper, const uint32_t& setWrapperCount);*/

            Core::ECS::Components::Material* CreateMaterial(const std::string& effectName);
            Core::ECS::Components::Material* CreateMaterial(const Core::ECS::Components::EffectType & effectType);
        };
    }
}
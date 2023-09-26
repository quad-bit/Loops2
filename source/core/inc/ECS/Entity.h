#pragma once
#include <iostream>
#include <string>
#include <Utility/RenderingWrappers/RenderingWrapper.h>
//using namespace std;
namespace Core
{
    namespace ECS
    {
        namespace Components
        {
            class Transform;
        }


        class Entity
        {
        private:
            std::vector<Core::Enums::RenderLayers> m_entityLayers{Core::Enums::RenderLayers::OPAQUE_LR};

        public:
            uint32_t id;

            Core::ECS::Components::Transform* transform;
            std::string entityName;
            std::string entityTag;
            bool isStatic;

            // To put it in maps
            friend bool operator<(const Entity& l, const Entity& r) { return l.id < r.id; }

            ~Entity()
            {
            }

            const std::vector<Core::Enums::RenderLayers>& GetEntityLayers()
            {
                return m_entityLayers;
            }

            void AddEntityLayer(const Core::Enums::RenderLayers& layer)
            {
                m_entityLayers.push_back(layer);
            }

            void CLearAndAddLayer(const Core::Enums::RenderLayers& layer)
            {
                m_entityLayers.clear();
                AddEntityLayer(layer);
            }
        };
    }
}
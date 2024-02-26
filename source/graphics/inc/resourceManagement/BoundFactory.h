#pragma once
#include "Utility/Container/BitArray.h"
#include <map>
#include "PlatformSettings.h"
#include <bitset>
#include <Utility/RenderingWrappers/AttributeHelper.h>
#include "Math/Quad.h"
#include "Math/Cube.h"

namespace Core
{
    namespace ECS
    {
        namespace Components
        {
            enum class BOUND_TYPE;
            class Bound;
        }

    }
}

namespace Renderer
{
    namespace ResourceManagement
    {
        class BoundFactory
        {
        private:
            BoundFactory() {}
            BoundFactory(BoundFactory const&) {}
            BoundFactory const& operator= (BoundFactory const&) {}

            static BoundFactory* instance;
            std::vector< Core::ECS::Components::Bound*> m_boundList;

            uint32_t idCounter = 0;
            uint32_t GenerateBoundId() { return idCounter++; }

            template<typename T>
            void FillMeshData(Core::ECS::Components::Bound* bound, bool addIndices);

        public:
            void Init();
            void DeInit();
            void Update();
            static BoundFactory* GetInstance();
            ~BoundFactory();

            void AddBound(Core::ECS::Components::Bound* bound);
            void DestroyBound(Core::ECS::Components::Bound* bound);
        };
    }
}

#include <ECS/Components/Mesh.h>
#include "Utility/RenderingWrappers/AttributeHelper.h"

template<typename T>
inline void Renderer::ResourceManagement::BoundFactory::FillMeshData(Core::ECS::Components::Bound* bound, bool addIndices)
{
    T obj{};

    size_t numVertices = obj.indices.size();
    bound->m_positions.resize(numVertices);

    for (int i = 0; i < numVertices; i++)
    {
        bound->m_positions[i] = obj.positions[i];
    }

    if(addIndices)
        bound->m_indicies = obj.indices;
}

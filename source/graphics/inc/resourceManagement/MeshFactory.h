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
            enum class MESH_TYPE;
            class Mesh;
        }

    }

    namespace Wrappers
    {
        struct MeshInfo;
        struct AttribStructBase;
    }
}

namespace Renderer
{
    namespace ResourceManagement
    {
        class MeshFactory
        {
        private:
            MeshFactory() {}
            MeshFactory(MeshFactory const&) {}
            MeshFactory const& operator= (MeshFactory const&) {}

            static MeshFactory* instance;
            //std::map<Core::ECS::Components::Mesh*, Core::Utility::AttribStructBase*> meshToVertWrapperMap;

            uint32_t idCounter = 0;
            uint32_t GenerateMeshId() { return idCounter++; }

            int* CreateVertexBuffer(uint32_t count);
            int CreateIndexBuffer();

            //std::bitset<(uint32_t)Core::Utility::ATTRIBUTES::NUM_ATTRIBUTES> pcMask;
            //std::bitset<(uint32_t)Core::Utility::ATTRIBUTES::NUM_ATTRIBUTES> pcnMask;

            //template<typename AttribType>
            //void FillMeshAttrib(Core::Utility::AttribStructBase* wrapper, Core::ECS::Components::MESH_TYPE* meshType, Core::ECS::Components::Mesh* mesh, const Core::Wrappers::MeshInfo* meshInfo);

            template<typename T>
            void FillMeshData(Core::ECS::Components::Mesh* mesh);

        public:
            void Init();
            void DeInit();
            void Update();
            static MeshFactory* GetInstance();
            ~MeshFactory();

            //old ====
            //Core::ECS::Components::Mesh* CreateMesh(const Core::Containers::BitArray& attribMaskReq, Core::ECS::Components::MESH_TYPE* meshType, bool isIndexed, bool individualBuffersRequired);
            //Core::ECS::Components::Mesh* CreateMesh(const Core::Wrappers::MeshInfo* meshInfo, Core::ECS::Components::MESH_TYPE* meshType);
            //old ====

            Core::ECS::Components::Mesh* CreateBasicPrimitiveMesh(const Core::ECS::Components::MESH_TYPE& meshType);
            void DestroyMesh(const uint32_t& meshId);
            void DestroyMesh(Core::ECS::Components::Mesh* mesh);
        };
    }
}

#include <ECS/Components/Mesh.h>
#include "Utility/RenderingWrappers/AttributeHelper.h"
//
//template<typename AttribType>
//inline void Renderer::ResourceManagement::MeshFactory::FillMeshAttrib(Core::Utility::AttribStructBase* wrapper, Core::ECS::Components::MESH_TYPE* meshType, Core::ECS::Components::Mesh* mesh, const Core::Wrappers::MeshInfo* meshInfo)
//{
//    switch (*meshType)
//    {
//    case Core::ECS::Components::MESH_TYPE::CUBE:
//
//        if (meshInfo->overrideColor)
//            ((AttribType*)wrapper)->FillData<Core::Math::CubeIndexed>(mesh, meshInfo->color);
//        else
//            ((AttribType*)wrapper)->FillData<Core::Math::CubeIndexed>(mesh);
//        break;
//
//    case Core::ECS::Components::MESH_TYPE::QUAD:
//
//        if (meshInfo->overrideColor)
//            ((AttribType*)wrapper)->FillData<Core::Math::QuadIndexed>(mesh, meshInfo->color);
//        else
//            ((AttribType*)wrapper)->FillData<Core::Math::QuadIndexed>(mesh);
//        break;
//    }
//}

template<typename T>
inline void Renderer::ResourceManagement::MeshFactory::FillMeshData(Core::ECS::Components::Mesh* mesh)
{
    T obj{};

    mesh->m_positions = obj.positions;
    mesh->m_colors = obj.colors;
    mesh->m_indicies = obj.indices;
    mesh->m_normals = obj.normals;
}

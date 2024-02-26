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
            std::vector< Core::ECS::Components::Mesh*> m_meshList;

            uint32_t idCounter = 0;
            uint32_t GenerateMeshId() { return idCounter++; }

            int* CreateVertexBuffer(uint32_t count);
            int CreateIndexBuffer();

            template<typename T>
            void FillMeshData(Core::ECS::Components::Mesh* mesh, bool addIndices);

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

            Core::ECS::Components::Mesh* CreateBasicPrimitiveMesh(const Core::ECS::Components::MESH_TYPE& meshType, bool isIndexed);
            void AddMesh(Core::ECS::Components::Mesh* mesh);
            //void DestroyMesh(const uint32_t& meshId);
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
inline void Renderer::ResourceManagement::MeshFactory::FillMeshData(Core::ECS::Components::Mesh* mesh, bool addIndices)
{
    T obj{};

    size_t numVertices = obj.indices.size();
    mesh->m_positions.resize(numVertices);
    mesh->m_colors.resize(numVertices);
    mesh->m_normals.resize(numVertices);

    for (int i = 0; i < numVertices; i++)
    {
        mesh->m_positions[i] = obj.positions[obj.indices[i]];
        mesh->m_colors[i] = glm::vec4(0.6, 0.2, 0.5, 1.0);
    }

    //mesh->m_positions = obj.positions;
    //mesh->m_colors = obj.colors;
    //mesh->m_normals = obj.normals;

    for (int i = 0; i < numVertices; i++)
    {
        mesh->m_normals[i] = obj.normals[obj.indices[i / 6]];
    }

    if(addIndices)
        mesh->m_indicies = obj.indices;
}

#pragma once

#include "ECS/Component.h"
#include "Math/BoundingBox.h"
#include <stdint.h>
#include <glm/glm.hpp>
#include <utility/RenderingWrappers/RenderingWrapper.h>

namespace Core
{
    namespace ECS
    {
        namespace Components
        {
            enum class MESH_TYPE
            {
                TRIANGLE,
                QUAD,
                CUBE,
                SPHERE,
                CYLINDER,
                CUSTOM
            };

            struct VertexBufferInfoWrapper
            {
                uint32_t m_bufferId, m_bufferMemoryId;
            };

            class Mesh : public Component<Mesh>
            {
            public:

                Core::Math::BoundingBox bounds;
                uint32_t submeshCount;

                std::vector<glm::vec2> m_uv0;
                std::vector<glm::vec2> m_uv1;
                std::vector<glm::vec2> m_uv2;
                std::vector<glm::vec2> m_uv3;
                std::vector<glm::vec2> m_uv4;
                std::vector<glm::vec2> m_uv5;
                std::vector<glm::vec2> m_uv6;
                std::vector<glm::vec2> m_uv7;
                std::vector<glm::vec2> m_uv8;

                std::vector<glm::vec3> m_positions;
                std::vector<glm::vec4> m_colors;
                std::vector<glm::vec3> m_normals;
                std::vector<glm::vec4> m_tangents;
                std::vector<uint32_t> m_indicies;

                uint32_t m_positionBufferId, m_positionBufferMemoryId;
                uint32_t m_colorBufferId, m_colorBufferMemoryId;
                uint32_t m_normalBufferId, m_normalBufferMemoryId;
                uint32_t m_tangentBufferId, m_tangentBufferMemoryId;
                uint32_t m_indexBufferId, m_indexBufferMemoryId;
                uint32_t m_uv0BufferId, m_uv0BufferMemoryId;

                glm::vec3 m_minPos, m_maxPos;

                Mesh()
                {
                    componentType = COMPONENT_TYPE::MESH;
                }

                ~Mesh()
                {
                }
            };
        }
    }
}

//Unity ref
/*

bindposes	The bind poses.The bind pose at each index refers to the bone with the same index.
blendShapeCount	Returns BlendShape count on this mesh.
boneWeights	The bone weights for each vertex in the Mesh, up to a maximum of 4.
bounds	The bounding volume of the Mesh.
colors	Vertex colors of the Mesh.
colors32	Vertex colors of the Mesh.
indexFormat	Format of the mesh index buffer data.
isReadable	Returns true if the Mesh is read / write enabled, or false if it is not.
normals	The normals of the Mesh.
subMeshCount	The number of sub - meshes inside the Mesh object.
tangents	The tangents of the Mesh.
triangles	An array containing all triangles in the Mesh.
uv	The base texture coordinates of the Mesh.
uv2	The second texture coordinate set of the mesh, if present.
uv3	The third texture coordinate set of the mesh, if present.
uv4	The fourth texture coordinate set of the mesh, if present.
uv5	The fifth texture coordinate set of the mesh, if present.
uv6	The sixth texture coordinate set of the mesh, if present.
uv7	The seventh texture coordinate set of the mesh, if present.
uv8	The eighth texture coordinate set of the mesh, if present.
vertexAttributeCount	Returns the number of vertex attributes that the mesh has. (Read Only)
vertexBufferCount	Gets the number of vertex buffers present in the Mesh. (Read Only)
vertexCount	Returns the number of vertices in the Mesh(Read Only).
vertices	Returns a copy of the vertex positions or assigns a new vertex positions array.

AddBlendShapeFrame	Adds a new blend shape frame.
Clear	Clears all vertex data and all triangle indices.
ClearBlendShapes	Clears all blend shapes from Mesh.
CombineMeshes	Combines several Meshes into this Mesh.
GetAllBoneWeights	Gets the bone weights for the Mesh.
GetBaseVertex	Gets the base vertex index of the given sub - mesh.
GetBindposes	Gets the bind poses of the Mesh.
GetBlendShapeFrameCount	Returns the frame count for a blend shape.
GetBlendShapeFrameVertices	Retreives deltaVertices, deltaNormals and deltaTangents of a blend shape frame.
GetBlendShapeFrameWeight	Returns the weight of a blend shape frame.
GetBlendShapeIndex	Returns index of BlendShape by given name.
GetBlendShapeName	Returns name of BlendShape by given index.

*/
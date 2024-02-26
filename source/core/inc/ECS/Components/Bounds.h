#pragma once
#include "ECS/Component.h"
#include "Platform/Assertion.h"
#include <Math/MathUtil.h>
#include <memory>
#include <optional>

namespace Core
{
    namespace ECS
    {
        namespace Components
        {
            struct BoundUniform
            {
                glm::mat4 m_modelMat;
                uint32_t m_parentType; // ECS::COMPONENT_TYPE
            };

            enum class BoundType
            {
                AABB,
                OBB,
                SPHERE
            };

            struct BoundCategory
            {
                BoundType m_type;
                const glm::mat4& m_parentTransform;
                const std::string& m_parentName;
                glm::mat4 m_transform;

                glm::vec3 m_minPos;
                glm::vec3 m_maxPos;

                float m_minX, m_maxX;
                float m_minY, m_maxY;
                float m_minZ, m_maxZ;


                BoundCategory(const BoundType& type, const glm::mat4& parentTranform, const std::string& parentName,
                    const glm::vec3& minPos, const glm::vec3& maxPos)
                    : m_type(type), m_parentTransform(parentTranform), m_parentName(parentName), m_minPos(minPos), m_maxPos(maxPos)
                {
                    m_minX = minPos.x;
                    m_maxX = maxPos.x;

                    m_minY = minPos.y;
                    m_maxY = maxPos.y;

                    m_minZ = minPos.z;
                    m_maxZ = maxPos.z;

                    m_minPos = glm::vec3(m_minX, m_minY, m_minZ);
                    m_maxPos = glm::vec3(m_maxX, m_maxY, m_maxZ);
                }

                BoundCategory(const BoundType& type, const glm::mat4& parentTranform, const std::string& parentName,
                    const glm::vec3& centerPos, const float& radius)
                    : m_type(type), m_parentTransform(parentTranform), m_parentName(parentName)
                {
                    m_minX = centerPos.x - radius;
                    m_maxX = centerPos.x + radius;

                    m_minY = centerPos.y - radius;
                    m_maxY = centerPos.y + radius;

                    m_minZ = centerPos.z - radius;
                    m_maxZ = centerPos.z + radius;

                    m_minPos = glm::vec3(m_minX, m_minY, m_minZ);
                    m_maxPos = glm::vec3(m_maxX, m_maxY, m_maxZ);
                }

                virtual void UpdateTransform()
                {
                    auto minX = m_parentTransform * glm::vec4(m_minX, 0, 0, 1.0f);
                    auto maxX = m_parentTransform * glm::vec4(m_maxX, 0, 0, 1.0f);

                    auto minY = m_parentTransform * glm::vec4(0, m_minY, 0, 1.0f);
                    auto maxY = m_parentTransform * glm::vec4(0, m_maxY, 0, 1.0f);

                    auto minZ = m_parentTransform * glm::vec4(0, 0, m_minZ, 1.0f);
                    auto maxZ = m_parentTransform * glm::vec4(0, 0, m_maxZ, 1.0f);

                    m_minPos = glm::vec3(minX.x, minY.y, minZ.z);
                    m_maxPos = glm::vec3(maxX.x, maxY.y, maxZ.z);

                    auto minPos = glm::vec3(m_minPos);
                    auto maxPos = glm::vec3(m_maxPos);

                    /*auto min = m_parentTransform * glm::vec4(m_minPos, 1.0f);
                    auto minPos = glm::vec3(min);

                    auto max = m_parentTransform * glm::vec4(m_maxPos, 1.0f);
                    auto maxPos = glm::vec3(max);*/

                    //glm::vec3 size = glm::vec3(glm::abs(maxPos.x - minPos.x), glm::abs(maxPos.y - minPos.y), glm::abs(maxPos.z - minPos.z));
                    glm::vec3 size = glm::vec3(glm::abs(m_maxX - m_minX), glm::abs(m_maxY - m_minY), glm::abs(m_maxZ - m_minZ));
                    glm::vec3 center = glm::vec3((maxPos.x + minPos.x) / 2, (maxPos.y + minPos.y) / 2, (maxPos.z + minPos.z) / 2);
                    m_transform = glm::translate(center) * glm::scale(size / 2.0f);
                }
            };

            struct AABB : BoundCategory
            {
                AABB(const glm::mat4& parentTranform, const std::string& parentName,
                    const glm::vec3& minPos, const glm::vec3& maxPos) :
                    BoundCategory(BoundType::AABB, parentTranform, parentName, minPos, maxPos)
                {
                }

                AABB(const glm::mat4& parentTranform, const std::string& parentName, const glm::vec3& centerPos, float radius):
                    BoundCategory(BoundType::AABB, parentTranform, parentName, centerPos, radius)
                {
                }
            };

            struct OBB : BoundCategory
            {
                OBB(const glm::mat4& parentTranform, const std::string& parentName,
                    const glm::vec3& minPos, const glm::vec3& maxPos) :
                    BoundCategory(BoundType::OBB, parentTranform, parentName, minPos, maxPos)
                {
                }
            };

            struct SphereBound : BoundCategory
            {
                glm::vec3 m_center;
                float m_radius;
                SphereBound(const glm::mat4& parentTranform, const std::string& parentName, const glm::vec3& centerPos, float radius) :
                    BoundCategory(BoundType::SPHERE, parentTranform, parentName, centerPos, radius),
                    m_center(centerPos), m_radius(radius)
                {
                }

                virtual void UpdateTransform() override
                {

                    auto center = glm::vec3(m_parentTransform[3].x, m_parentTransform[3].y, m_parentTransform[3].z);
                    m_transform = glm::translate(center) * glm::scale(glm::vec3(m_radius));


                    /*auto minX = m_parentTransform * glm::vec4(m_minX, 0, 0, 1.0f);
                    auto maxX = m_parentTransform * glm::vec4(m_maxX, 0, 0, 1.0f);

                    auto minY = m_parentTransform * glm::vec4(0, m_minY, 0, 1.0f);
                    auto maxY = m_parentTransform * glm::vec4(0, m_maxY, 0, 1.0f);

                    auto minZ = m_parentTransform * glm::vec4(0, 0, m_minZ, 1.0f);
                    auto maxZ = m_parentTransform * glm::vec4(0, 0, m_maxZ, 1.0f);

                    m_minPos = glm::vec3(minX.x, minY.y, minZ.z);
                    m_maxPos = glm::vec3(maxX.x, maxY.y, maxZ.z);

                    auto minPos = glm::vec3(m_minPos);
                    auto maxPos = glm::vec3(m_maxPos);

                    glm::vec3 size = glm::vec3(glm::abs(m_maxX - m_minX), glm::abs(m_maxY - m_minY), glm::abs(m_maxZ - m_minZ));
                    glm::vec3 center = glm::vec3((maxPos.x + minPos.x) / 2, (maxPos.y + minPos.y) / 2, (maxPos.z + minPos.z) / 2);
                    m_transform = glm::translate(center) * glm::scale(size);*/
                }
            };


            class Bound : public Component<Bound>
            {
            private:
                std::unique_ptr<BoundCategory> m_boundCatergory;

                Bound() = delete;
                Bound(const Bound&) = delete;
                Bound& operator=(const Bound&) = delete;

            public:
                std::vector<glm::vec3> m_positions;
                uint32_t m_vertexBufferId;
                uint32_t m_vertexBufferMemoryId;

                std::vector<uint32_t> m_indicies;
                std::optional<uint32_t> m_indexBufferId, m_indexBufferMemoryId;

                Core::ECS::COMPONENT_TYPE m_parentType;

                // set = TRANSFORM = 4
                uint32_t m_descriptorSetId;

                Bound(std::unique_ptr<BoundCategory>& category)
                {
                    m_boundCatergory = std::move(category);
                    componentType = COMPONENT_TYPE::BOUNDS;
                }

                const BoundType& GetBoundsType() const
                {
                    return m_boundCatergory->m_type;
                }

                BoundCategory* GetBoundsCategory()
                {
                    return m_boundCatergory.get();
                }

                ~Bound()
                {
                    m_boundCatergory.reset();
                }
            };
        }
    }
}
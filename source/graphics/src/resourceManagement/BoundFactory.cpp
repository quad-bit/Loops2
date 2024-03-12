
#include "resourceManagement/BoundFactory.h"
#include <ECS/Components/Bounds.h>
#include <stdint.h>
#include <algorithm>
#include <Platform/Assertion.h>
#include <VulkanInterface.h>
#include <Math/Cube.h>
#include <Math/Sphere.h>

Renderer::ResourceManagement::BoundFactory* Renderer::ResourceManagement::BoundFactory::instance = nullptr;

namespace
{
    void GetQubeData(std::vector<glm::vec3>& positionList)
    {
        Core::Math::Cube cube{};
        positionList = cube.positions;
    }

    void GetSphereData(std::vector<glm::vec3>& positionList, std::vector<uint32_t>& indexList)
    {
        Core::Math::Sphere obj(1);
        positionList = obj.m_vertices;
        indexList = obj.m_indices;
    }
};

void Renderer::ResourceManagement::BoundFactory::Init()
{
}

void Renderer::ResourceManagement::BoundFactory::DeInit()
{
    for (auto& bound : m_boundList)
    {
        delete bound;
    }
}

void Renderer::ResourceManagement::BoundFactory::Update()
{

}

Renderer::ResourceManagement::BoundFactory * Renderer::ResourceManagement::BoundFactory::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new Renderer::ResourceManagement::BoundFactory();
    }
    return instance;
}

Renderer::ResourceManagement::BoundFactory::~BoundFactory()
{
}

void Renderer::ResourceManagement::BoundFactory::AddBound(Core::ECS::Components::Bound* bound)
{
    bound->componentId = GenerateBoundId();

    // get the positions 
    switch (bound->GetBoundsType())
    {
    case Core::ECS::Components::BoundType::AABB:
    case Core::ECS::Components::BoundType::OBB:
    {
        GetQubeData(bound->m_positions);
    }
    break;
    case Core::ECS::Components::BoundType::SPHERE:
    {
        GetSphereData(bound->m_positions, bound->m_indicies);
    }
    break;
    default:
        break;
    }
    auto CreateBuffer = [](const size_t& dataSize, uint32_t& bufferId, uint32_t& memId)
    {
        Core::Enums::MemoryType mem[2]{ Core::Enums::MemoryType::HOST_VISIBLE_BIT, Core::Enums::MemoryType::HOST_COHERENT_BIT };
        Core::Wrappers::BufferCreateInfo info{};
        info.size = dataSize;
        info.usage.push_back(Core::Enums::BufferUsage::BUFFER_USAGE_VERTEX_BUFFER_BIT);

        bufferId = VulkanInterfaceAlias::CreateBuffer(info, "VertexBuffer");
        memId = VulkanInterfaceAlias::AllocateAndBindBufferMemory(bufferId, mem, 2, false, std::nullopt);
    };

    auto UploadData = [](uint32_t bufferId, size_t dataSize, void* data)
    {
        size_t memAlignedSize = dataSize;// VulkanInterfaceAlias::GetMemoryAlignedDataSizeForBuffer(dataSize);
        VulkanInterfaceAlias::CopyBufferDataToMemory(bufferId, dataSize, memAlignedSize, data, 0, false);
    };

    //position buffer
    size_t dataSize = sizeof(glm::vec3) * bound->m_positions.size();
    CreateBuffer(dataSize, bound->m_vertexBufferId, bound->m_vertexBufferMemoryId);
    UploadData(bound->m_vertexBufferId, dataSize, bound->m_positions.data());

    //index buffer
    if (bound->m_indicies.size() > 0)
    {
        Core::Enums::MemoryType mem[2]{ Core::Enums::MemoryType::HOST_VISIBLE_BIT, Core::Enums::MemoryType::HOST_COHERENT_BIT };
        Core::Wrappers::BufferCreateInfo info{};
        info.size = sizeof(uint32_t) * bound->m_indicies.size();
        info.usage.push_back(Core::Enums::BufferUsage::BUFFER_USAGE_INDEX_BUFFER_BIT);

        bound->m_indexBufferId = VulkanInterfaceAlias::CreateBuffer(info, "IndexBuffer");
        bound->m_indexBufferMemoryId = VulkanInterfaceAlias::AllocateAndBindBufferMemory(bound->m_indexBufferId.value(), mem, 2, false, std::nullopt);

        UploadData(bound->m_indexBufferId.value(), info.size, bound->m_indicies.data());
    }

    m_boundList.push_back(bound);
}

void Renderer::ResourceManagement::BoundFactory::DestroyBound(Core::ECS::Components::Bound* bound)
{
    VulkanInterfaceAlias::DestroyBuffer(&bound->m_vertexBufferId, 1, false);
    VulkanInterfaceAlias::FreeMemory(&bound->m_vertexBufferMemoryId, 1);

    if (bound->m_indicies.size() > 0)
    {
        VulkanInterfaceAlias::DestroyBuffer(&bound->m_indexBufferId.value(), 1, false);
        VulkanInterfaceAlias::FreeMemory(&bound->m_indexBufferMemoryId.value(), 1);
    }

    //delete bound;
}


#include "resourceManagement/MeshFactory.h"
#include <ECS/Components/Mesh.h>
#include <stdint.h>
#include <algorithm>
#include <Platform/Assertion.h>
#include <VulkanInterface.h>
//#include "GraphicsPipelineManager.h"

Renderer::ResourceManagement::MeshFactory* Renderer::ResourceManagement::MeshFactory::instance = nullptr;

int * Renderer::ResourceManagement::MeshFactory::CreateVertexBuffer(uint32_t count)
{
    return nullptr;
}

int Renderer::ResourceManagement::MeshFactory::CreateIndexBuffer()
{
    return 0;
}

void Renderer::ResourceManagement::MeshFactory::Init()
{
    /*pcMask.set((size_t)Core::Utility::ATTRIBUTES::POSITION);
    pcMask.set((size_t)Core::Utility::ATTRIBUTES::COLOR);

    pcnMask.set((size_t)Core::Utility::ATTRIBUTES::POSITION);
    pcnMask.set((size_t)Core::Utility::ATTRIBUTES::COLOR);
    pcnMask.set((size_t)Core::Utility::ATTRIBUTES::NORMAL);*/
}

void Renderer::ResourceManagement::MeshFactory::DeInit()
{
    for (auto& mesh : m_meshList)
    {
        delete mesh;
    }
}

void Renderer::ResourceManagement::MeshFactory::Update()
{

}

Renderer::ResourceManagement::MeshFactory * Renderer::ResourceManagement::MeshFactory::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new Renderer::ResourceManagement::MeshFactory();
    }
    return instance;
}

Renderer::ResourceManagement::MeshFactory::~MeshFactory()
{
}

#if 0
//deprecated.
Core::ECS::Components::Mesh * Renderer::ResourceManagement::MeshFactory::CreateMesh(const Core::Containers::BitArray & attribMaskReq, Core::ECS::Components::MESH_TYPE * meshType, bool isIndexed, bool individualBuffersRequired)
{
    Core::ECS::Components::Mesh * mesh = new Core::ECS::Components::Mesh();
    uint32_t numVertexBufferPerMesh = 1;
    //
    //if (attribMaskReq[(unsigned int)Core::Utility::ATTRIBUTES::POSITION] && attribMaskReq[(unsigned int)Core::Utility::ATTRIBUTES::COLOR])
    //{
    //    Core::Utility::AttribStructBase * wrapper = new Core::Utility::AttribPC();
    //    if (isIndexed)
    //        ((Core::Utility::AttribPC*)wrapper)->FillData<Core::Math::CubeIndexed>(mesh);

    //    mesh->vertexAttributeCount = 2;
    //    mesh->componentId = GenerateMeshId();
    //    mesh->vertexCount = wrapper->vertexCount;

    //    if (individualBuffersRequired == false)
    //    {
    //        Core::Enums::BufferType bufferType = Core::Enums::BufferType::VERTEX_BUFFER_BIT;
    //        Core::Enums::MemoryType memType = Core::Enums::MemoryType::HOST_VISIBLE_BIT;
    //        Core::Wrappers::BufferInfo info = {};
    //        info.bufType = &bufferType;
    //        info.memType = &memType;
    //        info.memTypeCount = 1;
    //        info.data = wrapper->vertexData;
    //        info.dataSize = wrapper->vertexDataSize;
    //        //info.pGpuMem = new void*;

    //        mesh->vertexBufferCount = numVertexBufferPerMesh;
    //        mesh->vertexBuffersIds = VulkanInterfaceAlias::CreateBuffers(&info, numVertexBufferPerMesh);
    //        //mesh->pGpuMemVB = new void*[numVertexBufferPerMesh];
    //        //mesh->pGpuMemVB[0] = info.pGpuMem;
    //    }

    //    Core::Enums::BufferType bufferType = Core::Enums::BufferType::INDEX_BUFFER_BIT;
    //    Core::Enums::MemoryType memType = Core::Enums::MemoryType::HOST_VISIBLE_BIT;
    //    Core::Wrappers::BufferInfo info = {};
    //    info.bufType = &bufferType;
    //    info.memType = &memType;
    //    info.data = wrapper->indexData;
    //    info.dataSize = wrapper->indexDataSize;
    //    info.memTypeCount = 1;
    //    //info.pGpuMem = new void*;
    //    mesh->vertexCount = wrapper->indexCount;
    //    mesh->indexBufferId = *VulkanInterfaceAlias::CreateBuffers(&info, 1);
    //    //mesh->pGpuMemIB = info.pGpuMem;

    //    meshToVertWrapperMap.insert({mesh, wrapper});

    //    Core::Wrappers::VertexInputBindingInfo * bindingInfo = new Core::Wrappers::VertexInputBindingInfo[numVertexBufferPerMesh];
    //    for (uint32_t i = 0; i < numVertexBufferPerMesh; i++)
    //    {
    //        bindingInfo[i].binding = i;
    //        bindingInfo[i].inputRate = Core::Enums::VertexIputRate::PerVertex;
    //        bindingInfo[i].stride = wrapper->metaData.vertexDataStride;
    //    }
    //    
    //    Core::Wrappers::VertexInputAttributeInfo * attribInfo = wrapper->metaData.attribInfoList;
    //    uint32_t attribCount = (uint32_t)wrapper->metaData.attribCount;

    //    //apiInterface->InitiateGraphicsPipelineCreation(mesh->meshId, attribInfo, attribCount, bindingInfo, numVertexBufferPerMesh);
    //    delete [] bindingInfo;
    //}
    //
    return mesh;
}

Core::ECS::Components::Mesh * Renderer::ResourceManagement::MeshFactory::CreateMesh(const Core::Wrappers::MeshInfo * meshInfo, Core::ECS::Components::MESH_TYPE * meshType)
{
    Core::ECS::Components::Mesh * mesh = new Core::ECS::Components::Mesh();
    mesh->componentId = GenerateMeshId();
    
    //uint32_t numVertexBufferPerMesh = 1;
    //Core::Utility::AttribStructBase * wrapper;

    //if((meshInfo->attribMaskReq | pcMask) == pcMask)
    //{
    //    wrapper = new Core::Utility::AttribPC();
    //    if (meshInfo->isIndexed)
    //    {
    //        FillMeshAttrib<Core::Utility::AttribPC>(wrapper, meshType, mesh, meshInfo);
    //    }
    //}
    //else if ((meshInfo->attribMaskReq | pcnMask) == pcnMask)
    //{
    //    wrapper = new Core::Utility::AttribPCN();
    //    if (meshInfo->isIndexed)
    //    {
    //        FillMeshAttrib<Core::Utility::AttribPCN>(wrapper, meshType, mesh, meshInfo);
    //    }
    //}
    //mesh->vertexAttributeCount = wrapper->metaData.attribCount;
    //mesh->vertexCount = wrapper->vertexCount;

    ////vertex buffer
    //if (meshInfo->bufferPerAttribRequired == false)
    //{
    //    Core::Enums::BufferType bufferType = Core::Enums::BufferType::VERTEX_BUFFER_BIT;
    //    Core::Enums::MemoryType memType = Core::Enums::MemoryType::HOST_VISIBLE_BIT;
    //    Core::Wrappers::BufferInfo info = {};
    //    info.bufType = &bufferType;
    //    info.memType = &memType;
    //    info.memTypeCount = 1;
    //    //info.data = wrapper->vertexData;
    //    info.dataSize = wrapper->vertexDataSize;
    //    //info.pGpuMem = new void*;
    //    mesh->vertexBufferCount = numVertexBufferPerMesh;
    //    mesh->vertexDataSizes = new size_t[mesh->vertexBufferCount];
    //    mesh->vertexDataSizes[0] = (size_t)wrapper->vertexDataSize;

    //    mesh->vertexBuffersIds = VulkanInterfaceAlias::CreateBuffers(&info, numVertexBufferPerMesh);
    //    //mesh->pGpuMemVB = new void*[numVertexBufferPerMesh];
    //    //mesh->pGpuMemVB[0] = info.pGpuMem;
    //}
    //else
    //{
    //    ASSERT_MSG_DEBUG(0, "individual buffers yet to be implemented.");
    //}

    ////index buffer
    //if(wrapper->indexCount > 0)
    //{
    //    Core::Enums::BufferType bufferType = Core::Enums::BufferType::INDEX_BUFFER_BIT;
    //    Core::Enums::MemoryType memType = Core::Enums::MemoryType::HOST_VISIBLE_BIT;
    //    Core::Wrappers::BufferInfo info = {};
    //    info.bufType = &bufferType;
    //    info.memType = &memType;
    //    info.memTypeCount = 1;
    //    //info.data = wrapper->indexData;
    //    info.dataSize = wrapper->indexDataSize;
    //    //info.pGpuMem = new void*;
    //    mesh->indexCount = wrapper->indexCount;
    //    mesh->indexBufferId = *VulkanInterfaceAlias::CreateBuffers(&info, 1);
    //    //mesh->pGpuMemIB = info.pGpuMem;
    //    mesh->indexDataSize = (size_t)wrapper->indexDataSize;

    //}
    //else
    //{
    //    mesh->indexCount = 0;
    //}

    //// memory allocations for buffers
    //{
    //    uint32_t numMemory = mesh->vertexBufferCount;
    //    std::vector<uint32_t> ids;
    //    ids.push_back(mesh->vertexBuffersIds[0]);
    //        
    //    if (mesh->indexCount > 0)
    //    {
    //        ids.push_back(mesh->indexBufferId);
    //        numMemory += 1;
    //    }
    //    mesh->memoryCount = numMemory;
    //    mesh->memoryIds = VulkanInterfaceAlias::AllocateBufferMemory(ids.data(), (uint32_t)ids.size());
    //}

    //// copy data to buffer memory
    //{
    //    size_t memAlignedSize = wrapper->vertexDataSize;// apiInterface->GetMemoryAlignedDataSizeForBuffer(wrapper->vertexDataSize);
    //    VulkanInterfaceAlias::CopyBufferDataToMemory(mesh->vertexBuffersIds[0], wrapper->vertexDataSize, memAlignedSize, wrapper->vertexData, 0, false);

    //    if (mesh->indexCount > 0)
    //    {
    //        memAlignedSize = wrapper->indexDataSize;// apiInterface->GetMemoryAlignedDataSizeForBuffer(wrapper->indexDataSize);
    //        VulkanInterfaceAlias::CopyBufferDataToMemory(mesh->indexBufferId, wrapper->indexDataSize, memAlignedSize, wrapper->indexData, 0, false);
    //    }
    //}

    //meshToVertWrapperMap.insert({ mesh, wrapper });

    /*
    Core::Wrappers::VertexInputBindingInfo * bindingInfo = new Core::Wrappers::VertexInputBindingInfo[numVertexBufferPerMesh];
    for (uint32_t i = 0; i < numVertexBufferPerMesh; i++)
    {
        bindingInfo[i].binding = i;
        bindingInfo[i].inputRate = Core::Enums::VertexIputRate::PerVertex;
        bindingInfo[i].stride = wrapper->metaData.vertexDataStride;
    }

    Core::Wrappers::VertexInputAttributeInfo * attribInfo = wrapper->metaData.attribInfoList;
    uint32_t attribCount = (uint32_t)wrapper->metaData.attribCount;

    Core::Wrappers::VertexInputState * inputState = new Core::Wrappers::VertexInputState;
    inputState->attribCount = attribCount;
    inputState->attribInfo = attribInfo;
    inputState->bindingCount = numVertexBufferPerMesh;
    inputState->bindingInfo = bindingInfo;
    inputState->state = Core::Enums::PipelineStates::VertexInputState;

    GraphicsPipelineManager<ApiInterface>::GetInstance()->CreateVertexInputState(mesh->componentId, inputState);

    if (meshInfo->primitive != nullptr)
    {
        //Create primitive assembly state
        Core::Wrappers::InputAssemblyState * assembly = new Core::Wrappers::InputAssemblyState;
        assembly->isPrimtiveRestartEnabled = meshInfo->isPrimitiveRestartEnabled;
        assembly->primitiveType = meshInfo->primitive;

        GraphicsPipelineManager<ApiInterface>::GetInstance()->CreateVertexAssemblyState(mesh->componentId, assembly);
    }
    else
    {
        // assign default primitive assembly state
        GraphicsPipelineManager<ApiInterface>::GetInstance()->AssignDefaultState(mesh->componentId, Core::Enums::PipelineStates::InputAssemblyState);
    }
    */
    return mesh;
}
#endif

Core::ECS::Components::Mesh* Renderer::ResourceManagement::MeshFactory::CreateBasicPrimitiveMesh(const Core::ECS::Components::MESH_TYPE& meshType, bool isIndexed)
{
    Core::ECS::Components::Mesh* mesh = new Core::ECS::Components::Mesh();
    mesh->componentId = GenerateMeshId();

    switch (meshType)
    {
    case Core::ECS::Components::MESH_TYPE::CUBE:
        FillMeshData<Core::Math::CubeIndexed>(mesh, isIndexed);
        break;
    case Core::ECS::Components::MESH_TYPE::QUAD:
        FillMeshData<Core::Math::QuadIndexed>(mesh, isIndexed);
        break;
    }

    ASSERT_MSG_DEBUG(mesh->m_positions.size() > 0, "Positions missing");

    auto CreateBuffer = [](const size_t& dataSize, uint32_t& bufferId, uint32_t& memId)
    {
        Core::Enums::BufferType bufferType = Core::Enums::BufferType::VERTEX_BUFFER_BIT;
        Core::Enums::MemoryType memType = Core::Enums::MemoryType::HOST_VISIBLE_BIT;
        Core::Wrappers::BufferInfo info = {};
        info.bufType = &bufferType;
        info.memType = &memType;
        info.memTypeCount = 1;
        info.dataSize = dataSize;

        bufferId = *VulkanInterfaceAlias::CreateBuffers(&info, 1);
        memId = *VulkanInterfaceAlias::AllocateBufferMemory(&bufferId, 1);
    };

    auto UploadData = [](uint32_t bufferId, size_t dataSize, void* data)
    {
        size_t memAlignedSize = dataSize;// VulkanInterfaceAlias::GetMemoryAlignedDataSizeForBuffer(dataSize);
        VulkanInterfaceAlias::CopyBufferDataToMemory(bufferId, dataSize, memAlignedSize, data, 0, false);
    };

    //position buffer
    size_t dataSize = sizeof(glm::vec3) * mesh->m_positions.size();
    CreateBuffer(dataSize, mesh->m_positionBufferId, mesh->m_positionBufferMemoryId);
    UploadData(mesh->m_positionBufferId, dataSize, mesh->m_positions.data());

    //color buffer
    if (mesh->m_colors.size() > 0)
    {
        dataSize = sizeof(glm::vec4) * mesh->m_colors.size();
        CreateBuffer(dataSize, mesh->m_colorBufferId, mesh->m_colorBufferMemoryId);
        UploadData(mesh->m_colorBufferId, dataSize, mesh->m_colors.data());
    }

    //normal buffer
    if (mesh->m_normals.size() > 0)
    {
        dataSize = sizeof(glm::vec3) * mesh->m_normals.size();
        CreateBuffer(dataSize, mesh->m_normalBufferId, mesh->m_normalBufferMemoryId);
        UploadData(mesh->m_normalBufferId, dataSize, mesh->m_normals.data());
    }

    //index buffer
    if(mesh->m_indicies.size() > 0)
    {
        Core::Enums::BufferType bufferType = Core::Enums::BufferType::INDEX_BUFFER_BIT;
        Core::Enums::MemoryType memType = Core::Enums::MemoryType::HOST_VISIBLE_BIT;
        Core::Wrappers::BufferInfo info = {};
        info.bufType = &bufferType;
        info.memType = &memType;
        info.memTypeCount = 1;
        info.dataSize = sizeof(uint32_t) * mesh->m_indicies.size();

        mesh->m_indexBufferId = *VulkanInterfaceAlias::CreateBuffers(&info, 1);
        mesh->m_indexBufferMemoryId = *VulkanInterfaceAlias::AllocateBufferMemory(&mesh->m_indexBufferId, 1);

        UploadData(mesh->m_indexBufferId, info.dataSize, mesh->m_indicies.data());
    }

    m_meshList.push_back(mesh);
    return mesh;
}

void Renderer::ResourceManagement::MeshFactory::AddMesh(Core::ECS::Components::Mesh* mesh)
{
    mesh->componentId = GenerateMeshId();

    ASSERT_MSG_DEBUG(mesh->m_positions.size() > 0, "Positions missing");

    auto CreateBuffer = [](const size_t& dataSize, uint32_t& bufferId, uint32_t& memId)
    {
        Core::Enums::BufferType bufferType = Core::Enums::BufferType::VERTEX_BUFFER_BIT;
        Core::Enums::MemoryType memType = Core::Enums::MemoryType::HOST_VISIBLE_BIT;
        Core::Wrappers::BufferInfo info = {};
        info.bufType = &bufferType;
        info.memType = &memType;
        info.memTypeCount = 1;
        info.dataSize = dataSize;

        bufferId = *VulkanInterfaceAlias::CreateBuffers(&info, 1);
        memId = *VulkanInterfaceAlias::AllocateBufferMemory(&bufferId, 1);
    };

    auto UploadData = [](uint32_t bufferId, size_t dataSize, void* data)
    {
        size_t memAlignedSize = dataSize;// VulkanInterfaceAlias::GetMemoryAlignedDataSizeForBuffer(dataSize);
        VulkanInterfaceAlias::CopyBufferDataToMemory(bufferId, dataSize, memAlignedSize, data, 0, false);
    };

    //position buffer
    size_t dataSize = sizeof(glm::vec3) * mesh->m_positions.size();
    CreateBuffer(dataSize, mesh->m_positionBufferId, mesh->m_positionBufferMemoryId);
    UploadData(mesh->m_positionBufferId, dataSize, mesh->m_positions.data());

    //color buffer
    if (mesh->m_colors.size() > 0)
    {
        dataSize = sizeof(glm::vec4) * mesh->m_colors.size();
        CreateBuffer(dataSize, mesh->m_colorBufferId, mesh->m_colorBufferMemoryId);
        UploadData(mesh->m_colorBufferId, dataSize, mesh->m_colors.data());
    }

    //normal buffer
    if (mesh->m_normals.size() > 0)
    {
        dataSize = sizeof(glm::vec3) * mesh->m_normals.size();
        CreateBuffer(dataSize, mesh->m_normalBufferId, mesh->m_normalBufferMemoryId);
        UploadData(mesh->m_normalBufferId, dataSize, mesh->m_normals.data());
    }

    //uv0 buffer
    if (mesh->m_uv0.size() > 0)
    {
        dataSize = sizeof(glm::vec2) * mesh->m_uv0.size();
        CreateBuffer(dataSize, mesh->m_uv0BufferId, mesh->m_uv0BufferMemoryId);
        UploadData(mesh->m_uv0BufferId, dataSize, mesh->m_uv0.data());
    }

    //index buffer
    if (mesh->m_indicies.size() > 0)
    {
        Core::Enums::BufferType bufferType = Core::Enums::BufferType::INDEX_BUFFER_BIT;
        Core::Enums::MemoryType memType = Core::Enums::MemoryType::HOST_VISIBLE_BIT;
        Core::Wrappers::BufferInfo info = {};
        info.bufType = &bufferType;
        info.memType = &memType;
        info.memTypeCount = 1;
        info.dataSize = sizeof(uint32_t) * mesh->m_indicies.size();

        mesh->m_indexBufferId = *VulkanInterfaceAlias::CreateBuffers(&info, 1);
        mesh->m_indexBufferMemoryId = *VulkanInterfaceAlias::AllocateBufferMemory(&mesh->m_indexBufferId, 1);

        UploadData(mesh->m_indexBufferId, info.dataSize, mesh->m_indicies.data());
    }

    m_meshList.push_back(mesh);
}


//void Renderer::ResourceManagement::MeshFactory::DestroyMesh(const uint32_t & meshId)
//{
//    ASSERT_MSG_DEBUG(0, "Fix this");

    //std::map<Core::ECS::Components::Mesh *, Core::Utility::AttribStructBase *>::iterator it;
    //std::pair<Core::ECS::Components::Mesh *, Core::Utility::AttribStructBase *> obj;
    //it = std::find_if(meshToVertWrapperMap.begin(), meshToVertWrapperMap.end(), 
    //    [&](std::pair<Core::ECS::Components::Mesh *, Core::Utility::AttribStructBase *> obj) { return obj.first->componentId == meshId; });

    //ASSERT_MSG_DEBUG(it != meshToVertWrapperMap.end(), "Core::ECS::Components::Mesh not found");

    //VulkanInterfaceAlias::DestroyBuffer((it)->first->vertexBuffersIds, (it)->first->vertexBufferCount);
    //if((it)->first->indexCount > 0)
    //    VulkanInterfaceAlias::DestroyBuffer(&(it)->first->indexBufferId, 1);
    //VulkanInterfaceAlias::FreeMemory(it->first->memoryIds, it->first->memoryCount);

    //delete[] it->first->vertexBuffersIds;
    //delete[] it->first->memoryIds;
    //delete[] it->first->vertexDataSizes;
//}

void Renderer::ResourceManagement::MeshFactory::DestroyMesh(Core::ECS::Components::Mesh* mesh)
{
    VulkanInterfaceAlias::DestroyBuffer(&mesh->m_positionBufferId, 1);
    VulkanInterfaceAlias::FreeMemory(&mesh->m_positionBufferMemoryId, 1);

    if (mesh->m_normals.size() > 0)
    {
        VulkanInterfaceAlias::DestroyBuffer(&mesh->m_normalBufferId, 1);
        VulkanInterfaceAlias::FreeMemory(&mesh->m_normalBufferMemoryId, 1);
    }

    if (mesh->m_colors.size() > 0)
    {
        VulkanInterfaceAlias::DestroyBuffer(&mesh->m_colorBufferId, 1);
        VulkanInterfaceAlias::FreeMemory(&mesh->m_colorBufferMemoryId, 1);
    }

    if (mesh->m_indicies.size() > 0)
    {
        VulkanInterfaceAlias::DestroyBuffer(&mesh->m_indexBufferId, 1);
        VulkanInterfaceAlias::FreeMemory(&mesh->m_indexBufferMemoryId, 1);
    }

    if (mesh->m_uv0.size() > 0)
    {
        VulkanInterfaceAlias::DestroyBuffer(&mesh->m_uv0BufferId, 1);
        VulkanInterfaceAlias::FreeMemory(&mesh->m_uv0BufferMemoryId, 1);
    }

    //delete mesh;
}

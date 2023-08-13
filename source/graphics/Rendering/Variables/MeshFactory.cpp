
#include "MeshFactory.h"
#include <Mesh.h>
#include <stdint.h>
#include <algorithm>
#include <Assertion.h>
#include "GraphicsPipelineManager.h"

MeshFactory* MeshFactory::instance = nullptr;

int * MeshFactory::CreateVertexBuffer(uint32_t count)
{
    return nullptr;
}

int MeshFactory::CreateIndexBuffer()
{
    return 0;
}

void MeshFactory::Init(ApiInterface * obj)
{
    apiInterface = obj;

    pcMask.set((size_t)ATTRIBUTES::POSITION);
    pcMask.set((size_t)ATTRIBUTES::COLOR);

    pcnMask.set((size_t)ATTRIBUTES::POSITION);
    pcnMask.set((size_t)ATTRIBUTES::COLOR);
    pcnMask.set((size_t)ATTRIBUTES::NORMAL);
}

void MeshFactory::DeInit()
{
    std::map<Mesh *, AttribStructBase *>::iterator it;

    for (it = meshToVertWrapperMap.begin(); it != meshToVertWrapperMap.end(); it++)
    {
        delete it->second;
    }
}

void MeshFactory::Update()
{

}

MeshFactory * MeshFactory::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new MeshFactory();
    }
    return instance;
}

MeshFactory::~MeshFactory()
{
}


//deprecated.
Mesh * MeshFactory::CreateMesh(const BitArray & attribMaskReq, MESH_TYPE * meshType, bool isIndexed, bool individualBuffersRequired)
{
    Mesh * mesh = new Mesh();
    uint32_t numVertexBufferPerMesh = 1;
    
    if (attribMaskReq[(unsigned int)ATTRIBUTES::POSITION] && attribMaskReq[(unsigned int)ATTRIBUTES::COLOR])
    {
        AttribStructBase * wrapper = new AttribPC();
        if (isIndexed)
            ((AttribPC*)wrapper)->FillData<CubeIndexed>(mesh);

        mesh->vertexAttributeCount = 2;
        mesh->componentId = GenerateMeshId();
        mesh->vertexCount = wrapper->vertexCount;

        if (individualBuffersRequired == false)
        {
            BufferType bufferType = BufferType::VERTEX_BUFFER_BIT;
            MemoryType memType = MemoryType::HOST_VISIBLE_BIT;
            BufferInfo info = {};
            info.bufType = &bufferType;
            info.memType = &memType;
            info.memTypeCount = 1;
            info.data = wrapper->vertexData;
            info.dataSize = wrapper->vertexDataSize;
            //info.pGpuMem = new void*;

            mesh->vertexBufferCount = numVertexBufferPerMesh;
            mesh->vertexBuffersIds = apiInterface->CreateBuffers(&info, numVertexBufferPerMesh);
            //mesh->pGpuMemVB = new void*[numVertexBufferPerMesh];
            //mesh->pGpuMemVB[0] = info.pGpuMem;
        }

        BufferType bufferType = BufferType::INDEX_BUFFER_BIT;
        MemoryType memType = MemoryType::HOST_VISIBLE_BIT;
        BufferInfo info = {};
        info.bufType = &bufferType;
        info.memType = &memType;
        info.data = wrapper->indexData;
        info.dataSize = wrapper->indexDataSize;
        info.memTypeCount = 1;
        //info.pGpuMem = new void*;
        mesh->vertexCount = wrapper->indexCount;
        mesh->indexBufferId = *apiInterface->CreateBuffers(&info, 1);
        //mesh->pGpuMemIB = info.pGpuMem;

        meshToVertWrapperMap.insert({mesh, wrapper});
        
        VertexInputBindingInfo * bindingInfo = new VertexInputBindingInfo[numVertexBufferPerMesh];
        for (uint32_t i = 0; i < numVertexBufferPerMesh; i++)
        {
            bindingInfo[i].binding = i;
            bindingInfo[i].inputRate = VertexIputRate::PerVertex;
            bindingInfo[i].stride = wrapper->metaData.vertexDataStride;
        }
        
        VertexInputAttributeInfo * attribInfo = wrapper->metaData.attribInfoList;
        uint32_t attribCount = (uint32_t)wrapper->metaData.attribCount;

        //apiInterface->InitiateGraphicsPipelineCreation(mesh->meshId, attribInfo, attribCount, bindingInfo, numVertexBufferPerMesh);
        delete [] bindingInfo;
    }
    
    return mesh;
}

Mesh * MeshFactory::CreateMesh(const MeshInfo * meshInfo, MESH_TYPE * meshType)
{
    Mesh * mesh = new Mesh();
    uint32_t numVertexBufferPerMesh = 1;
    AttribStructBase * wrapper;

    if((meshInfo->attribMaskReq | pcMask) == pcMask)
    {
        wrapper = new AttribPC();
        if (meshInfo->isIndexed)
        {
            FillMeshAttrib<AttribPC>(wrapper, meshType, mesh, meshInfo);
        }
    }
    else if ((meshInfo->attribMaskReq | pcnMask) == pcnMask)
    {
        wrapper = new AttribPCN();
        if (meshInfo->isIndexed)
        {
            FillMeshAttrib<AttribPCN>(wrapper, meshType, mesh, meshInfo);
        }
    }
        mesh->vertexAttributeCount = wrapper->metaData.attribCount;
        mesh->componentId = GenerateMeshId();
        mesh->vertexCount = wrapper->vertexCount;

        //vertex buffer
        if (meshInfo->bufferPerAttribRequired == false)
        {
            BufferType bufferType = BufferType::VERTEX_BUFFER_BIT;
            MemoryType memType = MemoryType::HOST_VISIBLE_BIT;
            BufferInfo info = {};
            info.bufType = &bufferType;
            info.memType = &memType;
            info.memTypeCount = 1;
            //info.data = wrapper->vertexData;
            info.dataSize = wrapper->vertexDataSize;
            //info.pGpuMem = new void*;
            mesh->vertexBufferCount = numVertexBufferPerMesh;
            mesh->vertexDataSizes = new size_t[mesh->vertexBufferCount];
            mesh->vertexDataSizes[0] = (size_t)wrapper->vertexDataSize;

            mesh->vertexBuffersIds = apiInterface->CreateBuffers(&info, numVertexBufferPerMesh);
            //mesh->pGpuMemVB = new void*[numVertexBufferPerMesh];
            //mesh->pGpuMemVB[0] = info.pGpuMem;
        }
        else
        {
            ASSERT_MSG_DEBUG(0, "individual buffers yet to be implemented.");
        }

        //index buffer
        if(wrapper->indexCount > 0)
        {
            BufferType bufferType = BufferType::INDEX_BUFFER_BIT;
            MemoryType memType = MemoryType::HOST_VISIBLE_BIT;
            BufferInfo info = {};
            info.bufType = &bufferType;
            info.memType = &memType;
            info.memTypeCount = 1;
            //info.data = wrapper->indexData;
            info.dataSize = wrapper->indexDataSize;
            //info.pGpuMem = new void*;
            mesh->indexCount = wrapper->indexCount;
            mesh->indexBufferId = *apiInterface->CreateBuffers(&info, 1);
            //mesh->pGpuMemIB = info.pGpuMem;
            mesh->indexDataSize = (size_t)wrapper->indexDataSize;

        }
        else
        {
            mesh->indexCount = 0;
        }

        // memory allocations for buffers
        {
            uint32_t numMemory = mesh->vertexBufferCount;
            std::vector<uint32_t> ids;
            ids.push_back(mesh->vertexBuffersIds[0]);
            
            if (mesh->indexCount > 0)
            {
                ids.push_back(mesh->indexBufferId);
                numMemory += 1;
            }
            mesh->memoryCount = numMemory;
            mesh->memoryIds = apiInterface->AllocateBufferMemory(ids.data(), (uint32_t)ids.size());
        }

        // copy data to buffer memory
        {
            size_t memAlignedSize = wrapper->vertexDataSize;// apiInterface->GetMemoryAlignedDataSizeForBuffer(wrapper->vertexDataSize);
            apiInterface->CopyBufferDataToMemory(mesh->vertexBuffersIds[0], wrapper->vertexDataSize, memAlignedSize, wrapper->vertexData, 0, false);

            if (mesh->indexCount > 0)
            {
                memAlignedSize = wrapper->indexDataSize;// apiInterface->GetMemoryAlignedDataSizeForBuffer(wrapper->indexDataSize);
                apiInterface->CopyBufferDataToMemory(mesh->indexBufferId, wrapper->indexDataSize, memAlignedSize, wrapper->indexData, 0, false);
            }
        }

        meshToVertWrapperMap.insert({ mesh, wrapper });

        VertexInputBindingInfo * bindingInfo = new VertexInputBindingInfo[numVertexBufferPerMesh];
        for (uint32_t i = 0; i < numVertexBufferPerMesh; i++)
        {
            bindingInfo[i].binding = i;
            bindingInfo[i].inputRate = VertexIputRate::PerVertex;
            bindingInfo[i].stride = wrapper->metaData.vertexDataStride;
        }

        VertexInputAttributeInfo * attribInfo = wrapper->metaData.attribInfoList;
        uint32_t attribCount = (uint32_t)wrapper->metaData.attribCount;

        VertexInputState * inputState = new VertexInputState;
        inputState->attribCount = attribCount;
        inputState->attribInfo = attribInfo;
        inputState->bindingCount = numVertexBufferPerMesh;
        inputState->bindingInfo = bindingInfo;
        inputState->state = PipelineStates::VertexInputState;

        GraphicsPipelineManager<ApiInterface>::GetInstance()->CreateVertexInputState(mesh->componentId, inputState);

        if (meshInfo->primitive != nullptr)
        {
            //Create primitive assembly state
            InputAssemblyState * assembly = new InputAssemblyState;
            assembly->isPrimtiveRestartEnabled = meshInfo->isPrimitiveRestartEnabled;
            assembly->primitiveType = meshInfo->primitive;

            GraphicsPipelineManager<ApiInterface>::GetInstance()->CreateVertexAssemblyState(mesh->componentId, assembly);
        }
        else
        {
            // assign default primitive assembly state
            GraphicsPipelineManager<ApiInterface>::GetInstance()->AssignDefaultState(mesh->componentId, PipelineStates::InputAssemblyState);
        }

    return mesh;
}


void MeshFactory::DestroyMesh(const uint32_t & meshId)
{
    std::map<Mesh *, AttribStructBase *>::iterator it;
    std::pair<Mesh *, AttribStructBase *> obj;
    it = std::find_if(meshToVertWrapperMap.begin(), meshToVertWrapperMap.end(), 
        [&](std::pair<Mesh *, AttribStructBase *> obj) { return obj.first->componentId == meshId; });

    ASSERT_MSG_DEBUG(it != meshToVertWrapperMap.end(), "Mesh not found");

    apiInterface->DestroyBuffer((it)->first->vertexBuffersIds, (it)->first->vertexBufferCount);
    apiInterface->DestroyBuffer(&(it)->first->indexBufferId, 1);
    apiInterface->FreeMemory(it->first->memoryIds, it->first->memoryCount);

    delete[] it->first->vertexBuffersIds;
    delete[] it->first->memoryIds;
    delete[] it->first->vertexDataSizes;
}

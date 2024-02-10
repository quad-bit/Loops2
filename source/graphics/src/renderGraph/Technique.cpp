#include "renderGraph/Technique.h"

void Renderer::RenderGraph::CreateDrawInfo(const Core::Utility::TransformData& data, const std::string& effectName, const std::string& techName, const std::string& taskName, Renderer::RenderGraph::Tasks::DrawInfo& drawInfo)
{
    Renderer::RenderGraph::Tasks::MeshDrawInfo meshInfo{};

    // index buffer info
    if (data.m_indexBufferId.has_value())
    {
        Core::Wrappers::IndexBufferBindingInfo indexInfo{};
        indexInfo.bufferId = data.m_indexBufferId.value();
        indexInfo.indexType = Core::Enums::IndexType::INDEX_TYPE_UINT32;
        indexInfo.offset = 0;
        meshInfo.m_indexBufferInfo = indexInfo;
        meshInfo.m_indexCount = data.m_indexCount.value();
    }

    auto effectId = VulkanInterfaceAlias::GetEffectId(effectName);
    auto techId = VulkanInterfaceAlias::GetTechniqueId(effectId, techName);
    auto taskId = VulkanInterfaceAlias::GetTaskId(effectId, techId, taskName);

    // vertex buffer infos
    auto& vertexBufferBindingTypeList = VulkanInterfaceAlias::GetVertexBindingTypeInfo(taskId);
    Core::Wrappers::VertexBufferBindingInfo vertexInfo{};
    vertexInfo.bindingCount = 0;
    vertexInfo.firstBinding = 0;
    for (auto& bindingType : vertexBufferBindingTypeList)
    {
        vertexInfo.pOffsets.push_back({ 0 });
        vertexInfo.bindingCount++;

        switch (bindingType.m_bindingType)
        {
        case Core::Enums::VertexAttributeType::POSITION:
            vertexInfo.bufferIds.push_back(data.m_positionBufferId);
            break;
        case Core::Enums::VertexAttributeType::COLOR:
            if (!data.m_colorBufferId.has_value())
                ASSERT_MSG_DEBUG(0, "No Color attrib in the json file");
            vertexInfo.bufferIds.push_back(data.m_colorBufferId.value());
            break;
        case Core::Enums::VertexAttributeType::NORMAL:
            if (!data.m_normalBufferId.has_value())
                ASSERT_MSG_DEBUG(0, "No Normal attrib in the json file");
            vertexInfo.bufferIds.push_back(data.m_normalBufferId.value());
            break;
        case Core::Enums::VertexAttributeType::UV0:
            if (!data.m_uv0BufferId.has_value())
                ASSERT_MSG_DEBUG(0, "No UV0 attrib in the json file");
            vertexInfo.bufferIds.push_back(data.m_uv0BufferId.value());
            break;
            //case Core::Enums::VertexAttributeType::TANGENT:
            //    vertexInfo.bufferIds.push_back(data.m_positionBufferId);
            //    break;
        }
    }
    meshInfo.m_vertexBufferInfo.push_back(vertexInfo);
    meshInfo.m_vertexCount = data.m_vertexCount;
    drawInfo.m_meshInfoList.push_back(meshInfo);
}

//Test data
        /*
    SetInfo tranInfo0{};
    tranInfo0.m_setValue = Core::Enums::TRANSFORM;
    tranInfo0.m_descriptorSetId = 12;

    SetInfo tranInfo1{};
    tranInfo1.m_setValue = Core::Enums::TRANSFORM;
    tranInfo1.m_descriptorSetId = 13;

    SetInfo tranInfo2{};
    tranInfo2.m_setValue = Core::Enums::TRANSFORM;
    tranInfo2.m_descriptorSetId = 14;

    SetInfo tranInfo3{};
    tranInfo3.m_setValue = Core::Enums::TRANSFORM;
    tranInfo3.m_descriptorSetId = 15;

    SetInfo mat0{};
    mat0.m_setValue = Core::Enums::MATERIAL;
    mat0.m_descriptorSetId = 8;
    mat0.m_childrenSet.push_back(&tranInfo0);
    mat0.m_childrenSet.push_back(&tranInfo1);

    SetInfo mat1{};
    mat1.m_setValue = Core::Enums::MATERIAL;
    mat1.m_descriptorSetId = 9;
    mat1.m_childrenSet.push_back(&tranInfo2);

    SetInfo mat2{};
    mat2.m_setValue = Core::Enums::MATERIAL;
    mat2.m_descriptorSetId = 10;
    mat2.m_childrenSet.push_back(&tranInfo3);

    SetInfo l1{};
    l1.m_setValue = Core::Enums::LIGHT;
    l1.m_descriptorSetId = 3;
    l1.m_childrenSet.push_back(&mat0);
    l1.m_childrenSet.push_back(&mat1);

    SetInfo l2{};
    l2.m_setValue = Core::Enums::LIGHT;
    l2.m_descriptorSetId = 5;
    l2.m_childrenSet.push_back(&mat2);

    SetInfo camInfo{};
    camInfo.m_setValue = Core::Enums::CAMERA;
    camInfo.m_descriptorSetId = 1;
    camInfo.m_childrenSet.push_back(&l1);
    camInfo.m_childrenSet.push_back(&l2);

    CreateSetList(&camInfo);
    */


void Renderer::RenderGraph::CreateSetInfo(std::map<Core::Enums::ResourceSets,
    std::map<uint32_t, SetInfo>>& setInfoMap, RenderGraph::Tasks::DrawInfo& drawInfo)
{
    auto ConnectSets = [&setInfoMap]()
    {
        for (auto it = setInfoMap.begin(); it != setInfoMap.end(); ++it) {
            auto& parentMap = it->second;
            auto next = std::next(it);
            if (next == setInfoMap.end())
                break;
            auto& childMap = next->second;

            // Compare parent map with child map
            // the ChildIndicies in parent will map into the map containing the children setInfo

            for (auto& parent : parentMap)
            {
                // link it to specific child index
                if (parent.second.m_indiciesCount > 0)
                {
                    auto begin = parent.second.m_childIndicies;
                    for (uint32_t i = 0; i < parent.second.m_indiciesCount; i++)
                    {
                        auto test = &childMap[*begin];
                        parent.second.m_childrenSet.push_back(&childMap[*begin]);
                        begin++;
                    }
                }
                else // link to all children
                {
                    for (auto& child : childMap)
                    {
                        // parent->setInfo contain child->setInfo pointer
                        parent.second.m_childrenSet.push_back(&child.second);
                    }
                }
            }

        }
    };

    ConnectSets();

    std::function<void(const SetInfo* node)> CreateSetList;

    auto begin = setInfoMap.begin();
    auto end = setInfoMap.rbegin();

    Core::Enums::ResourceSets minSet = begin->first;
    Core::Enums::ResourceSets maxSet = end->first;

    uint32_t bindCount = 0, firstSet = maxSet + 1;
    std::vector<int> idList;
    std::vector<int> setIdList;
    Core::Enums::ResourceSets prevSet{ minSet };
    uint32_t index = 0;
    CreateSetList = [&CreateSetList, &setIdList, &prevSet, &bindCount, &firstSet, &drawInfo, &index]
    (const SetInfo* node)
    {
        if ((uint32_t)node->m_setValue > (uint32_t)prevSet)
        {
            auto count = node->m_setValue - prevSet - 1;
            if (count > 0)
                for (int i = 0; i < count; i++)
                {
                    //std::cout << "id : -1 \n";
                    setIdList.push_back(-1);
                    bindCount++;
                    firstSet--;
                }
        }
        bindCount++;
        firstSet--;

        auto& descriptorId = node->m_descriptorSetId;
        if (descriptorId.has_value())
        {
            //std::cout << "id : " << descriptorId.value() << "\n";
            setIdList.push_back(descriptorId.value());
        }

        prevSet = node->m_setValue;

        if (node->m_childrenSet.size() == 0)
        {
            /*std::cout << "BindCount : " << bindCount << " firstSet : " <<firstSet << "\n";
            for (auto& id : setIdList)
                std::cout << id << "  ";
            std::cout << "\n";*/

            Core::Wrappers::DescriptorSetBindingInfo info{};
            info.descriptorSetIds = setIdList;
            info.firstSet = firstSet;
            info.dynamicOffsetCount = 0;
            info.numSetsToBind = bindCount;
            info.pDynamicOffsets = nullptr;
            info.pipelineBindPoint = Core::Enums::PipelineType::GRAPHICS;
            drawInfo.m_meshInfoList[index++].m_descriptorInfo = info;

            return;
        }

        for (auto& info : node->m_childrenSet)
        {
            CreateSetList(info);

            bindCount = 0;
            firstSet = Core::Enums::ResourceSets::TRANSFORM + 1;
            setIdList.erase(setIdList.end() - 1);
        }
    };

    CreateSetList(&setInfoMap[Core::Enums::CAMERA][0]);
}

void Renderer::RenderGraph::Filter(const FilterInfo& info,
    const std::function<bool(uint32_t index, void* data, void* next)>& filterFunc,
    std::map<Core::Enums::ResourceSets,
    std::vector<std::pair<uint32_t, void*>>>& filteredDataList,
    std::map<Core::Enums::ResourceSets, std::map<uint32_t, SetInfo>>& setMap)
{
    // pointer test
    /*void* data = (void*)m_renderData.m_transformData.data();
    data = static_cast<char*>(data) + sizeof(Core::Utility::TransformData);
    auto test = static_cast<Core::Utility::TransformData*>(data);*/

    void* dataPtr = info.m_data;

    for (uint32_t i = 0; i < info.m_dataCount; i++)
    {
        if (filterFunc(i, dataPtr, info.m_next))
        {
            auto pair = std::pair(i, dataPtr);
            bool insert = false;
            if (filteredDataList.find(info.m_setType) == filteredDataList.end())
            {
                filteredDataList.insert({ info.m_setType, {{pair}} });
                insert = true;
            }
            else
            {
                filteredDataList[info.m_setType].push_back(pair);
                insert = false;
            }

            SetInfo setInfo{};
            setInfo.m_setValue = info.m_setType;
            switch (info.m_setType)
            {
            case Core::Enums::CAMERA:
            {
                auto cam = static_cast<Core::Utility::CameraData*>(dataPtr);
                setInfo.m_descriptorSetId = cam->m_descriptorSetId;
                if (cam->childSetIndicies.size() > 0)
                {
                    setInfo.m_childIndicies = cam->childSetIndicies.data();
                    setInfo.m_indiciesCount = cam->childSetIndicies.size();
                }
            }
            break;
            case Core::Enums::UNKNOWN:
                //setInfo.m_descriptorSetId = static_cast<Core::Utility::CameraData*>(data)->m_descriptorSetId;
                break;
            case Core::Enums::LIGHT:
                //setInfo.m_descriptorSetId = static_cast<Core::Utility::CameraData*>(data)->m_descriptorSetId;
                break;
            case Core::Enums::MATERIAL:
            {
                auto mat = static_cast<Core::Utility::MaterialData*>(dataPtr);
                if (mat->m_descriptorSetId.has_value())
                {
                    setInfo.m_descriptorSetId = mat->m_descriptorSetId.value();
                    if (mat->m_childSetIndicies.size() > 0)
                    {
                        setInfo.m_childIndicies = mat->m_childSetIndicies.data();
                        setInfo.m_indiciesCount = mat->m_childSetIndicies.size();
                    }
                }
            }
            break;
            case Core::Enums::TRANSFORM:
                setInfo.m_descriptorSetId = static_cast<Core::Utility::TransformData*>(dataPtr)->m_descriptorSetId;
                break;
            }

            if (setInfo.m_descriptorSetId.has_value())
            {
                auto map = std::map<uint32_t, Renderer::RenderGraph::SetInfo>({ {i, setInfo} });
                if (insert)
                {
                    setMap.insert({ info.m_setType, map });
                }
                else
                {
                    setMap[info.m_setType].insert({ i, setInfo });
                }
            }
        }

        dataPtr = static_cast<char*>(dataPtr) + info.m_stride;
    }
}

bool Renderer::RenderGraph::CameraFilter(uint32_t index, void* data, void* next)
{
    if (index == 0)
        return true;
    return false;
}

bool Renderer::RenderGraph::MaterialFilter(uint32_t index, void* data, void* next)
{
    if (next == nullptr)
    {
        ASSERT_MSG_DEBUG(0, "NuLL");
    }

    auto effectInfo = static_cast<Core::Utility::EffectInfo*>(next);
    auto matData = static_cast<Core::Utility::MaterialData*>(data);
    if (matData->m_effectId == effectInfo->GetEffectId() &&
        matData->m_techniqueId == effectInfo->GetTechniqueId())
    {
        return true;
    }
    return false;
}

void Renderer::RenderGraph::CreateResourceNode(const std::string& nodeName,
    const std::vector<ResourceAlias*>& imageList,
    std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*>& resourceNodes,
    Renderer::RenderGraph::Graph<Renderer::RenderGraph::Utils::RenderGraphNodeBase>& graph,
    Renderer::RenderGraph::Utils::GraphTraversalCallback& graphTraversal,
    GraphNodeWrapper& resourceNode)
{
    resourceNode.m_nodeBase = std::make_unique<Renderer::RenderGraph::ResourceNode>(imageList, nodeName, Renderer::ResourceManagement::ResourceType::IMAGE, graphTraversal);
    resourceNode.m_graphNode = graph.Push(resourceNode.m_nodeBase.get());
    resourceNodes.push_back(resourceNode.m_graphNode);
}

void Renderer::RenderGraph::CreateTaskGraphNode(const std::string& taskName,
    const std::string& parentEffectName, const std::string& techName,
    const Core::Wrappers::Rect2D& renderArea,
    Renderer::RenderGraph::Graph<Renderer::RenderGraph::Utils::RenderGraphNodeBase>& graph,
    Renderer::RenderGraph::Utils::GraphTraversalCallback& graphTraversal,
    std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*>& taskNodes,
    GraphNodeWrapper& taskNode)
{
    auto m_opaqueRenderTask = std::make_unique<Renderer::RenderGraph::Tasks::RenderTask>(taskName, renderArea, parentEffectName, techName);
    taskNode.m_nodeBase = std::make_unique<Renderer::RenderGraph::TaskNode>(std::move(m_opaqueRenderTask), graphTraversal);
    taskNode.m_graphNode = graph.Push(taskNode.m_nodeBase.get());
    taskNodes.push_back(taskNode.m_graphNode);
}

std::string Renderer::RenderGraph::Technique::GetNodeName(const std::string& name)
{
    return name + "_" + m_name;
}

#include "renderGraph/Pipeline.h"
#include <vector>

static std::vector<Renderer::RenderGraph::Utils::RenderGraphNodeBase*> singlePath;

namespace
{
    /// <summary>
    /// The tasks are collected from all the existing paths with last node as the destination.
    /// These tasks are then segregated into levels. Levels can have dependency between the tasks.
    /// </summary>
    void LevelSegregation(std::vector<Renderer::RenderGraph::PathsPerOrigin>& pathList, std::map<uint32_t, Renderer::RenderGraph::PerLevelTaskInfo>& perLevelTaskInfo)
    {
        for (auto& origin : pathList)
        {
            for (auto& path : origin.m_paths)
            {
                uint32_t levelCounter = 0;
                for (auto taskNode : path)
                {
                    auto it = perLevelTaskInfo.find(levelCounter);
                    if (it != perLevelTaskInfo.end())
                    {
                        auto& taskList = perLevelTaskInfo[levelCounter].m_taskList;

                        // check if the task already exists in the task list
                        auto itt = std::find_if(taskList.begin(), taskList.end(),
                            [&taskNode](Renderer::RenderGraph::Utils::RenderGraphNodeBase* nodeBase)
                        {
                            return nodeBase->GetId() == taskNode->GetId();
                        });

                        if (itt == taskList.end())
                        {
                            auto task = ((Renderer::RenderGraph::TaskNode*)taskNode)->GetTask();
                            // If common resource found with the existing items in the list.
                            // Arrange the order of insertion based on the resource usage
                            // R > RW > W
                            uint32_t taskCounter = 0;
                            std::vector<uint32_t> insertPositions;
                            std::vector<Renderer::RenderGraph::Utils::ResourceMemoryUsage> existingUsages;
                            Renderer::RenderGraph::Utils::ResourceMemoryUsage usage = Renderer::RenderGraph::Utils::ResourceMemoryUsage::NONE;;

                            for (auto existingTaskNode : taskList)
                            {
                                auto existingTask = ((Renderer::RenderGraph::TaskNode*)existingTaskNode)->GetTask();
                                auto existingInputs = existingTask->GetInputs();

                                // Find the common resource in both tasks
                                bool inputClash = false;
                                for (auto& input : existingInputs)
                                {
                                    auto& targetTaskInputs = task->GetInputs();
                                    auto ittt = std::find_if(targetTaskInputs.begin(), targetTaskInputs.end(), [&input]
                                    (const Renderer::RenderGraph::TaskInputInfo& info)
                                    {
                                        return info.m_resourceParentNodeId == input.m_resourceParentNodeId;
                                    });

                                    // figure out the position in the tasklist where the task can be inserted
                                    if (ittt != targetTaskInputs.end())
                                    {
                                        ASSERT_MSG_DEBUG(!inputClash, "multiple input clash not handled");

                                        inputClash = true;
                                        insertPositions.push_back(taskCounter);
                                        existingUsages.push_back(input.m_usage);
                                        usage = ittt->m_usage;
                                    }
                                }
                                taskCounter++;
                            }

                            // iterate the usages and insert positions
                            if (insertPositions.size() == 0)
                            {
                                taskList.push_back(taskNode);
                            }
                            else
                            {
                                for (uint32_t i = 0; i < insertPositions.size(); i++)
                                {
                                    switch(existingUsages[i])
                                    {
                                    case Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_ONLY:
                                        {

                                            auto it = std::next(taskList.begin(), insertPositions[i] + 1);
                                            taskList.insert(it, { taskNode });

                                            /*perLevelTaskInfo[levelCounter].m_localDependency.push_back(
                                                std::make_pair(insertPositions[i], insertPositions[i] + 1));*/

                                            perLevelTaskInfo[levelCounter].m_dependency.push_back(
                                                std::make_pair(
                                                    *(std::next(
                                                        taskList.begin(),
                                                        insertPositions[i])),
                                                    *(std::next(
                                                        taskList.begin(),
                                                        insertPositions[i] + 1)))
                                            );

                                        }
                                        break;
                                    case Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_WRITE:
                                        if (usage == Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_ONLY)
                                        {
                                            auto it = std::next(taskList.begin(), insertPositions[i]);
                                            taskList.insert(it, { taskNode });
                                        }
                                        else if (usage == Renderer::RenderGraph::Utils::ResourceMemoryUsage::WRITE_ONLY ||
                                            usage == Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_WRITE)
                                        {
                                            auto it = std::next(taskList.begin(), insertPositions[i] + 1);
                                            taskList.insert(it, { taskNode });
                                        }
                                        /*perLevelTaskInfo[levelCounter].m_localDependency.push_back(
                                            std::make_pair(insertPositions[i], insertPositions[i] + 1));*/

                                        perLevelTaskInfo[levelCounter].m_dependency.push_back(
                                            std::make_pair(
                                                *(std::next(
                                                    taskList.begin(),
                                                    insertPositions[i])),
                                                *(std::next(
                                                    taskList.begin(),
                                                    insertPositions[i] + 1)))
                                        );
                                        break;
                                    case Renderer::RenderGraph::Utils::ResourceMemoryUsage::WRITE_ONLY:
                                        if (usage == Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_ONLY)
                                        {
                                            auto it = std::next(taskList.begin(), insertPositions[i]);
                                            taskList.insert(it, { taskNode });
                                        }
                                        /*perLevelTaskInfo[levelCounter].m_localDependency.push_back(
                                            std::make_pair(insertPositions[i], insertPositions[i] + 1));*/

                                        perLevelTaskInfo[levelCounter].m_dependency.push_back(
                                            std::make_pair(
                                                *(std::next(
                                                    taskList.begin(),
                                                    insertPositions[i])),
                                                *(std::next(
                                                    taskList.begin(),
                                                    insertPositions[i] + 1)))
                                        );
                                        break;
                                    default:
                                        ASSERT_MSG_DEBUG(0, "Invalid case");
                                    }
                                }
                            }
                        }//(itt == taskList.end())
                    }
                    else
                    {
                        Renderer::RenderGraph::PerLevelTaskInfo info = {};
                        info.m_level = levelCounter;
                        info.m_taskList.push_back(taskNode);

                        perLevelTaskInfo.insert({ levelCounter, info});
                    }
                    levelCounter++;
                }
            }
        }
    }

    /// <summary>
    /// Remove the duplicate accurancef of tasks.
    /// </summary>
    /// <param name="perLevelTaskInfo"></param>
    void LevelRefinement(uint32_t totalLevels, std::map<uint32_t, Renderer::RenderGraph::PerLevelTaskInfo>& perLevelTaskInfo)
    {
        // Sort the tasks within the level. 
        // If there is dependency (breadth first) within a level there might be depth first 
        // dependency between tasks.
        //for (auto& level : perLevelTaskInfo)
        //{
        //    for (auto pair : level.second.m_localDependency)
        //    {
        //        uint32_t index = 0;
        //        for (auto taskNode : level.second.m_taskList)
        //        {
        //            auto taskFirst = ((Renderer::RenderGraph::TaskNode*)level.second.m_taskList[pair.first])->GetTask();
        //            auto taskSecond = ((Renderer::RenderGraph::TaskNode*)level.second.m_taskList[pair.first])->GetTask();

        //            if (index != pair.first || index != pair.second)
        //            {

        //            }
        //        }
        //    }
        //}


        std::vector<Renderer::RenderGraph::Utils::RenderGraphNodeBase*> nodeListSorted;
        uint32_t levelCounter = 0;

        //for (auto iter = perLevelTaskInfo.rbegin(); iter != perLevelTaskInfo.rend(); iter++)
        for(int i = totalLevels - 1; i >= 0; i--)
        {
            if (nodeListSorted.empty())
            {
                //for (auto node : iter->second.m_taskList)
                for (auto node : perLevelTaskInfo[i].m_taskList)
                {
                    nodeListSorted.push_back(node);
                }
                levelCounter++;
                //continue;
            }
            else
            {
                // check if the item exists in the nodeListSorted
                // if no : insert the item into sorted list
                // if yes : check for local dependency
                    // if (breadthwise)local dependency then erase the item from sorted list and check for depthwise 
                    // dependency and rearrange.within the level
                    // if no local dependency erase the item from the level task list
                std::vector<uint32_t> eraseIdSorted, eraseId;
                uint32_t eraseCounter = 0;
                for (auto node : perLevelTaskInfo[i].m_taskList)
                {
                    auto it = std::find_if(nodeListSorted.begin(), nodeListSorted.end(),
                        [&node](Renderer::RenderGraph::Utils::RenderGraphNodeBase* sortedNode)
                    {
                        return sortedNode->GetId() == node->GetId();
                    });

                    if (it == nodeListSorted.end())
                    {
                        nodeListSorted.push_back(node);
                    }
                    else
                    {
                        // check for local dependency
                        if (perLevelTaskInfo[i].m_dependency.size() > 0)
                        {
                            for (auto pair : perLevelTaskInfo[i].m_dependency)
                            {
                                auto firstNode = pair.first;// std::next(perLevelTaskInfo[i].m_taskList.begin(), pair.first);
                                auto secondNode = pair.second;// std::next(perLevelTaskInfo[i].m_taskList.begin(), pair.second);

                                // If the local dependency pair contains the current node
                                if (node->GetId() == (firstNode)->GetId() || node->GetId() == (secondNode)->GetId())
                                {
                                    // Erase from the higher levels (reverse order, hence higher)
                                    for (uint32_t j = i + 1; j < perLevelTaskInfo.size(); j++)
                                    {
                                        auto& taskList = perLevelTaskInfo[j].m_taskList;
                                        auto itt = std::find_if(taskList.begin(), taskList.end(),
                                            [&node](Renderer::RenderGraph::Utils::RenderGraphNodeBase* existingNode)
                                        {
                                            return node->GetId() == existingNode->GetId();
                                        });

                                        if (itt != taskList.end())
                                        {
                                            taskList.erase(itt);
                                        }
                                    }

                                    // check for depthwise dependency within the level
                                    /*uint32_t index = 0;
                                    auto firstTask = ((Renderer::RenderGraph::TaskNode*)(firstNode))->GetTask();
                                    auto secondTask = ((Renderer::RenderGraph::TaskNode*)(secondNode))->GetTask();
                                    for (auto taskNode : perLevelTaskInfo[i].m_taskList)
                                    {
                                        if (index != pair.first || index != pair.second)
                                        {
                                            auto inputs = ((Renderer::RenderGraph::TaskNode*)node)->GetTask()->GetInputs();
                                            auto outputs = ((Renderer::RenderGraph::TaskNode*)taskNode)->GetTask()->GetOutputs();

                                            auto result = std::find_first_of(inputs.begin(), inputs.end(), outputs.begin(), outputs.end());
                                            if (result != inputs.end())
                                            {

                                            }
                                        }
                                        index++;
                                    }*/
                                }
                                else // Erase it from sorted list
                                {
                                    eraseId.push_back(eraseCounter);
                                }
                            }
                        }
                        else
                        {
                            eraseId.push_back(eraseCounter);
                        }
                    }
                    eraseCounter++;
                }

                for (auto id : eraseId)
                {
                    auto it = std::next(perLevelTaskInfo[i].m_taskList.begin(), id);
                    std::cout << "erasing : " << (*it)->GetNodeName();
                    perLevelTaskInfo[i].m_taskList.erase(it);
                }
            }
            levelCounter++;
        }
    }
}

void Renderer::RenderGraph::Pipeline::PopulatePath(Renderer::RenderGraph::Utils::RenderGraphNodeBase* node)
{
    //std::cout<<node->GetNodeName() << " ";
    //1. Add the tasks to the array
    auto index = m_pathPerOrigin.size() - 1;
    if (node->GetNodeName() == m_currentOrigin->GetNodeName())
    {
        if (m_pathPerOrigin[index].m_origin == nullptr)
            m_pathPerOrigin[index].m_origin = m_currentOrigin;
        else
        {
            m_pathPerOrigin[index].m_paths.push_back(singlePath);
            singlePath.clear();
        }
    }

    if (node->GetNodeType() == Renderer::RenderGraph::Utils::RenderGraphNodeType::TASK_NODE)
    {
        singlePath.push_back(node);
    }
}

void Renderer::RenderGraph::Pipeline::CompilePipeline()
{
    auto ends = m_graph->GetGraphEnds();
    auto origins = m_graph->GetGraphOrigins();

    m_currentOrigin = origins[0]->GetNodeData();

    for (auto origin : origins)
    {
        m_currentOrigin = origin->GetNodeData();
        m_pathPerOrigin.push_back(Renderer::RenderGraph::PathsPerOrigin{});
        m_graph->FindAllPaths(origin->GetNodeId(), ends[0]->GetNodeId());
        m_pathPerOrigin[m_pathPerOrigin.size() - 1].m_paths.push_back(singlePath);
        if (singlePath.size() > m_passLevelCount)
            m_passLevelCount = singlePath.size();
        singlePath.clear();
    }

    auto print = [&]()
    {
        std::cout << "\ninfo";
        for (auto& perLevelInfo : m_perLevelTaskInfo)
        {
            std::cout << "\n";
            for (auto task : perLevelInfo.second.m_taskList)
            {
                std::cout << task->GetNodeName() << " ";
            }

            if (!perLevelInfo.second.m_dependency.empty())
            {
                for (auto pair : perLevelInfo.second.m_dependency)
                {
                    std::cout << "dependency : " << pair.first->GetNodeName() << " ";
                    std::cout << pair.second->GetNodeName();
                }
            }
        }
    };

    //2. Level wise segregation
    LevelSegregation(m_pathPerOrigin, m_perLevelTaskInfo);
    print();

    //3. Go through the levels and refine the dependency
    LevelRefinement(m_passLevelCount, m_perLevelTaskInfo);
    print();
}

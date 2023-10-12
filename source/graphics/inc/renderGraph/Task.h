#ifndef RENDERER_NODE_H_
#define RENDERER_NODE_H_

#include <optional>
#include <functional>
#include <CorePrecompiled.h>
#include <string>
#include <Utility/RenderingWrappers/RenderingWrapper.h>

namespace Renderer
{
    namespace RenderGraph
    {
        enum class RenderGraphNodeType
        {
            TASK_NODE,
            RESOURCE_NODE
        };

        enum class ResourceType
        {
            BUFFER,
            IMAGE
        };

        enum class ResourceMemoryUsage
        {
            READ_ONLY,
            READ_WRITE,
            WRITE_ONLY
        };

        enum class TaskType
        {
            RENDER_TASK,
            COMPUTE_TASK,
            BLIT_TASK,
            DOWNLOAD_TASK
        };

        class RenderGraphNodeBase
        {
        protected:
            RenderGraphNodeType m_type;
            const std::string m_name;

        public:
            RenderGraphNodeBase(const RenderGraphNodeType& nodeType, const std::string& nodeName) :
                m_type(nodeType), m_name(nodeName)
            {}

            const std::string& GetNodeName()
            {
                return m_name;
            }

            virtual void Execute() = 0;
        };


        class Resource
        {
        protected:
            /// <summary>
            /// The resource might be represented by multiple nodes in the graph, hence the logical id 
            /// is assigned to identify the uniqueness of the resource thorughtout the graph.
            /// The physicalId will be assigned in later stages.
            /// </summary>
            uint32_t m_logicalResourceId;

            /// <summary>
            /// Will point to the TextureId or BufferId
            /// </summary>
            std::optional<uint32_t> m_physicalResourceId;
            ResourceType m_resourceType;
            std::string m_name;

        public:
            Resource() = delete;
            Resource(const ResourceType& resourceType, const std::string& name) :
                m_resourceType(resourceType), m_name(name)
            {}

            void AssignPhysicalResourceId(const uint32_t id)
            {
                m_physicalResourceId = id;
            }

        };

        class ImageResource final : public Resource
        {
        private:
            Core::Enums::ImageLayout m_currentLayout, m_targetLayout;
            size_t m_width, m_height;

        public:
            ImageResource(const size_t& width, const size_t& height, const std::string& name) :
                Resource(ResourceType::IMAGE, name), m_width(width), m_height(height)
            {}
        };

        class BufferResource final : public Resource
        {
        private:
            size_t m_dataSize, m_dataSizeAligned;

        public:
        };

        /// <summary>
        /// Any cmd can be encapsulated into a task node. The standard types will be RenderTask, ComputeTask, DownloadTask
        /// and BlitTask
        /// </summary>
        class Task
        {
        private:
        protected:
            std::vector<Resource*> m_inputs, m_outputs;
            std::string m_name;
            TaskType m_taskType;
        public:
            //virtual const uint32_t& GetId() = 0;
            Task() = delete;
            Task(const std::string& name, const TaskType& taskType) :
                m_name(name), m_taskType(taskType)
            {}

            std::string GetTaskName()
            {
                return m_name;
            }
            void AddInput(Resource* input)
            {
                m_inputs.push_back(input);
            }
            void AddOutput(Resource* output)
            {
                m_outputs.push_back(output);
            }
            const std::vector<Resource*>& GetInputs()
            {
                return m_inputs;
            }
            const std::vector<Resource*>& GetOutputs()
            {
                return m_outputs;
            }
        };

        class TaskNode : public RenderGraphNodeBase
        {
        protected:
            std::unique_ptr<Task> m_task;
        public:
            TaskNode(std::unique_ptr<Task> task) :
                RenderGraphNodeBase(RenderGraphNodeType::TASK_NODE, task->GetTaskName()), m_task(std::move(task))
            {}

            virtual void Execute() override
            {
                //m_task->
            }
        };

        class ResourceNode : public RenderGraphNodeBase
        {
        protected:
            ResourceType m_resourceType;
            std::map<uint32_t, ResourceMemoryUsage> sourceTaskIdToUsageMap, destTaskIdToUsageMap;
            Resource* m_resource;
        public:
            ResourceNode(Resource* resource, const std::string& nodeName, const ResourceType& resourceType) :
                RenderGraphNodeBase(RenderGraphNodeType::RESOURCE_NODE, nodeName), m_resourceType(resourceType), m_resource(resource)
            {}

            void RegisterAsTaskInput(const uint32_t taskId, const ResourceMemoryUsage& usage)
            {
                if(sourceTaskIdToUsageMap.find(taskId) != sourceTaskIdToUsageMap.end())
                {
                    ASSERT_MSG_DEBUG(0, "duplicate edge.");
                }
                sourceTaskIdToUsageMap.insert({ taskId, usage });
            }

            void RegisterAsTaskOutput(const uint32_t taskId, const ResourceMemoryUsage& usage)
            {
                if (destTaskIdToUsageMap.find(taskId) != destTaskIdToUsageMap.end())
                {
                    ASSERT_MSG_DEBUG(0, "duplicate edge.");
                }
                destTaskIdToUsageMap.insert({ taskId, usage });
            }

            virtual void Execute() override
            {

            }
        };

    }
}

#endif //RENDERER_NODE_H_
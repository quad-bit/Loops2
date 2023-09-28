#pragma once
#include <ECS/System.h>
#include <glm/glm.hpp>
#include "Utility/ShaderResourceDescription.h"
#include "RenderData.h"
//#include "DrawGraphNode.h"
//#include "MeshAdditionEvent.h"
#include <map>
#include <tuple>

template <typename T>
class GraphNode;


namespace Core
{
    namespace ECS
    {
        template <typename T>
        class ComponentHandle;

        namespace Components
        {
            class Camera;
            enum class Camera_Movement;
        }

        namespace Events
        {
            class CameraAdditionEvent;
        }
    }
}

namespace Engine
{
    namespace ECS
    {
        namespace Systems
        {
            class CameraSystem : public Core::ECS::System
            {
            private:
                // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
                void ProcessKeyboard(Core::ECS::Components::Camera* cam, glm::vec3* camTransformPos, Core::ECS::Components::Camera_Movement* direction, float deltaTime);

                // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
                void ProcessMouseMovement(Core::ECS::Components::Camera* cam, float xOffset, float yOffset, bool constrainPitch = true);

                // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
                void ProcessMouseScroll(float yOffset) {}

                // Calculates the front vector from the Camera's (updated) Eular Angles
                void UpdateCameraVectors(Core::ECS::Components::Camera* cam);

                std::vector<Core::ECS::Components::Camera*> cameraList;
                //std::vector<Core::Utility::ShaderBindingDescription*> resDescriptionList;
                std::vector<Core::Utility::DescriptorSetInfo> resDescriptionList;
                Core::Utility::GlobalResourceAllocationConfig allocConfig;
                Core::Utility::GlobalResourceSharingConfig resourceSharingConfig;

                uint32_t idCounter = 0;
                uint32_t GeneratedCamId();
                //size_t GetDataSizeMeantForSharing();
                //std::vector<size_t> CalculateOffsetsForDescInUniform(size_t dataSizePerDescriptor);
                //bool IsNewAllocationRequired();

                Core::Wrappers::SetWrapper* cameraSetWrapper;

                //std::vector<GraphNode<DrawGraphNode> *> cameraGraphNodeList;
                //std::map<DrawGraphNode *, ShaderBindingDescription *> nodeToDescriptionMap;
                std::map<Core::ECS::Components::Camera *, Core::Utility::DescriptorSetInfo> camToDescriptionMap;
                size_t memoryAlignedUniformSize;

                uint32_t numDescriptorSetsPerUniform;
                uint32_t mainCameraId = 0;

                std::vector<Core::Utility::CameraData>& m_cameraDataList;

            public:
                virtual void Init() override;
                virtual void DeInit() override;
                virtual void Update(float dt) override;

                void HandleCameraAddition(Core::ECS::Events::CameraAdditionEvent* inputEvent);
                //GraphNode<DrawGraphNode> * HandleCameraAddition(Camera * camera, const RenderPassTag & tag);
                //void HandleMeshAddition(Core::ECS::Events::MeshToMatAdditionEvent *  meshAdditionEvent);

                Core::Utility::GlobalResourceAllocationConfig GetAllocConfig() const;
                Core::Utility::GlobalResourceSharingConfig GetResourceSharingConfig() const;

                CameraSystem(std::vector<Core::Utility::CameraData>& cameraData);
                virtual ~CameraSystem();

                void SetMainCamera(uint32_t componentId);
            };

        }
    }
}

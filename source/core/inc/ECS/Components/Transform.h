#include "ECS/Component.h"
#include "ECS/SceneNode.h"

#include "Math/MathUtil.h"

namespace Core
{
    namespace ECS
    {
        class Entity;
        namespace Components
        {
            struct TransformUniform
            {
                glm::mat4 modelMat;
            };

            class Transform : public Component<Transform>, public Core::ECS::SceneNode
            {
            private:
                Transform* parent;
                std::vector<Transform*> childrenList;

                Entity* owner;
                std::string* entityName;

                void Init();
                Transform();

                glm::vec3 localPosition, globalPosition;
                glm::vec3 localScale, globalScale;
                glm::vec3 localEulerAngle, globalEulerAngle;

                glm::mat4 localModelMatrix;
                glm::mat4 globalModelMatrix;
                glm::mat4 translationMat, scaleMat, rotationMat;

                glm::vec3 forward, right, up;

            public:

                bool isTransformUpdated;

                Transform(Core::ECS::Entity* entity);

                Transform* GetParent();
                void SetParent(Transform* transform);
                void UpdateGlobalParams();
                void UpdateLocalParams();
                const std::vector<Transform*>& GetChildren();

                glm::vec3 GetForward();
                glm::vec3 GetUp();
                glm::vec3 GetRight();

                glm::vec3 GetLocalPosition();
                glm::vec3 GetLocalEulerAngles();
                glm::vec3 GetLocalScale();
                glm::mat4 GetLocalModelMatrix();

                glm::vec3 GetGlobalPosition();
                glm::vec3 GetGlobalEulerAngles();
                glm::vec3 GetGlobalScale();
                glm::mat4 GetGlobalModelMatrix();

                glm::vec3* GetLocalPositionRef() { return &localPosition; }
                glm::vec3* GetLocalEulerAnglesRef() { return &localEulerAngle; }

                void SetLocalPosition(const glm::vec3& pos);
                void SetLocalEulerAngles(const glm::vec3& angle);
                void SetLocalScale(const glm::vec3& scale);
                void SetLocalModelMatrix(const glm::mat4& mat);

                void SetGlobalPosition(const glm::vec3& pos);
                void SetGlobalEulerAngles(const glm::vec3& angle);
                void SetGlobalScale(const glm::vec3& scale);
                void SetGlobalModelMatrix(const glm::mat4& mat);

                virtual void Entry() override;
                virtual void Exit() override;

                ~Transform()
                {
                }
            };
        }
    }
}
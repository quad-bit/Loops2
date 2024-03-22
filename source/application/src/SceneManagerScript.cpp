#include "SceneManagerScript.h"
#include "CameraController.h"
#include "PlayerHandlerScript.h"
#include <ECS/Components/Camera.h>
#include <glm/glm.hpp>
#include <ECS/EntityHandle.h>
#include <ECS/Components/Transform.h>
#include <ECS/Components/Light.h>
#include <ECS/Components/Bounds.h>
#include <bitset>
#include <resourceManagement/MeshFactory.h>
#include <resourceManagement/MaterialFactory.h>
#include <resourceManagement/BoundFactory.h>
#include <random>
#include "Utility/Timer.h"
#include <Utility/RenderingWrappers/AttributeHelper.h>

#define LIGHT_SPHERE_VISUAL 0

SceneManagerScript::SceneManagerScript() : Core::ECS::Components::Scriptable(false)
{
    scriptName = typeid(this).raw_name();

    camHandle0 = worldObj->CreateEntity();
    camHandle0->GetEntity()->entityName = "MainCamera";
    camHandle0->GetTransform()->SetLocalPosition(glm::vec3(0, 30, 70));
    camHandle0->GetTransform()->SetLocalEulerAngles(glm::vec3(glm::radians(-10.0f), 0, 0));

    //camHandle0->GetTransform()->SetLocalPosition(glm::vec3(0, 0.5, 1.5));
    //camHandle0->GetTransform()->SetLocalEulerAngles(glm::vec3(glm::radians(-27.0f), 0, 0));

    Core::ECS::Components::Camera * camera = new Core::ECS::Components::Camera(
        camHandle0->GetTransform(), 1280.0f/720.0f);
    camHandle0->AddComponent<Core::ECS::Components::Camera>(camera);

    worldObj->SetMainCamera(camera->componentId);

    cameraController = new CameraController();
    camHandle0->AddComponent<Core::ECS::Components::Scriptable>(cameraController);

    playerObject = worldObj->CreateEntity("playerObject");
    ASSERT_MSG_DEBUG(playerObject != nullptr, "Object not found");

    auto seed = 15;// Timer::GetInstance()->GetSeconds();
    srand( seed );

    uint32_t radius = 10;

    float xStart = -10.0f;
    float yStart = 5.0f;

    for (uint32_t i = 0; i < NUM_LIGHTS; i++)
    {
        int theta = rand() % (uint32_t)((360.0f));
        int phi = rand() % (uint32_t)((180.0f));
        float x = 30 * glm::sin(glm::radians((float)phi)) * glm::cos(glm::radians((float)theta));
        float y = radius * glm::sin(glm::radians((float)phi)) * glm::sin(glm::radians((float)theta));
        float z = radius * glm::cos(glm::radians((float)phi));

        auto& lightEntity = lightHandles[i];
        lightEntity = worldObj->CreateEntity("light_" + std::to_string(i));
        Core::ECS::ComponentHandle<Core::ECS::Components::Transform> lightTrfHandle = lightEntity->GetComponent<Core::ECS::Components::Transform>();
        //auto lighPosition = glm::vec3(x + 10 * (i % 2 == 0 ? -1 : 1), 10, z + (i % 2 == 0 ? -1.0f : 1.0f) * i * 5);
        auto lighPosition = glm::vec3(-15, 10, z + (i%2 == 0 ? -1.0f : 1.0f) * i * 5);
        lightTrfHandle->SetLocalPosition(lighPosition);

        std::unique_ptr<Core::ECS::Components::LightCategory> category(new Core::ECS::Components::PointLight(radius));
        lightComponents[i] = new Core::ECS::Components::Light(category);
        lightEntity->AddComponent<Core::ECS::Components::Light>(lightComponents[i]);

        // Bounds
#if LIGHT_SPHERE_VISUAL
        std::unique_ptr<Core::ECS::Components::BoundCategory> boundCategory(new Core::ECS::Components::SphereBound(
            lightTrfHandle->GetGlobalModelMatrix(), lightEntity->GetEntity()->entityName, lightTrfHandle->GetGlobalPosition(), radius));
#else
        std::unique_ptr<Core::ECS::Components::BoundCategory> boundCategory(new Core::ECS::Components::AABB(
            lightTrfHandle->GetGlobalModelMatrix(), lightEntity->GetEntity()->entityName, radius));
#endif

        Core::ECS::Components::Bound* bound = new Core::ECS::Components::Bound(boundCategory);
        Renderer::ResourceManagement::BoundFactory::GetInstance()->AddBound(bound);
        lightEntity->AddComponent<Core::ECS::Components::Bound>(bound);
    }
}

SceneManagerScript::~SceneManagerScript()
{
    for (uint32_t i = 0; i < NUM_LIGHTS; i++)
    {
        lightHandles[i]->RemoveComponent<Core::ECS::Components::Light>(lightComponents[i]);
        delete lightComponents[i];

        auto bound = lightHandles[i]->GetComponent<Core::ECS::Components::Bound>();
        auto ptr = bound.GetComponent();
        Renderer::ResourceManagement::BoundFactory::GetInstance()->DestroyBound(ptr);
        bound.DestroyComponent();
        delete ptr;

        worldObj->DestroyEntity(lightHandles[i]);
    }

    delete cameraController;
    worldObj->DestroyEntity(camHandle0);
}

void SceneManagerScript::Init()
{
}

static float counter = 60.0f;
static float factor = .5f;
void SceneManagerScript::Update(float dt)
{
    float upperLimit = 120.0f;
    float lowerLimit = 60.0f;

    if (counter >= upperLimit)
    {
        factor = -1.0f;
    }
    else if (counter <= lowerLimit)
    {
        factor = 1.0f;
    }
    
    {
        counter += factor/10.f ;
    }

    float theta = glm::radians((float)counter);

    auto obj = worldObj->FindEntity("Knight_B1");
    ASSERT_MSG_DEBUG(obj != nullptr, "object not found");

    auto transform = obj->GetTransform();
    transform->SetLocalPosition(glm::vec3(-15.0f, 5.0f, 22.0f));

    float radius = glm::distance(glm::vec3(0, 0, 0), transform->GetGlobalPosition());
    float x = radius * glm::cos(theta);
    float y = radius * glm::sin(theta);

    //PLOGD << x << " " << y;

    {
        float angle = Core::Math::lerp(prevAngle, currentAngle, dt);
        prevAngle = currentAngle;

        //transform->SetLocalPosition(glm::vec3(x, y, transform->GetLocalPosition().z));
        transform->SetLocalEulerAngles(glm::vec3(transform->GetLocalEulerAngles().x, theta * 2.0, transform->GetLocalEulerAngles().z));
    }

    for (uint32_t i = 0; i < NUM_LIGHTS; i++)
    {
        auto& lightEntity = lightHandles[i];
        Core::ECS::ComponentHandle<Core::ECS::Components::Transform> lightTrfHandle = lightEntity->GetComponent<Core::ECS::Components::Transform>();
        float yPos = lightTrfHandle->GetLocalPosition().y + 0.05f * factor;
        float xPos = lightTrfHandle->GetLocalPosition().x + 0.02f * factor;
        float zPos = lightTrfHandle->GetLocalPosition().z + 0.03f * factor;
        lightTrfHandle->SetLocalPosition(glm::vec3((i%2 == 0 ? xPos : lightTrfHandle->GetLocalPosition().x), (i%3 == 0 ? yPos : lightTrfHandle->GetLocalPosition().y), (i == 1 ? zPos : lightTrfHandle->GetLocalPosition().z )));
    }

}

void SceneManagerScript::Render(float dt)
{

}

void SceneManagerScript::DeInit()
{
}

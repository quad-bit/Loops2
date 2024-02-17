#include "SceneManagerScript.h"
#include "CameraController.h"
#include "PlayerHandlerScript.h"
#include <ECS/Components/Camera.h>
#include <glm/glm.hpp>
#include <ECS/EntityHandle.h>
#include <ECS/Components/Transform.h>
#include <ECS/Components/Light.h>
#include <bitset>
//#include <AttributeHelper.h>
#include <resourceManagement/MeshFactory.h>
#include <resourceManagement/MaterialFactory.h>
//#include <MaterialFactory.h>
#include <random>
#include "Utility/Timer.h"
#include <Utility/RenderingWrappers/AttributeHelper.h>

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

    auto seed = 4;// Timer::GetInstance()->GetSeconds();
    PLOGD << "Seed : " << seed;
    srand( seed );

    uint32_t radius = 20;

    float xStart = -5.0f;
    float yStart = 20.0f;

    for (uint32_t i = 0; i < NUM_LIGHTS; i++)
    {
        auto& lightHandle = lightHandles[i];
        lightHandle = worldObj->CreateEntity("light");
        Core::ECS::ComponentHandle<Core::ECS::Components::Transform> lightTrfHandle = lightHandle->GetComponent<Core::ECS::Components::Transform>();
        lightTrfHandle->SetLocalPosition(glm::vec3(xStart + i * 4, yStart + i * 2, 0));
        //lightTrfHandle->SetLocalPosition(glm::vec3(0, 500, 20));

        std::unique_ptr<Core::ECS::Components::LightCategory> category = std::make_unique< Core::ECS::Components::PointLight>();
        lightComponents[i] = new Core::ECS::Components::Light(category);
        lightHandles[i]->AddComponent<Core::ECS::Components::Light>(lightComponents[i]);
    }
}

SceneManagerScript::~SceneManagerScript()
{
    for (uint32_t i = 0; i < NUM_LIGHTS; i++)
    {
        lightHandles[i]->RemoveComponent<Core::ECS::Components::Light>(lightComponents[i]);
        worldObj->DestroyEntity(lightHandles[i]);
        delete lightComponents[i];
    }

    delete cameraController;
    worldObj->DestroyEntity(camHandle0);
}

void SceneManagerScript::Init()
{
}

static float counter = 60.0f;
static float factor = 1.0f;
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

    //float radius = glm::distance(glm::vec3(0, 0, 0), lightHandle->GetTransform()->GetGlobalPosition());
    //float x = radius * glm::cos(theta);
    //float y = radius * glm::sin(theta);

    ////PLOGD << x << " " << y;

    //{
    //    Core::ECS::Components::Transform * transform = lightHandle->GetTransform();

    //    float angle = Core::Math::lerp(prevAngle, currentAngle, dt);
    //    prevAngle = currentAngle;

    //    transform->SetLocalPosition(glm::vec3(x, y, transform->GetLocalPosition().z));

    //    transform->SetLocalEulerAngles(glm::vec3(theta, 
    //        transform->GetLocalEulerAngles().y, theta * 2.0));
    //}
}

void SceneManagerScript::Render(float dt)
{

}

void SceneManagerScript::DeInit()
{
}

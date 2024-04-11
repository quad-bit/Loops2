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

struct Line
{
    glm::vec3 m_startPos, m_endPos;
    uint32_t m_numDivisions;
    std::vector<glm::vec3> m_positions;
};

struct Sector
{
    std::vector<Line> m_lines;
};

void GetPositions(Sector& sector)
{
    auto GeneratePositions = [](Line& line)
    {
        glm::vec3 direction = glm::normalize(line.m_endPos - line.m_startPos);
        float divisionValue = glm::distance(line.m_endPos, line.m_startPos) / line.m_numDivisions;
        for (uint32_t i = 0; i < line.m_numDivisions; i++)
        {
            glm::vec3 position = line.m_startPos + i * divisionValue * direction;
            line.m_positions.push_back(position);
        }
    };

    for (auto& line : sector.m_lines)
    {
        GeneratePositions(line);
    }
}

std::vector<Sector> CreateSectors()
{
    Sector sector0, sector1, sector2, sector3;
    float sectorYPos = 10.0f;
    float zStart = 150, zEnd = -210;
    float xStart = 600, xEnd = -600;
    //sector0
    {
        {
            Line line;
            line.m_startPos = glm::vec3(xStart, sectorYPos, zEnd);
            line.m_endPos = glm::vec3(xEnd, sectorYPos, zEnd);
            line.m_numDivisions = 25;
            sector0.m_lines.push_back(line);
        }
        {
            Line line;
            line.m_startPos = glm::vec3(xStart, sectorYPos, zStart);
            line.m_endPos = glm::vec3(xEnd, sectorYPos, zStart);
            line.m_numDivisions = 25;
            sector0.m_lines.push_back(line);
        }
        GetPositions(sector0);
    }

    //sector1
    {
        sectorYPos = 100.0f;

        {
            Line line;
            line.m_startPos = glm::vec3(xStart, sectorYPos, zEnd);
            line.m_endPos = glm::vec3(xEnd, sectorYPos, zEnd);
            line.m_numDivisions = 25;
            sector1.m_lines.push_back(line);
        }
        {
            Line line;
            line.m_startPos = glm::vec3(xStart, sectorYPos, zStart);
            line.m_endPos = glm::vec3(xEnd, sectorYPos, zStart);
            line.m_numDivisions = 25;
            sector1.m_lines.push_back(line);
        }
        GetPositions(sector1);
    }

    //sector1
    {
        sectorYPos = 200.0f;

        {
            Line line;
            line.m_startPos = glm::vec3(xStart, sectorYPos, zEnd);
            line.m_endPos = glm::vec3(xEnd, sectorYPos, zEnd);
            line.m_numDivisions = 25;
            sector2.m_lines.push_back(line);
        }
        {
            Line line;
            line.m_startPos = glm::vec3(xStart, sectorYPos, zStart);
            line.m_endPos = glm::vec3(xEnd, sectorYPos, zStart);
            line.m_numDivisions = 25;
            sector2.m_lines.push_back(line);
        }
        GetPositions(sector2);
    }

    //sector3
    {
        sectorYPos = 10.0f;
        {
            {
                Line line;
                line.m_startPos = glm::vec3(xStart, sectorYPos, 0);
                line.m_endPos = glm::vec3(xEnd, sectorYPos, 0);
                line.m_numDivisions = 25;
                sector3.m_lines.push_back(line);
            }
            sectorYPos = 150.0f;
            {
                Line line;
                line.m_startPos = glm::vec3(-1200, 600,0);
                line.m_endPos = glm::vec3(-1200, 0, 0);
                line.m_numDivisions = 25;
                //sector3.m_lines.push_back(line);
            }
        }
        GetPositions(sector3);
    }

    std::vector<Sector> sectors{ sector0, sector1, sector2, sector3 };
    //std::vector<Sector> sectors{ sector3 };
    return sectors;
}

void SceneManagerScript::CreateLights()
{
    auto sectors = CreateSectors();

    float radius = 20;
    uint32_t count = 0;
    for (auto& sector : sectors)
    {
        for (auto& line : sector.m_lines)
        {
            for (uint32_t i = 0; i < line.m_positions.size(); i++)
            {
                auto& lightEntity = lightHandles[count];
                lightEntity = worldObj->CreateEntity("light_" + std::to_string(count));
                Core::ECS::ComponentHandle<Core::ECS::Components::Transform> lightTrfHandle = lightEntity->GetComponent<Core::ECS::Components::Transform>();
                lightTrfHandle->SetLocalPosition(line.m_positions[i]);

                glm::vec4 ambient = glm::vec4(0.05f);
                glm::vec4 diffuse = glm::vec4(.8f);
                glm::vec4 specular = glm::vec4(.5f);
                glm::vec4 color = glm::vec4(0.08f);

                std::unique_ptr<Core::ECS::Components::LightCategory> category(
                    new Core::ECS::Components::PointLight(radius, ambient, diffuse, specular, color));
                lightComponents[count] = new Core::ECS::Components::Light(category);
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

                count++;
            }
        }
    }
}

SceneManagerScript::SceneManagerScript() : Core::ECS::Components::Scriptable(false)
{
    scriptName = typeid(this).raw_name();

    camHandle0 = worldObj->CreateEntity();
    camHandle0->GetEntity()->entityName = "MainCamera";
    //camHandle0->GetTransform()->SetLocalPosition(glm::vec3(3, 1, 0));
    camHandle0->GetTransform()->SetLocalPosition(glm::vec3(300, 100, 0));
    camHandle0->GetTransform()->SetLocalEulerAngles(glm::vec3(glm::radians(0.0f), glm::radians(-90.0f), 0));

    Core::ECS::Components::Camera * camera = new Core::ECS::Components::Camera(
        camHandle0->GetTransform(), 1280.0f/720.0f);
    camHandle0->AddComponent<Core::ECS::Components::Camera>(camera);

    worldObj->SetMainCamera(camera->componentId);

    cameraController = new CameraController();
    camHandle0->AddComponent<Core::ECS::Components::Scriptable>(cameraController);

    playerObject = worldObj->CreateEntity("playerObject");
    ASSERT_MSG_DEBUG(playerObject != nullptr, "Object not found");

    CreateSectors();
    CreateLights();
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

    for (uint32_t i = 0; i < NUM_LIGHTS; i++)
    {
        auto& lightEntity = lightHandles[i];
        Core::ECS::ComponentHandle<Core::ECS::Components::Transform> lightTrfHandle = lightEntity->GetComponent<Core::ECS::Components::Transform>();
        float yPos = lightTrfHandle->GetLocalPosition().y + 0.05f * factor;
        float xPos = lightTrfHandle->GetLocalPosition().x + 0.02f * factor;
        float zPos = lightTrfHandle->GetLocalPosition().z - 0.02f * factor;
        lightTrfHandle->SetLocalPosition(glm::vec3((i%2 == 0 ? xPos : lightTrfHandle->GetLocalPosition().x), (i%3 == 0 ? yPos : lightTrfHandle->GetLocalPosition().y), (i == 1 ? zPos : lightTrfHandle->GetLocalPosition().z )));
    }

}

void SceneManagerScript::Render(float dt)
{

}

void SceneManagerScript::DeInit()
{
}

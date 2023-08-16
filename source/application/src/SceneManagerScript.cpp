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
//#include <MeshFactory.h>
//#include <MaterialFactory.h>
#include <random>
#include "Utility/Timer.h"

#define debugMeshForLight 0

SceneManagerScript::SceneManagerScript() : Core::ECS::Components::Scriptable(false)
{
    scriptName = typeid(this).raw_name();

    camHandle0 = worldObj->CreateEntity();
    camHandle0->GetEntity()->entityName = "MainCamera";
    camHandle0->GetTransform()->SetLocalPosition(glm::vec3(0, 35, 50));
    camHandle0->GetTransform()->SetLocalEulerAngles(glm::vec3(glm::radians(-27.0f), 0, 0));

    Core::ECS::Components::Camera * camera = new Core::ECS::Components::Camera(camHandle0->GetTransform());
    camHandle0->AddComponent<Core::ECS::Components::Camera>(camera);
    
    cameraController = new CameraController();
    camHandle0->AddComponent<Core::ECS::Components::Scriptable>(cameraController);

    playerObject = worldObj->CreateEntity("playerObject");
    ASSERT_MSG_DEBUG(playerObject != nullptr, "Object not found");

    //playerHandlerScript = new PlayerHandlerScript();
    //playerObject->AddComponent<Core::ECS::Components::Scriptable>(playerHandlerScript);

    lightHandle = worldObj->CreateEntity("light");
    Core::ECS::ComponentHandle<Core::ECS::Components::Transform> lightTrfHandle = lightHandle->GetComponent<Core::ECS::Components::Transform>();
    lightTrfHandle->SetLocalPosition(glm::vec3(19, 29, 0));
    // ligth is using forward
    lightTrfHandle->SetLocalEulerAngles(glm::vec3(glm::radians(-60.0f), glm::radians(90.0), glm::radians(0.0f)));

    lightComponent = new Core::ECS::Components::Light(lightTrfHandle.GetComponent());
    lightHandle->AddComponent<Core::ECS::Components::Light>(lightComponent);
    
    Core::ECS::Components::Material *colMat, *floorMat , *wallMat;

#if (debugMeshForLight)
    {
        std::bitset<(unsigned int)ATTRIBUTES::NUM_ATTRIBUTES> req;
        req.set((unsigned int)ATTRIBUTES::POSITION);
        req.set((unsigned int)ATTRIBUTES::COLOR);

        PrimtiveType * prim = new PrimtiveType{ PrimtiveType::TOPOLOGY_TRIANGLE_LIST };
        MeshInfo meshInfo{};
        meshInfo.attribMaskReq = req;
        meshInfo.bufferPerAttribRequired = false;
        meshInfo.isIndexed = true;
        meshInfo.isPrimitiveRestartEnabled = false;
        meshInfo.primitive = prim;
        MESH_TYPE meshType = MESH_TYPE::CUBE;

        Mesh * mesh = MeshFactory::GetInstance()->CreateMesh(&meshInfo, &meshType);
        lightHandle->AddComponent<Mesh>(mesh);

        ShaderDescription shaders[2];
        shaders[0].type = ShaderType::VERTEX;
        shaders[0].shaderName = "PC.vert";

        shaders[1].type = ShaderType::FRAGMENT;
        shaders[1].shaderName = "Color.frag";

        colMat = MaterialFactory::GetInstance()->CreateMaterial(shaders, 2, mesh->componentId);
        lightHandle->AddComponent<Material>(colMat);

        lightDebugRenderer = new MeshRenderer(mesh, colMat, lightTrfHandle.GetComponent());
        lightDebugRenderer->castShadows = false;
        lightDebugRenderer->receiveShadows = false;

        lightHandle->AddComponent<MeshRenderer>(lightDebugRenderer);
    }
#endif

    floorHandle = worldObj->CreateEntity("floor");
    Core::ECS::ComponentHandle<Core::ECS::Components::Transform> floorTrfHandle = floorHandle->GetComponent<Core::ECS::Components::Transform>();
    floorTrfHandle->SetLocalPosition(glm::vec3(0, -20, 0));
    floorTrfHandle->SetLocalScale(glm::vec3(200, 200, 1));
    floorTrfHandle->SetLocalEulerAngles(glm::vec3(glm::radians(90.0), 0, 0));

#if 0
    {
        std::bitset<(unsigned int)ATTRIBUTES::NUM_ATTRIBUTES> req;
        req.set((unsigned int)ATTRIBUTES::POSITION);
        req.set((unsigned int)ATTRIBUTES::COLOR);
        req.set((unsigned int)ATTRIBUTES::NORMAL);

        PrimtiveType * prim = new PrimtiveType{ PrimtiveType::TOPOLOGY_TRIANGLE_LIST };
        MeshInfo meshInfo{};
        meshInfo.attribMaskReq = req;
        meshInfo.bufferPerAttribRequired = false;
        meshInfo.isIndexed = true; // needs to be corrected, as we are using indexed mesh but not the index buffer
        meshInfo.isPrimitiveRestartEnabled = false;
        meshInfo.primitive = prim;
        meshInfo.overrideColor = true;
        meshInfo.color = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
        MESH_TYPE meshType = MESH_TYPE::QUAD;

        Mesh * mesh = MeshFactory::GetInstance()->CreateMesh(&meshInfo, &meshType);
        floorHandle->AddComponent<Mesh>(mesh);

        ShaderDescription shaders[2];
        shaders[0].type = ShaderType::VERTEX;
        shaders[0].shaderName = "PCN_Lighting.vert";

        shaders[1].type = ShaderType::FRAGMENT;
        shaders[1].shaderName = "ColorLighting.frag";

        floorMat = MaterialFactory::GetInstance()->CreateMaterial(shaders, 2, mesh->componentId);
        floorHandle->AddComponent<Material>(floorMat);

        floorRenderer = new MeshRenderer(mesh, floorMat, floorTrfHandle.GetComponent());
        floorHandle->AddComponent<MeshRenderer>(floorRenderer);
    }
#endif
    auto seed = 4;// Timer::GetInstance()->GetSeconds();
    PLOGD << "Seed : " << seed;
    srand( seed );

    uint32_t radius = 14;

    /*std::bitset<(unsigned int)ATTRIBUTES::NUM_ATTRIBUTES> req;
    req.set((unsigned int)ATTRIBUTES::POSITION);
    req.set((unsigned int)ATTRIBUTES::COLOR);
    req.set((unsigned int)ATTRIBUTES::NORMAL);*/

    boxHandles.resize(numBoxes);
    boxRenderer.resize(numBoxes);

    for (uint32_t i = 0; i < numBoxes; i++)
    {
        boxHandles[i] = worldObj->CreateEntity("box" + std::to_string(i));
        Core::ECS::ComponentHandle<Core::ECS::Components::Transform> trfHandle = boxHandles[i]->GetComponent<Core::ECS::Components::Transform>();
        
        /*
        theta = random(0,TWO_PI);
        phi = random(0,PI);
        r = random(0,1);
        x = r * sin(phi) * cos(theta);
        y = r * sin(phi) * sin(theta);
        z = r * cos(phi);
        */

        int theta = rand() % (uint32_t)((360.0f));
        int phi = rand() % (uint32_t)((180.0f));
        float x = -5 + radius * glm::sin(glm::radians((float)phi)) * glm::cos(glm::radians((float)theta));
        float y = radius * glm::sin(glm::radians((float)phi)) * glm::sin(glm::radians((float)theta));
        float z = radius * glm::cos(glm::radians((float)phi));
        
        trfHandle->SetLocalPosition(glm::vec3(x, y, z));
        trfHandle->SetLocalScale(glm::vec3(1, 1, 1));
        trfHandle->SetLocalEulerAngles(glm::vec3(0, glm::radians(-180.0), 0));
#if 0
        {
            PrimtiveType * prim = new PrimtiveType{ PrimtiveType::TOPOLOGY_TRIANGLE_LIST };
            //PrimtiveType  prim{ PrimtiveType::TOPOLOGY_TRIANGLE_LIST };
            MeshInfo meshInfo{};
            meshInfo.attribMaskReq = req;
            meshInfo.bufferPerAttribRequired = false;
            meshInfo.isIndexed = true; // needs to be corrected, as we are using indexed mesh but not the index buffer
            meshInfo.isPrimitiveRestartEnabled = false;
            meshInfo.primitive = prim;
            meshInfo.overrideColor = true;

            //srand(10);
            float r = (float)(rand() % 10) / 10.0f;
            float g = (float)(rand() % 10) / 10.0f;
            float b = (float)(rand() % 10) / 10.0f;

            meshInfo.color = glm::vec4(r, g, b, 1.0f);

            MESH_TYPE meshType = MESH_TYPE::CUBE;

            Mesh * mesh = MeshFactory::GetInstance()->CreateMesh(&meshInfo, &meshType);
            boxHandles[i]->AddComponent<Mesh>(mesh);

            boxHandles[i]->AddComponent<Material>(floorMat);
            MaterialFactory::GetInstance()->AddMeshIds(floorMat, mesh->componentId);
            boxRenderer[i] = new MeshRenderer(mesh, floorMat, trfHandle.GetComponent());
            boxHandles[i]->AddComponent<MeshRenderer>(boxRenderer[i]);
        }
#endif
    }

    /*
    wallHandle = worldObj->CreateEntity("wall");
    ComponentHandle<Transform> wallTrfHandle = wallHandle->GetComponent<Transform>();
    wallTrfHandle->SetLocalPosition(glm::vec3(0, 0, -40));
    wallTrfHandle->SetLocalScale(glm::vec3(20, 20, 1));
    wallTrfHandle->SetLocalEulerAngles(glm::vec3(0, glm::radians(-180.0), 0));

    {
        std::bitset<(unsigned int)ATTRIBUTES::NUM_ATTRIBUTES> req;
        req.set((unsigned int)ATTRIBUTES::POSITION);
        req.set((unsigned int)ATTRIBUTES::COLOR);
        req.set((unsigned int)ATTRIBUTES::NORMAL);

        PrimtiveType * prim = new PrimtiveType{ PrimtiveType::TOPOLOGY_TRIANGLE_LIST };
        MeshInfo meshInfo{};
        meshInfo.attribMaskReq = req;
        meshInfo.bufferPerAttribRequired = false;
        meshInfo.isIndexed = true; // needs to be corrected, as we are using indexed mesh but not the index buffer
        meshInfo.isPrimitiveRestartEnabled = false;
        meshInfo.primitive = prim;
        MESH_TYPE meshType = MESH_TYPE::QUAD;

        Mesh * mesh = MeshFactory::GetInstance()->CreateMesh(&meshInfo, &meshType);
        wallHandle->AddComponent<Mesh>(mesh);

        ShaderDescription shaders[2];
        shaders[0].type = ShaderType::VERTEX;
        shaders[0].shaderName = "PCN_Lighting.vert";

        shaders[1].type = ShaderType::FRAGMENT;
        shaders[1].shaderName = "ColorLighting.frag";

        wallMat = MaterialFactory::GetInstance()->CreateMaterial(shaders, 2, mesh->componentId);
        wallHandle->AddComponent<Material>(wallMat);

        wallRenderer = new MeshRenderer(mesh, wallMat, wallTrfHandle.GetComponent());
        wallHandle->AddComponent<MeshRenderer>(wallRenderer);
    }
    */
}

SceneManagerScript::~SceneManagerScript()
{
    for (uint32_t i = 0; i < boxHandles.size(); i++)
    {
        {
            //Core::ECS::ComponentHandle<Core::ECS::Components::Mesh> mesh = boxHandles[i]->GetComponent<Core::ECS::Components::Mesh>();
            ////MeshFactory::GetInstance()->DestroyMesh(mesh->componentId);
            //mesh.DestroyComponent();

            //Core::ECS::ComponentHandle<Core::ECS::Components::Material> mat = boxHandles[i]->GetComponent<Core::ECS::Components::Material>();
            //mat.DestroyComponent();

        }
        worldObj->DestroyEntity(boxHandles[i]);
    }


    {
        //Core::ECS::ComponentHandle<Core::ECS::Components::Mesh> mesh = floorHandle->GetComponent<Core::ECS::Components::Mesh>();
        ////MeshFactory::GetInstance()->DestroyMesh(mesh->componentId);
        //mesh.DestroyComponent();
        //Core::ECS::ComponentHandle<Core::ECS::Components::Material> mat = floorHandle->GetComponent<Core::ECS::Components::Material>();
        //mat.DestroyComponent();
    }
    worldObj->DestroyEntity(floorHandle);

#if (debugMeshForLight)
    {
        ComponentHandle<Mesh> mesh = lightHandle->GetComponent<Mesh>();
        MeshFactory::GetInstance()->DestroyMesh(mesh->componentId);
        mesh.DestroyComponent();
        ComponentHandle<Material> mat = lightHandle->GetComponent<Material>();
        mat.DestroyComponent();
    }
#endif

    lightHandle->RemoveComponent<Core::ECS::Components::Light>(lightComponent);
    worldObj->DestroyEntity(lightHandle);

    delete cameraController;
    worldObj->DestroyEntity(camHandle0);

    /*playerObject->RemoveComponent<Core::ECS::Components::Scriptable>(playerHandlerScript);
    delete playerHandlerScript;
    playerHandlerScript = NULL;*/
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

    float radius = glm::distance(glm::vec3(0, 0, 0), lightHandle->GetTransform()->GetGlobalPosition());
    float x = radius * glm::cos(theta);
    float y = radius * glm::sin(theta);

    //PLOGD << x << " " << y;

    {
        Core::ECS::Components::Transform * transform = lightHandle->GetTransform();

        float angle = MathUtil::lerp(prevAngle, currentAngle, dt);
        prevAngle = currentAngle;

        transform->SetLocalPosition(glm::vec3(x, y, transform->GetLocalPosition().z));

        transform->SetLocalEulerAngles(glm::vec3(theta, 
            transform->GetLocalEulerAngles().y, theta * 2.0));
    }
}

void SceneManagerScript::Render(float dt)
{

}

void SceneManagerScript::DeInit()
{
}

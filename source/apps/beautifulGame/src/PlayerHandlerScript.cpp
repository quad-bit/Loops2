#if 0
#include "CorePrecompiled.h"
#include "PlayerHandlerScript.h"
#include <ECS/ECS_Setting.h>
#include <EntityHandle.h>
#include <typeinfo>
#include <Transform.h>
#include <Mesh.h>
#include <AttributeHelper.h>
#include <MeshFactory.h>
#include <bitset>
#include <MaterialFactory.h>
#include <Camera.h>
#include <MeshRenderer.h>
#include <Timer.h>
#include <glm\glm.hpp>
#include <MathUtil.h>
#include "CameraController.h"

PlayerHandlerScript::PlayerHandlerScript() : Scriptable(false)
{
    scriptName = typeid(this).raw_name();

    playerHandle = worldObj->CreateEntity();
    playerHandle->GetEntity()->entityName = "player";
    Transform * playerTrf = playerHandle->GetTransform();
    playerTrf->SetLocalPosition(glm ::vec3(0, 0, 0));
    
    // TORSO
    torso = worldObj->CreateEntity();
    torso->GetEntity()->entityName = "torso";
    Transform * torsoTrf = torso->GetTransform();
    torsoTrf->SetLocalPosition(glm::vec3(0, -5, 0));

    Material * colMat, * colLitMat;
    torsoTrf->SetParent(playerTrf);
    
    {
        std::bitset<(unsigned int)ATTRIBUTES::NUM_ATTRIBUTES> req;
        req.set((unsigned int)ATTRIBUTES::POSITION);
        req.set((unsigned int)ATTRIBUTES::COLOR);
        req.set((unsigned int)ATTRIBUTES::NORMAL);

        PrimtiveType * prim = new PrimtiveType{ PrimtiveType::TOPOLOGY_TRIANGLE_LIST};
        MeshInfo meshInfo{};
        meshInfo.attribMaskReq = req;
        meshInfo.bufferPerAttribRequired = false;
        meshInfo.isIndexed = true;
        meshInfo.isPrimitiveRestartEnabled = false;
        meshInfo.primitive = prim;
        MESH_TYPE meshType = MESH_TYPE::CUBE;

        Mesh * torsoMesh = MeshFactory::GetInstance()->CreateMesh(&meshInfo, &meshType);
        torso->AddComponent<Mesh>(torsoMesh);

        ShaderDescription shaders[2];
        shaders[0].type = ShaderType::VERTEX;
        shaders[0].shaderName = "PC.vert";

        shaders[1].type = ShaderType::FRAGMENT;
        shaders[1].shaderName = "Color.frag";
        
        /*
        ShaderDescription shaders[2];
        shaders[0].type = ShaderType::VERTEX;
        shaders[0].shaderName = "PCN_Lighting.vert";

        shaders[1].type = ShaderType::FRAGMENT;
        shaders[1].shaderName = "ColorLighting.frag";
        */
        
        colMat = MaterialFactory::GetInstance()->CreateMaterial(shaders, 2, torsoMesh->componentId);
        torso->AddComponent<Material>(colMat);

        torsoMeshRenderer = new MeshRenderer(torsoMesh, colMat, torsoTrf);
        torso->AddComponent<MeshRenderer>(torsoMeshRenderer);
    }
    
    // HEAD
    head = worldObj->CreateEntity();
    head->GetEntity()->entityName = "head";
    Transform * headTrf = head->GetTransform();
    headTrf->SetParent(torsoTrf);
    headTrf->SetLocalPosition( glm::vec3(0, 5, 0));
    headTrf->SetLocalEulerAngles(glm::vec3(glm::radians(90.0), 0, 0));
    
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
        meshInfo.color = glm::vec4(.8, .8, .8, 1.0);

        MESH_TYPE meshType = MESH_TYPE::CUBE;

        Mesh * headMesh = MeshFactory::GetInstance()->CreateMesh(&meshInfo, &meshType);
        head->AddComponent<Mesh>(headMesh);

        //head->AddComponent<Material>(colMat);
        ShaderDescription shaders[2];
        shaders[0].type = ShaderType::VERTEX;
        shaders[0].shaderName = "PCN_Lighting.vert";

        shaders[1].type = ShaderType::FRAGMENT;
        shaders[1].shaderName = "ColorLighting.frag";

    #if 1
        colLitMat = MaterialFactory::GetInstance()->CreateMaterial(shaders, 2, headMesh->componentId);
        head->AddComponent<Material>(colLitMat);

        headMeshRenderer = new MeshRenderer(headMesh, colLitMat, headTrf);
        head->AddComponent<MeshRenderer>(headMeshRenderer);

    #else
        head->AddComponent<Material>(colMat);
        MaterialFactory::GetInstance()->AddMeshIds(colMat, headMesh->componentId);

        headMeshRenderer = new MeshRenderer(headMesh, colMat, headTrf);
        head->AddComponent<MeshRenderer>(headMeshRenderer);

    #endif
    }
    

    //LEFT ARM
    leftArm = worldObj->CreateEntity();
    leftArm->GetEntity()->entityName = "leftArm";
    leftArm->GetTransform()->SetParent(torsoTrf);

    //RIGHT ARM
    rightArm = worldObj->CreateEntity();
    rightArm->GetEntity()->entityName = "rightArm";
    rightArm->GetTransform()->SetParent(torsoTrf);

    //RIGHT LEG
    rightLeg = worldObj->CreateEntity();
    rightLeg->GetEntity()->entityName = "rightLeg";
    rightLeg->GetTransform()->SetParent(torsoTrf);

    //LEFT LEG
    leftLeg = worldObj->CreateEntity();
    leftLeg->GetEntity()->entityName = "leftLeg";
    leftLeg->GetTransform()->SetParent(torsoTrf);
}

void PlayerHandlerScript::Init()
{
    
}

static uint32_t counter = 0;
void PlayerHandlerScript::Update(float dt)
{
    // head rotation
    {
        Transform * transform = torso->GetTransform();
        
        currentAngle = glm::sin(counter++ / 300.0f);

        float angle = MathUtil::lerp(prevAngle, currentAngle, dt);
        prevAngle = currentAngle;

        transform->SetLocalEulerAngles(glm::vec3(0, angle, 0));
    }
}

// not getting used 
void PlayerHandlerScript::Render(float dt)
{
    //// head rotation
    //{
    //    Transform * headTrf = head->GetTransform();

    //    float angle = glm::sin(counter++ / 100.0f) * dt;
    //    PLOGD << "Render " <<dt;

    //    headTrf->SetLocalEulerAngles(glm::vec3(0, angle, 0));
    //}

}

void PlayerHandlerScript::DeInit()
{
    int k = 4;
}

PlayerHandlerScript::~PlayerHandlerScript()
{
    worldObj->DestroyEntity(playerHandle);
    {
        ComponentHandle<Mesh> torsoMesh = torso->GetComponent<Mesh>();
        MeshFactory::GetInstance()->DestroyMesh(torsoMesh->componentId);
        torsoMesh.DestroyComponent();
        //torso->RemoveComponent<Mesh>(torsoMesh.GetComponent());
        //delete torsoMesh.GetComponent(); //TODO  fix this get component function
        ComponentHandle<Material> torsoMat = torso->GetComponent<Material>();
        torsoMat.DestroyComponent();
        worldObj->DestroyEntity(torso);
    }

#if 1
    ComponentHandle<Mesh> headMesh = head->GetComponent<Mesh>();
    MeshFactory::GetInstance()->DestroyMesh(headMesh->componentId);
    headMesh.DestroyComponent();
    //head->RemoveComponent<Mesh>(headMesh.GetComponent());
    ComponentHandle<Material> headMat = head->GetComponent<Material>();
    headMat.DestroyComponent();
#endif
    worldObj->DestroyEntity(head);

    worldObj->DestroyEntity(leftArm);
    worldObj->DestroyEntity(rightArm);
    worldObj->DestroyEntity(leftLeg);
    worldObj->DestroyEntity(rightLeg);
    
    //delete cameraController;
    //worldObj->DestroyEntity(camHandle0);
}
#endif
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE

#include "ECS/ECS_Setting.h"
#include "ECS/World.h"
#include "SceneLoader.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Mesh.h"
#include "resourceManagement/MeshFactory.h"
#include "resourceManagement/MaterialFactory.h"
#include <glm/gtc/type_ptr.hpp>

void Engine::Utility::GltfLoader::LoadNode(const tinygltf::Node& inputNode, const tinygltf::Model& input, Core::ECS::EntityHandle* parent)
{
    Core::ECS::EntityHandle* entity = m_ecsWorldObj->CreateEntity();
    entity->GetEntity()->entityName = inputNode.name;
    auto transform = entity->GetTransform();
    if(parent)
        transform->SetParent(parent->GetTransform());

    float scaleFactor = 100.0f;

    // Get the local node matrix
    // It's either made up from translation, rotation, scale or a 4x4 matrix
    if (inputNode.translation.size() == 3)
    {
        auto position = glm::vec3(glm::make_vec3(inputNode.translation.data())) * scaleFactor;
        /*
        * global to local 
        * posLocal = globalInverMatOfObject * (globalPosOfObject - theGlobalPosition)
        */

        auto localPosition = glm::inverse(parent->GetTransform()->GetGlobalModelMatrix()) * glm::vec4(parent->GetTransform()->GetGlobalPosition() - position, 1.0f);
        transform->SetLocalPosition(position);
    }
    if (inputNode.rotation.size() == 4)
    {
        glm::quat q = glm::make_quat(inputNode.rotation.data());
        glm::vec3 euler = glm::eulerAngles(q);// *3.14159f / 180.f;
        //euler = glm::inverse(parent->GetTransform()->GetGlobalModelMatrix())* glm::vec4(parent->GetTransform()->GetLocalEulerAngles() - euler, 1.0f);
        transform->SetLocalEulerAngles(euler);
    }

    if (inputNode.scale.size() == 3)
    {
        transform->SetLocalScale(glm::vec3(glm::make_vec3(inputNode.scale.data())));
    }

    if (inputNode.matrix.size() == 16)
    {
        //node->matrix = glm::make_mat4x4(inputNode.matrix.data());
        ASSERT_MSG_DEBUG(0, "not handled");
    };


    // If the node contains mesh data, we load vertices and indices from the buffers
    // In glTF this is done via accessors and buffer views
    if (inputNode.mesh > -1) {
        const tinygltf::Mesh mesh = input.meshes[inputNode.mesh];
        if (mesh.primitives.size() > 1)
        {
            ASSERT_MSG_DEBUG(0, "Sub meshes not yet supported");
        }

        // Iterate through all primitives of this node's mesh
        for (size_t i = 0; i < mesh.primitives.size(); i++)
        {
            const tinygltf::Primitive& glTFPrimitive = mesh.primitives[i];
            uint32_t firstIndex = 0;// static_cast<uint32_t>(indexBuffer.size());
            uint32_t vertexStart = 0;// static_cast<uint32_t>(vertexBuffer.size());
            uint32_t indexCount = 0;
            Core::ECS::Components::Mesh* mesh = new Core::ECS::Components::Mesh();

            // Vertices
            {
                const float* positionBuffer = nullptr;
                const float* normalsBuffer = nullptr;
                const float* texCoordsBuffer = nullptr;
                const float* tangentsBuffer = nullptr;
                size_t vertexCount = 0;

                // Get buffer data for vertex normals
                if (glTFPrimitive.attributes.find("POSITION") != glTFPrimitive.attributes.end()) {
                    const tinygltf::Accessor& accessor = input.accessors[glTFPrimitive.attributes.find("POSITION")->second];
                    const tinygltf::BufferView& view = input.bufferViews[accessor.bufferView];
                    positionBuffer = reinterpret_cast<const float*>(&(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
                    vertexCount = accessor.count;
                }
                // Get buffer data for vertex normals
                if (glTFPrimitive.attributes.find("NORMAL") != glTFPrimitive.attributes.end()) {
                    const tinygltf::Accessor& accessor = input.accessors[glTFPrimitive.attributes.find("NORMAL")->second];
                    const tinygltf::BufferView& view = input.bufferViews[accessor.bufferView];
                    normalsBuffer = reinterpret_cast<const float*>(&(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
                }
                // Get buffer data for vertex texture coordinates
                // glTF supports multiple sets, we only load the first one
                if (glTFPrimitive.attributes.find("TEXCOORD_0") != glTFPrimitive.attributes.end()) {
                    const tinygltf::Accessor& accessor = input.accessors[glTFPrimitive.attributes.find("TEXCOORD_0")->second];
                    const tinygltf::BufferView& view = input.bufferViews[accessor.bufferView];
                    texCoordsBuffer = reinterpret_cast<const float*>(&(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
                }
                // POI: This sample uses normal mapping, so we also need to load the tangents from the glTF file
                if (glTFPrimitive.attributes.find("TANGENT") != glTFPrimitive.attributes.end()) {
                    const tinygltf::Accessor& accessor = input.accessors[glTFPrimitive.attributes.find("TANGENT")->second];
                    const tinygltf::BufferView& view = input.bufferViews[accessor.bufferView];
                    tangentsBuffer = reinterpret_cast<const float*>(&(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
                }


                // Append data to model's vertex buffer
                for (size_t v = 0; v < vertexCount; v++) {
                    mesh->m_positions.push_back(glm::vec3(glm::make_vec3(&positionBuffer[v * 3])) * scaleFactor);
                    mesh->m_normals.push_back(glm::vec3(normalsBuffer ? glm::make_vec3(&normalsBuffer[v * 3]) : glm::vec3(0.0f)));
                    mesh->m_uv0.push_back(texCoordsBuffer ? glm::make_vec2(&texCoordsBuffer[v * 2]) : glm::vec3(0.0f));
                    mesh->m_colors.push_back(glm::vec4(1.0f));
                    mesh->m_tangents.push_back(tangentsBuffer ? glm::make_vec4(&tangentsBuffer[v * 4]) : glm::vec4(0.0f));
                }
            }

            // Indices
            {
                const tinygltf::Accessor& accessor = input.accessors[glTFPrimitive.indices];
                const tinygltf::BufferView& bufferView = input.bufferViews[accessor.bufferView];
                const tinygltf::Buffer& buffer = input.buffers[bufferView.buffer];

                indexCount += static_cast<uint32_t>(accessor.count);

                // glTF supports different component types of indices
                switch (accessor.componentType)
                {
                case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:
                {
                    const uint32_t* buf = reinterpret_cast<const uint32_t*>(&buffer.data[accessor.byteOffset + bufferView.byteOffset]);
                    for (size_t index = 0; index < accessor.count; index++)
                    {
                        mesh->m_indicies.push_back(buf[index] + vertexStart);
                    }
                    break;
                }
                case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
                    const uint16_t* buf = reinterpret_cast<const uint16_t*>(&buffer.data[accessor.byteOffset + bufferView.byteOffset]);
                    for (size_t index = 0; index < accessor.count; index++) {
                        mesh->m_indicies.push_back(buf[index] + vertexStart);
                    }
                    break;
                }
                case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
                    const uint8_t* buf = reinterpret_cast<const uint8_t*>(&buffer.data[accessor.byteOffset + bufferView.byteOffset]);
                    for (size_t index = 0; index < accessor.count; index++) {
                        mesh->m_indicies.push_back(buf[index] + vertexStart);
                    }
                    break;
                }
                default:
                    std::cerr << "Index component type " << accessor.componentType << " not supported!" << std::endl;
                    return;
                }
            }

            Renderer::ResourceManagement::MeshFactory::GetInstance()->AddMesh(mesh);
            entity->AddComponent<Core::ECS::Components::Mesh>(mesh);

            auto floorMat = Renderer::ResourceManagement::MaterialFactory::GetInstance()->CreateMaterial(Core::ECS::Components::EffectType::OPAQUE_E);
            entity->AddComponent<Core::ECS::Components::Material>(floorMat);

            auto floorRenderer = new Core::ECS::Components::MeshRenderer(mesh, floorMat, transform);
            entity->AddComponent<Core::ECS::Components::MeshRenderer>(floorRenderer);
        }
    }

    m_entityList.push_back(entity);

    // Load node's children
    if (inputNode.children.size() > 0)
    {
        for (size_t i = 0; i < inputNode.children.size(); i++)
        {
            LoadNode(input.nodes[inputNode.children[i]], input, entity);
        }
    }
}

Engine::Utility::GltfLoader::GltfLoader(Core::ECS::World* worldObj)
{
    m_ecsWorldObj = worldObj;
}

void Engine::Utility::GltfLoader::LoadGltf(const std::string& assetName, std::vector<Core::ECS::EntityHandle*>& entityList)
{
    // load file from gltfPath
    std::string path = ASSETS_PATH + std::string{ "\\" } + assetName;

    tinygltf::Model glTFInput;
    tinygltf::TinyGLTF gltfContext;
    std::string error, warning;

    bool fileLoaded = gltfContext.LoadASCIIFromFile(&glTFInput, &error, &warning, path);

    Core::ECS::EntityHandle* entity = m_ecsWorldObj->CreateEntity();
    entity->GetEntity()->entityName = "modelParent";
    auto transform = entity->GetTransform();
    transform->SetLocalPosition(glm::vec3(0, 0, 0));
    transform->SetLocalScale(glm::vec3(1, 1, 1));
    transform->SetLocalEulerAngles(glm::vec3(0, glm::radians(90.0), 0));

    m_entityList.push_back(entity);

    if (fileLoaded) {
        //glTFScene.loadImages(glTFInput);
        //glTFScene.loadMaterials(glTFInput);
        //glTFScene.loadTextures(glTFInput);
        const tinygltf::Scene& scene = glTFInput.scenes[0];
        for (size_t i = 0; i < scene.nodes.size(); i++) {
            const tinygltf::Node node = glTFInput.nodes[scene.nodes[i]];
            LoadNode(node, glTFInput, entity);
        }
    }
    else {
        ASSERT_MSG_DEBUG(0, "File load failed");
        return;
    }

    // create entity and fill entityList

}

Engine::Utility::GltfLoader::~GltfLoader()
{
    for (auto& handle : m_entityList)
    {
        {
            if (handle->HasComponent<Core::ECS::Components::Mesh>())
            {
                auto mesh = handle->GetComponent<Core::ECS::Components::Mesh>();
                Renderer::ResourceManagement::MeshFactory::GetInstance()->DestroyMesh(mesh.GetComponent());
                mesh.DestroyComponent();
            }

            if (handle->HasComponent<Core::ECS::Components::Material>())
            {
                auto mat = handle->GetComponent<Core::ECS::Components::Material>();
                mat.DestroyComponent();
            }

            if (handle->HasComponent<Core::ECS::Components::MeshRenderer>())
            {
                auto renderer = handle->GetComponent<Core::ECS::Components::MeshRenderer>();
                renderer.DestroyComponent();
            }
        }
        worldObj->DestroyEntity(handle);
    }
}

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE
//#define TINYGLTF_NO_STB_IMAGE
//#define TINYGLTF_NO_EXTERNAL_IMAGE

#include "ECS/ECS_Setting.h"
#include "ECS/World.h"
#include "SceneLoader.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Mesh.h"
#include "ECS/Components/Bounds.h"
#include "resourceManagement/MeshFactory.h"
#include "resourceManagement/BoundFactory.h"
#include "resourceManagement/MaterialFactory.h"
#include <glm/gtc/type_ptr.hpp>
#include "VulkanInterface.h"
#include "resourceManagement/ShaderResourceManager.h"
#include <utility>
#include <Math/TangentCalculator.h>


void Engine::Utility::GltfLoader::LoadNode(const tinygltf::Node& inputNode, const tinygltf::Model& input, Core::ECS::EntityHandle* parent)
{
    bool generateTangents = false;

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
            size_t vertexCount = 0;

            // Vertices
            {
                const float* positionBuffer = nullptr;
                const float* normalsBuffer = nullptr;
                const float* texCoordsBuffer = nullptr;
                const float* tangentsBuffer = nullptr;

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
                if (glTFPrimitive.attributes.find("TANGENT") != glTFPrimitive.attributes.end()) {
                    const tinygltf::Accessor& accessor = input.accessors[glTFPrimitive.attributes.find("TANGENT")->second];
                    const tinygltf::BufferView& view = input.bufferViews[accessor.bufferView];
                    tangentsBuffer = reinterpret_cast<const float*>(&(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
                }


                glm::vec3 minPos{}, maxPos{};

                // Append data to model's vertex buffer
                for (size_t v = 0; v < vertexCount; v++) {
                    mesh->m_positions.push_back(glm::vec3(glm::make_vec3(&positionBuffer[v * 3])) * scaleFactor);
                    mesh->m_normals.push_back(glm::vec3(normalsBuffer ? glm::make_vec3(&normalsBuffer[v * 3]) : glm::vec3(0.0f)));
                    mesh->m_uv0.push_back(texCoordsBuffer ? glm::make_vec2(&texCoordsBuffer[v * 2]) : glm::vec3(0.0f));
                    mesh->m_colors.push_back(glm::vec4(1.0f));

                    // if normals are present and tangents are missing 
                    if (normalsBuffer && !tangentsBuffer)
                    {
                        generateTangents = true;
                    }
                    else
                        mesh->m_tangents.push_back(glm::make_vec4(&tangentsBuffer[v * 4]));

                    {
                        // min
                        if (mesh->m_positions[v].x < minPos.x)
                        {
                            minPos.x = mesh->m_positions[v].x;
                        }

                        if (mesh->m_positions[v].y < minPos.y)
                        {
                            minPos.y = mesh->m_positions[v].y;
                        }

                        if (mesh->m_positions[v].z < minPos.z)
                        {
                            minPos.z = mesh->m_positions[v].z;
                        }

                        // max
                        if (mesh->m_positions[v].x > maxPos.x)
                        {
                            maxPos.x = mesh->m_positions[v].x;
                        }

                        if (mesh->m_positions[v].y > maxPos.y)
                        {
                            maxPos.y = mesh->m_positions[v].y;
                        }

                        if (mesh->m_positions[v].z > maxPos.z)
                        {
                            maxPos.z = mesh->m_positions[v].z;
                        }
                    }
                    mesh->m_minPos = minPos;
                    mesh->m_maxPos = maxPos;
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

            if (generateTangents)
            {
                mesh->m_tangents.resize(vertexCount);
                Core::Math::CalculateTangentArray(vertexCount, mesh->m_positions.data(), mesh->m_normals.data(),
                    mesh->m_uv0.data(), mesh->m_indicies.data(), mesh->m_indicies.size(), mesh->m_tangents.data());
            }

            Renderer::ResourceManagement::MeshFactory::GetInstance()->AddMesh(mesh);
            entity->AddComponent<Core::ECS::Components::Mesh>(mesh);

            auto mat = GetMaterial(input, glTFPrimitive.material);
            entity->AddComponent<Core::ECS::Components::Material>(mat);

            auto renderer = new Core::ECS::Components::MeshRenderer(mesh, mat, transform);
            entity->AddComponent<Core::ECS::Components::MeshRenderer>(renderer);

            // Bounds
            {
                std::unique_ptr<Core::ECS::Components::BoundCategory> category = std::make_unique<Core::ECS::Components::AABB>(
                    entity->GetTransform()->GetGlobalModelMatrix(), entity->GetEntity()->entityName, mesh->m_minPos, mesh->m_maxPos);
                Core::ECS::Components::Bound * bound = new Core::ECS::Components::Bound(category);
                Renderer::ResourceManagement::BoundFactory::GetInstance()->AddBound(bound);
                entity->AddComponent<Core::ECS::Components::Bound>(bound);
            }
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

void Engine::Utility::GltfLoader::LoadMaterials(const tinygltf::Model& input)
{
    for (size_t i = 0; i < input.materials.size(); i++) {
        // We only read the most basic properties required for our sample
        tinygltf::Material glTFMaterial = input.materials[i];

        auto& name = glTFMaterial.name;

        glm::vec4 color{};
        std::optional<uint32_t> baseTextureId, baseSamplerId, normalTextureId;

        // Get the base color factor
        if (glTFMaterial.values.find("baseColorFactor") != glTFMaterial.values.end()) {
            color = glm::make_vec4(glTFMaterial.values["baseColorFactor"].ColorFactor().data());
        }

        // Get base color texture index
        if (glTFMaterial.values.find("baseColorTexture") != glTFMaterial.values.end()) {
            baseTextureId = GetTexture(glTFMaterial.values["baseColorTexture"].TextureIndex());
            baseSamplerId = GetSampler(input.textures[glTFMaterial.values["baseColorTexture"].TextureIndex()].sampler);
        }

        // Get the normal map texture index
        if (glTFMaterial.additionalValues.find("normalTexture") != glTFMaterial.additionalValues.end()) {
            normalTextureId = GetTexture(glTFMaterial.additionalValues["normalTexture"].TextureIndex());
        }

        //// Get some additional material parameters that are used in this sample
        //materials[i].alphaMode = glTFMaterial.alphaMode;
        //materials[i].alphaCutOff = (float)glTFMaterial.alphaCutoff;
        //materials[i].doubleSided = glTFMaterial.doubleSided;

        std::string tech = "OpaqueUnlit";

        if (baseTextureId != std::nullopt && baseSamplerId != std::nullopt && normalTextureId != std::nullopt)
        {
            tech = "OpaqueTexturedLit";
        }
        else if (baseTextureId != std::nullopt && baseSamplerId != std::nullopt)
        {
            tech = "OpaqueTexturedUnlit";
        }

        auto effectId = VulkanInterfaceAlias::GetEffectId("OpaquePass");
        auto techId = VulkanInterfaceAlias::GetTechniqueId(effectId, tech);
        auto effectInfo = Core::Utility::EffectInfo{ effectId, techId };

        Core::ECS::Components::MaterialCreateInfo info{
            color,
            effectInfo,
            baseTextureId,
            baseSamplerId,
            normalTextureId,
            name
        };

        auto mat = Renderer::ResourceManagement::MaterialFactory::GetInstance()->CreateMaterial(info);
        m_materialMap.insert({ (int)i, mat });
    }
}

Core::ECS::Components::Material* Engine::Utility::GltfLoader::GetMaterial(const tinygltf::Model& input, uint32_t materialIndex)
{
    if (m_materialMap.find(materialIndex) == m_materialMap.end())
    {
        ASSERT_MSG_DEBUG(0, "material not found");
    }

    return m_materialMap[materialIndex];
}

void Engine::Utility::GltfLoader::LoadTextures(const tinygltf::Model& input)
{
    const auto imageFormat = Core::Enums::Format::R8G8B8A8_UNORM;
    uint32_t textureIndex = 0;
    for (auto& gltfTexture : input.textures)
    {
        tinygltf::Image gltfImage = input.images[gltfTexture.source];

        unsigned char* buffer = nullptr;
        size_t bufferSize = 0;

        if (gltfImage.component == 3)
        {
            bufferSize = gltfImage.width * gltfImage.height * 4;
            buffer = new unsigned char[bufferSize];

            auto rgba = buffer;
            auto rgb = &gltfImage.image[0];

            for (uint32_t i = 0; i < gltfImage.width * gltfImage.height; ++i)
            {
                memcpy(rgba, rgb, sizeof(unsigned char) * 3);
                rgba += 4;
                rgb += 3;
            }
        }
        else
        {
            buffer = &gltfImage.image[0];
            bufferSize = gltfImage.image.size();
        }

        Core::Wrappers::ImageCreateInfo imageInfo = {};
        imageInfo.m_colorSpace = Core::Enums::ColorSpace::COLOR_SPACE_SRGB_NONLINEAR_KHR;
        imageInfo.m_depth = 1;
        imageInfo.m_format = imageFormat;
        imageInfo.m_height = gltfImage.height;
        imageInfo.m_imageType = Core::Enums::ImageType::IMAGE_TYPE_2D;
        imageInfo.m_initialLayout = Core::Enums::ImageLayout::LAYOUT_UNDEFINED;
        imageInfo.m_layers = 1;
        imageInfo.m_mips = 1;
        imageInfo.m_sampleCount = Core::Enums::Samples::SAMPLE_COUNT_1_BIT;
        imageInfo.m_usages = { Core::Enums::ImageUsage::USAGE_TRANSFER_DST_BIT, Core::Enums::ImageUsage::USAGE_SAMPLED_BIT };
        imageInfo.m_viewType = Core::Enums::ImageViewType::IMAGE_VIEW_TYPE_2D;
        imageInfo.m_width = gltfImage.width;

        auto id = VulkanInterfaceAlias::CreateImage(buffer, bufferSize, imageInfo, gltfImage.name);
        m_imageList.insert({ textureIndex++, id });
    }
}

uint32_t Engine::Utility::GltfLoader::GetTexture(int textureIndex)
{
    auto it = m_imageList.find(textureIndex);
    ASSERT_MSG_DEBUG( it != m_imageList.end(), "image not found");

    return it->second;
}

void Engine::Utility::GltfLoader::LoadSamplers(const tinygltf::Model& input)
{
    // Not loading any info from gltf for samplers, for now.
    // can take the filter valur from gltf

    auto& samplers = input.samplers;
    uint32_t index = 0;
    for (auto& sampler : samplers)
    {
        Core::Wrappers::SamplerCreateInfo info = {};
        info.minFilter = Core::Enums::Filter::FILTER_NEAREST;
        info.magFilter = Core::Enums::Filter::FILTER_NEAREST;
        info.addressModeU = Core::Enums::SamplerAddressMode::SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        info.addressModeV = info.addressModeU;
        info.addressModeW = info.addressModeU;
        info.anisotropyEnable = false;
        info.maxAnisotropy = 1.0f;
        info.borderColor = Core::Enums::BorderColor::BORDER_COLOR_FLOAT_OPAQUE_WHITE;
        info.unnormalizedCoordinates = false;
        info.compareEnable = false;
        info.compareOp = Core::Enums::CompareOp::COMPARE_OP_ALWAYS;
        info.mipmapMode = Core::Enums::SamplerMipmapMode::SAMPLER_MIPMAP_MODE_NEAREST;
        info.minLod = 0.0f;
        info.maxLod = 0.0f;
        info.mipLodBias = 0.0f;

        uint32_t samplerId = VulkanInterfaceAlias::CreateSampler(info);
        m_samplerList.insert({index++, samplerId });
    }
}

uint32_t Engine::Utility::GltfLoader::GetSampler(uint32_t index)
{
    //auto it = std::find_if(m_samplerList.begin(), m_samplerList.end(), [&index](const uint32_t& e) {return e == index; });
    auto it = m_samplerList.find(index);
    ASSERT_MSG_DEBUG(it != m_samplerList.end(), "sampler not found");

    return it->second;
}

Engine::Utility::GltfLoader::GltfLoader(Core::ECS::World* worldObj)
{
    m_ecsWorldObj = worldObj;
}

void Engine::Utility::GltfLoader::LoadGltf(const std::string& assetName, std::vector<Core::ECS::EntityHandle*>& entityList)
{
    // load file from gltfPath
    std::string path = ASSETS_PATH + std::string{ "\\models\\" } + assetName;

    tinygltf::Model glTFInput;
    tinygltf::TinyGLTF gltfContext;
    std::string error, warning;

    bool fileLoaded = gltfContext.LoadASCIIFromFile(&glTFInput, &error, &warning, path);

    Core::ECS::EntityHandle* entity = m_ecsWorldObj->CreateEntity();
    entity->GetEntity()->entityName = "modelParent";
    auto transform = entity->GetTransform();
    transform->SetLocalPosition(glm::vec3(0, 0, 0));
    transform->SetLocalScale(glm::vec3(1, 1, 1));
    //transform->SetLocalEulerAngles(glm::vec3(0, glm::radians(90.0), 0));
    transform->SetLocalEulerAngles(glm::vec3(0, 0, 0));

    m_entityList.push_back(entity);

    if (fileLoaded) {
        LoadSamplers(glTFInput);
        LoadTextures(glTFInput);
        LoadMaterials(glTFInput);
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

            if (handle->HasComponent<Core::ECS::Components::Bound>())
            {
                auto bound = handle->GetComponent<Core::ECS::Components::Bound>();
                auto ptr = bound.GetComponent();
                Renderer::ResourceManagement::BoundFactory::GetInstance()->DestroyBound(ptr);
                bound.DestroyComponent();
                delete ptr;
            }
        }
        worldObj->DestroyEntity(handle);
    }

    for (auto& pair : m_imageList)
    {
        VulkanInterfaceAlias::DestroyImage(pair.second, true);
    }
}

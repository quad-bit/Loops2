#pragma once
#include <vulkan/vulkan.h>
#include <string>
#include <vector>

namespace Core
{
    namespace Enums
    {
        enum class ShaderType;
    }
}

namespace GfxVk
{
    namespace Shading
    {
        struct ShaderModuleWrapper
        {
            std::string shaderName;
            Core::Enums::ShaderType* shaderType;
            VkShaderModule* module;
            VkShaderStageFlagBits stageFlag;
            uint32_t shaderId;
        };

        class VkShaderFactory
        {
        private:
            VkShaderFactory() {}
            VkShaderFactory(VkShaderFactory const&) {}
            VkShaderFactory const& operator= (VkShaderFactory const&) {}

            static VkShaderFactory* instance;

            void LoadSpv();
            uint32_t idCounter = 0;
            uint32_t AssignId();

            std::vector<ShaderModuleWrapper*> shaderModuleList;
            VkShaderModule* CreateShaderModule(uint32_t* shaderCode, size_t codeLength);

            void GetSpvNameList(const std::vector<std::string>& fileNameList);
            std::string GetSpvName(const std::string& shaderName);

            std::vector<std::string> spvNames;

        public:
            void Init();
            void DeInit();
            void Update();
            static VkShaderFactory* GetInstance();
            ~VkShaderFactory();

            void GetShaderIds(char** shaderName, Core::Enums::ShaderType* type, uint32_t* id, const uint32_t& shaderCount);
            VkShaderModule* GetShaderModule(const uint32_t& id);
            VkShaderModule* GetShaderModule(const std::string& shaderName, const Core::Enums::ShaderType& type);
            VkShaderStageFlagBits GetShaderStageFlag(const uint32_t& id);

        };
    }
}
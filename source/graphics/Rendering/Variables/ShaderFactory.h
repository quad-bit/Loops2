#pragma once
#include <BitArray.h>
#include <RenderingWrapper.h>
#include <vector>
#include <Assertion.h>
#include <map>
#include <string>

enum class ShaderType;
class Shader;

#if (RENDERING_API == VULKAN)
class VulkanInterface;
typedef VulkanInterface ApiInterface;
#elif (RENDERING_API == DX)
class DxInterface;
typedef DxInterface ApiInterface;
#endif

class ShaderFactory
{
private:
    ShaderFactory(){}
    ShaderFactory(ShaderFactory const &) {}
    ShaderFactory const & operator= (ShaderFactory const &) {}

    static ShaderFactory* instance;

    ApiInterface * apiInterface;

    std::vector<Shader *> shaderList;
    

public:
    void Init(ApiInterface * apiInterface);
    void DeInit();
    void Update();
    static ShaderFactory* GetInstance();
    ~ShaderFactory();

    //deprecated
    std::vector<SetWrapper*> CreateShader(const uint32_t & meshId, Shader * shaders, 
        const uint32_t & shaderCount, const RenderPassTag & tag);
    std::vector<SetWrapper*> CreateShader(const uint32_t & meshId, Shader * shaders,
        const uint32_t & shaderCount, const uint16_t & tagMask);

    void AddMeshToShader(const uint32_t & meshId, Shader * shaders, const uint32_t & shaderCount);
};



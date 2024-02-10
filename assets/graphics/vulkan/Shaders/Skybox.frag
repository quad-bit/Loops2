#version 400 
#extension GL_ARB_separate_shader_objects : enable 
#extension GL_ARB_shading_language_420pack : enable 

layout (set = 3, binding = 0) uniform samplerCube samplerCubeMap;

layout (location = 0) in vec3 inTexcoord;

layout (location = 0) out vec4 outFragColor;

void main() 
{
    outFragColor = texture(samplerCubeMap, inTexcoord);
}
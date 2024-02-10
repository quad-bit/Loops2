#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_GOOGLE_include_directive : enable

layout (location = 0) in vec3 inPos;

layout (std140, set = 0, binding = 0) uniform View
{
    mat4 view;
    mat4 projection;
    vec3 cameraPos;
} view;

layout (location = 0) out vec3 outTexcoord;

void main() 
{
    mat4 clip = mat4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f, 0.0f,
        0.0f, 0.0f, 0.5f, 1.0f
    );

    outTexcoord = inPos;
    mat4 viewMat = mat4(mat3(view.view));
    vec4 pos = clip * view.projection * viewMat * vec4(inPos.xyz, 1.0);
    gl_Position = pos.xyww;
    vec3 cam = view.cameraPos; // so that spir cross does not ignore it
}
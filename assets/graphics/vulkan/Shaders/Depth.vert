#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_GOOGLE_include_directive : enable

layout (std140, set = 0, binding = 0) uniform View
{
    mat4 view;
    mat4 projection;
    vec3 cameraPos;
} view;

layout(std140, set = 4, binding = 0) uniform Transform
{
    mat4 model;
}transform;

layout (location = 0) in vec3 pos;

void main()
{
    mat4 clip = mat4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f, 0.0f,
        0.0f, 0.0f, 0.5f, 1.0f
    );

   gl_Position = view.projection * view.view * transform.model * vec4(pos.xyz, 1.0);
   vec3 cam = view.cameraPos; // so that spir cross does not ignore it
}

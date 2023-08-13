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

layout (location = 0) in vec4 pos;
layout (location = 1) in vec4 inColor;
layout (location = 2) in vec3 inNormal;

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec3 outNormal;
layout (location = 2) out vec4 fragPos;
layout (location = 3) out vec3 viewPos;

void main()
{
    mat4 clip = mat4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f, 0.0f,
        0.0f, 0.0f, 0.5f, 1.0f
    );
    
    mat3 normalMat = mat3(transpose(inverse(transform.model))); // needs to be calculated on cpu side, normal Mat

    outColor = inColor;
    fragPos = transform.model * vec4(pos.xyz, 1.0);
    gl_Position = clip * view.projection * view.view * fragPos;
    outNormal = normalMat * inNormal;
    viewPos = view.cameraPos;
}

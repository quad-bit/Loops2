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
    uint parentType;
}transform;

layout (location = 0) in vec4 pos;

layout (location = 0) out vec4 outColor;

void main()
{
    if(transform.parentType == 0)//CAMERA
    {
        outColor = vec4(0.0f, .5f, .2f, 1.0f);
    }
    else if(transform.parentType == 6)//LIGHT
    {
        outColor = vec4(.5f, 0.0f, .2f, 1.0f);
    }
    else if(transform.parentType == 4)//MESH RENDERER
    {
        outColor = vec4(.2f);
    }

    gl_Position = view.projection * view.view * transform.model * vec4(pos.xyz, 1.0);
    vec3 cam = view.cameraPos; // so that spir cross does not ignore it
}

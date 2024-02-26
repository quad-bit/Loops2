#version 400 
#extension GL_ARB_separate_shader_objects : enable 
#extension GL_ARB_shading_language_420pack : enable 

layout (location = 0) in vec4 inColor; 
layout (location = 1) in vec2 inFragTexCoord;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec3 inViewPos;
layout (location = 4) in vec4 inTangent;
layout (location = 5) in vec4 inWorldSpacePos;

layout (location = 0) out vec4 outColor;

struct PointLight
{
    vec3 lightPos;
    float lightRadius;
    vec4 lightColor;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

const int NUM_LIGHTS = 5;

layout(std140, set = 2, binding = 0) uniform lightUniform
{
    PointLight pointLights[NUM_LIGHTS];
};

layout(set = 3, binding = 0) uniform sampler2D diffuseSampler;
layout(set = 3, binding = 1) uniform sampler2D normalSampler;

struct Surface
{
    vec3 specular; // will be replaced by specular map
    float shininess;
}surface;

void main() 
{
    surface.specular = vec3(1.0);
    surface.shininess = 32;

    vec4 color = texture(diffuseSampler, inFragTexCoord) * inColor;

    vec3 viewDir = normalize(inViewPos - inWorldSpacePos.xyz);

    vec3 N = normalize(inNormal);
    vec3 T = normalize(inTangent.xyz);
    vec3 B = cross(inNormal, inTangent.xyz) * inTangent.w;
    mat3 TBN = mat3(T, B, N);
    N = TBN * normalize(texture(normalSampler, inFragTexCoord).xyz * 2.0 - vec3(1.0));

    vec3 V = normalize(viewDir);

    vec3 diffuse = vec3(0.0);
    vec3 ambient = vec3(0.0);
    vec3 specular = vec3(0.0);

    for(int i = 0; i < NUM_LIGHTS; i++)
    {
        vec3 lightAmbient = pointLights[i].ambient.xyz;
        vec3 lightDiffuse = pointLights[i].diffuse.xyz; 
        vec3 lightSpecular = pointLights[i].specular.xyz; 
        vec3 lightPos = pointLights[i].lightPos.xyz; 

        vec3 lightDir = normalize(lightPos.xyz - inWorldSpacePos.xyz);
        vec3 L = normalize(lightDir);
        vec3 R = reflect(-L, N);

        float distanceVal = distance(lightPos.xyz, inWorldSpacePos.xyz);
        bool testRadius = true;
        if(testRadius)
        {
            if(distanceVal < pointLights[i].lightRadius)
            {
                diffuse += lightDiffuse * max(dot(N, L), 0.0);// * 1.0f/(distanceVal);
                specular += pow(max(dot(R, V), 0.0), surface.shininess) * lightSpecular * surface.specular;// * 1.0f/(distanceVal);
            }
        }
        else
        {
            diffuse += lightDiffuse * max(dot(N, L), 0.0);
            specular += pow(max(dot(R, V), 0.0), surface.shininess) * lightSpecular * surface.specular;
        }


        ambient += lightAmbient;
    }

    outColor = vec4((diffuse + specular + ambient) * color.rgb, color.a);
}



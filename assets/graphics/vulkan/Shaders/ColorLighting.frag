#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec4 color;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 fragPos;
layout (location = 3) in vec3 viewPos;
layout (location = 4) in vec4 lightPos;
layout (location = 5) in vec3 ambient;
layout (location = 6) in vec3 specular;
layout (location = 7) in vec3 diffuse;
layout (location = 8) in float beamHeight;
layout (location = 9) in float beamRadius;
layout (location = 10) in vec3 lightForward;


layout (location = 0) out vec4 outColor;

/*
layout(set = 2, binding = 1) uniform sampler2D combined_shadowSampler;


float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(combined_shadowSampler, projCoords.xy).r; 
    
    float shadow = 0.0;

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos).xyz;
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.003);

    // PCF
    vec2 texelSize = 1.0 / textureSize(combined_shadowSampler, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(combined_shadowSampler, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}
*/

void main()
{
   // ambient
   //vec3 ambient = light.ambient.xyz;
   //vec3 diffuse = light.diffuse.xyz; 
   //vec3 specular = light.specular.xyz; 
   //vec3 lightPos = light.lightPos.xyz; 

   mat4 biasMatrix = mat4(
    0.5, 0.0, 0.0, 0.0,
    0.0, 0.5, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    0.5, 0.5, 0.0, 1.0
    );
  
   //vec4 fragPosLightSpace = biasMatrix * light.lightSpaceMat * vec4(fragPos.xyz, 1.0);
   //float shadow = ShadowCalculation(fragPosLightSpace); 

   vec3 norm = normalize(normal);
   vec3 lightDir = normalize(lightPos.xyz - fragPos.xyz);
   vec3 viewDir = normalize(viewPos - fragPos.xyz);

   // diffuse 
   float diff = max(dot(norm, lightDir), 0.0);
   vec3 diffuseVal;
   
   //specular       
   vec3 reflectDir = reflect(-lightDir, norm);  
   float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
   vec3 specularVal = spec * specular.xyz;

   // check if the fragment lies in the cone
   float distanceOnAxis = max(dot(-lightDir, normalize(lightForward.xyz)), 0.0);
   
   bool spotLight = true;

   if(!spotLight)
   {
       diffuseVal = diff * diffuse.xyz; 
       specularVal = spec * specular.xyz; 
   }
   else
   {
       if(distanceOnAxis > 0 && distanceOnAxis < beamHeight)
       {
           float distanceFromAxis = length(lightForward.xyz * distanceOnAxis - ( -lightDir));
           float radiusAtFragment = (distanceOnAxis * beamRadius)/beamHeight;

           if(distanceFromAxis < radiusAtFragment)
           {
               //float mixPercentage = cosh(2 * pow(distanceFromAxis/radiusAtFragment, 2)) - 1;
               float mixPercentage = cosh(1.4 * pow(distanceFromAxis/radiusAtFragment, 2)) - 1;
               diffuseVal = mix(diff * diffuse.xyz, vec3(0.04), mixPercentage); 
               specularVal = spec * specular.xyz; 
           }
           else
           {
               diffuseVal = vec3(0.0f);
               specularVal = vec3(0.0f, 0.0, 0.0);
           }
       }
       else
       {
           diffuseVal = vec3(0.0f, 0.0, 0.0);
           specularVal = vec3(0.0f, 0.0, 0.0);
       }
   }
   
   //vec3 result = (ambient + (1.0 - shadow) * (diffuseVal + specularVal)) * color.xyz;
   vec3 result = (ambient + diffuseVal + specularVal) * color.xyz;
    
   outColor = vec4(result, 1.0); 
}

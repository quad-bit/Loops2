#pragma once
#include "ECS/Component.h"
#include <glm\glm.hpp>

namespace ECS
{
    namespace Components
    {

        class Transform;

        enum class LightType
        {
            Point,
            Directional,
            SpotLight
        };

        enum class SpreadEquation
        {
            Circle = 0,
            Square = 1,
        };

        struct LightUniform
        {
            glm::vec4 lightPos;
            glm::vec4 lightDir;
            glm::vec4 ambient;
            glm::vec4 diffuse;
            glm::vec4 specular;
            glm::mat4 lightSpaceMat;
            float beamHeight;
            float beamRadius;
        };

        class Light : public Component<Light>
        {
        private:
            Transform* transform;

            LightType lightType;

            //valid in case of directional and spot light
            SpreadEquation equation;

            float beamRadius;
            float beamHeight;

            float intensity;
            // meant to be used on c++ side to check if the object should be lit or not
            float range; // should be directly proportional to the intensity
            float effectiveRange; // max( intensity * range, range) 

            glm::vec3 ambient;
            glm::vec3 diffuse;
            glm::vec3 specular;

            Light() = delete;
            Light(const Light&) = delete;
            Light& operator=(const Light&) = delete;

        public:
            Light(Transform* transform, LightType lightType = LightType::Point,
                SpreadEquation equation = SpreadEquation::Circle)
            {
                this->transform = transform;
                this->lightType = lightType;
                intensity = 0.5f;
                range = 5.0f;
                ambient = glm::vec3(0.6f, 0.6f, 0.6f);
                diffuse = glm::vec3(0.45f, 0.45f, 0.45f);
                specular = glm::vec3(0.45f, 0.45f, 0.45f);
                this->beamRadius = 15.0;
                this->beamHeight = 20.0;

                componentType = COMPONENT_TYPE::LIGHT;
            }

            void SetEquationType(const SpreadEquation& equation)
            {
                this->equation = equation;
            }

            void SetRadius(const float& rad)
            {
                this->beamRadius = rad;
            }

            void SetWidth(const float& beamWidth)
            {
                this->beamHeight = beamWidth;
            }

            void SetDiffuse(const glm::vec3& strength)
            {
                this->diffuse = strength;
            }

            void SetSpecular(const glm::vec3& strength)
            {
                this->specular = strength;
            }

            void SetRange(const float& range)
            {
                this->range = range;
            }

            void SetIntensity(const float& intensity)
            {
                ASSERT_MSG_DEBUG(intensity > 0.0f && intensity < 1.0f, "intensity should be [0,1]");
                this->intensity = intensity;
            }

            float GetEffectiveRange()
            {
                ASSERT_MSG(0, "yet to be implemented");
                return 0.0f;// glm::max(range, intensity * range);
            }

            Transform* const GetTransform()
            {
                return transform;
            }

            glm::vec3 GetSpecular()
            {
                return this->specular;
            }

            glm::vec3 GetAmbient()
            {
                return this->ambient;
            }

            glm::vec3 GetDiffuse()
            {
                return this->diffuse;
            }

            float GetBeamRadius()
            {
                return this->beamRadius;
            }

            float GetBeamHeight()
            {
                return this->beamHeight;
            }
        };
    }
}
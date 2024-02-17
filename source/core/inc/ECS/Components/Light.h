#pragma once
#include "ECS/Component.h"
#include "Platform/Assertion.h"
#include <glm/glm.hpp>
#include <memory>

namespace Core
{
    namespace ECS
    {
        namespace Components
        {
            enum class LightType
            {
                Point,
                Directional,
                SpotLight
            };

            struct LightCategory
            {
                glm::vec4 m_ambient;
                glm::vec4 m_diffuse;
                glm::vec4 m_specular;
                glm::vec4 m_color;
                LightType m_type;

                LightCategory(const LightType& type) : m_type(type)
                {
                    m_ambient = glm::vec4(.01f);
                    m_diffuse = glm::vec4(.2f);
                    m_specular = glm::vec4(.2f);
                    m_color = glm::vec4(.20f);
                }
            };

            struct SpotLight : LightCategory
            {
                float m_beamHeight = 10.0f;
                float m_beamRadius = 10.0f;
                SpotLight() : LightCategory(LightType::SpotLight)
                {
                }
            };

            struct PointLight : LightCategory
            {
                float m_lightRadius = 10.0f;
                PointLight() : LightCategory(LightType::Point)
                {
                }
            };

            class Light : public Component<Light>
            {
            private:

                std::unique_ptr<LightCategory> m_lightCatergory;

                Light() = delete;
                Light(const Light&) = delete;
                Light& operator=(const Light&) = delete;

            public:
                Light(std::unique_ptr<LightCategory>& category)
                {
                    m_lightCatergory = std::move(category);
                    componentType = COMPONENT_TYPE::LIGHT;
                }

                const LightType& GetLightType() const
                {
                    return m_lightCatergory->m_type;
                }

                const LightCategory* GetLightCategory() const
                {
                    return m_lightCatergory.get();
                }

                ~Light()
                {
                    m_lightCatergory.reset();
                }
            };
        }
    }
}
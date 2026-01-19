#ifndef MATERIAL_HPP_RM89ZWAD
#define MATERIAL_HPP_RM89ZWAD

#include <glm/glm.hpp>

#include <array>

namespace material
{
    struct Material
    {
        glm::vec3 m_ambient;
        glm::vec3 m_diffuse;
        glm::vec3 m_specular;
        float     m_shininess;
    };

    enum class MaterialType
    {
        EMERALD,
        JADE,
        OBSIDIAN,
        TURQOISE,
        BRASS,
        BRONZE,
        CHROME,
        COPPER,
        GOLD,
        SILVER,

        numOfMaterialTypes
    };

    inline constexpr Material emerald{
        .m_ambient   = { 0.0215f, 0.1745f, 0.0215f },
        .m_diffuse   = { 0.07568f, 0.61424f, 0.7568f },
        .m_specular  = { 0.633f, 0.727811f, 0.633f },
        .m_shininess = 0.6f * 128.0f
    };

    inline constexpr Material jade{
        .m_ambient   = { 0.135f, 0.2225f, 0.1575f },
        .m_diffuse   = { 0.54f, 0.89f, 0.63f },
        .m_specular  = { 0.316228f, 0.316228f, 0.316228f },
        .m_shininess = 0.1f * 128.0f
    };

    inline constexpr Material obsidian{
        .m_ambient   = { 0.05375f, 0.05f, 0.06625f },
        .m_diffuse   = { 0.18275f, 0.17f, 0.22525f },
        .m_specular  = { 0.332741f, 0.328634f, 0.346435f },
        .m_shininess = 0.3f * 128.0f
    };

    inline constexpr Material pearl{
        .m_ambient   = { 0.25f, 0.20725f, 0.20725f },
        .m_diffuse   = { 1.0f, 0.829f, 0.829f },
        .m_specular  = { 0.296648f, 0.296648f, 0.296648f },
        .m_shininess = 0.088f * 128.0f
    };

    inline constexpr Material ruby{
        .m_ambient   = { 0.1745f, 0.01175f, 0.01175f },
        .m_diffuse   = { 0.61424f, 0.04136f, 0.04136f },
        .m_specular  = { 0.727811f, 0.626959f, 0.626959f },
        .m_shininess = 0.6f * 128.0f
    };

    inline constexpr Material turqoise{
        .m_ambient   = { 0.1f, 0.18725f, 0.1745f },
        .m_diffuse   = { 0.396f, 0.74151f, 0.69102f },
        .m_specular  = { 0.297254f, 0.30829f, 0.306678f },
        .m_shininess = 0.1f * 128.0f
    };

    inline constexpr Material brass{
        .m_ambient   = { 0.329412f, 0.223529f, 0.027451f },
        .m_diffuse   = { 0.780392f, 0.568627f, 0.113725f },
        .m_specular  = { 0.992157f, 0.941176f, 0.807843f },
        .m_shininess = 0.21794872f * 128.0f
    };

    inline constexpr Material bronze{
        .m_ambient   = { 0.2125f, 0.1275f, 0.054f },
        .m_diffuse   = { 0.714f, 0.4284f, 0.18144f },
        .m_specular  = { 0.393548f, 0.271906f, 0.166721 },
        .m_shininess = 0.2f * 128.0f
    };

    inline constexpr Material chrome{
        .m_ambient   = { 0.25f, 0.25f, 0.25f },
        .m_diffuse   = { 0.4f, 0.4f, 0.4f },
        .m_specular  = { 0.774597f, 0.774597f, 0.774597f },
        .m_shininess = 0.6f * 128.0f
    };

    inline constexpr Material copper{
        .m_ambient   = { 0.19125f, 0.0735f, 0.0225f },
        .m_diffuse   = { 0.7038f, 0.27048f, 0.0828f },
        .m_specular  = { 0.256777f, 0.137622f, 0.086014f },
        .m_shininess = 0.1f * 128.0f
    };

    inline constexpr Material gold{
        .m_ambient   = { 0.24725f, 0.1995f, 0.0745f },
        .m_diffuse   = { 0.75164f, 0.60648f, 0.22648f },
        .m_specular  = { 0.628281f, 0.555802f, 0.366065f },
        .m_shininess = 0.4f * 128.0f
    };

    inline constexpr Material silver{
        .m_ambient   = { 0.19225f, 0.19225f, 0.19225f },
        .m_diffuse   = { 0.50754f, 0.50754f, 0.50754f },
        .m_specular  = { 0.508273f, 0.508273f, 0.508273f },
        .m_shininess = 0.4f * 128.0f
    };

    inline const Material& get(MaterialType type)
    {
        using enum MaterialType;
        // clang-format off
        switch (type) {
        case MaterialType::EMERALD:  return emerald;
        case MaterialType::JADE:     return jade;
        case MaterialType::OBSIDIAN: return obsidian;
        case MaterialType::TURQOISE: return turqoise;
        case MaterialType::BRASS:    return brass;
        case MaterialType::BRONZE:   return bronze;
        case MaterialType::CHROME:   return chrome;
        case MaterialType::COPPER:   return copper;
        case MaterialType::GOLD:     return gold;
        case MaterialType::SILVER:   return silver;
        case MaterialType::numOfMaterialTypes: std::unreachable();
        }
        // clang-format on

        std::unreachable();
    }
}

#endif /* end of include guard: MATERIAL_HPP_RM89ZWAD */

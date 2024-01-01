#ifndef UNIFORM_BUFFER_HPP_LAZMTXKP
#define UNIFORM_BUFFER_HPP_LAZMTXKP

#include <concepts>
#include <cstddef>
#include <format>
#include <map>
#include <math.h>
#include <unordered_set>

#include <glbinding/gl/gl.h>
#include <glm/glm.hpp>

#include "shader.hpp"

namespace std140
{
#define MakeType(BaseType, Name, Align)                      \
    {                                                        \
        using Value                              = BaseType; \
        static constexpr std::size_t s_alignment = Align;    \
        static constexpr std::string s_name      = Name;     \
                                                             \
        Value m_value;                                       \
    }

    // clang-format off
    struct Bool  MakeType(gl::GLboolean, "bool",     sizeof(gl::GLboolean));
    struct Int   MakeType(gl::GLint,     "int",      sizeof(gl::GLint));
    struct Uint  MakeType(gl::GLuint,    "uint",     sizeof(gl::GLuint));
    struct Float MakeType(gl::GLfloat,   "float",    sizeof(gl::GLfloat));
    struct Vec2  MakeType(glm::vec2,     "vec2",     sizeof(glm::vec2));
    struct Vec3  MakeType(glm::vec3,     "vec3", 4 * sizeof(glm::vec4::value_type));
    struct Vec4  MakeType(glm::vec3,     "vec4",     sizeof(glm::vec4));
    struct Mat4  MakeType(glm::mat4,     "mat4",     sizeof(glm::mat4));
    // clang-format on

#undef MakeType

    namespace detail
    {
        // check if types are unieque (quadratic complexity)
        template <typename T, typename... Types>
        constexpr bool are_types_unique_v = (!std::is_same_v<T, Types> && ...) && are_types_unique_v<Types...>;
        template <typename T>
        constexpr bool are_types_unique_v<T> = true;

        // check if any of U derived from T
        template <typename T, typename... U>
        constexpr bool contains_v = (std::derived_from<U, T> || ...);

        // check if T is derived from any types defined above
        template <typename T>
        constexpr bool contains_std140_v = detail::contains_v<T, Bool, Int, Uint, Float, Vec2, Vec3, Vec4, Mat4>;
    }

    template <typename... T>
    concept UniqueTypes = detail::are_types_unique_v<T...>;

    template <typename... T>
    concept ContainsStd140Type = (detail::contains_std140_v<T> || ...);
}

template <typename... Args>
    requires std140::ContainsStd140Type<Args...> && std140::UniqueTypes<Args...>
class UniformBuffer
{
private:
    inline static gl::GLuint s_bindingPointCounter{ 0 };

public:
    const gl::GLuint m_id;
    const gl::GLuint m_bindingPoint;

private:
    std::tuple<Args...> m_data;

private:
    static constexpr std::size_t roundUpRemainder(std::size_t x, std::size_t multiple)
    {
        std::size_t remainder{ x % multiple };
        if (remainder == 0) { return 0; }
        return multiple - remainder;
    }

    template <typename T>
        requires std140::ContainsStd140Type<T>
    static constexpr std::size_t getOffset()
    {
        bool        found{ false };
        std::size_t offset{ 0 };

        ([&] {
            if (std::same_as<T, Args>) {
                found = true;
            } else {
                offset += found ? 0 : sizeof(typename Args::Value) + roundUpRemainder(offset, Args::s_alignment);
            }
        }(),
         ...);

        return offset + roundUpRemainder(offset, T::s_alignment);
    }

    static constexpr std::size_t getGpuSize()
    {
        std::size_t size{ 0 };
        ([&] {
            size += (sizeof(Args::Value) + roundUpRemainder(size, Args::s_alignment));
        },
         ...);
        return size;
    }

public:
    UniformBuffer()
        : m_id{ [] {
            // gl::GLuint id;
            // gl::glGenBuffers(1, &id);
            // return id;
            return 0;
        }() }
        , m_bindingPoint{ s_bindingPointCounter++ }
    {
        // gl::glBindBuffer(gl::GL_UNIFORM_BUFFER, m_id);
        // gl::glBufferData(gl::GL_UNIFORM_BUFFER, blockSize, nullptr, gl::GL_STATIC_DRAW);
        // gl::glBindBuffer(gl::GL_UNIFORM_BUFFER, 0);

        // gl::glBindBufferBase(gl::GL_UNIFORM_BUFFER, m_bindingPoint, m_id);
        // // gl::glBindBufferRange(gl::GL_UNIFORM_BUFFER, m_bindingPoint, m_id, 0, blockSize);    // equivalent as above
    }

    ~UniformBuffer()
    {
        gl::glDeleteBuffers(1, &m_id);
    }

public:
    UniformBuffer& bind(Shader& shader, const std::string blockName)
    {
        auto blockIndex{ gl::glGetUniformBlockIndex(shader.m_id, blockName.c_str()) };
        gl::glUniformBlockBinding(shader.m_id, blockIndex, m_bindingPoint);
        return *this;
    }

    template <typename T>
    typename T::Value& get()
    {
        return std::get<T>(m_data).m_value;
    }

    template <typename T>
    const typename T::Value& get() const
    {
        return std::get<T>(m_data).m_value;
    }
};

#endif /* end of include guard: UNIFORM_BUFFER_HPP_LAZMTXKP */

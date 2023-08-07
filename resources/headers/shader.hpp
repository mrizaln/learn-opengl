#ifndef SHADER_HPP_CM510QXM
#define SHADER_HPP_CM510QXM

#include <array>
#include <concepts>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>

#include <glbinding/gl/gl.h>
#include <glm/glm.hpp>

template <typename GLtype>
concept UniformValueType = ((std::same_as<GLtype, gl::GLfloat>
                             || std::same_as<GLtype, gl::GLdouble>
                             || std::same_as<GLtype, gl::GLint>
                             || std::same_as<GLtype, gl::GLuint>
                             || std::same_as<GLtype, bool>))
                           && std::is_fundamental_v<GLtype>;

template <typename Float>
concept UniformMatType = std::same_as<Float, gl::GLfloat> || std::same_as<Float, gl::GLdouble>;

class Shader
{
private:
    enum class ShaderStage
    {
        VERTEX,
        FRAGMENT,
        GEOMETRY,
    };

public:
    gl::GLuint m_id;

public:
    Shader() = delete;

    Shader(
        std::filesystem::path                vsPath,
        std::filesystem::path                fsPath,
        std::optional<std::filesystem::path> gsPath = {}
    )
    {
        std::string   vsSource;
        std::ifstream vsFile{ vsPath };
        if (!vsFile) {
            std::cerr << "Error reading vertex shader file: " << vsPath << '\n';
        } else {
            std::stringstream buffer;
            buffer << vsFile.rdbuf();
            vsSource = buffer.str();
        }

        std::string   fsSource;
        std::ifstream fsFile{ fsPath };
        if (!fsFile) {
            std::cerr << "Error reading fragment shader file: " << fsPath << '\n';
        } else {
            std::stringstream buffer;
            buffer << fsFile.rdbuf();
            fsSource = buffer.str();
        }

        auto vsId{ prepareShader(vsSource, ShaderStage::VERTEX) };
        auto fsId{ prepareShader(fsSource, ShaderStage::FRAGMENT) };

        auto gsId = [&]() -> std::optional<gl::GLuint> {
            if (gsPath) {
                std::string   gsSource;
                std::ifstream gsFile{ gsPath.value() };
                if (!gsFile) {
                    std::cerr << "Error reading fragment shader file: " << gsPath.value() << '\n';
                    return {};
                } else {
                    std::stringstream buffer;
                    buffer << gsFile.rdbuf();
                    gsSource = buffer.str();
                }
                return prepareShader(gsSource, ShaderStage::GEOMETRY);
            } else {
                return {};
            }
        }();

        // create shader program then link shaders to it
        m_id = gl::glCreateProgram();
        gl::glAttachShader(m_id, vsId);
        gl::glAttachShader(m_id, fsId);
        if (gsId) { gl::glAttachShader(m_id, gsId.value()); }
        gl::glLinkProgram(m_id);
        shaderLinkInfo(m_id);

        // delete shader objects
        gl::glDeleteShader(vsId);
        gl::glDeleteShader(fsId);
        if (gsId) { gl::glDeleteShader(gsId.value()); }
    }

    ~Shader()
    {
        gl::glDeleteProgram(m_id);
    }

public:
    void use()
    {
        gl::glUseProgram(m_id);
    }

    // glm vector
    // clang-format off
    template <UniformValueType Type> void setUniform(const std::string& name, const glm::vec<2, Type>& vec) { setUniform_vec_impl<Type, 2>(name, &vec[0]); }
    template <UniformValueType Type> void setUniform(const std::string& name, const glm::vec<3, Type>& vec) { setUniform_vec_impl<Type, 3>(name, &vec[0]); }
    template <UniformValueType Type> void setUniform(const std::string& name, const glm::vec<4, Type>& vec) { setUniform_vec_impl<Type, 4>(name, &vec[0]); }

    // glm::matrix
    template <UniformMatType Type> void setUniform(const std::string& name, const glm::mat<2, 2, Type>& mat4) { setUniform_mat_impl<Type, 2>(name, mat4); }
    template <UniformMatType Type> void setUniform(const std::string& name, const glm::mat<3, 3, Type>& mat3) { setUniform_mat_impl<Type, 3>(name, mat3); }
    template <UniformMatType Type> void setUniform(const std::string& name, const glm::mat<4, 4, Type>& mat4) { setUniform_mat_impl<Type, 4>(name, mat4); }

    // simple array; 2 to 4 elements
    template <UniformValueType Type, std::size_t N> requires(N >= 2 && N <= 4) void setUniform(const std::string& name, const std::array<Type, N>& value) { setUniform_vec_impl<Type, N>(name, &value[0]); }
    // clang-format on

    // one value
    template <UniformValueType Type>
    void setUniform(const std::string& name, Type value)
    {
        gl::GLint loc{ gl::glGetUniformLocation(m_id, name.c_str()) };
        if (loc == -1) {
            std::cerr << std::format("Shader [{}]: Uniform of name '{}' can't be found\n", m_id, name);
        }

        // clang-format off
        if      constexpr (std::same_as<Type, gl::GLfloat>)  gl::glUniform1f(loc, value);
        else if constexpr (std::same_as<Type, gl::GLdouble>) gl::glUniform1d(loc, value);
        else if constexpr (std::same_as<Type, gl::GLint>)    gl::glUniform1i(loc, value);
        else if constexpr (std::same_as<Type, bool>)         gl::glUniform1i(loc, value);
        else if constexpr (std::same_as<Type, gl::GLuint>)   gl::glUniform1ui(loc, value);
        // clang-format on
    }

    // two values (use array)
    template <UniformValueType Type>
    void setUniform(const std::string& name, Type v0, Type v1)
    {
        std::array value{ v0, v1 };
        setUniform<Type, 2>(name, value);
    }

    // three values (use array)
    template <UniformValueType Type>
    void setUniform(const std::string& name, Type v0, Type v1, Type v2)
    {
        std::array value{ v0, v1, v2 };
        setUniform<Type, 3>(name, value);
    }

    // four values (use array)
    template <UniformValueType Type>
    void setUniform(const std::string& name, Type v0, Type v1, Type v2, Type v3)
    {
        std::array value{ v0, v1, v2, v3 };
        setUniform<Type, 4>(name, value);
    }

private:
    void shaderCompileInfo(gl::GLuint shader, ShaderStage stage)
    {
        std::string_view name;
        switch (stage) {
        case ShaderStage::VERTEX: name = "VERTEX"; break;
        case ShaderStage::FRAGMENT: name = "FRAGMENT"; break;
        case ShaderStage::GEOMETRY: name = "GEOMETRY"; break;
        }

        gl::GLint status{};
        gl::glGetShaderiv(shader, gl::GL_COMPILE_STATUS, &status);
        if (status != 1) {
            gl::GLint maxLength{};
            gl::GLint logLength{};

            gl::glGetShaderiv(shader, gl::GL_INFO_LOG_LENGTH, &maxLength);
            auto log{ new gl::GLchar[(std::size_t)maxLength] };
            gl::glGetShaderInfoLog(shader, maxLength, &logLength, log);
            std::cerr << std::format("Shader compilation of type {} failed:\n{}\n", name, log);
            delete[] log;
        }
    }

    void shaderLinkInfo(gl::GLuint program)
    {
        gl::GLint status{};
        glGetProgramiv(program, gl::GL_LINK_STATUS, &status);
        if (status != 1) {
            gl::GLint maxLength{};
            gl::GLint logLength{};

            glGetProgramiv(program, gl::GL_INFO_LOG_LENGTH, &maxLength);
            auto log{ new gl::GLchar[(std::size_t)maxLength] };
            gl::glGetProgramInfoLog(program, maxLength, &logLength, log);
            std::cerr << "Program linking failed: \n"
                      << log << '\n';
            delete[] log;
        }
    }

    gl::GLuint prepareShader(const std::string& vsSource, ShaderStage stage)
    {
        gl::GLenum type;
        switch (stage) {
        case ShaderStage::VERTEX: type = gl::GL_VERTEX_SHADER; break;
        case ShaderStage::FRAGMENT: type = gl::GL_FRAGMENT_SHADER; break;
        case ShaderStage::GEOMETRY: type = gl::GL_GEOMETRY_SHADER; break;
        }

        // compile vertex shader
        gl::GLuint  vsId{ glCreateShader(type) };
        const char* vsSourceCharPtr{ vsSource.c_str() };
        gl::glShaderSource(vsId, 1, &vsSourceCharPtr, nullptr);
        gl::glCompileShader(vsId);
        shaderCompileInfo(vsId, stage);

        return vsId;
    }

    // vector
    template <UniformValueType Type, std::size_t N>
        requires(N >= 2 && N <= 4)
    void setUniform_vec_impl(const std::string& name, const Type* value)
    {
        gl::GLint loc{ gl::glGetUniformLocation(m_id, name.c_str()) };
        if (loc == -1) {
            std::cerr << std::format("Shader [{}]: Uniform of name '{}' can't be found\n", m_id, name);
        }

        // another C limitation, the const does not matter
        auto val{ const_cast<Type*>(value) };

        // clang-format off
        if      constexpr (std::same_as<Type, gl::GLfloat>  && N == 2) gl::glUniform2fv(loc, 1, val);
        else if constexpr (std::same_as<Type, gl::GLfloat>  && N == 3) gl::glUniform3fv(loc, 1, val);
        else if constexpr (std::same_as<Type, gl::GLfloat>  && N == 4) gl::glUniform4fv(loc, 1, val);

        else if constexpr (std::same_as<Type, gl::GLdouble> && N == 2) gl::glUniform2dv(loc, 1, val);
        else if constexpr (std::same_as<Type, gl::GLdouble> && N == 3) gl::glUniform3dv(loc, 1, val);
        else if constexpr (std::same_as<Type, gl::GLdouble> && N == 4) gl::glUniform4dv(loc, 1, val);

        else if constexpr (std::same_as<Type, gl::GLint>    && N == 2) gl::glUniform2iv(loc, 1, val);
        else if constexpr (std::same_as<Type, gl::GLint>    && N == 3) gl::glUniform3iv(loc, 1, val);
        else if constexpr (std::same_as<Type, gl::GLint>    && N == 4) gl::glUniform4iv(loc, 1, val);

        else if constexpr (std::same_as<Type, bool>         && N == 2) gl::glUniform2iv(loc, 1, val);
        else if constexpr (std::same_as<Type, bool>         && N == 3) gl::glUniform3iv(loc, 1, val);
        else if constexpr (std::same_as<Type, bool>         && N == 4) gl::glUniform4iv(loc, 1, val);

        else if constexpr (std::same_as<Type, gl::GLuint>   && N == 2) gl::glUniform2uiv(loc, 1, val);
        else if constexpr (std::same_as<Type, gl::GLuint>   && N == 3) gl::glUniform3uiv(loc, 1, val);
        else if constexpr (std::same_as<Type, gl::GLuint>   && N == 4) gl::glUniform4uiv(loc, 1, val);
        // clang-format on
    }

    // matrix
    template <UniformMatType Type, std::size_t N>
        requires(N >= 2 && N <= 4)
    void setUniform_mat_impl(const std::string& name, const glm::mat<N, N, Type>& mat)
    {
        gl::GLint loc{ gl::glGetUniformLocation(m_id, name.c_str()) };
        if (loc == -1) {
            std::cerr << std::format("Shader [{}]: Uniform of name '{}' can't be found\n", m_id, name);
        }
        // clang-format off
        if      constexpr (std::same_as<Type, gl::GLfloat> && N == 2) gl::glUniformMatrix2fv(loc, 1, gl::GL_FALSE, &mat[0][0]);
        else if constexpr (std::same_as<Type, gl::GLfloat> && N == 3) gl::glUniformMatrix3fv(loc, 1, gl::GL_FALSE, &mat[0][0]);
        else if constexpr (std::same_as<Type, gl::GLfloat> && N == 4) gl::glUniformMatrix4fv(loc, 1, gl::GL_FALSE, &mat[0][0]);

        else if constexpr (std::same_as<Type, gl::GLdouble> && N == 2) gl::glUniformMatrix2dv(loc, 1, gl::GL_FALSE, &mat[0][0]);
        else if constexpr (std::same_as<Type, gl::GLdouble> && N == 3) gl::glUniformMatrix3dv(loc, 1, gl::GL_FALSE, &mat[0][0]);
        else if constexpr (std::same_as<Type, gl::GLdouble> && N == 4) gl::glUniformMatrix4dv(loc, 1, gl::GL_FALSE, &mat[0][0]);
        // clang-format on
    }
};

#endif /* end of include guard: SHADER_HPP_CM510QXM */

#ifndef SHADER_HPP_CM510QXM
#define SHADER_HPP_CM510QXM

#include <array>
#include <concepts>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <type_traits>

#include <glbinding/gl/gl.h>

#define _shader_hpp_glUniform_(num, type) glUniform##num##type

template <typename GLtype>
concept UniformValueType = ((std::same_as<GLtype, gl::GLfloat>
                             || std::same_as<GLtype, gl::GLdouble>
                             || std::same_as<GLtype, gl::GLint>
                             || std::same_as<GLtype, gl::GLuint>
                             || std::same_as<GLtype, bool>))
                           && std::is_fundamental_v<GLtype>;

class Shader
{
public:
    gl::GLuint m_id;

public:
    Shader() = delete;

    Shader(std::filesystem::path vsPath, std::filesystem::path fsPath)
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

        prepareShader(vsSource, fsSource);
    }

    ~Shader()
    {
        gl::glDeleteProgram(m_id);
    }

    void use()
    {
        gl::glUseProgram(m_id);
    }

    template <UniformValueType Type, std::size_t N>
        requires(N >= 2 && N <= 4)
    void setUniform(const std::string& name, const std::array<Type, N>& value)
    {
        setUniform_impl<Type, N>(name, &value[0]);
    }

    template <UniformValueType Type>
    void setUniform(const std::string& name, Type value)
    {
        gl::GLint loc{ gl::glGetUniformLocation(m_id, name.c_str()) };

        // clang-format off
        if      constexpr (std::same_as<Type, gl::GLfloat>)  gl::glUniform1f(loc, value);
        else if constexpr (std::same_as<Type, gl::GLdouble>) gl::glUniform1d(loc, value);
        else if constexpr (std::same_as<Type, gl::GLint>)    gl::glUniform1i(loc, value);
        else if constexpr (std::same_as<Type, bool>)         gl::glUniform1i(loc, value);
        else if constexpr (std::same_as<Type, gl::GLuint>)   gl::glUniform1ui(loc, value);
        // clang-format on
    }

    template <UniformValueType Type>
    void setUniform(const std::string& name, Type v0, Type v1)
    {
        std::array value{ v0, v1 };
        setUniform<Type, 2>(name, value);
    }

    template <UniformValueType Type>
    void setUniform(const std::string& name, Type v0, Type v1, Type v2)
    {
        std::array value{ v0, v1, v2 };
        setUniform<Type, 3>(name, value);
    }

    template <UniformValueType Type>
    void setUniform(const std::string& name, Type v0, Type v1, Type v2, Type v3)
    {
        std::array value{ v0, v1, v2, v3 };
        setUniform<Type, 4>(name, value);
    }

private:
    void shaderCompileInfo(gl::GLuint shader)
    {
        gl::GLint status{};
        gl::glGetShaderiv(shader, gl::GL_COMPILE_STATUS, &status);
        if (status != 1) {
            gl::GLint maxLength{};
            gl::GLint logLength{};

            gl::glGetShaderiv(shader, gl::GL_INFO_LOG_LENGTH, &maxLength);
            auto log{ new gl::GLchar[(std::size_t)maxLength] };
            gl::glGetShaderInfoLog(shader, maxLength, &logLength, log);
            std::cerr << "Shader compilation failed: \n"
                      << log << '\n';
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

    void prepareShader(const std::string& vsSource, const std::string& fsSource)
    {
        // compile vertex shader
        gl::GLuint  vsId{ glCreateShader(gl::GL_VERTEX_SHADER) };
        const char* vsSourceCharPtr{ vsSource.c_str() };
        gl::glShaderSource(vsId, 1, &vsSourceCharPtr, nullptr);
        gl::glCompileShader(vsId);
        shaderCompileInfo(vsId);

        // compile fragment shader
        gl::GLuint  fsId{ glCreateShader(gl::GL_FRAGMENT_SHADER) };
        const char* fsSourceCharPtr{ fsSource.c_str() };
        gl::glShaderSource(fsId, 1, &fsSourceCharPtr, nullptr);
        gl::glCompileShader(fsId);
        shaderCompileInfo(fsId);

        // create shader program then link shaders to it
        m_id = gl::glCreateProgram();
        gl::glAttachShader(m_id, vsId);
        gl::glAttachShader(m_id, fsId);
        gl::glLinkProgram(m_id);
        shaderLinkInfo(m_id);

        // delete shader objects
        gl::glDeleteShader(vsId);
        gl::glDeleteShader(fsId);
    }

    template <UniformValueType Type, std::size_t N>
        requires(N >= 2 && N <= 4)
    void setUniform_impl(const std::string& name, const Type* value)
    {
        gl::GLint loc{ gl::glGetUniformLocation(m_id, name.c_str()) };
        auto      val{ const_cast<Type*>(value) };

        // clang-format off
        // if      constexpr (std::same_as<Type, gl::GLfloat>  && N == 1) gl::glUniform1fv(loc, 1, &val[0]);
        // else if constexpr (std::same_as<Type, gl::GLfloat>  && N == 2) gl::glUniform2fv(loc, 1, &val[0]);
        if      constexpr (std::same_as<Type, gl::GLfloat>  && N == 2) gl::glUniform2fv(loc, 1, &val[0]);
        else if constexpr (std::same_as<Type, gl::GLfloat>  && N == 3) gl::glUniform3fv(loc, 1, &val[0]);
        else if constexpr (std::same_as<Type, gl::GLfloat>  && N == 4) gl::glUniform4fv(loc, 1, &val[0]);

        // else if constexpr (std::same_as<Type, gl::GLdouble> && N == 1) gl::glUniform1dv(loc, 1, &val[0]);
        else if constexpr (std::same_as<Type, gl::GLdouble> && N == 2) gl::glUniform2dv(loc, 1, &val[0]);
        else if constexpr (std::same_as<Type, gl::GLdouble> && N == 3) gl::glUniform3dv(loc, 1, &val[0]);
        else if constexpr (std::same_as<Type, gl::GLdouble> && N == 4) gl::glUniform4dv(loc, 1, &val[0]);

        // else if constexpr (std::same_as<Type, gl::GLint>    && N == 1) gl::glUniform1iv(loc, 1, &val[0]);
        else if constexpr (std::same_as<Type, gl::GLint>    && N == 2) gl::glUniform2iv(loc, 1, &val[0]);
        else if constexpr (std::same_as<Type, gl::GLint>    && N == 3) gl::glUniform3iv(loc, 1, &val[0]);
        else if constexpr (std::same_as<Type, gl::GLint>    && N == 4) gl::glUniform4iv(loc, 1, &val[0]);

        // else if constexpr (std::same_as<Type, bool>         && N == 1) gl::glUniform1iv(loc, 1, &val[0]);
        else if constexpr (std::same_as<Type, bool>         && N == 2) gl::glUniform2iv(loc, 1, &val[0]);
        else if constexpr (std::same_as<Type, bool>         && N == 3) gl::glUniform3iv(loc, 1, &val[0]);
        else if constexpr (std::same_as<Type, bool>         && N == 4) gl::glUniform4iv(loc, 1, &val[0]);

        // else if constexpr (std::same_as<Type, gl::GLuint>   && N == 1) gl::glUniform1uiv(loc, 1, &val[0]);
        else if constexpr (std::same_as<Type, gl::GLuint>   && N == 2) gl::glUniform2uiv(loc, 1, &val[0]);
        else if constexpr (std::same_as<Type, gl::GLuint>   && N == 3) gl::glUniform3uiv(loc, 1, &val[0]);
        else if constexpr (std::same_as<Type, gl::GLuint>   && N == 4) gl::glUniform4uiv(loc, 1, &val[0]);
        // clang-format on
    }
};

#endif /* end of include guard: SHADER_HPP_CM510QXM */

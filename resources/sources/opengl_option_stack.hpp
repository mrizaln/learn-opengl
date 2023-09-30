#ifndef OPENGL_OPTION_STACK_HPP_KRAS5MVG
#define OPENGL_OPTION_STACK_HPP_KRAS5MVG

#include <cassert>
#include <format>
#include <iostream>
#include <stack>

#include "glbinding/gl/gl.h"

class OpenGLOptionStack
{
public:
    using Base_type = int;

    enum Option : Base_type
    {
        DEPTH_TEST  = 1 << 0,
        STECIL_TEST = 1 << 1,
        BLEND       = 1 << 2,
        CULL_FACE   = 1 << 3,
        WIREFRAME   = 1 << 4,
        ALL         = DEPTH_TEST | STECIL_TEST | BLEND | CULL_FACE | WIREFRAME,
    };

private:
    std::stack<std::pair<Base_type, Base_type>> m_stack;    // first: saved option, second: saved value

public:
    OpenGLOptionStack() = default;

public:
    // zero argument: save all options
    void push(std::same_as<Option> auto... option)
    {
        const Base_type options{ (0 | ... | option) };
        Base_type       value{ 0 };

        if constexpr (sizeof...(option) == 0) {
            for (int i{ 1 << 0 }; i < Option::ALL; i <<= 1) {
                value |= checkFlag((Option)i) * i;
            }
            m_stack.push({ Option::ALL, value });
        } else {
            if (options == ALL) {
                for (int i{ 1 << 0 }; i < Option::ALL; i <<= 1) {
                    value |= checkFlag((Option)i) * i;
                }
                m_stack.push({ Option::ALL, value });
            } else {
                for (const auto& f : { option... }) {
                    value |= checkFlag(f) * f;
                }
                m_stack.push({ options, value });
            }
        }
    }

    // restore last saved options
    void pop()
    {
        assert(!m_stack.empty() && "stack is empty!");

        auto [flags, value]{ m_stack.top() };
        m_stack.pop();

        if (flags & Option::DEPTH_TEST) {
            setFlag(Option::DEPTH_TEST, value & Option::DEPTH_TEST);
        }
        if (flags & Option::STECIL_TEST) {
            setFlag(Option::STECIL_TEST, value & Option::STECIL_TEST);
        }
        if (flags & Option::BLEND) {
            setFlag(Option::BLEND, value & Option::BLEND);
        }
        if (flags & Option::CULL_FACE) {
            setFlag(Option::CULL_FACE, value & Option::CULL_FACE);
        }
        if (flags & Option::WIREFRAME) {
            setFlag(Option::WIREFRAME, value & Option::WIREFRAME);
        }
    }

    // recommended to call push() before calling this function
    void loadDefaults()
    {
        gl::glDisable(gl::GL_DEPTH_TEST);
        gl::glDisable(gl::GL_STENCIL_TEST);
        gl::glDisable(gl::GL_BLEND);
        gl::glDisable(gl::GL_CULL_FACE);
        gl::glPolygonMode(gl::GL_FRONT_AND_BACK, gl::GL_FILL);
    }

private:
    bool checkFlag(Option flag)
    {
        switch (flag) {
        case Option::DEPTH_TEST:
            return gl::glIsEnabled(gl::GL_DEPTH_TEST) == gl::GL_TRUE;
        case Option::STECIL_TEST:
            return gl::glIsEnabled(gl::GL_STENCIL_TEST) == gl::GL_TRUE;
        case Option::BLEND:
            return gl::glIsEnabled(gl::GL_BLEND) == gl::GL_TRUE;
        case Option::CULL_FACE:
            return gl::glIsEnabled(gl::GL_CULL_FACE) == gl::GL_TRUE;
        case Option::WIREFRAME:
        {
            gl::GLint value;
            gl::glGetIntegerv(gl::GL_POLYGON_MODE, &value);
            return (gl::GLenum)value == gl::GL_LINE;
        }
        default:
            return false;
        }
    }

    void setFlag(Option flag, bool value)
    {
        switch (flag) {
        case Option::DEPTH_TEST:
            value ? gl::glEnable(gl::GL_DEPTH_TEST) : gl::glDisable(gl::GL_DEPTH_TEST);
            break;
        case Option::STECIL_TEST:
            value ? gl::glEnable(gl::GL_STENCIL_TEST) : gl::glDisable(gl::GL_STENCIL_TEST);
            break;
        case Option::BLEND:
            value ? gl::glEnable(gl::GL_BLEND) : gl::glDisable(gl::GL_BLEND);
            break;
        case Option::CULL_FACE:
            value ? gl::glEnable(gl::GL_CULL_FACE) : gl::glDisable(gl::GL_CULL_FACE);
            break;
        case Option::WIREFRAME:
            gl::glPolygonMode(gl::GL_FRONT_AND_BACK, value ? gl::GL_LINE : gl::GL_FILL);
            break;
        default:
            break;
        }
    }
};

#endif /* end of include guard: OPENGL_OPTION_STACK_HPP_KRAS5MVG */

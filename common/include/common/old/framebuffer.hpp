#ifndef FRAMEBUFFER_HPP_UKRHGFNS
#define FRAMEBUFFER_HPP_UKRHGFNS

#include <functional>
#include <iostream>
#include <optional>
#include <utility>

#include <glbinding/gl/gl.h>

class Framebuffer
{
public:
    gl::GLuint m_fbo;
    gl::GLuint m_tex;
    gl::GLuint m_rbo;

public:
    static std::optional<Framebuffer> create(gl::GLint width, gl::GLint height)
    {
        using namespace gl;

        // generate framebuffer
        GLuint framebuffer;
        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        auto [textureColorbuffer, rbo]{ createAttachmentBuffers(width, height) };

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "ERROR: Framebuffer is not complete!" << '\n';

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            return {};
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return Framebuffer{ framebuffer, textureColorbuffer, rbo };
    }

private:
    // return textureColorbuffer and rbo
    [[nodiscard]]
    static std::pair<gl::GLuint, gl::GLuint> createAttachmentBuffers(gl::GLint width, gl::GLint height)
    {
        using namespace gl;

        // generate texture for color attachment
        GLuint textureColorbuffer;
        glGenTextures(1, &textureColorbuffer);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        // generate renderbuffer object for depth and stencil attachment
        GLuint rbo;
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        // attach the attachments
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

        return { textureColorbuffer, rbo };
    }

public:
    Framebuffer(Framebuffer&& other)
        : m_fbo{ other.m_fbo }
        , m_tex{ other.m_tex }
        , m_rbo{ other.m_rbo }
    {
        other.m_fbo = 0;
        other.m_tex = 0;
        other.m_rbo = 0;
    }

    ~Framebuffer()
    {
        if (m_fbo != 0) {
            gl::glDeleteFramebuffers(1, &m_fbo);
        }
        if (m_tex != 0) {
            gl::glDeleteTextures(1, &m_tex);
        }
        if (m_rbo != 0) {
            gl::glDeleteRenderbuffers(1, &m_rbo);
        }
    }

private:
    Framebuffer()                   = delete;
    Framebuffer(const Framebuffer&) = delete;

    Framebuffer(gl::GLuint framebuffer, gl::GLuint textureColorbuffer, gl::GLuint rbo)
        : m_fbo{ framebuffer }
        , m_tex{ textureColorbuffer }
        , m_rbo{ rbo }
    {
    }

public:
    void resize(gl::GLint width, gl::GLint height)
    {
        using namespace gl;

        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

        // detach the old attachments
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0);

        // delete the old attachments
        glDeleteTextures(1, &m_tex);
        glDeleteRenderbuffers(1, &m_rbo);

        // recreate attachments
        auto [newTexture, newRbo]{ createAttachmentBuffers(width, height) };

        m_tex = newTexture;
        m_rbo = newRbo;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void bind() const { gl::glBindFramebuffer(gl::GL_FRAMEBUFFER, m_fbo); }

    void unbind() const { gl::glBindFramebuffer(gl::GL_FRAMEBUFFER, 0); }

    void use(std::function<void()>&& func) const
    {
        bind();
        func();
        unbind();
    }

    void bindTexture() const { gl::glBindTexture(gl::GL_TEXTURE_2D, m_tex); }
};

#endif /* end of include guard: FRAMEBUFFER_HPP_UKRHGFNS */

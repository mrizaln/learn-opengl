#ifndef PLANE_HPP_GR5KWQFD
#define PLANE_HPP_GR5KWQFD

#include <cstddef>
#include <array>
#include <vector>

#include <glm/glm.hpp>
#include <glbinding/gl/gl.h>

class Plane
{
private:
    using Triple = glm::vec3;
    using Pair   = glm::vec2;

    struct VertexData
    {
        Triple m_position;
        Triple m_normal;
        Pair   m_texCoord;
    };

public:
    inline static constexpr std::size_t NUM_OF_VERTICES = 6;

private:
    // clang-format off
    inline static constexpr std::array<Triple, NUM_OF_VERTICES> s_planeVertices{ {
        {  1.0f, 0.0f,  1.0f },
        { -1.0f, 0.0f,  1.0f },
        { -1.0f, 0.0f, -1.0f },

        {  1.0f, 0.0f,  1.0f },
        { -1.0f, 0.0f, -1.0f },
        {  1.0f, 0.0f, -1.0f },
    } };
    // clang-format on

    // clang-format off
    inline static constexpr std::array<Triple, NUM_OF_VERTICES> s_planeNormals{ {
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },

        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },
    } };
    // clang-format on

    // clang-format off
    inline static constexpr std::array<Pair, NUM_OF_VERTICES> s_planeTexCoords{ {
        { 1.0f, 1.0f },
        { 0.0f, 1.0f },
        { 0.0f, 0.0f },

        { 1.0f, 1.0f },
        { 0.0f, 0.0f },
        { 1.0f, 0.0f },
    } };
    // clang-format on

private:
    float                   m_sideLength;
    std::vector<VertexData> m_vertices;
    gl::GLuint              m_vao;
    gl::GLuint              m_vbo;

public:
    Plane(float sideLength = 1.0f)
        : m_sideLength{ sideLength }
        , m_vertices(NUM_OF_VERTICES)
    {
        for (std::size_t i{ 0 }; i < NUM_OF_VERTICES; ++i) {
            m_vertices[i] = {
                .m_position = s_planeVertices[i] * m_sideLength / 2.0f,    // divide by 2.0f since the s_cubeVertices side length is 2.0f
                .m_normal   = s_planeNormals[i],
                .m_texCoord = s_planeTexCoords[i],
            };
        }

        setBuffers();
    }

    ~Plane()
    {
        deleteBuffers();
    }

    void draw() const
    {
        gl::glBindVertexArray(m_vao);
        gl::glDrawArrays(gl::GL_TRIANGLES, 0, static_cast<gl::GLsizei>(m_vertices.size()));
        gl::glBindVertexArray(0);
    }

private:
    void setBuffers()
    {
        constexpr auto stride{ static_cast<gl::GLsizei>(sizeof(VertexData)) };

        gl::glGenVertexArrays(1, &m_vao);
        gl::glGenBuffers(1, &m_vbo);

        gl::glBindVertexArray(m_vao);
        gl::glBindBuffer(gl::GL_ARRAY_BUFFER, m_vbo);
        gl::glBufferData(gl::GL_ARRAY_BUFFER, static_cast<gl::GLsizeiptr>(m_vertices.size() * stride), &m_vertices.front(), gl::GL_STATIC_DRAW);

        gl::glVertexAttribPointer(0, decltype(VertexData::m_position)::length(), gl::GL_FLOAT, gl::GL_FALSE, stride, (void*)offsetof(VertexData, m_position));
        gl::glVertexAttribPointer(1, decltype(VertexData::m_normal)::length(), gl::GL_FLOAT, gl::GL_FALSE, stride, (void*)offsetof(VertexData, m_normal));
        gl::glVertexAttribPointer(2, decltype(VertexData::m_texCoord)::length(), gl::GL_FLOAT, gl::GL_FALSE, stride, (void*)offsetof(VertexData, m_texCoord));
        gl::glEnableVertexAttribArray(0);
        gl::glEnableVertexAttribArray(1);
        gl::glEnableVertexAttribArray(2);

        gl::glBindBuffer(gl::GL_ARRAY_BUFFER, 0);
        gl::glBindVertexArray(0);
    }

    void deleteBuffers()
    {
        gl::glDeleteVertexArrays(1, &m_vao);
        gl::glDeleteBuffers(1, &m_vbo);
    }
};

#endif /* end of include guard: PLANE_HPP_GR5KWQFD */

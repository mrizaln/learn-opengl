#ifndef CUBE_HPP_UP3OQBTC
#define CUBE_HPP_UP3OQBTC

#include <cstddef>
#include <format>
#include <iostream>

#include <glm/glm.hpp>

#include <glbinding/gl/gl.h>

class Cube
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
    static constexpr std::size_t NUM_OF_VERTICES = 36;

private:
    // clang-format off
    inline static constexpr std::array<Triple, NUM_OF_VERTICES> s_cubeVertices{ {
        // back face
        { -1.0f, -1.0f, -1.0f },
        {  1.0f,  1.0f, -1.0f },
        {  1.0f, -1.0f, -1.0f },
        {  1.0f,  1.0f, -1.0f },
        { -1.0f, -1.0f, -1.0f },
        { -1.0f,  1.0f, -1.0f },

        // front face
        { -1.0f, -1.0f,  1.0f },
        {  1.0f, -1.0f,  1.0f },
        {  1.0f,  1.0f,  1.0f },
        {  1.0f,  1.0f,  1.0f },
        { -1.0f,  1.0f,  1.0f },
        { -1.0f, -1.0f,  1.0f },

        // left face
        { -1.0f,  1.0f,  1.0f },
        { -1.0f,  1.0f, -1.0f },
        { -1.0f, -1.0f, -1.0f },
        { -1.0f, -1.0f, -1.0f },
        { -1.0f, -1.0f,  1.0f },
        { -1.0f,  1.0f,  1.0f },

        // right face
        {  1.0f,  1.0f,  1.0f },
        {  1.0f, -1.0f, -1.0f },
        {  1.0f,  1.0f, -1.0f },
        {  1.0f, -1.0f, -1.0f },
        {  1.0f,  1.0f,  1.0f },
        {  1.0f, -1.0f,  1.0f },

        // bottom face
        { -1.0f, -1.0f, -1.0f },
        {  1.0f, -1.0f, -1.0f },
        {  1.0f, -1.0f,  1.0f },
        {  1.0f, -1.0f,  1.0f },
        { -1.0f, -1.0f,  1.0f },
        { -1.0f, -1.0f, -1.0f },

        // top face
        { -1.0f,  1.0f, -1.0f },
        {  1.0f,  1.0f,  1.0f },
        {  1.0f,  1.0f, -1.0f },
        {  1.0f,  1.0f,  1.0f },
        { -1.0f,  1.0f, -1.0f },
        { -1.0f,  1.0f,  1.0f },
    } };
    // clang-format on

    // clang-format off
    inline static constexpr std::array<Triple, NUM_OF_VERTICES> s_cubeNormals{ {
        {  0.0f,  0.0f, -1.0f },
        {  0.0f,  0.0f, -1.0f },
        {  0.0f,  0.0f, -1.0f },
        {  0.0f,  0.0f, -1.0f },
        {  0.0f,  0.0f, -1.0f },
        {  0.0f,  0.0f, -1.0f },

        {  0.0f,  0.0f,  1.0f },
        {  0.0f,  0.0f,  1.0f },
        {  0.0f,  0.0f,  1.0f },
        {  0.0f,  0.0f,  1.0f },
        {  0.0f,  0.0f,  1.0f },
        {  0.0f,  0.0f,  1.0f },

        { -1.0f,  0.0f,  0.0f },
        { -1.0f,  0.0f,  0.0f },
        { -1.0f,  0.0f,  0.0f },
        { -1.0f,  0.0f,  0.0f },
        { -1.0f,  0.0f,  0.0f },
        { -1.0f,  0.0f,  0.0f },

        {  1.0f,  0.0f,  0.0f },
        {  1.0f,  0.0f,  0.0f },
        {  1.0f,  0.0f,  0.0f },
        {  1.0f,  0.0f,  0.0f },
        {  1.0f,  0.0f,  0.0f },
        {  1.0f,  0.0f,  0.0f },

        {  0.0f, -1.0f,  0.0f },
        {  0.0f, -1.0f,  0.0f },
        {  0.0f, -1.0f,  0.0f },
        {  0.0f, -1.0f,  0.0f },
        {  0.0f, -1.0f,  0.0f },
        {  0.0f, -1.0f,  0.0f },

        {  0.0f,  1.0f,  0.0f },
        {  0.0f,  1.0f,  0.0f },
        {  0.0f,  1.0f,  0.0f },
        {  0.0f,  1.0f,  0.0f },
        {  0.0f,  1.0f,  0.0f },
        {  0.0f,  1.0f,  0.0f }
    } };
    // clang-format on

    // clang-format off
    inline static constexpr std::array<Pair, NUM_OF_VERTICES> s_cubeTexCoords{ {
        // back face
        { 0.0f, 0.0f },
        { 1.0f, 1.0f },
        { 1.0f, 0.0f },
        { 1.0f, 1.0f },
        { 0.0f, 0.0f },
        { 0.0f, 1.0f },

        // front face
        { 0.0f, 0.0f },
        { 1.0f, 0.0f },
        { 1.0f, 1.0f },
        { 1.0f, 1.0f },
        { 0.0f, 1.0f },
        { 0.0f, 0.0f },

        // left face
        { 1.0f, 0.0f },
        { 1.0f, 1.0f },
        { 0.0f, 1.0f },
        { 0.0f, 1.0f },
        { 0.0f, 0.0f },
        { 1.0f, 0.0f },

        // right face
        { 1.0f, 0.0f },
        { 0.0f, 1.0f },
        { 1.0f, 1.0f },
        { 0.0f, 1.0f },
        { 1.0f, 0.0f },
        { 0.0f, 0.0f },

        // bottom face
        { 0.0f, 1.0f },
        { 1.0f, 1.0f },
        { 1.0f, 0.0f },
        { 1.0f, 0.0f },
        { 0.0f, 0.0f },
        { 0.0f, 1.0f },

        // top face
        { 0.0f, 1.0f },
        { 1.0f, 0.0f },
        { 1.0f, 1.0f },
        { 1.0f, 0.0f },
        { 0.0f, 1.0f },
        { 0.0f, 0.0f }
    } };
    // clang-format on

private:
    float                   m_sideLength;
    std::vector<VertexData> m_vertices;
    unsigned int            m_vao;
    unsigned int            m_vbo;

public:
    Cube(float sideLength = 1.0f)
        : m_sideLength{ sideLength }
        , m_vertices(NUM_OF_VERTICES)    // default initialize NUM_OF_VERTICES elements
    {
        for (std::size_t i = 0; i < NUM_OF_VERTICES; ++i) {
            m_vertices[i].m_position = s_cubeVertices[i] * m_sideLength / 2.0f;
            m_vertices[i].m_normal   = s_cubeNormals[i];
            m_vertices[i].m_texCoord = s_cubeTexCoords[i];
        }

        setBuffers();
    }

    // cube destructor must be done before glfwTerminate() is called else it will cause a segmentation fault
    ~Cube()
    {
        deleteBuffers();
    }

    void draw() const
    {
        gl::glBindVertexArray(m_vao);
        gl::glDrawArrays(gl::GL_TRIANGLES, 0, static_cast<gl::GLsizei>(m_vertices.size()));
        gl::glBindVertexArray(0);
    }

    void deleteBuffers()
    {
        gl::glDeleteVertexArrays(1, &m_vao);
        gl::glDeleteBuffers(1, &m_vbo);
    }

    void print() const
    {
        for (const auto& vertex : m_vertices) {
            std::cout << std::format("position: ({}, {}, {})\n", vertex.m_position.x, vertex.m_position.y, vertex.m_position.z)
                      << std::format("normal: ({}, {}, {})\n", vertex.m_normal.x, vertex.m_normal.y, vertex.m_normal.z)
                      << std::format("texCoord: ({}, {})\n", vertex.m_texCoord.x, vertex.m_texCoord.y)
                      << '\n';
        }
    }

private:
    void setBuffers()
    {
        gl::glGenVertexArrays(1, &m_vao);
        gl::glGenBuffers(1, &m_vbo);

        // bind
        //----
        gl::glBindVertexArray(m_vao);
        gl::glBindBuffer(gl::GL_ARRAY_BUFFER, m_vbo);
        gl::glBufferData(gl::GL_ARRAY_BUFFER, static_cast<gl::GLsizei>(sizeof(VertexData) * m_vertices.size()), &m_vertices.front(), gl::GL_STATIC_DRAW);

        // vertex attribute
        //-----------------
        auto stride{ static_cast<gl::GLsizei>(sizeof(VertexData)) };
        gl::glVertexAttribPointer(0, decltype(VertexData::m_position)::length(), gl::GL_FLOAT, gl::GL_FALSE, stride, (void*)(offsetof(VertexData, m_position)));
        gl::glVertexAttribPointer(1, decltype(VertexData::m_normal)::length(), gl::GL_FLOAT, gl::GL_FALSE, stride, (void*)(offsetof(VertexData, m_normal)));
        gl::glVertexAttribPointer(2, decltype(VertexData::m_texCoord)::length(), gl::GL_FLOAT, gl::GL_FALSE, stride, (void*)(offsetof(VertexData, m_texCoord)));
        gl::glEnableVertexAttribArray(0);
        gl::glEnableVertexAttribArray(1);
        gl::glEnableVertexAttribArray(2);

        // unbind
        //----
        gl::glBindBuffer(gl::GL_ARRAY_BUFFER, 0);
        gl::glBindVertexArray(0);
    }
};

#endif /* end of include guard: CUBE_HPP_UP3OQBTC */

#ifndef CUBE_HPP_UP3OQBTC
#define CUBE_HPP_UP3OQBTC

#include <cstddef>
#include <format>
#include <iostream>

#include <glbinding/gl/gl.h>

class Cube
{
    inline static constexpr std::array s_cubeVertices{
        // clang-format off
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,

         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,

        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
        // clang-format on
    };

    inline static constexpr std::array s_cubeNormals{
        // clang-format off
         0.0f,  0.0f, -1.0f,
         0.0f,  0.0f, -1.0f,
         0.0f,  0.0f, -1.0f,
         0.0f,  0.0f, -1.0f,
         0.0f,  0.0f, -1.0f,
         0.0f,  0.0f, -1.0f,

         0.0f,  0.0f,  1.0f,
         0.0f,  0.0f,  1.0f,
         0.0f,  0.0f,  1.0f,
         0.0f,  0.0f,  1.0f,
         0.0f,  0.0f,  1.0f,
         0.0f,  0.0f,  1.0f,

        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,

         1.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,

         0.0f, -1.0f,  0.0f,
         0.0f, -1.0f,  0.0f,
         0.0f, -1.0f,  0.0f,
         0.0f, -1.0f,  0.0f,
         0.0f, -1.0f,  0.0f,
         0.0f, -1.0f,  0.0f,

         0.0f,  1.0f,  0.0f,
         0.0f,  1.0f,  0.0f,
         0.0f,  1.0f,  0.0f,
         0.0f,  1.0f,  0.0f,
         0.0f,  1.0f,  0.0f,
         0.0f,  1.0f,  0.0f
        // clang-format on
    };

    inline static constexpr std::array s_cubeTexCoords{
        // clang-format off
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,

        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,

        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,

        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,

        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f,

        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f
        // clang-format on
    };

    float m_sideLength{};

    // vertices data
    std::vector<float> m_vertices;
    std::vector<float> m_normals;
    std::vector<float> m_texCoords;

    // interleaved vertices data
    std::vector<float> m_interleavedVertices{};
    int                m_interleavedVerticesStrideSize{};

    // buffers
    unsigned int m_vao;
    unsigned int m_vbo;

public:
    Cube(float sideLength = 1.0f)
        : m_sideLength{ sideLength }
        , m_vertices(s_cubeVertices.size())
        , m_normals(s_cubeNormals.size())
        , m_texCoords(s_cubeTexCoords.size())
        , m_interleavedVertices(m_vertices.size() + m_normals.size() + m_texCoords.size())
        , m_interleavedVerticesStrideSize{ 8 * sizeof(float) }
    {
        // copy vertices multiplied by sidelength
        for (std::size_t i{ 0 }; i < std::size(m_vertices); i++) {
            m_vertices[i] = s_cubeVertices[i] * m_sideLength / 2.0f;
        }

        // copy normals
        std::copy(std::begin(s_cubeNormals), std::end(s_cubeNormals), std::begin(m_normals));

        // copy texCoords
        std::copy(std::begin(s_cubeTexCoords), std::end(s_cubeTexCoords), std::begin(m_texCoords));

        buildInterleavedVertices();
        setBuffers();
    }

    ~Cube()
    {
        // deleteBuffers();     // segmentation fault???
    }

    void draw() const
    {
        // bind buffer
        gl::glBindVertexArray(m_vao);

        // draw
        gl::glDrawArrays(gl::GL_TRIANGLES, 0, (gl::GLsizei)m_interleavedVertices.size());

        // unbind buffer
        gl::glBindVertexArray(0);
    }

    void deleteBuffers()
    {
        gl::glDeleteVertexArrays(1, &m_vao);
        gl::glDeleteBuffers(1, &m_vbo);
    }

    void print() const
    {
        auto& v{ m_interleavedVertices };
        for (std::size_t i{ 0 }; i < std::size(m_interleavedVertices); i += 8) {
            std::cout.precision(2);
            std::cout << v[i] << '\t' << v[i + 1] << '\t' << v[i + 2] << "\t\t"
                      << v[i + 3] << '\t' << v[i + 4] << '\t' << v[i + 5] << "\t\t"
                      << v[i + 6] << '\t' << v[i + 7] << '\n';
        }
    }

private:
    void buildInterleavedVertices()
    {
        for (std::size_t i{ 0 }, j{ 0 }, k{ 0 }, l{ 0 }; i < m_interleavedVertices.size(); i += 8, j += 3, k += 3, l += 2) {
            m_interleavedVertices[i]     = m_vertices[j];
            m_interleavedVertices[i + 1] = m_vertices[j + 1];
            m_interleavedVertices[i + 2] = m_vertices[j + 2];

            m_interleavedVertices[i + 3] = m_normals[k];
            m_interleavedVertices[i + 4] = m_normals[k + 1];
            m_interleavedVertices[i + 5] = m_normals[k + 2];

            m_interleavedVertices[i + 6] = m_texCoords[l];
            m_interleavedVertices[i + 7] = m_texCoords[l + 1];
        }
    }

    void setBuffers()
    {
        gl::glGenVertexArrays(1, &m_vao);
        gl::glGenBuffers(1, &m_vbo);

        // bind
        //----
        gl::glBindVertexArray(m_vao);

        gl::glBindBuffer(gl::GL_ARRAY_BUFFER, m_vbo);
        gl::glBufferData(gl::GL_ARRAY_BUFFER, static_cast<gl::GLsizei>(m_interleavedVertices.size() * sizeof(float)), m_interleavedVertices.data(), gl::GL_STATIC_DRAW);

        // vertex attribute
        //-----------------
        // position
        gl::glVertexAttribPointer(0, 3, gl::GL_FLOAT, gl::GL_FALSE, m_interleavedVerticesStrideSize, (void*)(0));
        gl::glEnableVertexAttribArray(0);

        // normal
        gl::glVertexAttribPointer(1, 3, gl::GL_FLOAT, gl::GL_FALSE, m_interleavedVerticesStrideSize, (void*)(3 * sizeof(float)));
        gl::glEnableVertexAttribArray(1);

        // texcoords
        gl::glVertexAttribPointer(2, 2, gl::GL_FLOAT, gl::GL_FALSE, m_interleavedVerticesStrideSize, (void*)(6 * sizeof(float)));
        gl::glEnableVertexAttribArray(2);

        // unbind
        //----
        gl::glBindBuffer(gl::GL_ARRAY_BUFFER, 0);
        gl::glBindVertexArray(0);
    }
};

#endif /* end of include guard: CUBE_HPP_UP3OQBTC */

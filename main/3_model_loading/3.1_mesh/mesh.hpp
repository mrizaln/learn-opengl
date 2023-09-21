#ifndef MESH_HPP_FCAKKYD8
#define MESH_HPP_FCAKKYD8

#include <array>
#include <cstddef>
#include <string>
#include <vector>

#include <glbinding/gl/gl.h>
#include <glm/glm.hpp>

#include "shader.hpp"
#include "texture.hpp"

// #define ENABLE_BONE_INFLUENCE

struct Vertex
{
    static inline constexpr std::size_t s_maxBoneInfluence{ 4 };

    glm::vec3 m_position{};
    glm::vec3 m_normal{};
    glm::vec2 m_texCoords{};
    glm::vec3 m_tangent{};
    glm::vec3 m_bitangent{};

#ifdef ENABLE_BONE_INFLUENCE
    std::array<std::size_t, s_maxBoneInfluence> m_boneIDs{};    // bone indexes which will influence this vertex
    std::array<float, s_maxBoneInfluence>       m_weights{};    // weights from each bone
#endif
};

class Mesh
{
private:
    std::vector<Vertex>       m_vertices{};
    std::vector<unsigned int> m_indices{};
    std::vector<Texture*>     m_textures{};    // a mesh does not own its textures

    gl::GLuint m_vao{};
    gl::GLuint m_vbo{};
    gl::GLuint m_ebo{};

public:
    Mesh(
        std::vector<Vertex>&&       vertices,
        std::vector<unsigned int>&& indices,
        std::vector<Texture*>&&     textures
    )
        : m_vertices{ vertices }
        , m_indices{ indices }
        , m_textures{ textures }
    {
        setupMesh();
    }

    void draw(Shader& shader) const
    {
        for (auto* tex : m_textures) {
            tex->activate(shader);
        }

        using namespace gl;
        glBindVertexArray(m_vao);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

private:
    void setupMesh()
    {
        using namespace gl;

        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vbo);
        glGenBuffers(1, &m_ebo);

        glBindVertexArray(m_vao);

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(m_vertices.size() * sizeof(Vertex)), &m_vertices.front(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(m_indices.size() * sizeof(m_indices.front())), &m_indices.front(), GL_STATIC_DRAW);

        // clang-format off
        glVertexAttribPointer(0, decltype(Vertex::m_position )::length(), GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, m_position)));
        glVertexAttribPointer(1, decltype(Vertex::m_normal   )::length(), GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, m_normal)));
        glVertexAttribPointer(2, decltype(Vertex::m_texCoords)::length(), GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, m_texCoords)));
        glVertexAttribPointer(3, decltype(Vertex::m_tangent  )::length(), GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, m_tangent)));
        glVertexAttribPointer(4, decltype(Vertex::m_bitangent)::length(), GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, m_bitangent)));
#ifdef ENABLE_BONE_INFLUENCE
        glVertexAttribPointer(5, Vertex::s_maxBoneInfluence, GL_INT,   GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, m_boneIDs)));
        glVertexAttribPointer(6, Vertex::s_maxBoneInfluence, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, m_weights)));
#endif
        // clang-format on

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);
        glEnableVertexAttribArray(4);
        glEnableVertexAttribArray(5);
        glEnableVertexAttribArray(6);

        glBindVertexArray(0);
    }
};

#endif /* end of include guard: MESH_HPP_FCAKKYD8 */

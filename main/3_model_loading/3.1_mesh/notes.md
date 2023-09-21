# Mesh

Mesh is basically a shape used to create a model. A model can (and usually) consists of multiple meshes combined as one. A single mesh is the minimal representation of what we need to draw an object in OpenGL (vertex data, indices, and material properties).

A mesh should at least need a set of vertices contains a position vector, a normal vector, and a texture coordinate vector. A mesh should also contain indices for indexed drawing, and material data in the form of textures (diffuse/specular maps).

Now we can define a vertex in our program.

```cpp
struct Vertex {
    glm::vec3 m_position;
    glm::vec3 m_normal;
    glm::vec2 m_texCoords;
};
```

Next to a `Vertex` struct, we also want to organize the texture data in a `Texture` struct

```cpp
struct Texture {
    unsigned int m_id;
    string m_type;      // type of texture, e.g. diffuse or specular texture
};
```

We then use `Vertex` and `Texture` struct to define the structure of the mesh class

```cpp
class Mesh {
public:
    // mesh data
    std::vector<Vertex>       m_vertices;
    std::vector<unsigned int> m_indices;
    std::vector<Texture>      m_textures;

    Mesh(std::vector<Vertex>&& vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    void draw(Shader& shader);

private:
    // render data
    unsigned int m_vao;
    unsigned int m_vbo;
    unsigned int m_ebo;

    void setupMesh();
};
```

## Initialization

```cpp
Mesh(
    std::vector<Vertex>&&     vertices,
    std::vector<unsigned int> indices,
    std::vector<Texture>      textures
)
    : m_vertices{ vertices }
    , m_indices{ indices }
    , m_textures{ textures }
{
    setupMesh();
}
```

```cpp
void setupMesh()
{
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size()*sizeof(Vertex), &m_vertices.front(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size()*sizeof(unsigned int), &m_indices.front(), GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(0));
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, m_normal)));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, m_texCoords)));

    glBindVertexArray(0);
}
```

## Rendering

Before rendering the mesh (through `draw` function), we first want to bind the appropriate textures before calling `glDrawElements`. However, this is somewhat difficult since we don't know form the start how many (if any) textures the mesh has and what type they may have.

To solve this issue we're going to assume a certain naming convertion: each diffuse texture is named `texture_diffuseN`, and each specular texture should be named `texture_specularN` where `N` is any number ranging from 1 to the maximum number of texture samplers allowed.

The resulting drawing code then becomes

```cpp
void draw(Shader& shader)
{
    unsigned int diffuseNr  = 1;
    unsigned int specularNr = 1;

    for (unsigned int i{ 0 }; i < m_textures.size()); ++i) {
        glActiveTexture(GL_TEXTURE0 + i);   // activate proper texture unit before binding

        std::string texName{ m_textures[i] };
        unsigned int texNum{ [&] {
            if      (texName == "texture_diffuse")  return diffuseNr++;
            else if (texName == "texture_specular") return specularNr++;
        }() };
        shader.setUniform(std::format("material.{}{}", texName, texNum), i);

        glBindTexture(GL_TEXTURE_2D, textures[i].m_id);
    }
    glActiveTexture(GL_TEXTURE0);

    // draw mesh
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
```

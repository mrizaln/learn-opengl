#ifndef MODEL_HPP_EAGQLJBT
#define MODEL_HPP_EAGQLJBT

#include <concepts>
#include <filesystem>
#include <format>
#include <iostream>
#include <map>
#include <optional>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// #include "stringified_enum.hpp"

#include "image_texture.hpp"
#include "mesh.hpp"

/*
#define FIELD(M)                    \
    M(DIFFUSE, "texture_diffuse")   \
    M(SPECULAR, "texture_specular") \
    M(NORMAL, "texture_normal")     \
    M(HEIGHT, "texture_height")
using TextureType = STRINGIFIED_ENUM(TextureType, unsigned int, FIELD);
#undef FIELD 
*/

class Model
{
private:
    static inline const std::map<aiTextureType, std::string> s_textureTypeToName{
        { aiTextureType_DIFFUSE, "u_texture_diffuse" },
        { aiTextureType_SPECULAR, "u_texture_specular" },
        { aiTextureType_NORMALS, "u_texture_normal" },
        { aiTextureType_HEIGHT, "u_texture_height" },
    };

public:
    // can't wait for std::expected to come so i can return the error
    static std::optional<Model> load(std::filesystem::path filePath)
    {
        Assimp::Importer importer;

        // flags: https://assimp.sourceforge.net/lib_html/postprocess_8h.html
        const aiScene* scenePtr{ importer.ReadFile(filePath.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs) };
        if (!scenePtr || scenePtr->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
            std::cerr << std::format("ERROR: [Assimp] {}\n", importer.GetErrorString());
            return {};
        }
        const aiScene& scene{ *scenePtr };
        return Model{ scene, filePath };
    }

private:
    std::vector<Mesh>     m_meshes;
    std::filesystem::path m_filePath;

    // NOTE: need a container that does not invalidate its reference on insertion
    std::map<std::string, ImageTexture> s_loadedTextures;    // loaded textures are stored here to prevent loading the same texture multiple times

public:
    void draw(Shader& shader) const
    {
        for (const auto& mesh : m_meshes) {
            mesh.draw(shader);
        }
    }

private:
    Model() = delete;

    Model(const aiScene& scene, const std::filesystem::path& filePath)
        : m_filePath{ filePath }
    {
        std::cout << std::format("INFO: [Model] Loading model at '{}'\n", filePath.c_str());

        m_meshes.reserve(scene.mNumMeshes);
        processNodeRecursive(*scene.mRootNode, scene);

        std::cout << std::format("INFO: [Model] Loaded model at '{}'\n", filePath.c_str());
    }

    void processNodeRecursive(const aiNode& node, const aiScene& scene)
    {
        for (std::size_t i{ 0 }; i < node.mNumMeshes; ++i) {
            const aiMesh& mesh{ *scene.mMeshes[node.mMeshes[i]] };
            m_meshes.emplace_back(processMesh(mesh, scene));
        }
        for (std::size_t i{ 0 }; i < node.mNumChildren; ++i) {
            processNodeRecursive(*node.mChildren[i], scene);
        }
    };

    Mesh processMesh(const aiMesh& mesh, const aiScene& scene)
    {
        std::vector<Vertex>       vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture*>     textures;

        // vertices
        vertices.reserve(mesh.mNumVertices);
        for (std::size_t i{ 0 }; i < mesh.mNumVertices; ++i) {
            constexpr auto v2 = [](const auto& aiVec) -> glm::vec2 { return { aiVec.x, aiVec.y }; };
            constexpr auto v3 = [](const auto& aiVec) -> glm::vec3 { return { aiVec.x, aiVec.y, aiVec.z }; };
            using v           = glm::vec3;

            // assimp allow up to 8 texture coordinates; we only use the first one.
            // the texture coordinates are also 3D, but we only need 2D.
            vertices.emplace_back(Vertex{
                // clang-format off
                .m_position  = v3(mesh.mVertices[i]),
                .m_normal    = mesh.HasNormals()               ? v3(mesh.mNormals[i])          : v{},
                .m_texCoords = mesh.HasTextureCoords(0)        ? v2(mesh.mTextureCoords[0][i]) : v{},   // here, using v2
                .m_tangent   = mesh.HasTangentsAndBitangents() ? v3(mesh.mTangents[i])         : v{},
                .m_bitangent = mesh.HasTangentsAndBitangents() ? v3(mesh.mBitangents[i])       : v{},
                // clang-format on
            });
        }

        // indices
        indices.reserve(mesh.mNumFaces * 3);    // triangles (i think, because of aiProcess_Triangulate)
        for (std::size_t i{ 0 }; i < mesh.mNumFaces; ++i) {
            const aiFace& face{ mesh.mFaces[i] };
            for (std::size_t j{ 0 }; j < face.mNumIndices; ++j) {
                indices.push_back(face.mIndices[j]);
            }
        }

        // textures (materials)
        // for now, we only use on material only
        const aiMaterial& material{ *scene.mMaterials[mesh.mMaterialIndex] };    // guaranteed at least one material if AI_SCENE_FLAGS_INCOMPLETE is not set
        gl::GLint         overallTextureCount{ 0 };                              // will be used as texture unit index

        const auto loadTexture = [&material, &textures, &overallTextureCount, this](aiTextureType type) {
            std::size_t textureCount{ material.GetTextureCount(type) };

            for (std::size_t i{ 0 }; i < textureCount; ++i) {
                aiString path;
                material.GetTexture(type, (unsigned int)i, &path);

                auto texturePath{ m_filePath.parent_path() / path.C_Str() };
                if (auto found{ s_loadedTextures.find(texturePath) }; found != s_loadedTextures.end()) {
                    // TODO: add a check on the texture uniform name (thus texture type), it may be different
                    textures.push_back(&found->second);    // save pointer to loaded texture
                    continue;
                }

                /*
                    assimp allow up to 8 texture

                    we assume that each diffuse texture is named texture_diffuseN and each specular
                    texture should be named texture_specularN where N is any number ranging from 1
                    to the max number of texture samplers allowed.

                    naming candidate: texture_diffuseN
                                      material.texture_diffuseN
                                      materials[N].texture_diffuse

                    > I chose 'u_texture_diffuse_N' N ranging from 0 to max texture samplers allowed
                */

                auto name{ std::format("{}_{}", s_textureTypeToName.at(type), i) };    // e.g. "texture_diffuse_0"; yes, it starts with 0
                auto unitNum{ overallTextureCount };
                auto maybeTexture{ ImageTexture::from(texturePath, name, unitNum) };
                if (!maybeTexture.has_value()) {
                    std::cerr << std::format("ERROR: [Texture] Failed to load texture at {}\n", path.C_Str());
                    continue;
                }
                std::cout << std::format("INFO: [Model] texture '{}' loaded\n", texturePath.filename().c_str());

                auto loaded{ s_loadedTextures.emplace(texturePath, std::move(maybeTexture.value())) };
                textures.push_back(&loaded.first->second);    // save pointer to loaded texture
                overallTextureCount++;
            }
        };

        // load all textures
        for (const auto& [type, _] : s_textureTypeToName) {
            loadTexture(type);
        }

        return { std::move(vertices), std::move(indices), std::move(textures) };
    }
};

#endif /* end of include guard: MODEL_HPP_EAGQLJBT */

#ifndef TEXTURE_HPP_QDZVR1QU
#define TEXTURE_HPP_QDZVR1QU

#include <filesystem>
#include <format>
#include <iostream>
#include <optional>
#include <type_traits>

#include <glbinding/gl/gl.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "shader.hpp"

class Texture
{
private:
    class ImageData
    {
    public:
        const int            m_width{};
        const int            m_height{};
        const int            m_nrChannels{};
        const unsigned char* m_data{};

    public:
        ImageData(const ImageData&)            = delete;
        ImageData& operator=(const ImageData&) = delete;
        ImageData(ImageData&& other)
            : m_width{ other.m_width }
            , m_height{ other.m_height }
            , m_nrChannels{ other.m_nrChannels }
            , m_data{ other.m_data }
        {
            other.m_data = nullptr;
        }

        ~ImageData()
        {
            if (m_data) {
                stbi_image_free(const_cast<unsigned char*>(m_data));
            }
        }

    private:
        ImageData(int w, int h, int c, unsigned char* d)
            : m_width{ w }
            , m_height{ h }
            , m_nrChannels{ c }
            , m_data{ d }
        {
        }

    public:
        static std::optional<ImageData> from(std::filesystem::path imagePath)
        {
            int            width, height, nrChannels;
            unsigned char* data{ stbi_load(imagePath.c_str(), &width, &height, &nrChannels, 0) };
            if (!data) {
                std::cerr << std::format("Failed to load image at {}\n", imagePath.string());
                return {};
            }
            std::cout << std::format("Loaded image at {} with dimensions {}x{} and {} channels\n", imagePath.string(), width, height, nrChannels);
            return ImageData{ width, height, nrChannels, data };
        }
    };

private:
    gl::GLuint            m_id;
    gl::GLint             m_unitNum;
    std::filesystem::path m_imagePath;
    std::string           m_uniformName;

public:
    static std::optional<Texture> from(std::filesystem::path imagePath, const std::string& uniformName, gl::GLint textureUnitNum)
    {
        stbi_set_flip_vertically_on_load(true);

        auto maybeImageData{ ImageData::from(imagePath) };
        if (!maybeImageData) {
            return {};
        }
        return Texture{ std::move(*maybeImageData), imagePath, uniformName, textureUnitNum };
    }

public:
    gl::GLuint getId() const
    {
        return m_id;
    }

    gl::GLint getUnitNum() const
    {
        return m_unitNum;
    }

    const std::string& getUniformName() const
    {
        return m_uniformName;
    }

    void activate(Shader& shader) const
    {
        shader.setUniform(m_uniformName, m_unitNum);
        gl::glActiveTexture(gl::GL_TEXTURE0 + std::underlying_type_t<gl::GLenum>(m_unitNum));
        gl::glBindTexture(gl::GL_TEXTURE_2D, m_id);
    }

private:
    Texture(ImageData&& imageData, std::filesystem::path imagePath, const std::string& uniformName, gl::GLint textureUnitNum)
        : m_unitNum{ textureUnitNum }
        , m_imagePath{ imagePath }
        , m_uniformName{ uniformName }
    {
        gl::glGenTextures(1, &m_id);
        gl::glBindTexture(gl::GL_TEXTURE_2D, m_id);

        gl::glTexParameteri(gl::GL_TEXTURE_2D, gl::GL_TEXTURE_WRAP_S, gl::GL_REPEAT);
        gl::glTexParameteri(gl::GL_TEXTURE_2D, gl::GL_TEXTURE_WRAP_T, gl::GL_REPEAT);
        gl::glTexParameteri(gl::GL_TEXTURE_2D, gl::GL_TEXTURE_MIN_FILTER, gl::GL_LINEAR_MIPMAP_NEAREST);
        gl::glTexParameteri(gl::GL_TEXTURE_2D, gl::GL_TEXTURE_MAG_FILTER, gl::GL_LINEAR);

        gl::GLenum format;
        if (imageData.m_nrChannels == 3) {
            format = gl::GL_RGB;
        } else if (imageData.m_nrChannels == 4) {
            format = gl::GL_RGBA;
        } else {
            std::cerr << "Image number of channels is not supported: " << imageData.m_nrChannels << '\n';
            format = gl::GL_RGB;    // fallback
        }
        gl::glTexImage2D(gl::GL_TEXTURE_2D, 0, format, imageData.m_width, imageData.m_height, 0, format, gl::GL_UNSIGNED_BYTE, imageData.m_data);
        gl::glGenerateMipmap(gl::GL_TEXTURE_2D);

        gl::glBindTexture(gl::GL_TEXTURE_2D, 0);
    }
};

#endif /* end of include guard: TEXTURE_HPP_QDZVR1QU */

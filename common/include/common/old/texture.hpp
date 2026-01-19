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
    static std::optional<ImageData> from(std::filesystem::path imagePath, bool flipVertically = true)
    {
        stbi_set_flip_vertically_on_load(flipVertically);

        int            width, height, nrChannels;
        unsigned char* data{ stbi_load(imagePath.c_str(), &width, &height, &nrChannels, 0) };
        if (!data) {
            std::cerr << std::format("Failed to load image at {}\n", imagePath.string());
            return {};
        }
        return ImageData{ width, height, nrChannels, data };
    }

    // pad data to 4 channels
    static std::vector<std::array<unsigned char, 4>> addPadding(const ImageData& data)
    {
        std::vector<std::array<unsigned char, 4>> newData(std::size_t(data.m_width * data.m_height)
        );    // default initialize
        for (std::size_t i{ 0 }; i < newData.size(); ++i) {
            auto& byte{ newData[i] };
            for (int channel{ 0 }; channel < data.m_nrChannels; ++channel) {
                auto idx{ i * (std::size_t)data.m_nrChannels + (std::size_t)channel };
                byte[(std::size_t)channel] = data.m_data[idx];
            }
            byte[3] = 0xff;    // set alpha to max value
        }
        return newData;
    }
};

// base class for all textures
class Texture
{
protected:
    const gl::GLenum m_target;
    gl::GLuint       m_id;
    gl::GLint        m_unitNum;
    std::string      m_uniformName;

protected:
    Texture() = delete;

    Texture(gl::GLenum target, gl::GLint unitNum, const std::string& uniformName)
        : m_target{ target }
        , m_id{ 0 }
        , m_unitNum{ unitNum }
        , m_uniformName{ uniformName }
    {
    }

    Texture(gl::GLenum target, gl::GLuint id, gl::GLint unitNum, const std::string& uniformName)
        : m_target{ target }
        , m_id{ id }
        , m_unitNum{ unitNum }
        , m_uniformName{ uniformName }
    {
    }

public:
    virtual ~Texture() = 0;    // so that the class can't be instantiated

    gl::GLuint getId() const { return m_id; }

    gl::GLint getUnitNum() const { return m_unitNum; }

    const std::string& getUniformName() const { return m_uniformName; }

    void setUniformName(const std::string& name) { m_uniformName = name; }

    void activate(Shader& shader) const
    {
        shader.setUniform(m_uniformName, m_unitNum);
        gl::glActiveTexture(gl::GL_TEXTURE0 + std::underlying_type_t<gl::GLenum>(m_unitNum));
        gl::glBindTexture(m_target, m_id);
    }
};

// handle the deletion of the texture object, the derived class doesn't need to worry about it
inline Texture::~Texture()
{
    if (m_id != 0) {
        gl::glDeleteTextures(1, &m_id);
    }
}

#endif /* end of include guard: TEXTURE_HPP_QDZVR1QU */

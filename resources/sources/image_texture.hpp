#ifndef IMAGE_TEXTURE_HPP_MZAGCFYB
#define IMAGE_TEXTURE_HPP_MZAGCFYB

#include <array>
#include <filesystem>
#include <optional>
#include <utility>
#include <vector>

#include <glbinding/gl/gl.h>

#include "texture.hpp"

class ImageTexture final : public Texture
{
private:
    std::filesystem::path m_imagePath;

public:
    static std::optional<ImageTexture> from(
        std::filesystem::path imagePath,
        const std::string&    uniformName,
        gl::GLint             textureUnitNum
    )
    {
        auto maybeImageData{ ImageData::from(imagePath) };
        if (!maybeImageData) {
            return {};
        }
        return ImageTexture{ std::move(*maybeImageData), imagePath, uniformName, textureUnitNum };
    }

public:
    ImageTexture(const ImageTexture&) = delete;

    ImageTexture(ImageTexture&& other) noexcept
        : Texture{ gl::GL_TEXTURE_2D, other.m_id, other.m_unitNum, other.m_uniformName }
        , m_imagePath{ std::move(other.m_imagePath) }
    {
        other.m_id = 0;
    }

    const std::filesystem::path& getImagePath() const { return m_imagePath; }

private:
    ImageTexture(
        ImageData&&           imageData,
        std::filesystem::path imagePath,
        const std::string&    uniformName,
        gl::GLint             textureUnitNum
    )
        : Texture{ gl::GL_TEXTURE_2D, textureUnitNum, uniformName }
        , m_imagePath{ std::move(imagePath) }
    {
        gl::glGenTextures(1, &m_id);
        gl::glBindTexture(m_target, m_id);

        gl::glTexParameteri(m_target, gl::GL_TEXTURE_WRAP_S, gl::GL_MIRRORED_REPEAT);
        gl::glTexParameteri(m_target, gl::GL_TEXTURE_WRAP_T, gl::GL_MIRRORED_REPEAT);
        gl::glTexParameteri(m_target, gl::GL_TEXTURE_MIN_FILTER, gl::GL_LINEAR_MIPMAP_NEAREST);
        gl::glTexParameteri(m_target, gl::GL_TEXTURE_MAG_FILTER, gl::GL_LINEAR);

        if (imageData.m_nrChannels == 4) {
            gl::glTexImage2D(
                m_target,
                0,
                gl::GL_RGBA,
                imageData.m_width,
                imageData.m_height,
                0,
                gl::GL_RGBA,
                gl::GL_UNSIGNED_BYTE,
                imageData.m_data
            );
        } else if (imageData.m_nrChannels == 3) {
            gl::glTexImage2D(
                m_target,
                0,
                gl::GL_RGB,
                imageData.m_width,
                imageData.m_height,
                0,
                gl::GL_RGB,
                gl::GL_UNSIGNED_BYTE,
                imageData.m_data
            );
        } else {
            // pad data if not rgb or rgba
            auto newData{ ImageData::addPadding(imageData) };
            gl::glTexImage2D(
                m_target,
                0,
                gl::GL_RGBA,
                imageData.m_width,
                imageData.m_height,
                0,
                gl::GL_RGBA,
                gl::GL_UNSIGNED_BYTE,
                &newData.front()
            );
        }
        gl::glGenerateMipmap(m_target);

        gl::glBindTexture(m_target, 0);
    }
};

#endif /* end of include guard: IMAGE_TEXTURE_HPP_MZAGCFYB */

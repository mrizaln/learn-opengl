#ifndef CUBEMAP_HPP_NSIPCAFR
#define CUBEMAP_HPP_NSIPCAFR

#include <filesystem>
#include <optional>
#include <type_traits>
#include <vector>

#include <glbinding/gl/gl.h>

#include "texture.hpp"

class Cubemap final : public Texture
{
private:
    static inline constexpr std::size_t s_numFaces{ 6 };

public:
    // Note that the coordinate system for cubemap is left-handed. Z is flipped (front
    // and back are swapped) if you are working with right-handed coordinate system.
    // see: https://www.khronos.org/opengl/wiki/Cubemap_Texture
    enum class Face : int
    {
        // enum         left-handed     right-handed
        RIGHT  = 0,    // +x             +x
        LEFT   = 1,    // -x             -x
        TOP    = 2,    // +y             +y
        BOTTOM = 3,    // -y             -y
        BACK   = 5,    // +z             -z             (notice that this is swapped)
        FRONT  = 4,    // -z             +z             (notice that this is swapped)

        // I declare the order of the enum values to be the same as the order of the
        // enum values in the OpenGL (i.e. left-handed). While the number that represents
        // the enum values has the order of the right-handed coordinate system.
        // The difference between them is just in the Z axis.
    };

    struct CubeImagePath
    {
        std::filesystem::path right;
        std::filesystem::path left;
        std::filesystem::path top;
        std::filesystem::path bottom;
        std::filesystem::path back;
        std::filesystem::path front;

        bool empty() const
        {
            return right.empty() || left.empty() || top.empty() || bottom.empty() || back.empty()
                || front.empty();
        }

        const std::filesystem::path& get(Face face) const
        {
            switch (face) {
            case Face::RIGHT: return right;
            case Face::LEFT: return left;
            case Face::TOP: return top;
            case Face::BOTTOM: return bottom;
            case Face::BACK: return back;
            case Face::FRONT: return front;
            default: [[unlikely]] return right;
            }
        }
    };

private:
    CubeImagePath m_imagePaths;

public:
    static std::optional<Cubemap> from(
        CubeImagePath&&    imagePaths,
        const std::string& uniformName,
        gl::GLint          textureUnitNum
    )
    {
        if (imagePaths.empty()) {
            return {};
        }

        using Int = std::underlying_type_t<Face>;

        std::vector<ImageData> imageDatas;
        imageDatas.reserve(s_numFaces);

        for (Int face{ 0 }; face < static_cast<Int>(s_numFaces); ++face) {
            // the image is flipped vertically by the opengl on the cubemap, so we don't need to flip it on
            // load
            auto maybeImageData{ ImageData::from(imagePaths.get(static_cast<Face>(face)), false) };
            if (!maybeImageData) {
                return {};
            }
            imageDatas.emplace_back(std::move(*maybeImageData));
        }
        return Cubemap{ std::move(imageDatas), std::move(imagePaths), uniformName, textureUnitNum };
    }

public:
    Cubemap(const Cubemap&) = delete;

    Cubemap(Cubemap&& other) noexcept
        : Texture{ gl::GL_TEXTURE_CUBE_MAP, other.m_id, other.m_unitNum, other.m_uniformName }
        , m_imagePaths{ std::move(other.m_imagePaths) }
    {
        other.m_id = 0;
    }

    const CubeImagePath& getImagePaths() const { return m_imagePaths; }

    const std::filesystem::path& getImagePath(Face face) const { return m_imagePaths.get(face); }

private:
    Cubemap(
        std::vector<ImageData>&& imageDatas,
        CubeImagePath&&          imagePaths,
        const std::string&       uniformName,
        gl::GLint                textureUnitNum
    )
        : Texture{ gl::GL_TEXTURE_CUBE_MAP, textureUnitNum, uniformName }
        , m_imagePaths{ std::move(imagePaths) }
    {
        gl::glGenTextures(1, &m_id);
        gl::glBindTexture(m_target, m_id);

        gl::glTexParameteri(m_target, gl::GL_TEXTURE_WRAP_S, gl::GL_CLAMP_TO_EDGE);
        gl::glTexParameteri(m_target, gl::GL_TEXTURE_WRAP_T, gl::GL_CLAMP_TO_EDGE);
        gl::glTexParameteri(m_target, gl::GL_TEXTURE_WRAP_R, gl::GL_CLAMP_TO_EDGE);
        gl::glTexParameteri(m_target, gl::GL_TEXTURE_MIN_FILTER, gl::GL_LINEAR);
        gl::glTexParameteri(m_target, gl::GL_TEXTURE_MAG_FILTER, gl::GL_LINEAR);

        for (std::size_t face{ 0 }; face < s_numFaces; ++face) {
            const auto& imageData{ imageDatas[face] };

            using Int = std::underlying_type_t<gl::GLenum>;
            gl::GLenum texFace{ gl::GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<Int>(face) };

            if (imageData.m_nrChannels == 4) {
                gl::glTexImage2D(
                    texFace,
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
                    texFace,
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
                    texFace,
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
        }

        gl::glBindTexture(m_target, 0);
    }
};

#endif /* end of include guard: CUBEMAP_HPP_NSIPCAFR */

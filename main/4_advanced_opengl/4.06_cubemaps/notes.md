# Cubemaps

A cubemap is a texture that contains 6 individual 2D textures that each form one side of a cube: a textured cube. Cubemaps have useful property that they can be indexed/sampled using a direction vector.

## Creating a cubemap

Cubemap is a texture, so we use the usual function we used to create texture.

```cpp
GLuint textureId;
glGenTexture(1, &textureId);
glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
```

Because a cubemap contains 6 textures (for each side of a cube), we have to call `glTexImage2D` six times with their parameters set similarly to previous chapters. We have to set the texture **target** parameter to match a specific face of the cubemap.

| Texture target                   | Orientation (left-handed) |
| -------------------------------- | ------------------------- |
| `GL_TEXTURE_CUBE_MAP_POSITIVE_X` | Right                     |
| `GL_TEXTURE_CUBE_MAP_NEGATIVE_X` | Left                      |
| `GL_TEXTURE_CUBE_MAP_POSITIVE_Y` | Top                       |
| `GL_TEXTURE_CUBE_MAP_NEGATIVE_Y` | Bottom                    |
| `GL_TEXTURE_CUBE_MAP_POSITIVE_Z` | Back                      |
| `GL_TEXTURE_CUBE_MAP_NEGATIVE_Z` | Front                     |

> The texture target enumerations is linearly incremented by $1$ from `GL_TEXTURE_CUBE_MAP_POSITIVE_X` to `GL_TEXTURE_CUBE_MAP_NEGATIVE_Z` so you can loop it.

Because a cubemap is a texture like any other texture, we need to specify its wrapping and filtering methods

```cpp
glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);    // 3rd dimension
```

> Cubemap has an additional texture coordinate beside $S$ and $T$

Within the fragment shader, we also have to use a different sampler of the type `samplerCube` that we sample from using the `texture` function, but this time using a `vec3` direction vector instead of a `vec2`.

> Example

```glsl
#version 330 core

layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = aPos;
    gl_Position = projection * view * vec4(aPos, 1.0);
}
```

## Skybox

A cubemap, can be used to create a skybox. A Skybox is a (large) cube that ecompasses the entire scene and contains 6 images of a surrounding environment, giving the illusion that the environment is actually larger than it actually is.

# Textures

A texture is a 2D image (even 1D and 3D textures exist) used to add detail to an object.

> Next to images, textures can also be used to store a large collection of arbitrary data to send to the shaders.

In order to map a texture to an object we need to tell each vertex of the triangle which part of the texture it correspons to. Each vertex should thus have a **texture coordinate** associated with them that specifies what part of the texture image to sample from.

Texture coordinates range from `0` to `0` in the `x` and `y` axis. Retrieving the texture color using texture coordinates is called **sampling**. Texture coordinates start at `(0,0)` for the lower left corner of a texture image to `(1,1)` for the upper right corner of a texture image.

## Texture wrapping

Texture coordinates usually range from `(0,0)` to `(1,1)` but what happens if we specify coordinates outside this range? The default behavior of OpenGL is to repeat the texture images, but there are more options:

- `GL_REPEAT`: The default, repeats the tecture image
- `GL_MIRRORED_REPEAT`: Same as `GL_REPEAT` but mirrors image with each repeat
- `GL_CLAMP_TO_EDGE`: Clamps the coordinates between `0` and `1`.
- `GL_CLAMP_TO_BORDER`: Coordinates outside the range are now given a user-specified border color.

Each of the aforementioned options can be set per coordinate axis (`s`, `t` (and `r` if you're using 3D textures) equivalent to `x`, `y`, `z`) with `glTexParameter*` function.

```cpp
// signature: glTexParameteri(<target>, <what_option_and_which_axis>, <option>)
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
```

If we choose the `GL_CLAMP_TO_BORDER` option, we should also specify a border color. This is done using the `fv` equivalent of the `glTexParameter*` function.

```cpp
float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
```

## Texture Filtering

Texture coordinates do not depend on resolution but can be any floating point value, thus OpenGL has to figure out which texture element (**texel**) to map to texture coordinate to. There are several options available for texture filtering:

- `GL_NEAREST`: Also known as **point filtering**, is the default texture filtering method. When set to this, OpenGL selects the texel that center is closest to the texture coordinate.
- `GL_LINEAR`: Also known as **(bi)linear filtering**, takes an interpolated value from the texture coordinate's neighboring texels, approximating a color between the texels.

Texture filtering can be set for **magnifying** and **minifying** operations. We can specify this via `glTexParameter*` function.

```cpp
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
```

### Mipmaps

Mipmap is basically a collection of texture images where each subsequent texture is twice as small compared to the previous one. The idea behind it is simple: after a certain distance threshold from the viewer, OpenGL will use a different mipmap texture that best suits the distance to the object. OpenGL is then able to sample the correct texels and there's less cache memory involved when sampling that part of the mipmaps.

OpenGL able to create such mipmaps automatically with a single call to `glGenerateMipmap` after we created a texture.

When switching between mipmaps levels during rendering OpengLG may show some artifacts like sharp edgesvisible between two mipmap layers. Just like normal texture filtering, it is also possible to filter between mipmap levels using `NEAREST` and `LINEAR` filtering. To specify the filtering method, we can replace the original filtering methods with one of the following:

- `GL_NEAREST_MIPMAP_NEAREST`: takes nearest mipmap level and use nearest sampling for the texture
- `GL_LINEAR_MIPMAP_NEAREST`: takes nearest mipmap level and use linear sampling for the texture
- `GL_NEAREST_MIPMAP_LINEAR`: linearly interpolates between two mipmap levels and use nearest sampling for the texture
- `GL_LINEAR_MIPMAP_LINEAR`: linearly interpolates between two mipmap levels and use linear sampling for the texture

```cpp
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);  //mipmap is for downscaling only!
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
```

A common mistake is to set one of the mipmap filtering options as the magnification filter. This doesn't have any effect since mipmaps are primarily used for when textures get downscaled.

## Loading image

To create texture, we first need to load the image into our application. There are many libraries that can do this, one of it is [stb_image](https://github.com/nothings/stb/blob/master/stb_image.h).

Using stb_image, you can load image like this

```cpp
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int width, height, nrChannels;
unsigned char* data = stbi_load("<file_name>", &width, &height, &nrChannels, 0);
```

The result of the data is an array of bytes which will be used to generate a texture.

## Generating texture

The steps for generating a texture is similar to how we generate other OpenGL objects

```cpp
// generate
GLint texture;
glGenTextures(1, &texture);

// bind
glBindTexture(GL_TEXTURE_2D, texture)   // GL_TEXTURE_2D since we're using 2D texture

// send data
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

// generate mipmap automatically
glGenerateMipmap(GL_TEXTURE_2D);
```

The `glTexImage2D` has quite many parameters:

- `1st`: specifies texture target
- `2nd`: specifies mipmap level for which we want to create a texture for if you want to set each mipmap level manually (0 is the base level)
- `3rd`: tells OpenGL in what kind of format we want to store the texture
- `4th` & `5th`: width and height of the resulting texture
- `6th`: **should be always be `0`** (some legacy stuff)
- `7th` & `8th`: specify the format and datatype of the source image
- `9th`: the actual data

After we're done generating the texture, we can free the image data.

```cpp
stbi_image_free(data);
```

## Applying texture

To add texture to our object, we need to add texture coordinate information into our vertex data.

For example, we're using a rectangle as our object.

```cpp
float vertices[] = {
    // positions          // colors           // tex coords
     0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,     // top right
     0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,     // bottom right
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,     // bottom left
    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f      // top left
};
```

The attribute pointer then needs to be updated accordingly.

```cpp
glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
glEnableVertexAttribArray(2);
```

Next, we need to modify our vertex shader to accept the texture coordinates as a vertex attribute and then forward it to the fragment shader.

```glsl
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;  // new attribute

out vec3 ourColor;
out vec2 TexCoord;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    ourColor = aColor;
    TexCoord = aTexCoord;
}
```

The fragment shader should then accept the TexCoord output variable as input variable.

```glsl
#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;

void main()
{
    FragColor = texture(ourTexture, TexCoord);    // sample texture color
    // FragColor = texture(ourTexture, TexCoord) * vec4(ourColor, 1.0);  // multiply with ourColor
}
```

When we want to draw, we first need not to forget to bind the texture.

```cpp
glBindTexture(GL_TEXTURE_2D, texture);
glBindVertexArray(VAO);
glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
```

## Texture units

You probably noticed that the `sampler2D` variable is a uniform. Why is that? Well actually using `glUniform1i` we can assign a _location_ value to the texture sampler so we can set multiple textures at once in a fragment shader. This location of a texture is more commonly known as a **texture unit**. The default texture unit for a texture is `0` which is the default active texture unit so we didn't need to assign a location in the previous section.

The main purpose of texture units is to allow us to use more than 1 texture in our shaders. Just like `glBindTexture` we can activate texture units using `glActiveTexture` passing in the texture unit we'd like to use.

```cpp
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, texture);
```

After activating a texture unit, a subsequent `glBindTexture` call will bind that texture to the currently active texture unit.

---

**Note**: OpenGL should have at least a minimum of `16` texture units for you to use which you can activate using `GL_TEXTURE0` to `GL_TEXTURE15`. They are defined in order so we could also get `GL_TEXTURE8` via `GL_TEXTURE0 + 8` for example.

---

The fragment shader then would be like this

```cpp
#version 330 core
...

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
    FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
}
```

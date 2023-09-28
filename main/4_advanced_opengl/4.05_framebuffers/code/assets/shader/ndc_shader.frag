#version 330 core

out vec4 o_fragColor;

in vec2 io_texCoords;

uniform sampler2D u_screenTexture;

const float g_offset = 1.0 / 300.0;    // where this number come from?

vec3 applyKernel(float kernel[9], float offset)
{
    // clang-format off
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset),     // top-left
        vec2( 0.0f,    offset),     // top-center
        vec2( offset,  offset),     // top-right
        vec2(-offset,  0.0f),       // center-left
        vec2( 0.0f,    0.0f),       // center-center
        vec2( offset,  0.0f),       // center-right
        vec2(-offset, -offset),     // bottom-left
        vec2( 0.0f,   -offset),     // bottom-center
        vec2( offset, -offset)      // bottom-right
    );
    // clang-format on

    vec3 sampleTex[9];
    for (int i = 0; i < 9; ++i) {
        sampleTex[i] = texture(u_screenTexture, io_texCoords.st + offsets[i]).rgb;
    }

    vec3 color = vec3(0.0);
    for (int i = 0; i < 9; ++i) {
        color += sampleTex[i] * kernel[i];
    }

    return color;
}

void main()
{
    // simple post-processing
    // -----------------------------------------------------------
    // vec3 texColor = texture(u_screenTexture, io_texCoords).xyz;

    // color
    // o_fragColor = texture(u_screenTexture, io_texCoords);

    // inverted color
    // o_fragColor = vec4(1.0 - texColor, 1.0);

    // grayscale (simple average)
    // float average = (texColor.r + texColor.g + texColor.b) / 3.0;
    // o_fragColor   = vec4(vec3(average), 1.0);

    // grayscale (physically accrurate)
    // float average = 0.2126 * texColor.r + 0.7152 * texColor.g + 0.0722 * texColor.b;
    // o_fragColor   = vec4(vec3(average), 1.0);
    // -----------------------------------------------------------

    // kernel effects
    // -----------------------------------------------------------
    // clang-format off
    float sharpen[9] = float[](
        -1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
    );

    float blur[9] = float[](
        1.0 / 16, 2.0 / 16, 1.0 / 16,
        2.0 / 16, 4.0 / 16, 2.0 / 16,
        1.0 / 16, 2.0 / 16, 1.0 / 16
    );

    float edge[9] = float[](
        1,  1, 1,
        1, -8, 1,
        1,  1, 1
    );
    // clang-format on

    float[9] kernel = edge;

    vec3 color  = applyKernel(kernel, g_offset);
    o_fragColor = vec4(color, 1.0);
    // -----------------------------------------------------------
}

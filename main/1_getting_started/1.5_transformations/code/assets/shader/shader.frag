#version 330 core

out vec4 o_fragColor;

in vec3 io_color;
in vec2 io_texCoord;

uniform bool      u_invertColor;
uniform bool      u_multiplyColor;
uniform float     u_mixValue;
uniform sampler2D u_texture0;
uniform sampler2D u_texture1;

void main()
{
    vec4 texColor = mix(
        texture(u_texture0, io_texCoord),
        texture(u_texture1, io_texCoord),
        u_mixValue
    );
    vec4 color = texColor;
    if (u_multiplyColor) {
        color *= vec4(io_color, 1.0);
    }

    if (u_invertColor) {
        o_fragColor = 1.0 - color;
    } else {
        o_fragColor = color;
    }
}

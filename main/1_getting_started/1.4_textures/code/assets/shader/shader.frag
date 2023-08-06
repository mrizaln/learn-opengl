#version 330 core

out vec4 o_fragColor;

in vec3 io_color;
in vec2 io_texCoord;

uniform int       u_invertColor;    // boolean
uniform sampler2D u_texture;

void main()
{
    vec4 texColor = texture(u_texture, io_texCoord);
    vec4 color    = texColor * vec4(io_color, 1.0);

    if (u_invertColor == 1) {
        o_fragColor = 1.0 - color;
    } else {
        o_fragColor = color;
    }
}

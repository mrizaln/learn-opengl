#version 330 core

out vec4 o_fragColor;

in vec2 io_texCoords;

uniform sampler2D u_texture;

void main()
{
    vec4 texColor = texture(u_texture, io_texCoords);
    if (texColor.a < 0.1) {
        discard;
    }
    o_fragColor = texColor;
}

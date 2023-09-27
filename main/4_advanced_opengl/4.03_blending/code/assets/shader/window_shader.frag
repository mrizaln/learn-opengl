#version 330 core

out vec4 o_fragColor;

in vec2 io_texCoords;

uniform sampler2D u_texture;

void main()
{
    o_fragColor = texture(u_texture, io_texCoords);
}

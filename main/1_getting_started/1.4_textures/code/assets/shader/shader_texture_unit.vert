#version 330 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_color;
layout(location = 2) in vec2 a_texCoord;

out vec3 io_color;
out vec2 io_texCoord;

void main()
{
    gl_Position = vec4(a_pos, 1.0);
    io_color    = a_color;
    io_texCoord = a_texCoord;
}

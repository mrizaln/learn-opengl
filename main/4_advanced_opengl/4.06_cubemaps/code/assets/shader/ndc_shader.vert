#version 330 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texCoords;

out vec2 io_texCoords;

void main()
{
    gl_Position  = vec4(a_pos.x, a_pos.z, 0.0, 1.0);
    io_texCoords = a_texCoords;
}

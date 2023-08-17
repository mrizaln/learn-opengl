#version 330 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texCoord;

// out vec3 io_color;
out vec2 io_texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // note that we read the multiplication from right to left
    gl_Position = projection * view * model * vec4(a_pos, 1.0);
    io_texCoord = a_texCoord;
    // io_color    = (a_normal + 1.0) / 2.0;
}

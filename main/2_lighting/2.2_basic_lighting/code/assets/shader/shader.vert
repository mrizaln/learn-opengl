#version 330 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_normal;

out vec3 io_fragPos;
out vec3 io_normal;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
    gl_Position = u_projection * u_view * u_model * vec4(a_pos, 1.0);
    io_fragPos  = vec3(u_model * vec4(a_pos, 1.0));
    // io_normal   = a_normal;
    io_normal = mat3(transpose(inverse(u_model))) * a_normal;
}

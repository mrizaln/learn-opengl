#version 330 core

layout(location = 0) in vec3 a_pos;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
    // note that we read the multiplication from right to left
    gl_Position = u_projection * u_view * u_model * vec4(a_pos, 1.0);
}

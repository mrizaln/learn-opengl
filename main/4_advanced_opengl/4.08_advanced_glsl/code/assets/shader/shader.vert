#version 330 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texCoords;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
    gl_Position = u_projection * u_view * u_model * vec4(a_pos, 1.0);

    // gl_PointSize = (1.0 + gl_Position.z) / 2.0;    // [-1.0, 1.0] -> [0.0, 1.0]

    // float z      = gl_Position.z / (gl_Position.w * gl_Position.w);
    // gl_PointSize = 25.0 * z;

    // float z      = gl_Position.z / gl_Position.w;
    // gl_PointSize = 10.0 * (-(z * z) / 8.0 + z / 4.0 + 7.0 / 8.0);

    gl_PointSize = 25.0 / gl_Position.z;
}

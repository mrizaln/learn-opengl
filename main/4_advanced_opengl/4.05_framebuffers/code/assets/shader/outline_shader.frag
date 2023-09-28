#version 330 core

out vec4 o_fragColor;

uniform vec3 u_outlineColor;

void main()
{
    o_fragColor = vec4(u_outlineColor, 1.0);
}

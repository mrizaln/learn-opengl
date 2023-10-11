#version 330 core

out vec4 o_fragColor;

in vec3 io_normal;
in vec3 io_fragPos;

uniform vec3        u_viewPos;
uniform samplerCube u_skybox;

void main()
{
    vec3 I      = normalize(io_fragPos - u_viewPos);
    vec3 R      = reflect(I, normalize(io_normal));
    o_fragColor = vec4(texture(u_skybox, R).rgb, 1.0);
}

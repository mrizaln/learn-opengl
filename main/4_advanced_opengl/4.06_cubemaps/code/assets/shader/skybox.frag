#version 330 core

out vec4 o_fragColor;

in vec3 vf_texCoords;

uniform samplerCube u_skybox;

void main()
{
    o_fragColor = texture(u_skybox, vf_texCoords);
}

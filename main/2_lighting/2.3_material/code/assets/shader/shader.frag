#version 330 core

struct Material
{
    vec3  m_ambient;
    vec3  m_diffuse;
    vec3  m_specular;
    float m_shininess;
};

struct Light
{
    vec3 m_position;
    vec3 m_ambient;
    vec3 m_diffuse;
    vec3 m_specular;
};

out vec4 o_fragColor;

in vec3 io_fragPos;
in vec3 io_normal;

uniform vec3     u_viewPos;
uniform Material u_material;
uniform Light    u_light;

void main()
{
    // ambient
    vec3 ambient = u_light.m_ambient * u_material.m_ambient;

    // diffuse
    vec3  normal       = normalize(io_normal);
    vec3  lightDir     = normalize(u_light.m_position - io_fragPos);
    float diffuseValue = max(dot(normal, lightDir), 0.0);    // clamp to non-negative
    vec3  diffuse      = diffuseValue * u_light.m_diffuse * u_material.m_diffuse;

    // specular
    vec3  viewDir       = normalize(u_viewPos - io_fragPos);
    vec3  reflectDir    = reflect(-lightDir, normal);                                         // 1st param expects a vector that points from light source towards the fragment
    float specularValue = pow(max(dot(viewDir, reflectDir), 0.0), u_material.m_shininess);    // 32 is the shininess value
    vec3  specular      = specularValue * u_light.m_specular * u_material.m_specular;

    // combine all
    vec3 result = ambient + diffuse + specular;
    o_fragColor = vec4(result, 1.0);
}

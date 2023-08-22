#version 330 core

struct Material
{
    sampler2D m_diffuse;
    sampler2D m_specular;
    sampler2D m_emission;
    float     m_shininess;
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
in vec2 io_texCoords;

uniform vec3     u_viewPos;
uniform Material u_material;
uniform Light    u_light;
uniform bool     u_enableEmissionMap;

void main()
{
    // ambient
    vec3 ambient = u_light.m_ambient * texture(u_material.m_diffuse, io_texCoords).rgb;

    // diffuse
    vec3  normal       = normalize(io_normal);
    vec3  lightDir     = normalize(u_light.m_position - io_fragPos);
    float diffuseValue = max(dot(normal, lightDir), 0.0);    // clamp to non-negative
    vec3  diffuse      = diffuseValue * u_light.m_diffuse * texture(u_material.m_diffuse, io_texCoords).rgb;

    // specular
    vec3  viewDir       = normalize(u_viewPos - io_fragPos);
    vec3  reflectDir    = reflect(-lightDir, normal);                                         // 1st param expects a vector that points from light source towards the fragment
    float specularValue = pow(max(dot(viewDir, reflectDir), 0.0), u_material.m_shininess);    // 32 is the shininess value
    vec3  specular      = specularValue * u_light.m_specular * texture(u_material.m_specular, io_texCoords).rgb;

    // emission
    vec3 emission = texture(u_material.m_emission, io_texCoords).rgb * vec3(u_enableEmissionMap);

    // combine all
    vec3 result = ambient + diffuse + specular + emission;
    o_fragColor = vec4(result, 1.0);
}

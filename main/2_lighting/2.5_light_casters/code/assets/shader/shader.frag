#version 330 core

struct Material
{
    sampler2D m_diffuse;
    sampler2D m_specular;
    sampler2D m_emission;
    float     m_shininess;
};

struct DirectionalLight
{
    vec3 m_direction;
    vec3 m_ambient;
    vec3 m_diffuse;
    vec3 m_specular;
};

struct PointLight
{
    vec3  m_position;
    vec3  m_ambient;
    vec3  m_diffuse;
    vec3  m_specular;
    float m_constant;
    float m_linear;
    float m_quadratic;
};

struct SpotLight
{
    vec3  m_position;
    vec3  m_direction;
    vec3  m_ambient;
    vec3  m_diffuse;
    vec3  m_specular;
    float m_cutOff;
    float m_outerCutOff;
    float m_constant;
    float m_linear;
    float m_quadratic;
};

out vec4 o_fragColor;

in vec3 io_fragPos;
in vec3 io_normal;
in vec2 io_texCoords;

uniform vec3             u_viewPos;
uniform Material         u_material;
uniform DirectionalLight u_directionalLight;
uniform PointLight       u_pointLight;
uniform SpotLight        u_spotLight;
uniform bool             u_enableEmissionMap;

vec3 calculateDirectionalLight()
{
    vec3 normal     = normalize(io_normal);
    vec3 viewDir    = normalize(u_viewPos - io_fragPos);
    vec3 lightDir   = normalize(-u_directionalLight.m_direction);    // direction vector from fragment to u_spotLight source
    vec3 reflectDir = reflect(-lightDir, normal);                    // 1st param expects a vector that points from u_spotLight source towards the fragment

    vec3 ambient = u_directionalLight.m_ambient * texture(u_material.m_diffuse, io_texCoords).rgb;

    float diffuseValue = max(dot(normal, lightDir), 0.0);    // clamp to non-negative
    vec3  diffuse      = diffuseValue * u_directionalLight.m_diffuse * texture(u_material.m_diffuse, io_texCoords).rgb;

    float specularValue = pow(max(dot(viewDir, reflectDir), 0.0), u_material.m_shininess);    // 32 is the shininess value
    vec3  specular      = specularValue * u_directionalLight.m_specular * texture(u_material.m_specular, io_texCoords).rgb;

    vec3 emission = texture(u_material.m_emission, io_texCoords).rgb * vec3(u_enableEmissionMap);

    vec3 result = ambient + diffuse + specular + emission;
    return result;
}

vec3 calculatePointLight()
{
    vec3 normal     = normalize(io_normal);
    vec3 viewDir    = normalize(u_viewPos - io_fragPos);
    vec3 lightDir   = normalize(u_pointLight.m_position - io_fragPos);    // direction vector from fragment to u_spotLight source
    vec3 reflectDir = reflect(-lightDir, normal);                         // 1st param expects a vector that points from u_spotLight source towards the fragment

    vec3 ambient = u_pointLight.m_ambient * texture(u_material.m_diffuse, io_texCoords).rgb;

    float diffuseValue = max(dot(normal, lightDir), 0.0);    // clamp to non-negative
    vec3  diffuse      = diffuseValue * u_pointLight.m_diffuse * texture(u_material.m_diffuse, io_texCoords).rgb;

    float specularValue = pow(max(dot(viewDir, reflectDir), 0.0), u_material.m_shininess);
    vec3  specular      = specularValue * u_pointLight.m_specular * texture(u_material.m_specular, io_texCoords).rgb;

    vec3 emission = texture(u_material.m_emission, io_texCoords).rgb * vec3(u_enableEmissionMap);

    float distance    = length(u_pointLight.m_position - io_fragPos);
    float attenuation = 1.0 / (u_pointLight.m_constant + u_pointLight.m_linear * distance + u_pointLight.m_quadratic * (distance * distance));

    vec3 result = (ambient + diffuse + specular) * attenuation + emission;
    return result;
}

vec3 calculateSpotLight()
{
    vec3 normal     = normalize(io_normal);
    vec3 viewDir    = normalize(u_viewPos - io_fragPos);
    vec3 lightDir   = normalize(u_spotLight.m_position - io_fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);

    vec3 ambient = u_pointLight.m_ambient * texture(u_material.m_diffuse, io_texCoords).rgb;

    float diffuseValue = max(dot(normal, lightDir), 0.0);    // clamp to non-negative
    vec3  diffuse      = diffuseValue * u_pointLight.m_diffuse * texture(u_material.m_diffuse, io_texCoords).rgb;

    float specularValue = pow(max(dot(viewDir, reflectDir), 0.0), u_material.m_shininess);
    vec3  specular      = specularValue * u_pointLight.m_specular * texture(u_material.m_specular, io_texCoords).rgb;

    vec3 emission = texture(u_material.m_emission, io_texCoords).rgb * vec3(u_enableEmissionMap);

    float distance    = length(u_spotLight.m_position - io_fragPos);
    float attenuation = 1.0 / (u_spotLight.m_constant + u_spotLight.m_linear * distance + u_spotLight.m_quadratic * distance * distance);

    // smooth edges
    float theta     = dot(lightDir, normalize(-u_spotLight.m_direction));    // negated because we want the vectors to point towards the u_spotLight source
    float epsilon   = u_spotLight.m_cutOff - u_spotLight.m_outerCutOff;
    float intensity = clamp((theta - u_spotLight.m_outerCutOff) / epsilon, 0.0, 1.0);

    vec3 result = (ambient + diffuse + specular) * attenuation * intensity + emission;
    return result;
}

void main()
{
    vec3 color = calculateDirectionalLight() + calculatePointLight() + calculateSpotLight();
    // vec3 color = calculateDirectionalLight();
    // vec3 color  = calculatePointLight();
    // vec3 color  = calculateSpotLight();
    o_fragColor = vec4(color, 1.0);
}

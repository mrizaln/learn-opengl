#version 330 core

out vec4 o_fragColor;

in vec3 io_fragPos;
in vec3 io_normal;

uniform vec3 u_objectColor;
uniform vec3 u_lightColor;
uniform vec3 u_lightPos;
uniform vec3 u_viewPos;

void main()
{
    // ambient
    float ambientStrength = 0.1;
    vec3  ambient         = ambientStrength * u_lightColor;

    // diffuse
    vec3  normal       = normalize(io_normal);
    vec3  lightDir     = normalize(u_lightPos - io_fragPos);
    float diffuseValue = max(dot(normal, lightDir), 0.0);    // clamp to non-negative
    vec3  diffuse      = u_lightColor * diffuseValue;

    // specular
    float specularStrength = 0.5;
    vec3  viewDir          = normalize(u_viewPos - io_fragPos);
    vec3  reflectDir       = reflect(-lightDir, normal);                     // 1st param expects a vector that points from light source towards the fragment
    float specularValue    = pow(max(dot(viewDir, reflectDir), 0.0), 32);    // 32 is the shininess value
    vec3  specular         = specularStrength * specularValue * u_lightColor;

    // combine all
    vec3 result = (ambient + diffuse + specular) * u_objectColor;
    o_fragColor = vec4(result, 2.0);
}

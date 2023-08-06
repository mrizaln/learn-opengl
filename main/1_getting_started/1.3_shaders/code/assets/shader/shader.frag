#version 330 core

out vec4    FragColor;
in vec3     ourColor;
uniform int u_invertColor;    // boolean

void main()
{
    if (u_invertColor == 1) {
        FragColor = vec4(1.0 - ourColor, 1.0);
    } else {
        FragColor = vec4(ourColor, 1.0);
    }
}

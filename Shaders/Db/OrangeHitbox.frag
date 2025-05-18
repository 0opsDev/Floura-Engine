#version 460 core
out vec4 FragColor;

void main()
{
    vec3 color = vec3(1.0, 0.412, 0.0);

    // Set the fragment color
    FragColor = vec4(color, 1.0);
}
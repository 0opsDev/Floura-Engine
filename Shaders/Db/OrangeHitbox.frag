#version 460 core
out vec4 FragColor;
//probably should change file name

uniform vec3 colour;

void main()
{
    // Set the fragment color
    FragColor = vec4(colour, 1.0);
}
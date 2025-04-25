#version 460 core

out vec4 FragColor;
in vec2 texCoords;

uniform sampler2D screenTexture;
uniform float time;  // Pass in a time uniform for animation

void main()
{
    vec4 texColor = texture(screenTexture, texCoords);

    FragColor = texColor;
}
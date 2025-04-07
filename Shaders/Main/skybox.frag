#version 330 core
out vec4 FragColor;

in vec3 texCoords;

uniform samplerCube skybox;
//color of light from sky
uniform vec4 skyColor;

vec4 skyTEX = texture(skybox, texCoords);

void main()
{
    FragColor = skyTEX;
}

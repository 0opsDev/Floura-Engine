#version 460 core
out vec4 FragColor;

in vec3 texCoords;

uniform samplerCube skybox;
//color of light from sky
uniform vec3 skyRGBA;

void main()
{
    vec4 skyTEX = texture(skybox, texCoords);
    vec3 color = skyTEX.rgb * skyRGBA;
    FragColor = vec4(color, 1.0);
}
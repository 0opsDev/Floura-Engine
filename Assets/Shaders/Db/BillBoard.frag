#version 460 core
out vec4 FragColor;

in vec2 texCoord;
uniform sampler2D texture0;

void main()
{
    vec4 texColor = texture(texture0, texCoord);

    // Discard fragment if alpha is too low
    if (texColor.a < 0.1) // Adjust threshold if needed
        discard;

    FragColor = texColor;
}
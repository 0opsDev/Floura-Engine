#version 330 core

// Outputs colors in RGBA
out vec4 FragColor;
in vec3 crntPos;
in vec3 Normal;
in vec3 color;
in vec2 texCoord;

uniform vec4 lightColor;
uniform sampler2D diffuse0;

vec4 LightShader()
{
    vec4 diffuseColor = texture(diffuse0, texCoord);
    diffuseColor.rgb = pow(diffuseColor.rgb, vec3(2.2)); // Correct the gamma
        if (diffuseColor.a < 0.1)
            discard;
    return diffuseColor * lightColor;
}

void main()
{
    FragColor = LightShader();
}

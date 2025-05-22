#version 460 core

out vec4 FragColor;
in vec2 texCoords;

uniform sampler2D depthMap;
uniform sampler2D screenTexture;
uniform float time;  // Pass in a time uniform for animation
uniform bool enableFB;
vec4 FragColorT;
const float near = 0.1;  // Set near plane to a small value
const float far = 100.0; // Set far plane to a large value

float linearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0; // Transform from [0,1] to [-1,1]
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
    vec4 texColor = texture(screenTexture, texCoords);
    float depthValue = texture(depthMap, texCoords).r; 
    float linearDepth = linearizeDepth(depthValue);

    FragColorT = vec4(vec3(linearDepth / far), 1.0);

    if (!enableFB)
    {
        FragColorT = texColor;
    }

    FragColor = FragColorT;
}
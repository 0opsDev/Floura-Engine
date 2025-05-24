#version 460 core

out vec4 FragColor;
in vec2 texCoords;

uniform sampler2D depthMap;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D screenTexture;
uniform float time;
uniform bool enableFB;

vec4 FragColorT;
const float near = 0.1;
const float far = 100.0;

float linearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
    vec4 texColor = texture(screenTexture, texCoords);
    vec3 Diffuse = texture(gAlbedoSpec, texCoords).rgb;
    float Spec = texture(gAlbedoSpec, texCoords).a;
    vec3 PositionM = texture(gPosition, texCoords).rgb;
    vec3 Normals = texture(gNormal, texCoords).rgb; 

    float depthValue = texture(depthMap, texCoords).r;
    float linearDepth = linearizeDepth(depthValue);

    //FragColorT = vec4(vec3(linearDepth / far), 1.0);
    //FragColorT = vec4(Diffuse, 1.0);
    //FragColorT = vec4(PositionM, 1.0);
    FragColorT = vec4(Normals, 1.0);
    //FragColorT = vec4(vec3(Spec), 1.0);
        
    if (!enableFB)
    {
        FragColorT = texColor;
    }

    FragColor = FragColorT;
}
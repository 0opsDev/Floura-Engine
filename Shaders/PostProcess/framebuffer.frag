#version 460 core

out vec4 FragColor;
in vec2 texCoords;

uniform sampler2D depthMap;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D screenTexture;
uniform mat4 cameraMatrix;

uniform vec3 fogColor;

uniform bool enableFB;

uniform float DepthDistance;
uniform float FarPlane;
uniform float NearPlane;

uniform bool doFog;

float linearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0;
    return (NearPlane * FarPlane) / (FarPlane - z * (FarPlane - NearPlane)); 
}

float logisticDepth(float depth, float steepness, float offset)
{
    float zVal = linearizeDepth(depth);
    float expVal = exp(clamp(-steepness * (zVal - offset), -10.0, 10.0));
    return 1.0 / (1.0 + expVal);
}

void main() {
    if (!doFog) {
        FragColor = texture(screenTexture, texCoords);
        return;
    }

    float depth = texture(depthMap, texCoords).r;
    float Depth = logisticDepth(depth, 0.1f, DepthDistance);
    vec3 final = texture(screenTexture, texCoords).rgb * (1.0f - Depth) + vec3(Depth * fogColor);

    FragColor = vec4(final, 1.0);
}
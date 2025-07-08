#version 460 core
out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D depthMap;
uniform mat4 cameraMatrix;

uniform vec3 lightColor;
uniform vec3 fogColor;
uniform vec3 skyColor;

uniform bool DEFtoggle;
uniform bool doFog;
uniform vec2 screenSize;
uniform float time;

uniform float DepthDistance;
uniform float FarPlane;
uniform float NearPlane;

// Depth conversion
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

void main()
{
    vec4 albedoSpec = texture(gAlbedoSpec, texCoord);


    /*
    FOG
    */
    if (doFog){
    float depth = texture(depthMap, texCoord).r;
    float Depth = logisticDepth(depth, 0.1f, DepthDistance);
     albedoSpec.rgb = albedoSpec.rgb * (1.0f - Depth) + vec3(Depth * fogColor);
    }

    FragColor = vec4(albedoSpec.rgb * skyColor, 1.0);

}
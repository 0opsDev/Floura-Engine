#version 460 core

out vec4 FragColor;
in vec2 texCoords;

uniform sampler2D depthMap;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D screenTexture;
uniform mat4 cameraMatrix;

uniform float gamma;

float calculateExposure(vec3 avgColor)
{
    float lum = dot(avgColor, vec3(0.2126, 0.7152, 0.0722));
    
    lum = max(lum, 0.0001f); 

    float middleGray = 1.0f; 
    float minExp = 0.01f;
    float maxExp = 10.0f;

    float targetExposure = middleGray / lum;

    return clamp(targetExposure, minExp, maxExp);
}

void main() {
    vec3 colour = texture(screenTexture, texCoords).rgb;

    int lastLOD = textureQueryLevels(screenTexture) - 1;
	vec3 avgColor = textureLod(screenTexture, vec2(0.5, 0.5), lastLOD).rgb;

	float autoExposure = calculateExposure(avgColor);

    vec3 tonemappedColour = vec3(1.0f) - exp(-colour  * autoExposure);

    FragColor.rgb = pow(tonemappedColour, vec3(gamma));
    //FragColor.rgb = pow(colour, vec3(gamma));

    //FragColor.rgb = texture(gNormal, texCoords).rgb;
}
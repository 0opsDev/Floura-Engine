#version 460 core

out vec4 FragColor;
in vec2 texCoords;

uniform sampler2D depthMap;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D screenTexture;
uniform sampler2D gSpecular;
uniform mat4 cameraMatrix;

uniform sampler2D dbgColour;
uniform bool overlayDebug;

uniform float gamma;

float calculateExposure(vec3 avgColor)
{
    float lum = dot(avgColor, vec3(0.2126, 0.7152, 0.0722));
    
    lum = max(lum, 0.0001f); 

    //float middleGray = 1.0f;
    float middleGray = 0.18f;
    float acesBoost = 1.6f;
    
    float minExp = 0.01f;
    float maxExp = 10.0f;

    //float targetExposure = (middleGray / lum);
    float targetExposure = (middleGray / lum) * acesBoost;

    return clamp(targetExposure, minExp, maxExp);
}

vec3 ACESFilm(vec3 x) {
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((x*(a*x+b))/(x*(c*x+d)+e), 0.0, 1.0);
}


void main() {
    vec3 colour = texture(screenTexture, texCoords).rgb;
    
    int lastLOD = textureQueryLevels(screenTexture) - 1;
	vec3 avgColor = textureLod(screenTexture, vec2(0.5, 0.5), lastLOD).rgb;
    //vec3 avgColor = textureLod(screenTexture, texCoords, lastLOD).rgb;

    float autoExposure = calculateExposure(avgColor);
    
    vec3 tonemappedColour = vec3(1.0f) - exp(-colour  * autoExposure);
    //vec3 tonemappedColour = vec3(1.0f) - exp(-colour  * autoExposure);

    //vec3 tonemappedColour = vec3(1.0f) - exp(-colour  * 5.0);

    vec3 aces = ACESFilm(colour * autoExposure * 1.8);
    
    FragColor.rgb = pow(aces, vec3(gamma)); // tonemap
    //FragColor.rgb = pow(aces, vec3(gamma)); // aces
    //FragColor.rgb = pow(colour, vec3(gamma)); // off

    if (overlayDebug){
        vec4 dbg = texture(dbgColour, texCoords);
        //FragColor.rgb *= dbg.rgb;
        FragColor.rgb = mix(FragColor.rgb, dbg.rgb, 0.5);
    }


    

    //FragColor.rgb = avgColor;
    
    //FragColor.rgb = texture(gNormal, texCoords).rgb;

    //FragColor.rgb = texture(gNormal, texCoords).rgb;
}
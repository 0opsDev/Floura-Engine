#version 460 core

out vec4 FragColor;
in vec2 texCoords;

uniform sampler2D depthMap;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D screenTexture;
uniform sampler2D gSpecular;
uniform sampler2D BlueNoiseTex;
uniform mat4 cameraMatrix;

uniform sampler2D dbgColour;
uniform bool overlayDebug;

uniform float gamma;
uniform float sharpness;
uniform float time;
uniform float deltaTime;
uniform float accum24value;

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

float random (vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
}


const float offset_x = 1.0f / 1605.0f;
const float offset_y = 1.0f / 885.0f;

vec2 offsets[9] = vec2[]
(
vec2(-offset_x,  offset_y), vec2( 0.0f,    offset_y), vec2( offset_x,  offset_y),
vec2(-offset_x,  0.0f),     vec2( 0.0f,    0.0f),     vec2( offset_x,  0.0f),
vec2(-offset_x, -offset_y), vec2( 0.0f,   -offset_y), vec2( offset_x, -offset_y)
);

float kernel[9] = float[](
0.0, -1.0,  0.0,
-1.0,  5.0, -1.0,
0.0, -1.0,  0.0
);


vec3 vignette(vec3 colour, float u, float v){
  vec2 nvuv = texCoords - 0.5;
  float nvdist = length(nvuv);
  float vignette = smoothstep(u, v, nvdist);
  colour.rgb *= vignette;

  return colour;
}

vec3 filmgrain(vec3 colour, float amount, float randomValue)
{
  float noise = random(vec2(texCoords.x * randomValue, texCoords.y + randomValue + 2) );

    //vec2 noiseUV = vec2(texCoords.xy) / vec2(textureSize(BlueNoiseTex, 0)); // new uvec2

    //vec2 scrollingUV = noiseUV + fract(randomValue * vec2(12.9898, 78.233));
    //vec2 blueNoise = texture(BlueNoiseTex, scrollingUV).rg;
    
  return colour + noise.r * amount;
}

vec3 filmgrainColoured(vec3 colour, float amount, float randomValue)
{
  float r = random(vec2(texCoords.x + randomValue, texCoords.y + randomValue) );
  float g = random(vec2(texCoords.x + randomValue + 0.1, texCoords.y + randomValue + 0.3) );
  float b = random(vec2(texCoords.x + randomValue + 0.2, texCoords.y + randomValue) );

    vec2 noiseUV = vec2(texCoords.xy) / vec2(textureSize(BlueNoiseTex, 0)); // new uvec2

    vec2 scrollingUV = noiseUV + fract(randomValue * vec2(12.9898, 78.233));
    vec2 blueNoise = texture(BlueNoiseTex, scrollingUV).rg;

  return colour + ( vec3(r,g,b) * amount );
}


float lumaFromRGB(vec3 rgb)
{
    vec3 weights = vec3(0.2126, 0.7152, 0.0722);
    float luminance = dot(rgb, weights);
    return luminance;
}

vec2 sharpenOffsets[4] = vec2[4](
vec2( 1.0,  0.0),
vec2( 0.0,  1.0),
vec2(-1.0,  0.0),
vec2( 0.0, -1.0)
);

vec3 spawnSharpenedImage(sampler2D image, vec2 texCoord2) {

    vec2 texelSize = 1.0 / textureSize(image, 0);

    float luma = lumaFromRGB(texture(image, texCoords).rgb);
    
    float nSharpness = clamp(luma * sharpness, 0.0, 1.0);
    //float nSharpness = luma;
    //float nSharpness =sharpness;

    float neighbor = nSharpness * -1.0;
    float center = nSharpness * 4.0 + 1.0;
    
    return texture(image, texCoords + vec2(0, texelSize.y)).rgb * neighbor +
    texture(image, texCoords + vec2(-texelSize.x, 0)).rgb * neighbor +
    texture(image, texCoords).rgb * center +
    texture(image, texCoords + vec2(texelSize.x, 0)).rgb * neighbor +
    texture(image, texCoords + vec2(0, -texelSize.y)).rgb * neighbor;
}

void main() {

    //FragColor.rgb =texture(screenTexture, texCoords).rgb; return;
    
    vec3 colour = spawnSharpenedImage(screenTexture, texCoords);

    colour = vignette(colour, 0.9, 0.5);
    
    int lastLOD = textureQueryLevels(screenTexture) - 1;
	vec3 avgColor = textureLod(screenTexture, vec2(0.5, 0.5), lastLOD).rgb;
    //vec3 avgColor = textureLod(screenTexture, texCoords, lastLOD).rgb;

    float autoExposure = calculateExposure(avgColor);
    ///float autoExposure = 0.1;
    
    vec3 tonemappedColour = vec3(1.0f) - exp(-colour  * autoExposure);
    //vec3 tonemappedColour = vec3(1.0f) - exp(-colour  * autoExposure);

    //vec3 tonemappedColour = vec3(1.0f) - exp(-colour  * 5.0);

    vec3 aces = ACESFilm(colour * autoExposure * 1.8);

    //aces = filmgrain(aces, 0.1, accum24value);
    
    FragColor.rgb = pow(aces, vec3(gamma)); // tonemap
    //FragColor.rgb = pow(aces, vec3(gamma)); // aces
    //FragColor.rgb = pow(colour, vec3(gamma)); // off

    if (overlayDebug){
        vec4 dbg = texture(dbgColour, texCoords);
        //FragColor.rgb *= dbg.rgb;
        FragColor.rgb = mix(FragColor.rgb, dbg.rgb, 0.5);
    }

    //float luma = lumaFromRGB(texture(screenTexture, texCoords).rgb);

    //FragColor.rgb = vec3(luma);
    
    //FragColor = texture(gPosition, texCoords);
    

    //FragColor.rgb = avgColor;
    
    //FragColor.rgb = texture(gNormal, texCoords).rgb;

    //FragColor.rgb = texture(gNormal, texCoords).rgb;
}
#version 460 core

#extension GL_ARB_gpu_shader_int64 : enable
#extension GL_ARB_bindless_texture : require

out vec4 FragColor;

in highp vec2 texCoord;

// geometry
uniform highp sampler2D screentexture;
uniform sampler2D gNormal;
uniform sampler2D depthMap;
uniform highp sampler2D gVelocity;

// history
uniform sampler2D hColour;
uniform sampler2D hDepthTexture;
uniform sampler2D hNormal;

uniform vec2 screenSize;
uniform float time;
uniform int frame;

uniform float FarPlane;
uniform float NearPlane;

float linearizeDepth(float depth, float NP, float FP)
{
    return (2.0 * NP * FP) / (FP + NP - (depth * 2.0 - 1.0) * (FP - NP));
}

float lumaFromRGB(vec3 rgb)
{
    vec3 weights = vec3(0.2126, 0.7152, 0.0722);
    float luminance = dot(rgb, weights);
    return luminance;
}

vec3 colourClamp(int radius, vec3 colour)
{
    vec3 minColour = vec3(9999.0);
    vec3 maxColour = vec3(-9999.0);

    vec2 screenSize = textureSize(screentexture, 0);
    vec2 texelSize = 1.0 / screenSize;

    for(int x = -radius; x <= radius; ++x){
        for(int y = -radius; y <= radius; ++y){
            // texcoords
            vec2 neighbourCoords = vec2(texCoord) + vec2(x, y);
            //neighbourCoords = clamp(neighbourCoords, vec2(0, 0), sreenSize - 1);

            vec2 offset = vec2(x, y) * texelSize;
            vec3 neighbourColour = texture(screentexture, texCoord + offset).rgb;
            // min and max screencolours in radius*radius
            minColour = min(minColour, neighbourColour);
            maxColour = max(maxColour, neighbourColour);
        }
    }

    return clamp(colour, minColour, maxColour);
}



void main()
{
    vec3 screen = texture(screentexture, texCoord).rgb;

    

    //early z cutoff
   // if (depth >= 0.99999)
   // {
    //    FragColor = vec4(screen, 1.0f);
     //   return;
    //}

    //vec3 normal = texture(gNormal, texCoord).rgb;
    //float displacement = texture(gNormal, texCoord).a;

    highp vec2 velocity = texture(gVelocity, texCoord).rg;

    highp vec2 historyTexCoord = texCoord - velocity;
    
    if (any(lessThan(historyTexCoord, vec2(0.0))) || any(greaterThan(historyTexCoord, vec2(1.0))))
    {
        FragColor = vec4(screen, 1.0f);
        return;
    }
    
    vec3 previousColour = texture(hColour, historyTexCoord).rgb;

    //FragColor = vec4(cLuminance.r, 0.0, 0.0, 1.0);
    //return;
  
    //vec4 previousNormals = texture(hNormal, historyTexCoord);
    //float gDepthLinearized = linearizeDepth(depth, NearPlane, FarPlane); // depth
    
    //float hDepth = texture(hDepthTexture, historyTexCoord).r;
    //float hDepthLinearized = linearizeDepth(hDepth, NearPlane, FarPlane);
    //float depth = texture(depthMap, texCoord).r;
    //float gDepthLinearized = linearizeDepth(depth, NearPlane, FarPlane);
    
    vec3 previousColourClamped = colourClamp(1, previousColour);
    
    //float blendFactor = 0.9;
    float blendFactor = 0.9;
   //vec3 accumulated = mix(screen, previousColourClamped, blendFactor);
    
    vec3 accumulated = mix(screen, previousColourClamped, blendFactor);
    //vec3 accumulated = mix(screen, previousColour, blendFactor);
    
    FragColor = vec4(accumulated, 1.0);

    //FragColor = vec4(vec3(v), 1.0);
    
    //FragColor = vec4(previousColourClamped, 1.0);
}
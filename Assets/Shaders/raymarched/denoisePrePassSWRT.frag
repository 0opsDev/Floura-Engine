#version 460 core

out vec4 FragColor;
in highp vec2 texCoord;

layout(location = 6) out vec4 filteredVariance;

uniform sampler2D depthMap;
uniform sampler2D gNormal;
uniform sampler2D swrtIndirect;
uniform sampler2D swrtEmission;
uniform sampler2D swrtIndirectSpecular;
uniform sampler2D swrtEmissionSpecular;

uniform bool doDenoise;
uniform bool doDenoiseSplitDBGView;
uniform float NearPlane;
uniform float FarPlane;

float lumaFromRGB(vec3 rgb){
    vec3 weights = vec3(0.2126, 0.7152, 0.0722);
    float luminance = dot(rgb, weights);
    return luminance;
}

float linearizeDepth(float depth, float NearPlane, float FarPlane){
    return (2.0 * NearPlane * FarPlane) / (FarPlane + NearPlane - (depth * 2.0 - 1.0) * (FarPlane - NearPlane));
}

float indirectDenoise(sampler2D inputSampler, float colour, float linearizedDepth, vec3 normal, float farplane, int radius){
    vec2 lscreenSize = textureSize(inputSampler, 0);
    vec2 ltexelSize = 1.0 / lscreenSize;

    float avgL =0.0; int h = 0;
    float avgC = 0.0f;
    for(int x = -radius; x <= radius; ++x){
        for(int y = -radius; y <= radius; ++y){
            vec2 offset = vec2(x, y) * ltexelSize;
            //if (offset.x > 1.0 || offset.y > 1.0 || offset.x < 0.0 || offset.y < 0.0) continue;
            float neighbourDepth = texture(depthMap, texCoord + offset).r;
            float neighbourDepthLinearized = linearizeDepth(neighbourDepth, 0.1f, farplane);
            float depthDifference = abs(neighbourDepthLinearized - linearizedDepth);

            vec3 neighbourNormal = texture(gNormal, texCoord + offset).rgb;
            float normalSimilarity = dot(neighbourNormal, normal);
            if (depthDifference > 0.1 || normalSimilarity < 0.9) continue;


            float neighbourColour = texture(inputSampler, texCoord + offset).r;
            //float nLumanance = lumaFromRGB(neighbourColour);
            avgC += neighbourColour;

            h++;
            //avgL += nLumanance;
        }
    }
    return avgC / h;
}


void main(){
    float indirectVariance = texture(swrtIndirect,texCoord).a;
    float emissionVariance = texture(swrtEmission,texCoord).a;
    float indirectSpecularVariance = texture(swrtIndirectSpecular,texCoord).a;
    float indirectEmissionVariance = texture(swrtEmissionSpecular,texCoord).a;

    float gdepth = texture2D(depthMap, texCoord).r;
    vec3 gnrm = normalize(texture(gNormal, texCoord).rgb);
    
    vec2 lscreenSize = textureSize(swrtIndirect, 0);
    bool split = true;
    
    vec4 finalVariance = vec4(indirectVariance, emissionVariance, indirectSpecularVariance, indirectEmissionVariance);
    
    int denoiseRadius = 3;
    
    if (gl_FragCoord.x > lscreenSize.x / 2 && doDenoiseSplitDBGView) split = false;
    if (doDenoise && split){
        float d = linearizeDepth(gdepth, NearPlane, FarPlane);
        finalVariance.r = indirectDenoise(swrtIndirect, indirectVariance, d, normalize(gnrm), FarPlane, denoiseRadius);
        finalVariance.g = indirectDenoise(swrtEmission,emissionVariance, d, normalize(gnrm), FarPlane, denoiseRadius);
        //finalVariance.b = indirectDenoise(swrtIndirectSpecular, indirectSpecularVariance, d, normalize(gnrm), FarPlane, denoiseRadius);
        finalVariance.a = indirectDenoise(swrtEmissionSpecular, indirectEmissionVariance, d, normalize(gnrm), FarPlane, denoiseRadius);
    }

    filteredVariance = finalVariance;
}


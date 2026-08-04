#version 460 core

//out vec4 FragColor;
in highp vec2 texCoord;

layout(location = 0) out vec4 hIndirect;
layout(location = 1) out vec4 hEmission;
layout(location = 2) out vec4 hIndirectSpecular;
layout(location = 3) out vec4 hEmissionSpecular;
layout(location = 4) out vec3 presentImage;
layout(location = 5) out float hdepth;

uniform sampler2D swrtSpecular;
uniform sampler2D swrtDbgColour;

uniform sampler2D gEmission;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D depthMap;
uniform sampler2D gSpecular;

uniform sampler2D swrtIndirect;
uniform sampler2D swrtEmission;
uniform sampler2D swrtIndirectSpecular;
uniform sampler2D swrtEmissionSpecular;
uniform sampler2D filteredVariance;

uniform int denoiseRadius;
uniform bool doDenoise;
uniform bool doDenoiseSplitDBGView;

uniform float NearPlane;
uniform float FarPlane;

uniform int passNum;

float lumaFromRGB(vec3 rgb){
    vec3 weights = vec3(0.2126, 0.7152, 0.0722);
    float luminance = dot(rgb, weights);
    return luminance;
}

float linearizeDepth(float depth, float NearPlane, float FarPlane){
    return (2.0 * NearPlane * FarPlane) / (FarPlane + NearPlane - (depth * 2.0 - 1.0) * (FarPlane - NearPlane));
}

vec3 indirectDenoise(sampler2D inputSampler, vec3 colour, float linearizedDepth, vec3 normal, float farplane, int radius){
    vec2 lscreenSize = textureSize(inputSampler, 0);    
    vec2 ltexelSize = 1.0 / lscreenSize;

    //float avgL =0.0; 
    int h = 0;
    vec3 avgC = vec3(0.0f);
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


            vec3 neighbourColour = texture(inputSampler, texCoord + offset).rgb;
            //float nLumanance = lumaFromRGB(neighbourColour);
            avgC += neighbourColour;

            h++;
            //avgL += nLumanance;
        }
    }
    return avgC / h;
    /*
    float avgLuminance = 0.0f; if (h > 0) avgLuminance = avgL / float(h);
    return colour * (avgLuminance / max(lumaFromRGB(colour), 0.0001f));
    */
}

vec3 specularDenoise(sampler2D inputSampler, vec3 colour, float linearizedDepth, vec3 normal, vec2 rm, float farplane, int radius){
    vec2 lscreenSize = textureSize(inputSampler, 0);
    vec2 ltexelSize = 1.0 / lscreenSize;

    //float avgL =0.0; 
    int h = 0;
    vec3 avgC = vec3(0.0f);
    for(int x = -radius; x <= radius; ++x){
        for(int y = -radius; y <= radius; ++y){
            vec2 offset = vec2(x, y) * ltexelSize;
           // if (offset.x > 1.0 || offset.y > 1.0 || offset.x < 0.0 || offset.y < 0.0) continue;
            
            float neighbourDepth = texture(depthMap, texCoord + offset).r;
            float neighbourDepthLinearized = linearizeDepth(neighbourDepth, 0.1f, farplane);
            float depthDifference = abs(neighbourDepthLinearized - linearizedDepth);

            vec2 neighbourRM = texture(gSpecular, texCoord + offset).gb;
            float roughDifference = abs(neighbourRM.r - rm.r);
            float metDifference = abs(neighbourRM.g - rm.g);
            
            vec3 neighbourNormal = texture(gNormal, texCoord + offset).rgb;
            float normalSimilarity = dot(neighbourNormal, normal);
            if (depthDifference > 0.1 || normalSimilarity < 0.95 || roughDifference > 0.1 || metDifference > 0.1) continue;


            vec3 neighbourColour = texture(inputSampler, texCoord + offset).rgb;
            //float nLumanance = lumaFromRGB(neighbourColour);
            avgC += neighbourColour;

            h++;
            //avgL += nLumanance;
        }
    }
    return avgC / h;
    /*
    float avgLuminance = 0.0f; if (h > 0) avgLuminance = avgL / float(h);
    return colour * (avgLuminance / max(lumaFromRGB(colour), 0.0001f));
    */
}

void main(){
    vec4 indirect                = texture(swrtIndirect, texCoord);
    vec4 emission              = texture(swrtEmission, texCoord);
    vec4 indirectSpecular   = texture(swrtIndirectSpecular, texCoord);
    vec4 emissionSpecular = texture(swrtEmissionSpecular, texCoord);
    float gdepth                 = texture2D(depthMap, texCoord).r;
    
    hIndirect                = indirect;
    hEmission              = emission;
    hIndirectSpecular   = indirectSpecular;
    hEmissionSpecular = emissionSpecular;
    hdepth = gdepth;

    if (gdepth >= 0.99999) discard;

    vec3 specular     = texture(swrtSpecular, texCoord).rgb;
    vec3 direct = texture2D(swrtDbgColour, texCoord).rgb;
    vec3 gemission  = texture(gEmission, texCoord).rgb;
    vec3 gnrm         = normalize(texture(gNormal, texCoord).rgb);
    vec3 galbedo     = texture(gAlbedoSpec, texCoord).rgb;
    vec3 gSpecular   = texture(gSpecular, texCoord).rgb;
    
    vec3 nIndirect               = indirect.rgb;
    vec3 nEmission             = emission.rgb;
    vec3 nSpecular              = specular.rgb;
    vec3 nIndirectSpecular   = indirectSpecular.rgb;
    vec3 nEmissionSpecular = emissionSpecular.rgb;

    vec2 lscreenSize = textureSize(swrtDbgColour, 0);
    bool split = true;
    if (gl_FragCoord.x > lscreenSize.x / 2 && doDenoiseSplitDBGView) split = false;
    
    if (doDenoise && split){
        vec4 nFV = texture(filteredVariance, texCoord);
        float u = float(denoiseRadius);
        float v = 1.0f;
        int INDradius        = int(clamp(mix(u, v, nFV.r), 1, denoiseRadius));
        int EMradius         = int(clamp(mix(u, v, nFV.g), 1, denoiseRadius));
        //int INDspecradius = int(clamp(mix(u, v, nFV.b), 1, denoiseRadius));
        //int EMspecradius  = int(clamp(mix(u, v, nFV.a), 1, denoiseRadius));
        //int specRadius      = int(clamp(mix(0, 3, gSpecular.g), 0, denoiseRadius));
        int specularRadius = 5;
        
        float d       = linearizeDepth(gdepth, NearPlane, FarPlane);
        nIndirect   = indirectDenoise(swrtIndirect, indirect.rgb, d, gnrm, FarPlane, INDradius); 
        nEmission = indirectDenoise(swrtEmission, emission.rgb, d, gnrm, FarPlane, EMradius);

        if (gSpecular.g > 0.4){
            //nIndirectSpecular    = specularDenoise(swrtIndirectSpecular, indirectSpecular.rgb, d, gnrm, vec2(gSpecular.g, gSpecular.b), FarPlane, specularRadius);
            nEmissionSpecular  = specularDenoise(swrtEmissionSpecular, emissionSpecular.rgb, d, gnrm, vec2(gSpecular.g, gSpecular.b), FarPlane, specularRadius);
            nSpecular               = specularDenoise(swrtSpecular, specular.rgb, d, gnrm, vec2(gSpecular.g, gSpecular.b), FarPlane, specularRadius);
        }
    }
    
    //vec3 gi = clamp((direct.rgb + nIndirect.rgb), 0.0001, 1.0);
    vec3 gi = direct.rgb + nIndirect.rgb;
    // nSpecular - indirectSpecular < i currently am passing specualr thru here temporarily 
    vec3 combine = gi + nIndirectSpecular.rgb + (nEmission.rgb + gemission);

    vec3 colour = galbedo * combine; // 0.001f
    
    //FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    //FragColor = vec4(gi, 1.0f);
    //presentImage = direct.rgb;
    //presentImage = combine.rgb;
    presentImage = colour.rgb;
    //presentImage = vec3(indirect.a);
}
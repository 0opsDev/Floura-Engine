#version 460 core

#extension GL_ARB_gpu_shader_int64 : enable
#extension GL_ARB_bindless_texture : require

out vec4 FragColor;

in vec2 texCoord;
//in vec3 camPositon;
uniform vec3 camPos;

uniform highp sampler2D screentexture;

// geometry
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D depthMap;
uniform sampler2D gSpecular;
uniform sampler2D gVelocity;
uniform sampler2D gEmission;

// history
uniform sampler2D hColour;
uniform sampler2D hDepthTexture;
uniform sampler2D hNormal;

uniform mat4 cameraMatrix;
uniform mat4 inverseViewMatrix;
uniform mat4 inverseProjection;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

uniform bool DEFtoggle;
uniform bool doFog;
uniform vec2 screenSize;
uniform float time;
uniform int frame;

uniform float FarPlane;
uniform float NearPlane;

uniform int NumberOfSamples;

uniform mat4 lightProjection;


uniform uint64_t BlueNoiseHandle;
uniform uint64_t bayerMatrixHandle;

uniform sampler2D shadowMap;

uniform uint64_t cmMainHandle;

uniform int indirectSamples;

// temp fields
float specularLight = 0.50f;

uniform bool doReflect;

uniform bool doSSR;
uniform bool doContactShadows;

float linearizeDepth(float depth, float NP, float FP)
{
    return (2.0 * NP * FP) / (FP + NP - (depth * 2.0 - 1.0) * (FP - NP));
}

float random(vec3 seed) {

    vec4 seed4 = vec4(seed, 1.0);
    float dot_product = dot(seed4, vec4(12.9898, 78.233, 45.164, 94.673));
    return fract(sin(dot_product) * 43758.5453);
}


vec4 RayCast(in vec3 dir, inout vec3 hitCoord, out float dDepth, int maxSteps, out float hit, in float step)
{
    hitCoord += dir * 0.1;

    dir *= step;
    float depth;
    vec4 projectedCoord = vec4(0.0);

    for (int i = 1; i < maxSteps; i++)
    {
        hitCoord += dir;
        
        projectedCoord = projectionMatrix * vec4(hitCoord, 1.0);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;

        if (projectedCoord.x < 0.0 || projectedCoord.x > 1.0 || projectedCoord.y < 0.0 || projectedCoord.y > 1.0) {
            break;
        }

        vec3 sampledWorldPos = textureLod(gPosition, projectedCoord.xy, 2).xyz;

        vec3 sampledViewPos = (viewMatrix * vec4(sampledWorldPos, 1.0)).xyz;

        if (length(sampledWorldPos) < 0.001) {
            continue;
        }
        dDepth = hitCoord.z - sampledViewPos.z;


        if((dir.z - dDepth) < 0.3)
        {
            if(dDepth <= 0.0)
            {
                vec4 Result;
                Result = vec4(projectedCoord.xy, dDepth, 1.0);
                hit = 1.0;

                return Result;
            }
        }
    }
    hit = 0.0;
    return vec4(projectedCoord.xy, dDepth, 1.0);
}

#define Scale vec3(.8, .8, .8)
#define K 19.19

vec3 hash(vec3 a)
{
    a = fract(a * Scale);
    a += dot(a, a.yxz + K);
    return fract((a.xxy + a.yxx)*a.zyx);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) { return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0); }

const float reflectionSpecularFalloffExponent = 3.0;

vec4 ssr(vec3 ARM, vec3 position, vec3 normal, int maxSteps, float step)// out bool bClamp
{
    float Metallic = ARM.b;
    float rough = ARM.g;

    //bool fallback;

    if(Metallic < 0.01) return vec4(0.0);

    vec3 viewPos    = (viewMatrix * vec4(position, 1.0)).xyz;
    vec3 viewNormal = mat3(viewMatrix) * normal;

    vec3 V = normalize(viewPos);
    vec3 R = reflect(V, viewNormal);

    vec3 hitPos = viewPos;
    float dDepth;
    float hit = 0.0;

    //return vec4(V, 1.0);
    //return vec4(normalize(viewPos) * 0.5 + 0.5, 1.0);

    vec3 F0 = vec3(0.04);
    F0      = mix(F0, texture(gAlbedoSpec, texCoord.xy).rgb, Metallic);
    vec3 Fresnel = fresnelSchlick(max(dot(normalize(viewNormal), normalize(V)), 0.0), F0);


    vec3 wp = vec3(vec4(viewPos, 1.0) * viewMatrix);
    vec3 t = vec3(time,time +1,time +3);
    vec3 jitt = mix(vec3(0.0), vec3(hash(wp + t)), rough); // time

    //vec4 csCoords = RayCast(R, hitPos, dDepth, 32);
    //vec4 csCoords = RayCast(R, hitPos, dDepth, 32, hit, 0.10);
    vec4 csCoords = RayCast(vec3(jitt) + R * max(0.1, -viewPos.z), hitPos, dDepth, maxSteps, hit, step);

    vec2 dCoords = smoothstep(0.2, 0.6, abs(vec2(0.5, 0.5) - csCoords.xy));

    float screenEdgefactor = clamp(1.0 - (dCoords.x + dCoords.y), 0.0, 1.0);

    float ReflectionMultiplier = pow(Metallic, reflectionSpecularFalloffExponent) *
    screenEdgefactor *
    -R.z;

    int lastLOD = textureQueryLevels(hColour) - 1;

    float maxLod = lastLOD;

    float lod =  rough * maxLod;

    lod = min(lod, 10.0);
    //lod=0;

    vec3 SSR = textureLod(screentexture, csCoords.xy, lod).rgb * clamp(ReflectionMultiplier, 0.0, 0.9) * Fresnel; //hColour
    //vec3 SSR = textureLod(gAlbedoSpec, csCoords.xy, lod).rgb * clamp(ReflectionMultiplier, 0.0, 0.9) * Fresnel; //hColour
    //vec3 SSR = textureLod(hColour, csCoords.xy, lod).rgb * clamp(ReflectionMultiplier, 0.0, 0.9);
    return vec4(SSR, hit); //Metallic
}

vec3 blur(sampler2D samp, vec2 coord)
{

    int samples = 10;
    vec3 colour = vec3(0.0);

    for (int i = 5; i < samples; i++)
    {
        vec3 mipmap = textureLod(samp, coord, i).rgb;

        colour += mipmap;
    }

    return colour / samples;
}


void main()
{
    vec2 velocity = texture(gVelocity, texCoord).rg;
    //vec2 scaledVelocity = velocity * 1.0;
    //FragColor = vec4(scaledVelocity, 0.0f, 1.0f);

    //return;

    vec4 albedo = texture(gAlbedoSpec, texCoord);
    
    vec3 colour = texture(screentexture, texCoord).rgb;

    if (albedo.a <= 0.0)
    {
        FragColor = vec4(albedo.rgb, 1.0f); return;
    }

    float depth = texture(depthMap, texCoord).r;

    //early z cutoff
    if (depth >= 0.99999)
    {
        FragColor = vec4(albedo.rgb, 1.0f);
        return;
    }

    vec3 normal = texture(gNormal, texCoord).rgb;
    //                      AO  Rough  MAT
    //vec3 ARM = vec3(0.0, 1.0, 1.0); // placeholder
    vec3 ARM = texture(gSpecular, texCoord).rgb;// placeholder
    //gSpecular  

    float displacement = texture(gNormal, texCoord).a;

    vec3 position = texture(gPosition, texCoord).rgb;

    //FragColor = vec4(position, 1.0); return;

    vec3 viewVector = position - camPos;

    vec3 emission = texture(gEmission, texCoord).rgb;
    vec3 nEmissionblur = blur(gEmission, texCoord) ;
    
    vec3 reflections = vec3(0.0);

    if (doSSR) reflections = ssr(ARM, position, normal, 64, 0.10).rgb;
    
    vec3 final = colour + reflections + emission;

    FragColor = vec4(vec3(final + nEmissionblur), 1.0f);
}
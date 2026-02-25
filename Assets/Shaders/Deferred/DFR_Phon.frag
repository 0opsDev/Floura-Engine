#version 460 core

#extension GL_ARB_gpu_shader_int64 : enable
#extension GL_ARB_bindless_texture : require

out vec4 FragColor;

in vec2 texCoord;
//in vec3 camPositon;
uniform vec3 camPos;

// geometry
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D depthMap;
uniform sampler2D gSpecular;
uniform sampler2D gVelocity;

// history
uniform sampler2D hColour;
uniform sampler2D hDepthTexture;
uniform sampler2D hNormal;

uniform mat4 cameraMatrix;

uniform vec3 lightColor;
uniform vec3 fogColor;
uniform vec3 skyColor;

uniform bool DEFtoggle;
uniform bool doFog;
uniform vec2 screenSize;
uniform float time;
uniform int frame;

uniform float fogDepthDistance;
uniform float fogFarPlane;
uniform float fogNearPlane;

uniform float FarPlane;
uniform float NearPlane;

uniform vec3 directLightPos;
uniform vec3 directLightCol;
uniform float directAmbient; // 0.20f
uniform float dirSpecularLight;
uniform bool doDirLight;
uniform bool doDirSpecularLight;
uniform bool doDirShadowMap;
uniform int FilterRadius;
uniform int NumberOfSamples;
uniform float DirSMMaxBias;

uniform mat4 lightProjection;


uniform uint64_t BlueNoiseHandle;
uniform uint64_t bayerMatrixHandle;

uniform sampler2D shadowMap;

uniform uint64_t cmMainHandle;

uniform int indirectSamples;

// temp fields
float specularLight = 0.50f;

uniform bool doReflect;

struct Light
{
    vec3 position;
    vec3 rotation;
    vec3 colour;
    float radius;
    int type;
};

uniform Light Lights[64];

uniform int lightCount;

float linearizeDepth(float depth, float NP, float FP)
{
    return (2.0 * NP * FP) / (FP + NP - (depth * 2.0 - 1.0) * (FP - NP));
}

float random(vec3 seed) {

    vec4 seed4 = vec4(seed, 1.0);
    float dot_product = dot(seed4, vec4(12.9898, 78.233, 45.164, 94.673));
    return fract(sin(dot_product) * 43758.5453);
}

float CalcShadowFactorDIR(vec4 LightSpacePos, vec3 lightDirection, vec3 normal, vec3 iPosition)
{
    // perform perspective divide
    vec3 lightCoords = LightSpacePos.xyz / LightSpacePos.w;
    float shadow = 0.0f;

    // shadow calculation
    if (lightCoords.z <= 1.0f)
    {
        sampler2D bluemap =sampler2D(BlueNoiseHandle) ;

        // transform to [0,1] range
        lightCoords = (lightCoords + 1.0f) / 2.0f;
        // get the current depth
        float currentDepth = lightCoords.z;
        // calculate shadow bias
        float bias = max(DirSMMaxBias * (1.0f - dot(normal, lightDirection)), 0.0005f);
        // PCF
        int sampleRadius = FilterRadius; // FilterRadius // NumberOfSamples
        //vec2 pixelSize = (float(NumberOfSamples) * 0.1) / textureSize(shadowMap, 0);
        
        vec2 texSize = vec2(textureSize(bluemap, 0));
        // uv
        vec2 offset = vec2(fract(frame * 0.618), fract(frame * 0.133));
        vec2 noiseUV = (gl_FragCoord.xy / texSize) + offset;

        vec2 pixelSize = 1.0 / textureSize(shadowMap, 0);
        for(int y = -sampleRadius; y <= sampleRadius; y++)
        {
            for(int x = -sampleRadius; x <= sampleRadius; x++)
            {
                float angle = texture(bluemap, noiseUV).r * NumberOfSamples;
                vec2 foffset = vec2(cos(angle), sin(angle));

                float closestDepth = texture(shadowMap, lightCoords.xy + (vec2(x, y) * foffset) * pixelSize).r;
                if (currentDepth > closestDepth + bias)
                shadow += 1.0f;

            }
        }

        shadow /= pow((sampleRadius * 2 + 1), 2);


    }

    return shadow;
}


vec4 direcLight(vec3 ARM, vec3 iNormal, vec3 iPosition)
{ // normals need to be recalculated based on rotation



  // shadow map 
  float shadow = 0.0f;


  vec3 normal = normalize(iNormal);

  vec3 lightDirection = normalize(directLightPos); //vec3(1.0f, 1.0f, 0.0f)
  float diffuse = max(dot(normal, lightDirection), 0.0f);
  
    vec4 fragPosLight = lightProjection * vec4(iPosition, 1.0f);
    
  if (doDirShadowMap)
  shadow = CalcShadowFactorDIR(fragPosLight, lightDirection, normal, iPosition);
  // shadow map end
    
  float specular = 0.0f;
  if (doReflect && doDirSpecularLight && diffuse != 0.0f){

      //return (vec4( vec3(1.0 , 0.0, 0.0), 1.0));
      
      vec3 reflectionDirection = reflect(-lightDirection, normal);
      vec3 viewDirection = normalize(camPos - iPosition);
      vec3 halfwayVec = normalize(viewDirection + lightDirection);

      float specAmount = pow(max(dot(normal, halfwayVec), 0.0f), 16);
      //float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
      specular = specAmount * dirSpecularLight;

      return ((diffuse * (1.0f - shadow) + directAmbient) + ARM.g* specular * (1.0f - shadow)) * vec4(directLightCol, 1.0f); }
  else{ return ((diffuse * (1.0f - shadow) + directAmbient)) * vec4(directLightCol, 1.0f); }
}

vec4 pointLight(int iteration, vec3 ARM, vec3 iNormal, vec3 iPosition)
{
    vec4 finalColour = vec4(0.0f);

    //vec3 lightVec = (Lights[iteration].position) - crntPos;
    vec3 lightVec = (Lights[iteration].position) - iPosition;

    // intensity of light with respect to distance
    float dist = length(lightVec);
    float a = 3.00f;
    float b = 0.70f;
    float inten = 1.0f / (a * dist * dist + b * dist + 1.0) * Lights[iteration].radius;

    // ambient lighting
    //float ambient = 0.0f;
    vec3 normal = normalize(iNormal);
    //vec3 normal = normalize(Normal); 

    vec3 lightDirection = normalize(lightVec);
    float diffuse = max(dot(normal, lightDirection), 0.0f);

    float specular = 0.0f;
    if (doReflect && diffuse != 0.0f){
        // specular lighting
        //float specularLight = 0.50f;
        vec3 viewDirection = normalize( (camPos) - iPosition);
        vec3 reflectionDirection = reflect(-lightDirection, normal);

        vec3 halfwayVec = normalize(lightDirection + viewDirection);

        float specAmount = pow(max(dot(normal, halfwayVec), 0.1f), 16);
        specular = specAmount * specularLight;

        finalColour = finalColour + ((diffuse * inten + 0.0f) +ARM.b * specular * inten) * vec4(Lights[iteration].colour, 1.0 ) * inten;
    }
    else{
        finalColour = finalColour + ( (diffuse * inten + 0.0f) * vec4(Lights[iteration].colour, 1.0) * inten);
        //finalColour = finalColour + (texture(diffuse0, texCoord) * (diffuse * inten + ambient) * vec4(Lights[iteration].colour, 1.0) * inten);
    }

    return finalColour;
}

vec4 spotLight(int iteration, vec3 ARM, vec3 iNormal, vec3 iPosition)
{
    // controls how big the area that is lit up is
    float outerCone = 0.90f;
    float innerCone = 0.95f;

    // ambient lighting
    //float ambient = 0.0f;

    vec4 finalColour = vec4(0.0f);

    // diffuse lighting
    vec3 normal = normalize(iNormal);

    vec3 lightDirection = normalize(Lights[iteration].position - iPosition);
    float diffuse = max(dot(normal, lightDirection), 0.0f);

    // calculates the intensity of the crntPos based on its angle to the center of the light cone
    float angle = dot(Lights[iteration].rotation, -lightDirection); // direction
    float inten = clamp( (angle - outerCone) / (innerCone - outerCone), 0.0f, 1.0);

    float specular = 0.0f;
    if (doReflect && diffuse != 0.0f){

        // specular lighting
        //float specularLight = 0.50f;
        vec3 viewDirection = normalize(camPos - iPosition);
        vec3 reflectionDirection = reflect(-lightDirection, normal);

        vec3 halfwayVec = normalize(lightDirection + viewDirection);

        float specAmount = pow(max(dot(normal, halfwayVec), 0.0f), 16);
        specular = specAmount * specularLight;

        finalColour = finalColour + ((diffuse * inten + 0.0f) + ARM.b * specular * inten) * vec4(Lights[iteration].colour, 1.0) * inten;

    }
    else{

        finalColour = finalColour + ((diffuse * inten + 0.0f) * vec4(Lights[iteration].colour, 1.0) * inten);
        //	finalColour = finalColour + (texture(diffuse0, texCoord) * (diffuse * inten + ambient) * vec4(Lights[iteration].colour, 1.0) * inten);
    }

    //finalColour = finalColour + (diffuse * inten + skyColor);

    return finalColour;
}

vec4 lights(vec3 ARM, vec3 iNormal, vec3 iPosition){
    //vec4 diffuseTex = texture(texture_diffuse0, texCoord);
    vec4 finalColour = vec4(0.0);
    //return (diffuseTex * skyColor);
    int maxLights = 64;
    for (int i = 0; i < min(lightCount, maxLights); i++)
    {
        if (Lights[i].type == 0){
            finalColour += spotLight(i, ARM, iNormal, iPosition);
        }

        if (Lights[i].type == 1){
            finalColour += pointLight(i, ARM, iNormal, iPosition);
        }

    }
    //return finalColour;
    //FragColor = direcLight(); doDirLight

    if (doDirLight) // if direct light is enabled, add it to the final color
    {
        finalColour += direcLight(ARM, iNormal, iPosition);
    }

    ///return vec4(finalColour.xyz, diffuseTex.a);
    return vec4(finalColour); // was xyz
    //return vec4((diffuseTex.xyz * skyColor.xyz) + finalColour.xyz, diffuseTex.a);
    //return (diffuseTex.xyz * skyColor.xyz) + finalColour.xyz;
}

// looks best on glass and solids
bool BayerNoiseOpacity(float Threshold) // for fade out or opacity (cheap) (could fade out near farplane or nearplane)
{
    sampler2D baySamp = sampler2D(bayerMatrixHandle);
    vec2 bayUV = vec2(gl_FragCoord.xy) / vec2(textureSize(baySamp, 0)); // new uvec2
    float bayer = texture(baySamp, bayUV).r;

    float clampedThreshold = clamp(Threshold, 0.2, 1.0);

    // normal ranges should be 0.0f-1.0f;
    if (bayer > Threshold || Threshold <= 0) return true;

    return false;
}

// looks best on decals and foliage
bool blueNoiseOpacity(float Threshold) // for fade out or opacity (cheap) (could fade out near farplane or nearplane)
{
    sampler2D bluemap =sampler2D(BlueNoiseHandle) ;
    vec2 noiseUV = vec2(gl_FragCoord.xy) / vec2(textureSize(bluemap, 0)); // new uvec2
    float noise = texture(bluemap, noiseUV).r;

    // normal ranges should be 0.0f-1.0f;
    if (noise > Threshold) return true;

    return false;
}

vec3 rough(vec3 ARM, vec3 iNormal, vec3 viewVector)
{

    //float rough = ARM.g;
    //return vec3(rough);
    if (doReflect)
    {
        float rough = ARM.g;
        float met = ARM.b; // just gonna use this as a multiplier thingy instead of a materiallic model
        //rough =0.0;
        
        // vectors
        vec3 normal = normalize(iNormal);
        //vec3 normal = normalize(Normal);
        vec3 v = normalize(viewVector);
        vec3 r = reflect(v, normal);
        float nDotV = max(dot(normal, -v), 0.0001);

        samplerCube cmSamp = samplerCube(cmMainHandle);

        // LOD based Roughness (irradiance probes);
        float maxLod = float(textureQueryLevels(cmSamp) - 1);
        float lod = rough * maxLod;
        lod = min(lod, 10.0);

        vec3 reflectionColour = textureLod(cmSamp, r, lod).rgb;
        return reflectionColour * met;
        //return reflectionColour;
    }
    return vec3(0.0f);
}

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898, 78.233))) * 43758.5453);
}

// (thanks learnopengl)
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float a = roughness;
    float k = (a * a) / 2.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / max(denom, 0.0001);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 metRough(vec3 albedo, vec3 iNormal, vec3 viewVector, out vec3 nFer, out float nMet, out vec3 irradiance, vec3 ARM)
{
    // textures
    vec3 metallicRoughness = ARM; // metalic
    float rough = metallicRoughness.g;
    float met =  metallicRoughness.b;
    nMet = met;

    // vectors
    vec3 normal = iNormal;
    //vec3 normal = normalize(Normal);
    vec3 v = normalize(viewVector);
    vec3 r = reflect(v, normal);
    float nDotV = max(dot(normal, -v), 0.0001);

    samplerCube cmSamp = samplerCube(cmMainHandle);

    // LOD based Roughness (irradiance probes);
    float maxLod = float(textureQueryLevels(cmSamp) - 1);
    float lod = rough * maxLod;
    lod = min(lod, 10.0);

    vec3 reflectionColour = textureLod(cmSamp, r, lod).rgb;
    irradiance = textureLod(cmSamp, normal, maxLod).rgb;

    // fresnel
    float reflectivity = 0.04f;
    vec3 nF = mix(vec3(reflectivity), albedo, met);
    nFer = nF;
    vec3 F = nF + (max(vec3(1.0 - rough), nF) - nF) * pow(clamp(1.0 - nDotV, 0.0, 1.0), 5.0);

    // visibility factor 
    float G = GeometrySmith(normal, -v, r, rough);

    //return reflectionColour * F;
    return reflectionColour * F * G;
}

void Reflect(vec3 albedo,  vec3 iNormal, vec3 viewVector, out vec3 diffuse, out vec3 specular, vec3 ARM)
{


    /*
    
        float fadeDistance = 10.0;
        float distToFar = maxDist - depth;
        float farOpacity = distToFar / fadeDistance;
        farOpacity = clamp(farOpacity, 0.0, 1.0);
    
        if (blueNoiseOpacity(farOpacity)) return;
    */

    //if (doReflect && depth < maxDist)
    if (doReflect)
    {
        float met = 0;
        vec3 nF = vec3(0.0f);
        vec3 irradiance = vec3(0.0f);
        specular = metRough(albedo,iNormal,viewVector, nF, met, irradiance, ARM);

        vec3 nDiffuse = vec3(0.0f);
        nDiffuse = albedo * (1.0f - nF);
        nDiffuse *= (1.0f - met);

        diffuse = irradiance * nDiffuse;
    }
}

vec3 sampleHemisphere(vec3 normal, float random)
{
    float u = rand(vec2(gl_FragCoord.xy) + random);
    float v = rand(vec2(u, random));

    float phi = 2.0 * 3.14159265 * u;
    float cosTheta = sqrt(1.0 - v);
    float sinTheta = sqrt(v);

    vec3 localDir = vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);

    vec3 helper = abs(normal.y) < 0.999 ? vec3(0, 1, 0) : vec3(1, 0, 0);
    vec3 tangent = normalize(cross(helper, normal));
    vec3 bitangent = cross(normal, tangent);

    return tangent * localDir.x + bitangent * localDir.y + normal * localDir.z;
}

vec3 indirectIBL(int samples, vec3 ARM, vec3 iNormal, vec3 viewVector)
{

    float rough = ARM.g;
    float met =  ARM.b;

    vec3 NreflectedVector = reflect(viewVector, iNormal);

    samplerCube cmSamp = samplerCube(cmMainHandle);

    int lastLOD = textureQueryLevels(cmSamp) - 1;

    float maxLod = lastLOD;

    float lod =  rough * maxLod;

    lod = min(lod, 10.0);

    //float specularIntensity = mix(0.04, 1.0, met);

    vec3 indirectColour = vec3(0.0f);

    if (samples <= 0) return indirectColour;

    for (int i = 0; i < samples; i++)
    {
        //gl_FragCoord
        vec3 randomDir = sampleHemisphere(normalize(NreflectedVector), rand(vec2(i + (gl_FragCoord.z * time) ) * vec2(1, 2)) );
        //vec3 randomDir = sampleHemisphere(normalize(NreflectedVector), i + (gl_FragCoord.z * time) );
        //vec3 randomDir = sampleHemisphere(normalize(NreflectedVector), i + time); // i thought it would be better to add time for a film grain look, it would also solve with taa

        //int skyLOD = textureQueryLevels(skybox) - 4; // use mipmap for more preformance
        vec3 skyboxColour = textureLod(cmSamp, randomDir, lod).rgb;
        //vec3 skyboxColour = texture(skybox, randomDir).rgb; 

        indirectColour += skyboxColour;
    }

    return (indirectColour / samples);
    //return (indirectColour / samples) * met;
}

void main()
{
    vec2 velocity = texture(gVelocity, texCoord).rg;
    //vec2 scaledVelocity = velocity * 1.0;
    //FragColor = vec4(scaledVelocity, 0.0f, 1.0f);

    //return;

    vec4 albedo = texture(gAlbedoSpec, texCoord);

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

    vec3 viewVector = position - camPos;

    vec3 roughness = rough(ARM, normal, viewVector);

    vec3 specular = vec3(0.0f);
    vec3 diffuse  = vec3(0.0f);

    Reflect(albedo.rgb, normal, viewVector, diffuse, specular, ARM);

    vec3 direct = lights(ARM, normal, position).rgb;
    vec3 indirect = indirectIBL(indirectSamples, ARM, normal, viewVector);// [placeholder

    vec3 gi = (direct + indirect);

    vec3 reflections = diffuse + specular;

    vec3 F0 = mix(vec3(0.04), albedo.rgb, ARM.b);
    vec3 kD = (vec3(1.0) - F0) * (1.0 - ARM.b);

    vec3 totalDiffuse = (gi * albedo.rgb * kD);

    vec3 finalRGB = totalDiffuse + specular;

    vec4 final = albedo * vec4(gi, 1.0f) + vec4(finalRGB, 1.0f);
    //vec3 final = albedo.rgb *  gi + roughness;
    
    FragColor = final;

    //FragColor = vec4(vec3(hDepthLinearized) , 1.0);
    
    //FragColor = vec4(texture(hColour, texCoord));
    
//    FragColor = vec4(final , 1.0f);

    //vec2 velocity = texture(gVelocity, texCoord).rg;
    
    //FragColor = vec4(vec3(toon) , 1.0f);
    
    //FragColor = vec4(rough(ARM, normal, viewVector), 1.0f);
    //FragColor = vec4(vec3(ARM.g), 1.0f);
    
    //FragColor = vec4(vec3(displacement), 1.0);
    //FragColor = vec4(normal, 1.0);
    //FragColor = vec4(normalize(normal), 1.0);
    //FragColor = vec4(albedo.rgb, 1.0);
}
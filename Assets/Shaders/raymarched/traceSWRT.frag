#version 460 core

#extension GL_ARB_gpu_shader_int64 : enable
#extension GL_ARB_bindless_texture : require

#define INF 3.40282347e+38f

//out vec4 FragColor;

in highp vec2 texCoord;

layout(location = 0) out vec4 oindirect;
layout(location = 1) out vec3 ospecular;
layout(location = 2) out vec4 oemission;
layout(location = 3) out vec4 oindirectSpecular;
layout(location = 4) out vec4 oemissionSpecular;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D depthMap;
uniform sampler2D gSpecular;
uniform sampler2D gEmission;
uniform highp sampler2D gVelocity;
uniform sampler2D swrtHDepth;
uniform float NearPlane;
uniform float FarPlane;

uniform mat4 viewMatrix;
uniform mat4 invViewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 invProjectionMatrix;

uniform mat4 invHViewMatrix;
uniform mat4 invHProjectionMatrix;

uniform vec3 cameraPosition;

uniform float time;
uniform int frame;

uniform vec3 directLightPos;
uniform vec3 directLightCol;
uniform bool doDirLight;
uniform float directAmbient;

uniform mat4 lightProjection;
uniform float dirSpecularLight;
uniform bool doDirSpecularLight;
uniform bool doReflect;
uniform sampler2D BlueNoiseHandle;
uniform samplerCube cmMainHandle;
uniform sampler2DShadow  shadowMap;
uniform int FilterRadius;
uniform int NumberOfSamples;
uniform float DirSMMaxBias;
uniform bool doDirShadowMap;

uniform vec3 sceneBoundPos;
uniform vec3 sceneBoundScale;

uniform bool doTemporalAccumulation;
uniform float temporalAccumulationBlendFactor;
uniform bool doDenoiseSplitDBGView;

uniform sampler2D hIndirect;
uniform sampler2D hEmission;
uniform sampler2D hIndirectSpecular;
uniform sampler2D hEmissionSpecular;
uniform sampler2D hSpecular;
uniform sampler2D presentImage;

uniform vec2 screenSize;
// temp

struct Light{
    vec3 position;
    vec3 rotation;
    vec3 colour;
    float radius;
    int type;
};
// i seriously need a light ssbo....
uniform Light Lights[64];
uniform int lightCount;
// temp fields
float specularLight = 0.50f;

struct MDF{
    vec4 position;  // uv.x
    vec4 extents;  // uv.y
    vec4 rootPosition;
    vec4 rootExtents;
    vec4 gPosition;
    vec4 gExtents;
    //vec4 gRotation;
    mat4 globalTransform;
    
    uint64_t instanceUUID;
    sampler3D SDF_Handle;
    
    sampler2D texture_diffuse_Handle;
    sampler2D texture_roughness_Handle;
    sampler2D texture_normal_Handle;
    sampler2D texture_emission_Handle;
    
};
// mesh distance fields
layout(std430, binding = 11) buffer MDF_Buffer {
    MDF MDFS[];
};

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898, 78.233))) * 43758.5453);
}

float r_maxdist = 64.0f;
float r_shadow_ambient = 0.0f; //0.07f;
float r_roughnessFloor = 0.0f;
int r_steps = 80;
int r_bounces = 1;
int r_samples  = 0;
int r_minLodLevel = 1;
float r_noiseThreshold = 1.0;
bool forceMirror = false;

float i_maxdist = 64.0f;
int i_steps = 80;
int i_samples = 1;  
int i_minLodLevel = 8;
int e_minLodLevel = 8;
float i_noiseThreshold  = 1.0;
float i_indirectBoost = 2.0f; // default at 1.0
float e_emissionBoost = 1.0f; // default at 1.0

bool drawSDFSCENE = false;

int maxlodLevel = 2; // off
float transitionRangeLOD = 30.0f;
float maxSDFDist = 64.0f;

const float eps = 0.01f;
float mindist = 0.01f;
float originEplison = 0.5f;

struct hitresult{
    vec3 normal;
    vec3 hitpos;
    vec2 uv;
    //float totalDistanceTravelled;
    float distance;
    float maxDist;
    float iterationsDBG;
    //float lowestDistance;
    int materialIndex;
    bool isHit;
};



uvec3 murmurHash31(uint src) {
    const uint M = 0x5bd1e995u;
    uvec3 h = uvec3(1190494759u, 2147483647u, 3559788179u);
    src *= M; src ^= src>>24u; src *= M;
    h *= M; h ^= src;
    h ^= h>>13u; h *= M; h ^= h>>15u;
    return h;
}

// 3 outputs, 1 input
vec3 hash31(float src) {
    uvec3 h = murmurHash31(floatBitsToUint(src));
    return uintBitsToFloat(h & 0x007fffffu | 0x3f800000u) - 1.0;
}

float lumaFromRGB(vec3 rgb){
    vec3 weights = vec3(0.2126, 0.7152, 0.0722);
    float luminance = dot(rgb, weights);
    return luminance;
}

float sdBox(vec3 p, vec3 b){
    vec3 q = abs(p) - b;
    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0f);
}

float fOpUnionID(float res1, float res2){
    return (res1 < res2) ? res1 : res2;
}
vec4 fOpUnionID4(vec4 res1, vec4 res2){
    return (res1.x < res2.x) ? res1 : res2;
}

vec3 transformP(vec3 p, mat4 matrix){
    vec4 np = matrix * vec4(p, 1.0);
    return np.xyz;
}

float texture3DSDF(vec3 p, vec3 s, sampler3D handle, int lodLevel){
    vec3 distance = abs(p) - s;
    
   float outsideDistance = length(max(distance, 0.0));
    
    // map coords to 3d uv space
    vec3 uvw = (p / (s* 2.0)) + 0.5;
    
    float SDF = textureLod(handle, uvw, lodLevel).r;
    
    if (outsideDistance > 0.0) return SDF + outsideDistance;
    
    return SDF;
}

vec3 texture3DSDFUV(vec3 p, vec3 s, sampler3D handle, int lodLevel){
    vec3 distance = abs(p) - s;

    float outsideDistance = length(max(distance, 0.0));

    // map coords to 3d uv space
    vec3 uvw = (p / (s* 2.0)) + 0.5;

    vec3 SDF = textureLod(handle, uvw, lodLevel).rgb;

    if (outsideDistance > 0.0) return vec3(SDF.r + outsideDistance, SDF.yz);

    return SDF;
}

vec3 nearestPointOnAABB(vec3 p, vec3 pos, const vec3 extents){
    return clamp(p, pos - extents, pos + extents);
}

int calculateLODLevel(vec3 vPosition, vec3 cameraPosition, float transitionDistance, int maxLOD){
    float distance = distance(vPosition, cameraPosition);
    int targetLOD = int(distance / transitionDistance);
    targetLOD = min(targetLOD, maxLOD);

    return targetLOD;
}

vec3 SDFMesh(vec3 p, MDF cMDF, int lodLevel){
    vec3 gp = p - cMDF.gPosition.xyz;
    vec3 fp = transformP(gp, cMDF.globalTransform);
    
    vec3 nsdfv = texture3DSDFUV(fp,cMDF.gExtents.rgb, cMDF.SDF_Handle, lodLevel);
    
    vec2 uv = nsdfv.gb * vec2(cMDF.position.w, cMDF.extents.w);
    
    return vec3(nsdfv.r, uv);
}

float SDFMeshDist(vec3 p, MDF cMDF, int lodLevel){
    vec3 gp = p - cMDF.gPosition.xyz;
    vec3 fp = transformP(gp, cMDF.globalTransform);
    
    float nsdfv = texture3DSDFUV(fp, cMDF.gExtents.rgb, cMDF.SDF_Handle, lodLevel).r;

    return nsdfv;
}

// moved up here since its in the way
hitresult aabbVsRay(vec3 ro, vec3 rd, vec3 p, vec3 s){
    hitresult hr; hr.isHit = false; hr.distance = INF;
    vec3 Min = p - s;
    vec3 Max = p + s;

    vec3 invRD = 1.0f / rd;

    vec3 t0 = (Min - ro) * invRD;
    vec3 t1 = (Max - ro) * invRD;

    vec3 tMin = min(t0, t1);
    vec3 tMax = max(t0, t1);

    float tNear = max(max(tMin.x, tMin.y), tMin.z);
    float tFar = min(min(tMax.x, tMax.y), tMax.z);

    if (tNear > tFar || tFar < 0) { return hr; }

    hr.isHit = true;
    hr.distance = tNear;
    hr.maxDist = tFar;
    return hr;
}

vec2 sceneSDFUVIndex(vec3 p, int i){ // scene
    vec3 tsdf = vec3(INF, 0.0, 0.0);
    MDF cMDF = MDFS[i];
    
        vec3 np = nearestPointOnAABB(cameraPosition, cMDF.rootPosition.xyz, cMDF.rootExtents.xyz);

        if (distance(np, cameraPosition) > maxSDFDist) return vec2(0.0, 0.0);

        float root = sdBox(p - cMDF.rootPosition.xyz, cMDF.rootExtents.xyz);
        if (root < tsdf.x){

            int lodLevel = calculateLODLevel(np, cameraPosition, transitionRangeLOD, maxlodLevel);
            tsdf.rgb = SDFMesh(p, cMDF, lodLevel);
        }
    return vec2(tsdf.gb);
}

float sceneSDFdistIndex(vec3 p, int i){ // scene
    float tsdf = INF;
    
    MDF cMDF = MDFS[i];
    
        vec3 np = nearestPointOnAABB(cameraPosition, cMDF.rootPosition.xyz, cMDF.rootExtents.xyz);

        if (distance(np, cameraPosition) > maxSDFDist) return INF;

        float root = sdBox(p - cMDF.rootPosition.xyz, cMDF.rootExtents.xyz);
        if (root < tsdf){

            int lodLevel = calculateLODLevel(np, cameraPosition, transitionRangeLOD, maxlodLevel);
            tsdf = SDFMeshDist(p, cMDF, lodLevel);
        }
    return tsdf;
}


vec4 sceneSDF(vec3 p, int mdfLength ){ // scene
    vec4 tsdf = vec4(INF, 0.0, 0.0, 0.0);

    //aabbHitDBGmode
    
    //if (sdBox(p - sceneBoundPos.xyz, sceneBoundScale.xyz) > tsdf.x) vec4(INF, 0.0, 0.0, 0.0);
    
    for (int i = 0; i < mdfLength; i++ ){
        MDF cMDF = MDFS[i];
        
        vec3 np = nearestPointOnAABB(cameraPosition, cMDF.rootPosition.xyz, cMDF.rootExtents.xyz);
        
        if (distance(np, cameraPosition) > maxSDFDist) continue;
        
        float root = sdBox(p - cMDF.rootPosition.xyz, cMDF.rootExtents.xyz);
        if (root < tsdf.x){
            
            int lodLevel = calculateLODLevel(np, cameraPosition, transitionRangeLOD, maxlodLevel);
            vec4 sdfm = vec4(SDFMesh(p, cMDF, lodLevel), float(i));
            //vec4 sdfm = vec4(root, 0.0, 0.0, 0.0);
            
            if (i == 0) {
                tsdf = sdfm;
                continue;
            }
            tsdf = fOpUnionID4(tsdf,sdfm); //fOpUnionRoundID4 fOpUnionID4
        }
    }
    return vec4(tsdf);
}

float sceneSDFdist(vec3 p , int mdfLength){ // scene
    float tsdf = INF;

    //if (sdBox(p - sceneBoundPos.xyz, sceneBoundScale.xyz) > tsdf.x) INF;
    
    for (int i = 0; i < mdfLength; i++ ){
        MDF cMDF = MDFS[i];
        
        vec3 np = nearestPointOnAABB(cameraPosition, cMDF.rootPosition.xyz, cMDF.rootExtents.xyz);

        if (distance(np, cameraPosition) > maxSDFDist) continue;

        float root = sdBox(p - cMDF.rootPosition.xyz, cMDF.rootExtents.xyz);
        if (root < tsdf){

            int lodLevel = calculateLODLevel(np, cameraPosition, transitionRangeLOD, maxlodLevel);
            float sdfm = SDFMeshDist(p, cMDF, lodLevel);

            if (i == 0) {
                tsdf = sdfm;
                continue;
            }
            tsdf = fOpUnionID(tsdf,sdfm); //fOpUnionRoundID4 fOpUnionID4

        }
    }
    return tsdf;
}

vec3 CalculateNormal( in vec3 p, int mdfLength ){
    const vec2 h = vec2(eps,0);
    return normalize( vec3(sceneSDFdist(p+h.xyy, mdfLength) - sceneSDFdist(p-h.xyy, mdfLength),
            sceneSDFdist(p+h.yxy, mdfLength) - sceneSDFdist(p-h.yxy, mdfLength),
            sceneSDFdist(p+h.yyx, mdfLength) - sceneSDFdist(p-h.yyx, mdfLength) ) );
}

vec3 CalculateNormalInd( in vec3 p, int index){
    const vec2 h = vec2(eps,0);
    return normalize( vec3(sceneSDFdistIndex(p+h.xyy, index) - sceneSDFdistIndex(p-h.xyy, index),
            sceneSDFdistIndex(p+h.yxy, index) - sceneSDFdistIndex(p-h.yxy, index),
            sceneSDFdistIndex(p+h.yyx, index) - sceneSDFdistIndex(p-h.yyx, index) ) );
}

vec3 CalculateNormalIndMinus( in vec3 p, int index){
    const vec2 h = vec2(eps,0);
    return normalize( vec3(-sceneSDFdistIndex(p+h.xyy, index) - -sceneSDFdistIndex(p-h.xyy, index),
            -sceneSDFdistIndex(p+h.yxy, index) - -sceneSDFdistIndex(p-h.yxy, index),
            -sceneSDFdistIndex(p+h.yyx, index) - -sceneSDFdistIndex(p-h.yyx, index) ) );
}


hitresult RayAcceleratedSphereMarchScene(vec3 ro, vec3 rd, float maxdist, float mindist, int steps, int index){
    hitresult hr;
    hr.isHit = false;
    hr.distance = INF;
    //hr.lowestDistance = INF;
    float t = 0.0; // total distance travelled
    // raymarching
    for (int i = 0; i < steps; i++){
        vec3 pos = ro + rd* t;// position along the ray
        
        float dist = sceneSDFdistIndex(pos, index);
        
        //if (hr.lowestDistance > dist) hr.lowestDistance = dist;

        if (dist < mindist){ // treat as if hit
            hr.uv = sceneSDFUVIndex(pos, index);
            hr.isHit = true;
            hr.distance = dist;
            //hr.totalDistanceTravelled = t;
            hr.normal = CalculateNormalInd(pos, index);
            hr.materialIndex = index;
            hr.hitpos = pos;
            hr.iterationsDBG = float(i) / float(steps);
            break;// how small dist (radius around march)
        }
        t += dist;
        
        if (t > maxdist)break; 
        // failed to hit
        //discard; // discard on far for transparency
        // how large dist (radius around march)
    }

    return hr;
}

hitresult invertedSphereMarchSceneIndex(vec3 ro, vec3 rd, float maxdist, float mindist, int steps, int index){
    hitresult hr;
    hr.isHit = false;
    hr.distance = INF;
    //hr.lowestDistance = INF;
    float t = 0.0; // total distance travelled
    // raymarching
    for (int i = 0; i < steps; i++){
        vec3 pos = ro + rd* t;// position along the ray

        float dist = -sceneSDFdistIndex(pos, index);

        //if (hr.lowestDistance > dist) hr.lowestDistance = dist;

        if (dist < mindist){ // treat as if hit
            hr.uv = -sceneSDFUVIndex(pos, index);
            hr.isHit = true;
            hr.distance = dist;
            //hr.totalDistanceTravelled = t;
            hr.normal = CalculateNormalIndMinus(pos, index);
            hr.materialIndex = index;
            hr.hitpos = pos;
            hr.iterationsDBG = float(i) / float(steps);
            break;// how small dist (radius around march)
        }
        t += dist;

        if (t > maxdist)break;
        // failed to hit
        //discard; // discard on far for transparency
        // how large dist (radius around march)
    }

    return hr;
}


hitresult raytraceRootHitST(vec3 ro, vec3 rd, float maxdist, float mindist, int steps, int mdfLength){
    hitresult hr; hr.isHit = false; hr.distance = INF;
    //return raymarchScene(ro, rd, maxdist, mindist, steps);

    for (int i = 0; i < mdfLength; i++ ){
        MDF cMDF = MDFS[i];
    
        hitresult aabbHR = aabbVsRay(ro, rd, cMDF.rootPosition.xyz, cMDF.rootExtents.xyz);

        if (!aabbHR.isHit) continue;
        float sd = max(0.0, aabbHR.distance);
        if (sd >= hr.distance)  continue;
        
        float elipson = (sd + 0.001f);
        vec3 pokePoint = ro + rd * elipson;
        
        float lMaxDist = min(maxdist - elipson, hr.distance - elipson);
        if (lMaxDist <= 0.0) continue; 
        
        hitresult rmHR = RayAcceleratedSphereMarchScene(pokePoint, rd, lMaxDist, mindist, steps, i);
        
        if (rmHR.isHit){
            float tHitDist = elipson + rmHR.distance;
            if (tHitDist < hr.distance) {
                hr = rmHR;
                hr.distance =tHitDist;
                hr.hitpos = ro + rd * tHitDist;
            }
        }
    }
    return hr;
}

hitresult raymarchScene(vec3 ro, vec3 rd, float maxdist, float mindist, int steps, int mdfLength){
    hitresult hr;
    hr.isHit = false;
    //hr.lowestDistance = INF;
    float t = 0.0; // total distance travelled
    // raymarching
    for (int i = 0; i < steps; i++){
        vec3 pos = ro + rd* t;// position along the ray
        
        float dist = sceneSDFdist(pos, mdfLength);

        t += dist;
        //if (hr.lowestDistance > dist) hr.lowestDistance = dist;
        
        if (dist < mindist){ // treat as if hit
            vec4 m = sceneSDF(pos, mdfLength);
            hr.isHit = true;
            hr.distance = dist;
            //hr.totalDistanceTravelled = t;
            hr.normal = CalculateNormal(pos, mdfLength);
            hr.materialIndex = int(m.w);

            hr.hitpos = pos;
            hr.uv = m.yz;
            
            break;// how small dist (radius around march)
        }
        if (dist > maxdist) break; // failed to hit
        //discard; // discard on far for transparency
        // how large dist (radius around march)
    }
    
    return hr;
}

float CalcShadowFactorDIR(vec4 LightSpacePos, vec3 lightDirection, vec3 normal, vec3 iPosition){
    // perform perspective divide
    vec3 lightCoords = LightSpacePos.xyz / LightSpacePos.w;
    float shadow = 0.0f;
    
    // shadow calculation
    if (lightCoords.z <= 1.0f){
        sampler2D bluemap =sampler2D(BlueNoiseHandle) ;
        lightCoords = (lightCoords + 1.0f) / 2.0f;
        // get the current depth
        float currentDepth = lightCoords.z;
        // calculate shadow bias
        float bias = max(DirSMMaxBias * (1.0f - dot(normal, lightDirection)), 0.0005f);
        // PCF
        int sampleRadius = FilterRadius; // FilterRadius // NumberOfSamples
        vec2 texSize = vec2(textureSize(bluemap, 0));
        // uv
        vec2 offset = vec2(fract(frame * 0.618), fract(frame * 0.133));
        vec2 noiseUV = (gl_FragCoord.xy / texSize) + offset;

        vec2 pixelSize = 1.0 / textureSize(shadowMap, 0);
        float tsamples = 0.0;
        for(int y = -sampleRadius; y <= sampleRadius; y++){
            for(int x = -sampleRadius; x <= sampleRadius; x++){
                float angle = texture(bluemap, noiseUV).r * NumberOfSamples;
                vec2 foffset = vec2(cos(angle), sin(angle));
                float closestDepth = texture(shadowMap, vec3(lightCoords.xy + (vec2(x, y) * foffset) * pixelSize, currentDepth - bias )).r;
                //if (currentDepth > closestDepth + bias)
                shadow += (1.0f - closestDepth);
                tsamples += 1.0f;

            }
        }

        shadow /= tsamples;
    }

    return shadow;
}

vec4 direcLight(vec3 ARM, vec3 iNormal, vec3 iPosition){ // normals need to be recalculated based on rotation
    // shadow map 
    float shadow = 0.0f;

    vec3 normal = normalize(iNormal);

    vec3 lightDirection = normalize(directLightPos); //vec3(1.0f, 1.0f, 0.0f)
    float diffuse = max(dot(normal, lightDirection), 0.0f);

    vec4 fragPosLight = lightProjection * vec4(iPosition, 1.0f);

    float smShadow = 0.0;
    //float rmShadow = 0.0f;
    float ssShadow = 0.0f;
    if (doDirShadowMap) shadow = CalcShadowFactorDIR(fragPosLight, lightDirection, normal, iPosition);
    //float fAmbient = directAmbient * ARM.r;
    float specularFactor = 1.0;
    float specular = 0.0f;
    if (doReflect && doDirSpecularLight && diffuse != 0.0f){
        //return (vec4( vec3(1.0 , 0.0, 0.0), 1.0));

        vec3 reflectionDirection = reflect(-lightDirection, normal);
        vec3 viewDirection = normalize(cameraPosition - iPosition);
        vec3 halfwayVec = normalize(viewDirection + lightDirection);

        float specAmount = pow(max(dot(normal, halfwayVec), 0.0f), 16);
        //float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
        specular = specAmount * dirSpecularLight;


        return ((diffuse * (1.0f - shadow) + directAmbient) + specularFactor* specular * (1.0f - shadow)) * vec4(directLightCol, 1.0f); }
    else{ return ((diffuse * (1.0f - shadow) + directAmbient)) * vec4(directLightCol, 1.0f); }
}

vec4 spotLight(int iteration, vec3 ARM, vec3 iNormal, vec3 iPosition){
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

    float shadow = 0.0;

    float specular = 0.0f;
    if (doReflect && diffuse != 0.0f){

        // specular lighting
        //float specularLight = 0.50f;
        vec3 viewDirection = normalize(cameraPosition - iPosition);
        vec3 reflectionDirection = reflect(-lightDirection, normal);

        vec3 halfwayVec = normalize(lightDirection + viewDirection);

        float specAmount = pow(max(dot(normal, halfwayVec), 0.0f), 16);
        specular = specAmount * specularLight;

        finalColour = finalColour + ((diffuse * (1.0f - shadow) * inten + 0.0f) + ARM.b * specular * (1.0f - shadow) * inten) * vec4(Lights[iteration].colour, 1.0) * inten;

    }
    else{

        finalColour = finalColour + ((diffuse * (1.0f - shadow) * inten + 0.0f) * vec4(Lights[iteration].colour, 1.0) * inten);
        //	finalColour = finalColour + (texture(diffuse0, texCoord) * (diffuse * inten + ambient) * vec4(Lights[iteration].colour, 1.0) * inten);
    }

    //finalColour = finalColour + (diffuse * inten + skyColor);

    return finalColour;
}

vec4 pointLight(int iteration, vec3 ARM, vec3 iNormal, vec3 iPosition){
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

    //float ssShadow = shadowTrace(lightDirection, normal, iPosition);

    float shadow = 0.0;

    float specular = 0.0f;
    if (doReflect && diffuse != 0.0f){
        // specular lighting
        //float specularLight = 0.50f;
        vec3 viewDirection = normalize( cameraPosition - iPosition);
        vec3 reflectionDirection = reflect(-lightDirection, normal);

        vec3 halfwayVec = normalize(lightDirection + viewDirection);

        float specAmount = pow(max(dot(normal, halfwayVec), 0.1f), 16);
        specular = specAmount * specularLight;

        finalColour = finalColour + ((diffuse * (1.0f - shadow)* inten + 0.0f) +ARM.b * specular * (1.0f - shadow)* inten) * vec4(Lights[iteration].colour, 1.0 ) * inten;
    }
    else{
        finalColour = finalColour + ( (diffuse * (1.0f - shadow) * inten + 0.0f) * vec4(Lights[iteration].colour, 1.0) * inten);
    }

    return finalColour;
}

vec4 lights(vec3 ARM, vec3 iNormal, vec3 iPosition){
    vec4 colour = vec4(0.0);

    int maxLights = 64;
    for (int i = 0; i < min(lightCount, maxLights); i++){
        if (Lights[i].type == 0)
            colour += spotLight(i, ARM, iNormal, iPosition);
        if (Lights[i].type == 1)
            colour += pointLight(i, ARM, iNormal, iPosition);
    }
    
    if (doDirLight)
        colour+= direcLight(ARM, iNormal, iPosition);
    
    return colour;
}

//https://www.shadertoy.com/view/ttc3zr thank you for the hash function
uvec3 murmurHash33(uvec3 src) {
    const uint M = 0x5bd1e995u;
    uvec3 h = uvec3(1190494759u, 2147483647u, 3559788179u);
    src *= M; src ^= src>>24u; src *= M;
    h *= M; h ^= src.x; h *= M; h ^= src.y; h *= M; h ^= src.z;
    h ^= h>>13u; h *= M; h ^= h>>15u;
    return h;
}

// 3 outputs, 3 inputs
vec3 hash33(vec3 src) {
    uvec3 h = murmurHash33(floatBitsToUint(src));
    return uintBitsToFloat(h & 0x007fffffu | 0x3f800000u) - 1.0;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) { return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0); }

vec3 reflection(vec3 pArm, vec3 pNrm, vec3 ro, vec3 rd, float maxdist, float mindist, int steps, int bounces, int refSteps,
        highp vec2 velocity, out vec3 specEmission, int mdfLength){
    vec3 colour = vec3(0.0); int hitcount = 0;
    
    vec3 oEM = vec3(0.0f);
    
    for (int x = 0; x < refSteps; x++){
        vec3 lastorigin = ro;
        vec3 lastdir = rd;
        
        float rough = pArm.g;
        float met = pArm.b;
        vec3 lColour = texture(gAlbedoSpec, texCoord.xy).rgb;
        
        float dim = 1.0f;
        
        for (int i = 0; i < bounces; i++){
            if (dim <= 0.0f) break;
            hitcount++;

            vec3 t = vec3(time, time +1.0 + float(x), time +3.0 + float( x + i));
            vec3 jitt = mix(vec3(0.0), vec3(hash33(lastorigin + t)), rough);
            vec3 incidentDir = normalize(lastdir + jitt);

            hitresult hr = raytraceRootHitST(lastorigin, incidentDir, maxdist, mindist, steps, mdfLength);
            
            if (hr.isHit){
                MDF cMDF = MDFS[hr.materialIndex];
                vec3 ncolour = textureLod(cMDF.texture_diffuse_Handle, hr.uv, r_minLodLevel).rgb;
                vec3 arm =  textureLod( cMDF.texture_roughness_Handle, hr.uv, r_minLodLevel).rgb;
                arm.g = max(r_roughnessFloor, arm.g);
                
                oEM += textureLod(cMDF.texture_emission_Handle, hr.uv, r_minLodLevel).rgb;
                
                if (!forceMirror){ // the parent material will be the mirror so this does
                    rough = arm.g;
                    met = arm.b;
                }
                
                vec3 shadow = lights(vec3(1.0, rough, met), hr.normal, hr.hitpos).rgb;
                shadow = clamp(shadow + directAmbient + r_shadow_ambient, 0.0, 1.0);

                vec3 F0 = mix(vec3(0.04), ncolour, met);
                vec3 Fresnel = fresnelSchlick(max(dot(hr.normal, -incidentDir), 0.0), F0);
                    
                colour += (ncolour * shadow) * Fresnel * dim;;
                dim *=  0.5f;

                // this helps with self intersection because apparently we are getting that
                vec3 origin = hr.hitpos + hr.normal * originEplison;
                lastorigin = origin;
                lastdir = reflect(lastdir, hr.normal); // probably looks very off cause the normal is smooth
            }
            else {
                vec3 sky = texture(cmMainHandle, incidentDir).rgb;
                colour += (sky* dim);
                //colour += sky * dim; 
                break;  
            }
        }
    }
    specEmission = oEM /  clamp(hitcount, 1.0f, hitcount);
    
    return colour/ clamp(hitcount, 1.0f, hitcount);
}

vec3 sampleHemisphere(vec3 normal, int index){
    float u = rand(texCoord + rand(vec2(time, float(index) ) ) );
    float v = rand(vec2(u, rand( vec2(time, float(index) ) ) ) );

    float phi = 2.0 * 3.14159265 * u;
    float cosTheta = sqrt(1.0 - v);
    float sinTheta = sqrt(v);

    vec3 localDir = vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);

    vec3 helper = abs(normal.y) < 0.999 ? vec3(0, 1, 0) : vec3(1, 0, 0);
    vec3 tangent = normalize(cross(helper, normal));
    vec3 bitangent = cross(normal, tangent);

    return tangent * localDir.x + bitangent * localDir.y + normal * localDir.z;
}

struct indirectChannels{
    vec3 specular; // this is here for accum
    vec4 indirect;
    vec4 emission;
    vec4 idirectSpecular;
    vec4 emissionSpecular;
};

void indirectAndEmissionMarch(vec3 pNrm, vec3 ro, float maxdist, float mindist, int steps, int samples, inout indirectChannels onic, int mdfLength){
    vec3 indColour = vec3(0.0);
    vec3 emColour = vec3(0.0);

    int indHitCount = 0;
    int emHitCount = 0;
    for (int i = 0; i < samples; i++){
        indHitCount++;

        vec3 newDir = sampleHemisphere(pNrm, i);
        hitresult hr = raytraceRootHitST(ro, newDir, maxdist, mindist, steps, mdfLength);
        
        
        if (hr.isHit){
            
            MDF cMDF = MDFS[hr.materialIndex];
            
            //material nMaterial = getMaterial(hr.materialIndex, hr.uv, i_minLodLevel);
            vec3 arm = textureLod( cMDF.texture_roughness_Handle, hr.uv, i_minLodLevel).rgb;
            vec4 mcolour = textureLod(cMDF.texture_diffuse_Handle, hr.uv, i_minLodLevel);
            vec3 mEmission = textureLod(cMDF.texture_emission_Handle, hr.uv, e_minLodLevel).rgb;
            
            //vec3 origin = hr.hitpos + hr.normal * originEplison;

            vec3 direct = lights(arm, hr.normal, hr.hitpos).rgb;
            direct += directAmbient;
            direct = clamp(direct, 0.0, 1.0);
            
            // instead we can check the alpha and attempt to continue firing in a while loop for translucency


            vec3 diffuseComponent = mcolour.rgb * direct;
            
            emHitCount++;
            emColour += mEmission * e_emissionBoost;
            
            indColour += diffuseComponent;
        }
        else {
            vec3 sky = textureLod(cmMainHandle, newDir, 5).rgb;
            indColour += sky;
            //break;
        }
        //colour = colour / hitcount;
        //colour = (colour)/ hitcount;
        // this is where we write cache
    }
    indirectChannels nIC;
    //nIC.indirect.rgb = clamp(indColour / indHitCount, 0.0, 1.0);
    //nIC.emission.rgb = clamp(emColour / emHitCount, 0.0, 1.0);
    nIC.indirect.rgb = ( indColour / clamp(indHitCount, 1, indHitCount) )* i_indirectBoost;
    nIC.emission.rgb = emColour /clamp(emHitCount, 1, emHitCount);

    onic = nIC;
}

vec4 blueNoise4(){ // for fade out or opacity (cheap) (could fade out near farplane or nearplane)
    vec2 texSize = vec2(textureSize(BlueNoiseHandle, 0));
    vec2 offset = vec2(fract(frame * 0.618), fract(frame * 0.133));
    vec2 noiseUV = (gl_FragCoord.xy / texSize) + offset;

    return texture(BlueNoiseHandle, noiseUV);
}

vec3 rayDirfromCam(mat4 projection, mat4 view, vec2 uv){
    vec4 target = projection * vec4(uv.x, uv.y, -1.0, 1.0);
    vec3 rayDirView = normalize(target.xyz / target.w);
    vec3 rayDirWorld = normalize(mat3(view) * rayDirView);

    return normalize(rayDirWorld);
}

float linearizeDepth(float depth, float NearPlane, float FarPlane){
    return (2.0 * NearPlane * FarPlane) / (FarPlane + NearPlane - (depth * 2.0 - 1.0) * (FarPlane - NearPlane));
}

vec3 WorldPosFromDepth(float depth, mat4 invProjMatrix, mat4 invViewMat) {
    float z = depth * 2.0 - 1.0;

    vec4 clipSpacePosition = vec4(texCoord * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = invProjMatrix * clipSpacePosition;

    // Perspective division
    viewSpacePosition /= viewSpacePosition.w;

    vec4 worldSpacePosition = invViewMat * viewSpacePosition;

    return worldSpacePosition.xyz;
}

vec4 accumulate(vec3 Input, sampler2D previous, highp vec2 historyTexCoord, float factor){
    vec4 previousColour = texture(previous, historyTexCoord);
    
    vec3 accumulated = mix(Input, previousColour.rgb, factor);

    // calc variance
    float inputLuma = lumaFromRGB(Input);
    float accumLuma = lumaFromRGB(accumulated);
    float nvariance = (inputLuma - accumLuma) * (inputLuma - accumLuma);
    float accumVariance = mix(nvariance, previousColour.a, temporalAccumulationBlendFactor);

    return vec4(accumulated, accumVariance);
}

indirectChannels temporalAccumulate(indirectChannels Input, highp vec2 historyTexCoord, float currentDepth, vec3 currentPosition, float roughness){
    // rejection
    if (any(lessThan(historyTexCoord, vec2(0.0))) || any(greaterThan(historyTexCoord, vec2(1.0)))){
        return Input;
    }

    // disocclusion rejection here
    float previousDepth = texture(swrtHDepth, historyTexCoord).r;
    float previousDepthLinearized = linearizeDepth(previousDepth, NearPlane, FarPlane);
    float depthDifference = abs(previousDepthLinearized - currentDepth);
    float threshold = 0.3 * currentDepth;
    //float threshold = 0.5 * currentDepth;

    //vec3 previousPosition = WorldPosFromDepth(previousDepth, invHProjectionMatrix, invHViewMatrix);
    //vec3 previousPosition = WorldPosFromDepth(previousDepth, invProjectionMatrix, invViewMatrix);
    //vec3 posDifference = previousPosition - currentPosition;
    //float posDifFactor = dot(currentPosition - previousPosition, currentPosition - previousPosition);
    if (depthDifference > threshold) return Input; //return vec4(vec3(1.0,0.0,0.0), 1.0);
   // if (depthDifference > threshold || posDifFactor >  0.999) return Input; //return vec4(vec3(1.0,0.0,0.0), 1.0);/if (depthDifference > threshold) return Input;
   // if (posDifFactor > 0.999) return Input;
    
    // accumulate
    //float blendFactor = 0.9;
    //clamp(temporalAccumulationBlendFactor, 0.0, 0.9)
    float factor = clamp(temporalAccumulationBlendFactor, 0.0, 0.99);

    // conserve brighter samples
    float pLuma = lumaFromRGB(texture(hEmission, historyTexCoord).rgb);
    float cLuma = lumaFromRGB(Input.emission.rgb);
    float variance = abs(cLuma - pLuma);
    variance = clamp(variance, 0.0, 1.0);
    float cweight = mix(factor, 0.0, variance);
    /**/

    indirectChannels nIC; nIC = Input;
    nIC.indirect = accumulate(Input.indirect.rgb, hIndirect, historyTexCoord, factor);
    nIC.emission = accumulate(Input.emission.rgb, hEmission, historyTexCoord, cweight);
    if (roughness > 0.3){
        nIC.idirectSpecular = accumulate(Input.idirectSpecular.rgb, hIndirectSpecular, historyTexCoord, factor);
        nIC.emissionSpecular = accumulate(Input.emissionSpecular.rgb, hEmissionSpecular, historyTexCoord, factor);
        nIC.specular = accumulate(Input.specular.rgb, hSpecular, historyTexCoord, factor).rgb;
    }
    return nIC;
}

void main(){
    
    int mdfLength = MDFS.length();

    bool split = true;
    if (gl_FragCoord.x > screenSize.x / 2 && doDenoiseSplitDBGView) split = false;
    
    if (drawSDFSCENE && split){ // SDF SCENE VIEW
        vec2 uv = (texCoord - 0.5) * 2.0;
        vec3 rayDir = rayDirfromCam(invProjectionMatrix, invViewMatrix, uv);

        hitresult hr = raytraceRootHitST(cameraPosition, rayDir, 128, mindist, 128, mdfLength);
        ospecular.rgb = textureLod(MDFS[hr.materialIndex].texture_diffuse_Handle, hr.uv, 0).rgb;
        return;
    }

    float gdepth = texture2D(depthMap, texCoord).r;
    if (gdepth >= 0.99999) discard;
    
    vec3 gp = texture(gPosition, texCoord).xyz;
    //vec3 gp                  = WorldPosFromDepth(gdepth, invProjectionMatrix, invViewMatrix);
    vec3 gnrm              = normalize(texture(gNormal, texCoord).xyz);
    vec3 galbedo          = texture(gAlbedoSpec, texCoord).xyz;
    vec3 gemission       = texture(gEmission, texCoord).xyz;
    vec3 garm              = texture(gSpecular, texCoord).xyz;
    highp vec2 velocity = texture(gVelocity, texCoord).rg;
    //odirect.rgb = gp2;
    //return;
    
    if (forceMirror) {garm.g = 0.0; garm.b = 1.0;}
    garm.g = max(r_roughnessFloor, garm.g);
    vec4 noise = blueNoise4();
    // final lighting
    
    vec3 origin = gp + gnrm * originEplison;
    
    indirectChannels nIC;
    vec3 tRef = vec3(0.0f);
    vec3 tRefEM = vec3(0.0f);
    if (noise.b <= r_noiseThreshold )  tRef = reflection(garm, gnrm, origin, reflect(normalize(gp - cameraPosition),  gnrm),
            r_maxdist, mindist, r_steps, r_bounces, r_samples, velocity, tRefEM, mdfLength);
    if (noise.g <=  i_noiseThreshold ) indirectAndEmissionMarch(gnrm, origin, i_maxdist, mindist, i_steps, i_samples, nIC, mdfLength);

    vec3 null = vec3(1.0f, 0.0f, 0.0f);
    nIC.idirectSpecular.rgb = null;
    nIC.emissionSpecular.rgb = tRefEM;
    nIC.specular = tRef;
    
    // temporal accumulation here
    
    if (doTemporalAccumulation && split){
        float d = linearizeDepth(texture2D(depthMap, texCoord).r, NearPlane, FarPlane);
        highp vec2 historyTexCoord = texCoord - velocity;

        indirectChannels nNIC = temporalAccumulate(nIC, historyTexCoord, d, gp, garm.g);
        
        oindirect = nNIC.indirect;
        oemission = nNIC.emission;
        oindirectSpecular = nNIC.idirectSpecular;
        oemissionSpecular = nNIC.emissionSpecular;
        ospecular = nNIC.specular;

        return;
    }
    
    oindirect = nIC.indirect;
    oemission = nIC.emission;
    oindirectSpecular = nIC.idirectSpecular;
    oemissionSpecular = nIC.emissionSpecular;
    ospecular = nIC.specular;

}
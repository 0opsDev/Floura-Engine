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
layout(location = 5) out vec3 odirect;

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
uniform sampler2D presentImage;

uniform vec2 screenSize;
// temp

struct localSDF{
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

layout(std430, binding = 11) buffer localSDF_Buffer {
    localSDF lSDFS[];
};

float mindist = 0.01f;

float r_maxdist = 64.0f;
float r_shadow_ambient = 0.0f; //0.07f;
int r_steps = 80;
int r_bounces = 1;
int r_samples  = 1;
int r_minLodLevel = 2;
float r_noiseThreshold =1.0;
int r_coneSteps = 32;
float r_coneApature =0.05; // 0.1 originally but 0.05 brings simular quality to rm
float r_coneKickInLevel = 0.4f; // .6 .7 are better settings
int r_coneBounceKickInLevel =1; // starts at 0
int r_ssrSteps = 256;
float r_ssrStepSize =0.4f;
bool r_doSSR = false;
bool forceMirror = false;

float i_maxdist = 64.0f;
int i_steps = 80;
int i_samples = 1;  
int i_minLodLevel = 7;
int e_minLodLevel = 7;
float i_noiseThreshold  = 1.0;
int i_coneSteps = 32;
float i_coneApature =0.05;  // 0.1 originally but 0.05 brings simular quality to rm
bool i_doConeTracing = true;

bool shadowsEnabled = true;
bool aosEnabled = false;
bool doContactShadows = true;
float ao_noiseThreshold =1.0;
bool doTextureAO = false;
//float maxshadowDistance = 0.0f;
//float ambientLight = 0.0;

bool drawSDFSCENE = false;

int maxlodLevel = 0; // off
float transitionRangeLOD = 20.0f;
//int SDFtextureLodLevel = 0;
float maxSDFDist = 128.0f;

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

float sdBox(vec3 p, vec3 b){
    vec3 q = abs(p) - b;
    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0f);
}

vec3 rot3D(vec3 p, vec3 axis, float angle){
    return mix(dot(axis, p) * axis, p, cos(angle))
            + cross(axis, p * sin(angle));
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

vec3 quatTransform( vec4 q, vec3 p ){
    return p + 2.*cross( q.xyz, cross( q.xyz, p ) + q.w*p );
}


vec4 SDFMesh(vec3 p, int index, int lodLevel){
    vec3 gp = p - lSDFS[index].gPosition.xyz;
    //vec3 fp = quatTransform( lSDFS[index].gRotation, gp);
    vec3 fp = transformP(gp, lSDFS[index].globalTransform);
    
    vec3 nsdfv = texture3DSDFUV(fp, lSDFS[index].gExtents.rgb, lSDFS[index].SDF_Handle, lodLevel);
    
    vec2 uv = nsdfv.gb * vec2(lSDFS[index].position.w, lSDFS[index].extents.w);
    
    return vec4(nsdfv.r, float(index), uv);
}

float SDFMeshDist(vec3 p, int index, int lodLevel){
    vec3 gp = p - lSDFS[index].gPosition.xyz;
    //vec3 fp = quatTransform( lSDFS[index].gRotation, gp);
    vec3 fp = transformP(gp, lSDFS[index].globalTransform);
    
    float nsdfv = texture3DSDFUV(fp, lSDFS[index].gExtents.rgb, lSDFS[index].SDF_Handle, lodLevel).r;

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

vec4 sceneSDFIndex(vec3 p, int i){ // scene
    vec4 tsdf = vec4(INF, 0.0, 0.0, 0.0);

    //aabbHitDBGmode
    //index
    if (sdBox(p - sceneBoundPos.xyz, sceneBoundScale.xyz) > tsdf.x) return tsdf;
    
        vec3 np = nearestPointOnAABB(cameraPosition, lSDFS[i].rootPosition.xyz, lSDFS[i].rootExtents.xyz);

        if (distance(np, cameraPosition) > maxSDFDist) return tsdf;

        float root = sdBox(p - lSDFS[i].rootPosition.xyz, lSDFS[i].rootExtents.xyz);
        if (root < tsdf.x){

            int lodLevel = calculateLODLevel(np, cameraPosition, transitionRangeLOD, maxlodLevel);
            vec4 sdfm = SDFMesh(p, i, lodLevel);
            //vec4 sdfm = vec4(root, 0.0, 0.0, 0.0);
            tsdf = sdfm;
        }
    return vec4(tsdf);
}

float sceneSDFdistIndex(vec3 p, int i){ // scene
    float tsdf = INF;

    if (sdBox(p - sceneBoundPos.xyz, sceneBoundScale.xyz) > tsdf) return tsdf;
    
        vec3 np = nearestPointOnAABB(cameraPosition, lSDFS[i].rootPosition.xyz, lSDFS[i].rootExtents.xyz);

        if (distance(np, cameraPosition) > maxSDFDist) return tsdf;

        float root = sdBox(p - lSDFS[i].rootPosition.xyz, lSDFS[i].rootExtents.xyz);
        if (root < tsdf){

            int lodLevel = calculateLODLevel(np, cameraPosition, transitionRangeLOD, maxlodLevel);
            float sdfm = SDFMeshDist(p, i, lodLevel);
            
            tsdf = sdfm;

        }
    return tsdf;
}


vec4 sceneSDF(vec3 p ){ // scene
    vec4 tsdf = vec4(INF, 0.0, 0.0, 0.0);

    //aabbHitDBGmode
    
    if (sdBox(p - sceneBoundPos.xyz, sceneBoundScale.xyz) > tsdf.x) return tsdf;
    
    for (int i = 0; i < lSDFS.length(); i++ ){
        vec3 np = nearestPointOnAABB(cameraPosition, lSDFS[i].rootPosition.xyz, lSDFS[i].rootExtents.xyz);
        
        if (distance(np, cameraPosition) > maxSDFDist) continue;
        
        float root = sdBox(p - lSDFS[i].rootPosition.xyz, lSDFS[i].rootExtents.xyz);
        if (root < tsdf.x){
            
            int lodLevel = calculateLODLevel(np, cameraPosition, transitionRangeLOD, maxlodLevel);
            vec4 sdfm = SDFMesh(p, i, lodLevel);
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

float sceneSDFdist(vec3 p ){ // scene
    float tsdf = INF;

    if (sdBox(p - sceneBoundPos.xyz, sceneBoundScale.xyz) > tsdf.x) return tsdf;
    
    for (int i = 0; i < lSDFS.length(); i++ ){

        vec3 np = nearestPointOnAABB(cameraPosition, lSDFS[i].rootPosition.xyz, lSDFS[i].rootExtents.xyz);

        if (distance(np, cameraPosition) > maxSDFDist) continue;

        float root = sdBox(p - lSDFS[i].rootPosition.xyz, lSDFS[i].rootExtents.xyz);
        if (root < tsdf){

            int lodLevel = calculateLODLevel(np, cameraPosition, transitionRangeLOD, maxlodLevel);
            float sdfm = SDFMeshDist(p, i, lodLevel);

            if (i == 0) {
                tsdf = sdfm;
                continue;
            }
            tsdf = fOpUnionID(tsdf,sdfm); //fOpUnionRoundID4 fOpUnionID4

        }
    }
    return tsdf;
}

vec3 CalculateNormal( in vec3 p ){
    const float eps = 0.01;
    const vec2 h = vec2(eps,0);
    return normalize( vec3(sceneSDFdist(p+h.xyy) - sceneSDFdist(p-h.xyy),
            sceneSDFdist(p+h.yxy) - sceneSDFdist(p-h.yxy),
            sceneSDFdist(p+h.yyx) - sceneSDFdist(p-h.yyx) ) );
}

vec3 CalculateNormalInd( in vec3 p, int index){
    const float eps = 0.01;
    const vec2 h = vec2(eps,0);
    return normalize( vec3(sceneSDFdistIndex(p+h.xyy, index) - sceneSDFdistIndex(p-h.xyy, index),
            sceneSDFdistIndex(p+h.yxy, index) - sceneSDFdistIndex(p-h.yxy, index),
            sceneSDFdistIndex(p+h.yyx, index) - sceneSDFdistIndex(p-h.yyx, index) ) );
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

        vec4 m = sceneSDFIndex(pos, index);
        float dist = m.x;
        
        //if (hr.lowestDistance > dist) hr.lowestDistance = dist;

        if (dist < mindist){ // treat as if hit
            hr.isHit = true;
            hr.distance = dist;
            //hr.totalDistanceTravelled = t;
            hr.normal = CalculateNormalInd(pos, index);
            hr.materialIndex = int(m.y);
            hr.hitpos = pos;
            hr.uv = m.zw;
            hr.iterationsDBG = float(i) / float(steps);

            break;// how small dist (radius around march)
        }
        t += dist;
        
        if (t > maxdist) break; // failed to hit
        //discard; // discard on far for transparency
        // how large dist (radius around march)
    }

    return hr;
}

hitresult raytraceRootHitST(vec3 ro, vec3 rd, float maxdist, float mindist, int steps){
    hitresult hr; hr.isHit = false; hr.distance = INF;

    //return raymarchScene(ro, rd, maxdist, mindist, steps);

    for (int i = 0; i < lSDFS.length(); i++ ){
        hitresult aabbHR = aabbVsRay(ro, rd, lSDFS[i].rootPosition.xyz, lSDFS[i].rootExtents.xyz);

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

hitresult RayAcceleratedConeTraceScene(vec3 ro, vec3 rd, float maxdist, int steps, float aperture,  int index){
    hitresult hr;
    hr.isHit = false;
    hr.distance = INF;
    float t = 0.0; // total distance travelled

    // raymarching
    for (int i = 0; i < steps; i++){
        vec3 pos = ro + rd* t;// position along the ray

        vec4 m = sceneSDFIndex(pos, index);
        float dist = m.x;

        float radius = t * aperture;

        //radius += radius * aperture; // add by current radius * the aperture
        //if (hr.lowestDistance > dist) hr.lowestDistance = dist;

        if (dist < radius){ // treat as if hit
            hr.isHit = true;
            hr.distance = dist;
            //hr.totalDistanceTravelled = t;
            hr.normal = CalculateNormalInd(pos, index);
            hr.materialIndex = int(m.y);
            hr.iterationsDBG = float(i) / float(steps);
            hr.hitpos = pos;
            hr.uv = m.zw;

            break;// how small dist (radius around march)
        }
        t += max(dist, radius);
        
        if (dist > maxdist) break; // failed to hit
        
    }

    return hr;
}

hitresult raytraceRootHitCT(vec3 ro, vec3 rd, float maxdist, int steps, float apature){
    hitresult hr; hr.isHit = false; hr.distance = INF;

    //return raymarchScene(ro, rd, maxdist, mindist, steps);

    for (int i = 0; i < lSDFS.length(); i++ ){
        hitresult aabbHR = aabbVsRay(ro, rd, lSDFS[i].rootPosition.xyz, lSDFS[i].rootExtents.xyz);

        if (!aabbHR.isHit) continue;
        float sd = max(0.0, aabbHR.distance);
        if (sd >= hr.distance)  continue;

        float elipson = (sd + 0.001f);
        vec3 pokePoint = ro + rd * elipson;

        float lMaxDist = min(maxdist - elipson, hr.distance - elipson);
        if (lMaxDist <= 0.0) continue;

        hitresult rmHR = RayAcceleratedConeTraceScene(pokePoint, rd, lMaxDist, steps, apature, i);

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


hitresult raymarchScene(vec3 ro, vec3 rd, float maxdist, float mindist, int steps){
    hitresult hr;
    hr.isHit = false;
    //hr.lowestDistance = INF;
    float t = 0.0; // total distance travelled

    // raymarching
    for (int i = 0; i < steps; i++){
        vec3 pos = ro + rd* t;// position along the ray
        
        vec4 m = sceneSDF(pos);
        float dist = m.x;

        t += dist;
        //if (hr.lowestDistance > dist) hr.lowestDistance = dist;
        
        if (dist < mindist){ // treat as if hit
            hr.isHit = true;
            hr.distance = dist;
            //hr.totalDistanceTravelled = t;
            hr.normal = CalculateNormal(pos);
            hr.materialIndex = int(m.y);

            hr.hitpos = pos;
            hr.uv = m.zw;
            
            break;// how small dist (radius around march)
        }
        if (dist > maxdist) break; // failed to hit
        //discard; // discard on far for transparency
        // how large dist (radius around march)
    }
    
    return hr;
}

hitresult coneTraceScene(vec3 ro, vec3 rd, float maxdist, int steps, float aperture){
    hitresult hr;
    hr.isHit = false;
    //hr.lowestDistance = INF;
    float t = 0.0; // total distance travelled
    
    // raymarching
    for (int i = 0; i < steps; i++){
        vec3 pos = ro + rd* t;// position along the ray

        vec4 m = sceneSDF(pos);
        float dist = m.x;
        
        float radius = t * aperture;
        
        //radius += radius * aperture; // add by current radius * the aperture
        //if (hr.lowestDistance > dist) hr.lowestDistance = dist;

        if (dist < radius){ // treat as if hit
            hr.isHit = true;
            hr.distance = dist;
            //hr.totalDistanceTravelled = t;
            hr.normal = CalculateNormal(pos);
            hr.materialIndex = int(m.y);

            hr.hitpos = pos;
            hr.uv = m.zw;

            break;// how small dist (radius around march)
        }
        if (dist > maxdist) break; // failed to hit

        t += max(dist, radius);
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

vec4 RayCast(in vec3 dir, inout vec3 hitCoord, out float dDepth, int maxSteps, out float hit, in float step){
    hitCoord += dir * 0.1;
    //hitCoord += dir;

    dir *= step;
    float depth;
    vec4 projectedCoord = vec4(0.0);

    for (int i = 1; i < maxSteps; i++){
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


        if((dir.z - dDepth) < 0.3){
            if(dDepth <= 0.0){
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

float hash2(vec2 p) {
    p = fract(p * vec2(123.34, 456.21));
    p += dot(p, p + 45.32);
    return fract(p.x * p.y);
}

float shadowTrace(vec3 lightDirection, vec3 normal, vec3 iPosition, out bool ohit){
    vec3 viewPos    = (viewMatrix * vec4(iPosition, 1.0)).xyz;
    vec3 viewNormal = normalize(mat3(viewMatrix) * normal);

    vec3 viewLightDir = normalize(mat3(viewMatrix) * lightDirection);

    vec3 rayOrigin = viewPos + viewNormal * 0.1;


    vec2 uv = gl_FragCoord.xy;
    float noiseX = hash2(uv + vec2(time));
    float noiseY = hash2(uv + vec2(time + 1.5));
    vec3 jitter = vec3(noiseX - 0.5, noiseY - 0.5, 0.0) * 0.03;

    vec3 rayDir = normalize(viewLightDir + jitter);

    float dDepth;
    float hit = 0.0;

    float shadow = 0.0f;
    ohit = false;
    
    //vec3 wp = vec3(vec4(viewPos, 1.0) * inverseViewMatrix);
    //vec3 t = vec3(time, time +2, time + 3);
    //vec3 jitt = mix(vec3(0.0), vec3(hash(wp + t)), 0.1); // time
    //jitt * time;

    //vec4 csCoords = RayCast((vec3(jitt)) + rayDir, rayOrigin, dDepth, 12, hit, 0.05);
    vec4 csCoords = RayCast(rayDir, rayOrigin, dDepth, 64, hit, 0.05);
    if (hit == 1.0f){
        shadow += 1.0f;
        ohit = true;
    }
    return shadow;
}

vec4 direcLight(vec3 ARM, vec3 iNormal, vec3 iPosition, bool isPrimary){ // normals need to be recalculated based on rotation
    if (!shadowsEnabled) return vec4(1.0);
    // shadow map 
    float shadow = 0.0f;

    vec3 normal = normalize(iNormal);

    vec3 lightDirection = normalize(directLightPos); //vec3(1.0f, 1.0f, 0.0f)
    float diffuse = max(dot(normal, lightDirection), 0.0f);

    vec4 fragPosLight = lightProjection * vec4(iPosition, 1.0f);

    float smShadow = 0.0;
    //float rmShadow = 0.0f;
    float ssShadow = 0.0f;
    bool hit = false; // will be set to false inside ssshadow;
    
    //if (!hit) 
    if (isPrimary) ssShadow = shadowTrace(lightDirection, normal, iPosition, hit);
    if (doDirShadowMap && !hit) smShadow = CalcShadowFactorDIR(fragPosLight, lightDirection, normal, iPosition);
    //vec3 origin = iPosition + iNormal * originEplison;
    //if (!hit) rmShadow = 1.0 - softshadow(origin, normalize(directLightPos), minShadowDistance, maxshadowDistance, shadowSteps);
    //shadow = max(rmShadow ,max(smShadow, ssShadow)); // rmshadow is just way too dam slow
    shadow = max(smShadow, ssShadow);
    //shadow = smShadow;
    if (doTextureAO) shadow *= ARM.r;


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


        //   return ((diffuse * (1.0f - shadow)) + ARM.g* specular * (1.0f - shadow)) * vec4(directLightCol, 1.0f); }
        //else{ return ((diffuse * (1.0f - shadow))) * vec4(directLightCol, 1.0f); }

        return ((diffuse * (1.0f - shadow) + directAmbient) + specularFactor* specular * (1.0f - shadow)) * vec4(directLightCol, 1.0f); }
    else{ return ((diffuse * (1.0f - shadow) + directAmbient)) * vec4(directLightCol, 1.0f); }
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

const float reflectionSpecularFalloffExponent = 3.0;

vec4 ssr(vec3 ARM, vec3 position, vec3 normal, int maxSteps, float step, sampler2D hColour, out vec3 gpos, out vec3 gNrm){
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
    //vec3 jitt = mix(vec3(0.0), vec3(hash31(rough)), rough); // time
    vec3 jitt = mix(vec3(0.0), vec3(hash33(wp + t)), rough); // time

    //vec4 csCoords = RayCast(R, hitPos, dDepth, 32);
    //vec4 csCoords = RayCast(R, hitPos, dDepth, 32, hit, 0.10);

    vec3 rayDir = normalize(R + jitt) * step;

    ivec2 size = ivec2(textureSize(gNormal, 0)); // get size of tex

    vec4 csCoords = RayCast(rayDir, hitPos, dDepth, maxSteps, hit, step);

    if (hit == 1.0){
        //vec4 csCoords = RayCast(vec3(jitt) + R, hitPos, dDepth, maxSteps, hit, step);

        vec2 dCoords = smoothstep(0.2, 0.6, abs(vec2(0.5, 0.5) - csCoords.xy));
        //vec2 dCoords = smoothstep(0.2, 0.6, abs(vec2(0.5, 0.5) - texCoord.xy));

        float screenEdgefactor = clamp(1.0 - (dCoords.x + dCoords.y), 0.0, 1.0);

        float reflectionMultiplier = pow(Metallic, reflectionSpecularFalloffExponent) *
        screenEdgefactor *
        -R.z;

        vec3 SSR = textureLod(hColour, csCoords.xy, 0).rgb;//hColour
        //vec3 SSR = textureLod(hColour, csCoords.xy, 0).rgb * clamp(ReflectionMultiplier, 0.0, 0.9);//hColour
        gpos = texture(gPosition, csCoords.xy, 0).rgb;
        gNrm = normalize(texture(gNormal, csCoords.xy, 0).xyz);
        return vec4(clamp(SSR,0.0, 1.0), reflectionMultiplier);//Metallic
    }
    return vec4(0.0);

}

vec3 reflection(vec3 pArm, vec3 pNrm, vec3 ro, vec3 rd, float maxdist, float mindist, int steps, int bounces, int refSteps, bool doSSR){
    vec3 colour = vec3(0.0); int hitcount = 0;
    
    vec3 F0 = vec3(0.04);
    F0      = mix(F0, texture(gAlbedoSpec, texCoord.xy).rgb, pArm.b);
    vec3 Fresnel = fresnelSchlick(max(dot(normalize(pNrm), normalize(normalize(rd))), 0.0), F0);
    
    //ssr
    vec3 gPos = vec3(0.0f);
    vec3 gNrm = vec3(0.0f);
    vec4 nSSR = vec4(0.0f);
    
    if (doSSR) nSSR = ssr(pArm, ro, pNrm, r_ssrSteps, r_ssrStepSize, presentImage, gPos, gNrm);
    
    if (nSSR.a == 1.0f){
        ro = gPos;
        rd = gNrm;
        colour = nSSR.rgb;

        bounces -= 1;
    }
    
    if (bounces <= 0) return nSSR.rgb;
    
    for (int x = 0; x < refSteps; x++){
        vec3 lastorigin = ro;
        vec3 lastdir = rd;
        
        float rough = pArm.g;
        float met = pArm.b;
        
        float dim = 1.0f;
        vec3 wp = vec3(ro);
        vec3 t = vec3(time, time +1 + x, time +3 + x);
        vec3 jitt = mix(vec3(0.0), vec3(hash33(wp + t)), pArm.g);
        
        for (int i = 0; i < bounces; i++){
            if (dim <= 0.0f) break;
            hitcount++;

            hitresult hr;
            if (rough >= r_coneKickInLevel || r_coneBounceKickInLevel == i) hr = raytraceRootHitCT(lastorigin, normalize(lastdir + jitt), maxdist, r_coneSteps, r_coneApature);
            else  hr = raytraceRootHitST(lastorigin, normalize(lastdir + jitt), maxdist, mindist, steps);
            //hitresult hr = raytraceRootHitCT(lastorigin, normalize(lastdir + jitt), maxdist, r_coneSteps, r_coneApature);
            //hitresult hr = raytraceRootHitST(lastorigin, normalize(lastdir + jitt), maxdist, mindist, steps);

            if (hr.isHit){

                vec3 origin = hr.hitpos + hr.normal * originEplison;
                lastorigin = origin;

                vec3 F0 = vec3(0.04);
                F0      = mix(F0, texture(gAlbedoSpec, texCoord.xy).rgb, met);
                vec3 Fresnel = fresnelSchlick(max(dot(normalize(hr.normal), normalize(normalize(lastdir))), 0.0), F0);
                
                lastdir = reflect(lastdir, hr.normal); // probably looks very off cause the normal is smooth

                //material nMaterial = getMaterial(hr.materialIndex, hr.uv, clamp(i + r_minLodLevel, 0, 10));
                vec3 ncolour = textureLod(lSDFS[hr.materialIndex].texture_diffuse_Handle, hr.uv, r_minLodLevel).rgb;
                vec3 arm = textureLod( lSDFS[hr.materialIndex].texture_roughness_Handle, hr.uv, r_minLodLevel).rgb;
                
                if (!forceMirror){ // the parent material will be the mirror so this does
                    rough = arm.g;
                    met = arm.b;
                }
                wp = vec3(hr.hitpos);
                t = vec3(time,time + i + x + 1,time + x + i + 3);
                jitt = mix(vec3(0.0), vec3(hash33(wp + t)), rough);
                
                vec3 shadow = direcLight(arm, hr.normal, hr.hitpos, false).rgb;
                shadow += directAmbient;
                shadow += r_shadow_ambient;
                shadow = clamp(shadow, 0.0, 1.0);

                vec3 diffuseComponent = ncolour * shadow;

                colour += (diffuseComponent * dim) * Fresnel;
                dim *=  0.5f;
            }
            else {
                vec3 sky = texture(cmMainHandle, normalize(lastdir + jitt)).rgb;
                colour += (sky* dim) * Fresnel;
                //colour += skycolour * dim; 
                break;
            }
        }
        
    }
    vec3 fColour = clamp((colour/ hitcount), 0.0f, 1.0f);
    
    fColour = mix(fColour, nSSR.rgb, nSSR.a);
    
    return fColour;
}

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898, 78.233))) * 43758.5453);
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

void indirectAndEmissionMarch(vec3 pArm, vec3 pNrm, vec3 ro, float maxdist, float mindist, int steps, int samples, inout indirectChannels onic){
    vec3 indColour = vec3(0.0);
    vec3 emColour = vec3(0.0);

    int indHitCount = 0;
    int emHitCount = 0;
    vec3 lastNrm = pNrm;
    vec3 lastArm = pArm;
    //                lastdir = reflect(lastdir, hr.normal); 
    for (int i = 0; i < samples; i++){
        indHitCount++;

        vec3 newDir = sampleHemisphere(pNrm, i);
        //newDir = mix(reflect(rd, pNrm), newDir, clamp(lastArm.g, 0.7, 1.0));
        //hitresult hr;
        //if (lastArm.g >= 0.6) hr = coneTraceScene(ro, newDir, maxdist, 32, 0.1);
        //else         hr = raymarchScene(ro, newDir, maxdist, mindist, steps);
        //hitresult hr = raymarchScene(ro, newDir, maxdist, mindist, steps);
        //hitresult hr = coneTraceScene(ro, newDir, maxdist, i_coneSteps, i_coneApature);
        hitresult hr;
        if (!i_doConeTracing) hr = raytraceRootHitST(ro, newDir, maxdist, mindist, steps);
        else hr = raytraceRootHitCT(ro, newDir, maxdist, i_coneSteps, i_coneApature);
        //i_doConeTracing

        if (hr.isHit){
            //material nMaterial = getMaterial(hr.materialIndex, hr.uv, i_minLodLevel);
            emHitCount++;
            lastArm = textureLod( lSDFS[hr.materialIndex].texture_roughness_Handle, hr.uv, i_minLodLevel).rgb;
            vec3 mcolour = textureLod(lSDFS[hr.materialIndex].texture_diffuse_Handle, hr.uv, i_minLodLevel).rgb;
            lastNrm = textureLod(lSDFS[hr.materialIndex].texture_normal_Handle, hr.uv, i_minLodLevel).rgb;
            
            vec3 origin = hr.hitpos + hr.normal * originEplison;

            vec3 shadow = direcLight(lastArm, hr.normal, hr.hitpos, false).rgb;
            shadow += directAmbient;
            shadow = clamp(shadow, 0.0, 1.0);

            vec3 diffuseComponent = mcolour * shadow;

            emColour += textureLod( lSDFS[hr.materialIndex].texture_emission_Handle, hr.uv, e_minLodLevel).rgb;
            indColour += diffuseComponent;
        }
        else {
            vec3 sky = textureLod(cmMainHandle, newDir, 5).rgb;
            indColour += sky;
            break;
        }
        //colour = colour / hitcount;
        //colour = (colour)/ hitcount;
        // this is where we write cache
        //hr.uvw;
    }
    indirectChannels nIC;
    nIC.indirect.rgb = clamp(indColour / indHitCount, 0.0, 1.0);
    nIC.emission.rgb = clamp(emColour / emHitCount, 0.0, 1.0);

    onic = nIC;
}

float blueNoise(){ // for fade out or opacity (cheap) (could fade out near farplane or nearplane)
    vec2 texSize = vec2(textureSize(BlueNoiseHandle, 0));
    vec2 offset = vec2(fract(frame * 0.618), fract(frame * 0.133));
    vec2 noiseUV = (gl_FragCoord.xy / texSize) + offset;

    return texture(BlueNoiseHandle, noiseUV).r;
}

bool discardThresholdFloat(float i, float t){
    if (i > t) return true;
    return false;
}

vec3 rayDirfromCam(mat4 projection, mat4 view, vec2 uv){
    vec4 target = projection * vec4(uv.x, uv.y, -1.0, 1.0);
    vec3 rayDirView = normalize(target.xyz / target.w);
    vec3 rayDirWorld = normalize(mat3(view) * rayDirView);

    return normalize(rayDirWorld);
}

float lumaFromRGB(vec3 rgb){
    vec3 weights = vec3(0.2126, 0.7152, 0.0722);
    float luminance = dot(rgb, weights);
    return luminance;
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
    //float threshold = 0.3 * currentDepth;
    float threshold = 0.5 * currentDepth;

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
    indirectChannels nIC; nIC = Input;
    float factor = clamp(temporalAccumulationBlendFactor, 0.0, 0.9);
    nIC.indirect = accumulate(Input.indirect.rgb, hIndirect, historyTexCoord, factor);
    nIC.emission = accumulate(Input.emission.rgb, hEmission, historyTexCoord, factor);
    if (roughness > 0.4){
        nIC.idirectSpecular = accumulate(Input.idirectSpecular.rgb, hIndirectSpecular, historyTexCoord, factor);
        nIC.emissionSpecular = accumulate(Input.emissionSpecular.rgb, hEmissionSpecular, historyTexCoord, factor);
    }
    //nIC.indirect = vec4(posDifFactor);
    //nIC.indirect = vec4(vec3(previousPosition), 1.0);
    //nIC.indirect = vec4(vec3(posDifference), 1.0);
    //posDifference
    
    return nIC;
}

float hash1( float n ){
    return fract(sin(n)*43758.5453123);
}

vec3 forwardSF( float i, float n ){
    const float PI  = 3.141592653589793238;
    const float PHI = 1.618033988749894848;
    float phi = 2.0*PI*fract(i/PHI);
    float zi = 1.0 - (2.0*i+1.0)/n;
    float sinTheta = sqrt( 1.0 - zi*zi);
    return vec3( cos(phi)*sinTheta, sin(phi)*sinTheta, zi);
}


float calcAO( in vec3 pos, in vec3 nor ){
    float ao = 0.0;
    for( int i=0; i<32; i++ ){
        vec3 ap = forwardSF( float(i), 32.0 );
        float h = hash1(float(i));
        ap *= sign( dot(ap,nor) ) * h*0.1;
        ao += clamp( sceneSDFdist( pos + nor*0.01 + ap )*3.0 , 0.0, 1.0 );
    }
    ao /= 32.0;

    return clamp( ao*6.0, 0.0, 1.0 );
}

void main(){
    if (drawSDFSCENE){ // SDF SCENE VIEW
        vec2 uv = (texCoord - 0.5) * 2.0;
        vec3 rayDir = rayDirfromCam(invProjectionMatrix, invViewMatrix, uv);

        //hitresult hr = raymarchScene(cameraPosition, rayDir, 128, mindist, 128);
        hitresult hr = raytraceRootHitST(cameraPosition, rayDir, 128, mindist, 128);
        //material nMaterial = getMaterial(hr.materialIndex, hr.uv, 0);
        //linearizeDepth(hr.distance);
        //odirect.rgb = hr.normal;
        //odirect.rgb = vec3(rand(vec2(hr.materialIndex, 0.0)), rand(vec2(hr.materialIndex, 1.0)), rand(vec2(hr.materialIndex, 2.0))); // mat id
        //odirect.rgb = vec3(hr.iterationsDBG, 0.0, 0.0);
        //odirect.rgb = vec3(vec2(hr.uv), 0.0);
        odirect.rgb = textureLod(lSDFS[hr.materialIndex].texture_diffuse_Handle, hr.uv, 0).rgb;
        return;
    }
    

    float gdepth = texture2D(depthMap, texCoord).r;
    if (gdepth >= 0.99999) discard;
    
    //vec3 gp = texture(gPosition, texCoord).xyz;
    vec3 gp = WorldPosFromDepth(gdepth, invProjectionMatrix, invViewMatrix);
    vec3 gnrm = normalize(texture(gNormal, texCoord).xyz);
    vec3 galbedo = texture(gAlbedoSpec, texCoord).xyz;
    //vec3 gemission = texture(gEmission, texCoord).xyz;
    vec3 garm = texture(gSpecular, texCoord).xyz;
    //odirect.rgb = gp2;
    //return;
    
    if (forceMirror) {garm.g = 0.0; garm.b = 1.0;}
    float noise = blueNoise();
    // final lighting

    //float ao = 1.0;
    vec3 origin = gp + gnrm * originEplison;
    //if (!discardThresholdFloat(noise, ao_noiseThreshold) && aosEnabled) ao = calcAO(origin, gnrm);
    vec3 shadow = vec3(0.0f);
    shadow = direcLight(garm, gnrm, gp, doContactShadows).rgb; // true false
    //shadow += directAmbient;
    //shadow *=  ao;
    shadow = clamp(shadow, 0.0, 1.0);
    
    indirectChannels nIC;
    if (!discardThresholdFloat(noise, i_noiseThreshold) ) indirectAndEmissionMarch(garm, gnrm, origin, i_maxdist, mindist, i_steps, i_samples, nIC);
    if (!discardThresholdFloat(noise, r_noiseThreshold) ) nIC.specular = reflection(garm, gnrm, origin, reflect(normalize(gp - cameraPosition),  gnrm), r_maxdist, mindist, r_steps, r_bounces, r_samples, r_doSSR);

    nIC.specular = clamp(nIC.specular, 0.0, 1.0);
    
    odirect = shadow;
    ospecular = nIC.specular;

    vec3 null = vec3(1.0f, 0.0f, 0.0f);
    nIC.idirectSpecular.rgb = nIC.specular; // nIC.specular null
    nIC.emissionSpecular.rgb = null;
    
    // temporal accumulation here
    highp vec2 velocity = texture(gVelocity, texCoord).rg;

    bool split = true;
    if (gl_FragCoord.x > screenSize.x / 2 && doDenoiseSplitDBGView) split = false;
    
    if (doTemporalAccumulation && split){
        float d = linearizeDepth(texture2D(depthMap, texCoord).r, NearPlane, FarPlane);
        highp vec2 historyTexCoord = texCoord - velocity;

        indirectChannels nNIC =  temporalAccumulate(nIC, historyTexCoord, d, gp, garm.g);
        
        oindirect = nNIC.indirect;
        oemission = nNIC.emission;
        oindirectSpecular = nNIC.idirectSpecular;
        oemissionSpecular = nNIC.emissionSpecular;

        return;
    }
    
    oindirect = nIC.indirect;
    oemission = nIC.emission;
    oindirectSpecular = nIC.idirectSpecular;
    oemissionSpecular = nIC.emissionSpecular;

}
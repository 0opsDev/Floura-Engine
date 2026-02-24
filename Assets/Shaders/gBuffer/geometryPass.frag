#version 460 core

#extension GL_ARB_gpu_shader_int64 : enable
#extension GL_ARB_bindless_texture : require

layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gAlbedo;
layout(location = 3) out vec4 gSpecular;
layout(location = 4) out vec4 gVelocity;

in highp vec3 crntPos;
in vec3 Normal;
in vec3 color;
in vec2 texCoord;

in vec3 Normal0;
in vec3 Tangent0;
in vec3 Bitangent0;

in vec4 currentPos;
in vec4 previousPos;

//uint64_t
// these need to move to bindless
uniform uint64_t texture_diffuse_Handle;
uniform uint64_t texture_roughness_Handle;
uniform uint64_t texture_normal_Handle;
//uniform sampler2D noiseMapTexture;

uniform uint64_t BlueNoiseHandle;
uniform uint64_t bayerMatrixHandle;
uniform int frame;

uniform vec2 currentJitter;
uniform vec2 previousJitter;
uniform vec2 scaledCurrentJitter;
uniform vec2 scaledPreviousJitter;

uniform float time;

vec3 CalcNewNormal()
{
	//	return normalize(Normal); 
	// texture
	//vec3 normalTex = texture(texture_normal0, texCoord).xyz;

	sampler2D nSamp = sampler2D(texture_normal_Handle);

	vec3 normalTex = normalize(texture(nSamp, texCoord).xyz * 2.0f - 1.0f);

	// transform from 0,1 to -1, 1
	//normalTex = 2.0 * normalTex - vec3(1.0);

	// normalize tangent space vector
	vec3 nNormal = normalize(Normal0);
	vec3 nTangent = normalize(Tangent0);
	vec3 nBitangent = normalize(Bitangent0);

	// make the tbn 
	mat3 nTBN = mat3(nTangent, nBitangent, nNormal);

	vec3 newNormal = normalize(nTBN * normalTex);

	return newNormal;
}

float random(vec3 seed) {

	vec4 seed4 = vec4(seed, 1.0);
	float dot_product = dot(seed4, vec4(12.9898, 78.233, 45.164, 94.673));
	return fract(sin(dot_product) * 43758.5453);
}

// looks best on decals and foliage
void blueNoiseOpacity(float Threshold) // for fade out or opacity (cheap) (could fade out near farplane or nearplane)
{
	sampler2D bluemap =sampler2D(BlueNoiseHandle) ;
	vec2 texSize = vec2(textureSize(bluemap, 0));
	
	// uv
	vec2 offset = vec2(fract(frame * 0.618), fract(frame * 0.133));
	vec2 noiseUV = (gl_FragCoord.xy / texSize) + offset;
	
	float noise = texture(bluemap, noiseUV).r;

	// normal ranges should be 0.0f-1.0f;
	if (noise > Threshold) discard;
}

// looks best on glass and solids
void BayerNoiseOpacity(float Threshold) // for fade out or opacity (cheap) (could fade out near farplane or nearplane)
{
	sampler2D baySamp = sampler2D(bayerMatrixHandle);
	vec2 bayUV = vec2(gl_FragCoord.xy) / vec2(textureSize(baySamp, 0)); // new uvec2
	
	float scrollSpeed = 0.5;
	
	bayUV = fract(bayUV + (scrollSpeed * time));
	
	float bayer = texture(baySamp, bayUV).r;
	

	float clampedThreshold = clamp(Threshold, 0.2, 1.0);

	// normal ranges should be 0.0f-1.0f;
	if (bayer > Threshold || Threshold <= 0) discard;
}

void main()
{
	//return;
	
	sampler2D aSamp = sampler2D(texture_diffuse_Handle);
	sampler2D nSamp = sampler2D(texture_normal_Handle);
	sampler2D sSamp = sampler2D(texture_roughness_Handle);

	//highp vec2 currentNDC = currentPos.xy / (currentPos.w + 1e-7);
	//highp vec2 previousNDC = previousPos.xy / (previousPos.w + 1e-7);
	//highp vec2 currentNDC = currentPos.xy / (currentPos.w + 1e-6);
	//highp vec2 previousNDC = previousPos.xy / (previousPos.w + 1e-6);
	highp vec2 currentNDC = currentPos.xy / (currentPos.w);
	highp vec2 previousNDC = previousPos.xy / (previousPos.w);
	//vec2 velocity = (currentNDC - previousNDC) * 0.5;
	highp vec2 velocity = (currentNDC + scaledCurrentJitter) - (previousNDC + scaledPreviousJitter);
	
	// elipson
	//	if (length(velocity) < 0.001)
	if (length(velocity) < 0.001) { velocity = vec2(0.0); }

	//scaledCurrentJitter
	//scaledPreviousJitter
	//CurrentJitter
	//PreviousJitter

	gVelocity = vec4(velocity * 0.5, 0.0, 1.0);
	
	vec4 albedoTex = texture(aSamp, texCoord);
    // Discard fragment if alpha is too low
    if (albedoTex.a <= 0.0) // Adjust threshold if needed
    discard;

	blueNoiseOpacity(albedoTex.a);
	//BayerNoiseOpacity(albedoTex.a);

	gPosition = crntPos; // Output position as-is
    float displacement = texture(nSamp, texCoord).a; // Fetch normal from texture

	gNormal.rgb = CalcNewNormal();
    
	gNormal.a = displacement;

    // Assign Albedo RGB from texture
    //gAlbedoSpec.rgb = texture(diffuse0, texCoord).rgb * (texture(noiseMapTexture, texCoord) * 5).rgb;
	gAlbedo = albedoTex;

	//gSpecular.rgb = vec3(1.0f, 0.0f, 0.0f);
	gSpecular = texture(sSamp, texCoord);
	//gVelocity = vec4(vec3(1.0, 0.0, 0.0), 1.0);
}
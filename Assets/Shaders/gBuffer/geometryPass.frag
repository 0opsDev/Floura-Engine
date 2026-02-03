#version 460 core

#extension GL_ARB_gpu_shader_int64 : enable
#extension GL_ARB_bindless_texture : require

layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gAlbedoSpec;
layout(location = 4) out vec4 gSpecular;

in vec3 crntPos;
in vec3 Normal;
in vec3 color;
in vec2 texCoord;

in vec3 Normal0;
in vec3 Tangent0;
in vec3 Bitangent0;


//uint64_t
// these need to move to bindless
uniform uint64_t texture_diffuse_Handle;
uniform uint64_t texture_roughness_Handle;
uniform uint64_t texture_normal_Handle;
//uniform sampler2D noiseMapTexture;

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


void main()
{

	sampler2D aSamp = sampler2D(texture_diffuse_Handle);
	sampler2D nSamp = sampler2D(texture_normal_Handle);
	sampler2D sSamp = sampler2D(texture_roughness_Handle);

    vec4 albedoTex = texture(aSamp, texCoord);
    // Discard fragment if alpha is too low
    if (albedoTex.a < 0.1) // Adjust threshold if needed
    discard;

    gPosition = crntPos; // Output position as-is
    float displacement = texture(nSamp, texCoord).a; // Fetch normal from texture

	gNormal.rgb = CalcNewNormal();
    
	gNormal.a = displacement;

    // Assign Albedo RGB from texture
    //gAlbedoSpec.rgb = texture(diffuse0, texCoord).rgb * (texture(noiseMapTexture, texCoord) * 5).rgb;
    gAlbedoSpec = albedoTex;

	//gSpecular.rgb = vec3(1.0f, 0.0f, 0.0f);
	gSpecular = texture(sSamp, texCoord);
}
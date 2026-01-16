#version 460 core

layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gAlbedoSpec;

in vec3 crntPos;
in vec3 Normal;
in vec3 color;
in vec2 texCoord;

in vec3 Normal0;
in vec3 Tangent0;
in vec3 Bitangent0;

uniform sampler2D texture_diffuse0;
uniform sampler2D texture_roughness0;
uniform sampler2D texture_normal0;
//uniform sampler2D noiseMapTexture;

vec3 CalcNewNormal()
{
	//	return normalize(Normal); 
	// texture
	//vec3 normalTex = texture(texture_normal0, texCoord).xyz;

	vec3 normalTex = normalize(texture(texture_normal0, texCoord).xyz * 2.0f - 1.0f);

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
    vec4 albedoTex = texture(texture_diffuse0, texCoord);
    // Discard fragment if alpha is too low
    if (albedoTex.a < 0.1) // Adjust threshold if needed
    discard;

    gPosition = crntPos; // Output position as-is
    vec3 gNormalTex = texture(texture_normal0, texCoord).rgb; // Fetch normal from texture

	gNormal = CalcNewNormal();
    

    // Assign Albedo RGB from texture
    //gAlbedoSpec.rgb = texture(diffuse0, texCoord).rgb * (texture(noiseMapTexture, texCoord) * 5).rgb;
    gAlbedoSpec.rgb = albedoTex.rgb;

    // Ensure alpha is correctly fetched
    gAlbedoSpec.a = texture(texture_roughness0, texCoord).r;
}
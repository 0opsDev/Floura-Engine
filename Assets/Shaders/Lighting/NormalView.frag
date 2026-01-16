#version 460 core

// Outputs colors in RGBA
out vec4 FragColor;

// Imports the current position from the Vertex Shader
in vec3 crntPos;
// Imports the normal from the Vertex Shader
in vec3 Normal;
// Imports the color from the Vertex Shader
in vec3 color;
// Imports the texture coordinates from the Vertex Shader
in vec2 texCoord;

in vec3 Normal0;
in vec3 Tangent0;
in vec3 Bitangent0;

uniform sampler2D texture_normal0;

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
	FragColor = vec4(vec3(CalcNewNormal()), 1.0f);
}
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
uniform float DepthDistance;
uniform float FarPlane;
uniform float NearPlane;
uniform sampler2D diffuse0;

//float near = 0.1f;
//float far = 100.0f;

float linearizeDepth(float depth)
{
	return (2.0 * NearPlane * FarPlane) / (FarPlane + NearPlane - (depth * 2.0 - 1.0) * (FarPlane - NearPlane));
}

	vec4 diffuseColor = texture(diffuse0, texCoord);
void main()
{
	if (diffuseColor.a < 0.1)
	discard;
	FragColor = vec4(vec3(linearizeDepth(gl_FragCoord.z) / DepthDistance), 1.0f);
}

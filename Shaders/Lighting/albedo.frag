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

// Gets the Texture Units from the main function
uniform sampler2D diffuse0;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

uniform float gamma;

vec3 unShaded()
{
	vec4 diffuseColor = texture(diffuse0, texCoord);
	vec3 albedo = texture(gAlbedoSpec, texCoord).rgb;

	diffuseColor.rgb = pow(diffuseColor.rgb, vec3(gamma)); // Correct the gamma

	if (diffuseColor.a < 0.1)
	discard;

    return albedo;
	
}
void main()
{
    FragColor = vec4(unShaded(), 1.0f);
	// Apply final gamma correction before output
    FragColor.rgb = pow(FragColor.rgb, vec3(1.0 / gamma)); 
}

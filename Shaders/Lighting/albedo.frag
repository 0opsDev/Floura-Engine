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

uniform float gamma;

vec4 unShaded()
{
	vec4 diffuseColor = texture(diffuse0, texCoord);
	diffuseColor.rgb = pow(diffuseColor.rgb, vec3(gamma)); // Correct the gamma

	if (diffuseColor.a < 0.1)
	discard;

    return diffuseColor;
	
}
void main()
{
    FragColor = unShaded();
	// Apply final gamma correction before output
    FragColor.rgb = pow(FragColor.rgb, vec3(1.0 / gamma)); 
}

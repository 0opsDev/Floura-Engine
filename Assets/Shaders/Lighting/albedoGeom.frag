#version 460 core

// Outputs colors in RGBA
out vec4 FragColor;
// Imports the normal from the Vertex Shader
in vec3 Normal;
// Imports the color from the Vertex Shader
in vec3 color;
// Imports the texture coordinates from the Vertex Shader
in vec2 texCoord;

// Gets the Texture Units from the main function
uniform sampler2D diffuse0;

// Gets the position of the camera from the main function
uniform vec3 camPos;
uniform float NearPlane;
uniform float FarPlane;

vec4 lights(){
	vec4 diffuseTex = texture(diffuse0, texCoord);
	vec4 finalColour = vec4(0.0);
		return (diffuseTex) + finalColour;
} 

void main()
{
	FragColor = lights();
}
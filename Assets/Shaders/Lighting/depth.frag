#version 460 core

// Outputs colors in RGBA
out vec4 FragColor;
// Imports the normal from the Vertex Shader
in vec3 Normal;
// Imports the color from the Vertex Shader
in vec3 color;
// Imports the texture coordinates from the Vertex Shader
in vec2 texCoord;

float near = 0.1; 
float far  = 200.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));
}


void main()
{
    float depth = LinearizeDepth(gl_FragCoord.z) / far;
	FragColor = vec4(vec3(depth), 1.0);
}
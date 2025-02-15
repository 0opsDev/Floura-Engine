#version 330 core

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
//determines the loaded shader
uniform int ShaderNumber;
// Gets the Texture Units from the main function
uniform sampler2D diffuse0;
uniform sampler2D specular0;
uniform sampler2D unshaded0;
// Gets the color of the light from the main function
uniform vec4 lightColor;
uniform vec3 fogColor;
// Gets the position of the camera from the main function
uniform vec3 camPos;
//cones size and intens
uniform int doReflect;
uniform int isUnshaded0;
uniform int doFog;

//color of light from sky
uniform vec4 skyColor;
vec4 unShaded()
{
	vec4 diffuseColor = texture(diffuse0, texCoord);
	float specularColor = texture(specular0, texCoord).r;
	vec4 unshadedColor = texture(unshaded0, texCoord) * skyColor;

    // If the unshaded0 texture is being used, return its color directly
    if (isUnshaded0 == 1)
    {
        return unshadedColor;
    }
	// ambient lighting
	float ambient = 0.20f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(vec3(1.0f, 1.0f, 0.0f));
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;
	switch (doReflect){
	case 0:
	return (diffuseColor) * skyColor;
	break;
	case 1:
	return (diffuseColor + specularColor * specular) * skyColor;
	break;
	case 2:
	return (specularColor * specular) * skyColor;
	break;
	}
	
}
float near = 0.1f;
float far = 100.0f;

float linearizeDepth(float depth)
{
	return (2.0 * near * far) / (far + near - (depth * 2.0 - 1.0) * (far - near));
}

//offset is distance from camera
float logisticDepth(float depth, float steepness = 0.1f, float offset = 100.0f)
{
	float zVal = linearizeDepth(depth);
	return (1 / (1 + exp(-steepness * (zVal - offset))));
}


void main()
{
switch (doFog)
{
	case 0:{
	FragColor = unShaded();
	break;
	}
	case 1:{
	float depth = logisticDepth(gl_FragCoord.z);
	FragColor = unShaded() * (1.0f - depth) + vec4(depth * vec3(fogColor), 1.0f);
	break;
	}
}

}

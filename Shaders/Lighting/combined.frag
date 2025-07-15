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
uniform sampler2D specular0;

// Gets the color of the light from the main function
uniform vec4 skyColor;
// Gets the position of the light from the main function
const vec3 lightPos = vec3(0.0, 5.0, 1.0);
uniform vec3 lightPos2[32];
uniform vec3 colour2[32];
uniform int sizeOfLights;
uniform vec2 radiusAndPower[32];
uniform int lightType[32];
uniform bool enabled[32];

// Gets the position of the camera from the main function
uniform vec3 camPos;

vec4 direcLight()
{
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

	return (texture(diffuse0, texCoord) * (diffuse + ambient) + texture(specular0, texCoord).r * specular);
}

vec4 pointLight(int iteration)
{	
	vec4 finalColour;

	// used in two variables so I calculate it here to not have to do it twice
	vec3 lightVec = lightPos2[iteration] - crntPos;

	// intensity of light with respect to distance
	float dist = length(lightVec);
	float a = 3.0;
	float b = 0.7;
	float inten = radiusAndPower[iteration].x / (a * dist * dist + b * dist + 1.0);

	// ambient lighting
	float ambient = 0.0f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightVec);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	finalColour = finalColour + (texture(diffuse0, texCoord) * (diffuse * inten + ambient) + texture(specular0, texCoord).r * specular * inten) * vec4(colour2[iteration], inten);

	return finalColour;
}

vec4 spotLight(int iteration)
{
	// controls how big the area that is lit up is
	float outerCone = 0.90f;
	float innerCone = 0.95f;

	// ambient lighting
	float ambient = 0.0f;

	vec4 finalColour;

		// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightPos2[iteration] - crntPos);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	// calculates the intensity of the crntPos based on its angle to the center of the light cone
	float angle = dot(vec3(0.0f, -1.0f, 0.0f), -lightDirection);
	float inten = clamp( (angle - outerCone) / (innerCone - outerCone), 0.0f, 1.0); 

	finalColour = finalColour + (texture(diffuse0, texCoord) * (diffuse * inten + ambient) + texture(specular0, texCoord).r * specular * inten) * vec4(colour2[iteration], inten);

	//finalColour = finalColour + (diffuse * inten + skyColor);

	return finalColour; 
}

vec4 lights(){

	vec4 finalColour;

	for (int i = 0; i < sizeOfLights; i++){
		if (enabled[i]){
			if (lightType[i] == 0){
			finalColour = finalColour + spotLight(i);
			}

			if (lightType[i] == 1){
			finalColour = finalColour + pointLight(i);
			}
		} 

	}

	return ((texture(diffuse0, texCoord) * skyColor) + finalColour);
} 


void main()
{
	// outputs final color
	//FragColor = direcLight();
	//FragColor = spotLight();
	FragColor = lights();
}
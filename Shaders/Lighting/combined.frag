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

uniform bool doReflect;
float specularLight = 0.50f; // 0.50f

// Gets the Texture Units from the main function
uniform sampler2D diffuse0;
uniform sampler2D specular0;

// Gets the color of the light from the main function
uniform vec4 skyColor;
// Gets the position of the light from the main function
const vec3 lightPos = vec3(0.0, 5.0, 1.0);

uniform vec3 directLightPos; //1.0f, 1.0f, 0.0f
uniform vec3 directLightCol;
uniform float directAmbient; // 0.20f
uniform float dirSpecularLight;
uniform bool doDirLight;
uniform bool doDirSpecularLight;

struct Light
{
	vec3 position;
	vec3 rotation;
	vec3 colour;
	float radius;
	int type;
};

uniform Light Lights[64];

uniform int lightCount;

// Gets the position of the camera from the main function
uniform vec3 camPos;
uniform float NearPlane;
uniform float FarPlane;

float linearizeDepth(float depth)
{
	return (2.0 * NearPlane * FarPlane) / (FarPlane + NearPlane - (depth * 2.0 - 1.0) * (FarPlane - NearPlane));
}

vec4 direcLight()
{ // normals need to be recalculated based on rotation
	// ambient lighting
	//float ambient = 0.20f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(directLightPos); //vec3(1.0f, 1.0f, 0.0f)
	float diffuse = max(dot(normal, lightDirection), 0.0f);
	if (doReflect && doDirSpecularLight){
	// specular lighting
	//float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 32);
	float specular = specAmount * dirSpecularLight;

	return (texture(diffuse0, texCoord) * (diffuse + directAmbient) + texture(specular0, texCoord).r * specular) * vec4(directLightCol, 1.0f);
	}
	else{
	return (texture(diffuse0, texCoord) * (diffuse + directAmbient)) * vec4(directLightCol, 1.0f);
	}
}

vec4 pointLight(int iteration)
{	
	vec4 finalColour = vec4(0.0f);

	// used in two variables so I calculate it here to not have to do it twice
	vec3 lightVec = Lights[iteration].position - crntPos;

	// intensity of light with respect to distance
	float dist = length(lightVec);
	float a = 3.0;
	float b = 0.7;
	float inten = Lights[iteration].radius / (a * dist * dist + b * dist + 1.0);

	// ambient lighting
	//float ambient = 0.0f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightVec);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	if (doReflect){
	// specular lighting
		//float specularLight = 0.50f;
		vec3 viewDirection = normalize(camPos - crntPos);
		vec3 reflectionDirection = reflect(-lightDirection, normal);
		float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.1f), 16);
		float specular = specAmount * specularLight;

		finalColour = finalColour + (texture(diffuse0, texCoord) * (diffuse * inten + 0.0f) + texture(specular0, texCoord).r * specular * inten) * vec4(Lights[iteration].colour, 1.0 ) * inten;
	}
	else{
		finalColour = finalColour + (texture(diffuse0, texCoord) * (diffuse * inten + 0.0f) * vec4(Lights[iteration].colour, 1.0) * inten);
		//finalColour = finalColour + (texture(diffuse0, texCoord) * (diffuse * inten + ambient) * vec4(Lights[iteration].colour, 1.0) * inten);
	}

	return finalColour;
}

vec4 spotLight(int iteration)
{
	// controls how big the area that is lit up is
	float outerCone = 0.90f;
	float innerCone = 0.95f;

	// ambient lighting
	//float ambient = 0.0f;

	vec4 finalColour = vec4(0.0f);

		// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(Lights[iteration].position - crntPos);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// calculates the intensity of the crntPos based on its angle to the center of the light cone
	float angle = dot(Lights[iteration].rotation, -lightDirection); // direction
	float inten = clamp( (angle - outerCone) / (innerCone - outerCone), 0.0f, 1.0); 

	if (doReflect){

		// specular lighting
	//float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	finalColour = finalColour + (texture(diffuse0, texCoord) * (diffuse * inten + 0.0f) + texture(specular0, texCoord).r * specular * inten) * vec4(Lights[iteration].colour, 1.0) * inten;

	}
	else{
	
	finalColour = finalColour + (texture(diffuse0, texCoord) * (diffuse * inten + 0.0f) * vec4(Lights[iteration].colour, 1.0) * inten);
	//	finalColour = finalColour + (texture(diffuse0, texCoord) * (diffuse * inten + ambient) * vec4(Lights[iteration].colour, 1.0) * inten);
	}

	//finalColour = finalColour + (diffuse * inten + skyColor);

	return finalColour; 
}

vec4 lights(){
	vec4 diffuseTex = texture(diffuse0, texCoord);
	vec4 finalColour = vec4(0.0);

	//early cutoff
	if (diffuseTex.a < 0.1)
	discard;

	if (linearizeDepth(gl_FragCoord.z) >= FarPlane) {
	discard;
    //return (diffuseTex * skyColor);
	}
	int maxLights = 64;
	for (int i = 0; i < min(lightCount, maxLights); i++)
		{
			if (Lights[i].type == 0){
			finalColour += spotLight(i);
			}

			if (Lights[i].type == 1){
			finalColour += pointLight(i);
		}

	}
	//return finalColour;
		//FragColor = direcLight(); doDirLight

	if (doDirLight) // if direct light is enabled, add it to the final color
	{
		finalColour += direcLight();
	}

		return (diffuseTex * skyColor) + finalColour;
} 

void main()
{
	FragColor = lights();
}
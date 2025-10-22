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

in vec4 fragPosLight;

//TBN
in vec3 Normal0;
in vec3 Tangent0;
in vec3 Bitangent0;

uniform bool doReflect;
float specularLight = 0.50f; // 0.50f

// Gets the Texture Units from the main function
uniform sampler2D texture_diffuse0;
uniform sampler2D texture_roughness0;
uniform sampler2D texture_normal0;
uniform sampler2D shadowMap;

// Gets the color of the light from the main function
uniform vec4 skyColor;
// Gets the position of the light from the main function
//const vec3 lightPos = vec3(0.0, 5.0, 1.0);
uniform vec3 directLightPos;
uniform vec3 directLightCol;
uniform float directAmbient; // 0.20f
uniform float dirSpecularLight;
uniform bool doDirLight;
uniform bool doDirSpecularLight;
uniform bool doDirShadowMap;
uniform int dirShadowMapHardness;
uniform float DirSMMaxBias;

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

uniform float NearPlane;
uniform float FarPlane;
uniform vec3 camPos;

float linearizeDepth(float depth)
{
	return (2.0 * NearPlane * FarPlane) / (FarPlane + NearPlane - (depth * 2.0 - 1.0) * (FarPlane - NearPlane));
}

vec3 CalcNewNormal()
{
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

vec4 direcLight()
{ // normals need to be recalculated based on rotation

	vec3 normal = CalcNewNormal();

	vec3 lightDirection = normalize(directLightPos); //vec3(1.0f, 1.0f, 0.0f)
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// shadow map 
	float shadow = 0.0f;
	vec3 lightCoords = fragPosLight.xyz / fragPosLight.w;
	if (lightCoords.z <= 1.0f && doDirShadowMap)
	{
		lightCoords = (lightCoords + 1.0f) / 2.0f;

		//float closestDepth = texture(shadowMap, lightCoords.xy).r;
		float currentDepth = lightCoords.z;

		//float bias = 0.005f; // 0.025f
		float bias = max(DirSMMaxBias * (1.0f - dot(normal, lightDirection)), 0.0005f);

		int sampleRadius = dirShadowMapHardness;
		vec2 pixelSize = 1.0 / textureSize(shadowMap, 0);
		for(int y = -sampleRadius; y <= sampleRadius; y++)
		{
		    for(int x = -sampleRadius; x <= sampleRadius; x++)
		    {
		        float closestDepth = texture(shadowMap, lightCoords.xy + vec2(x, y) * pixelSize).r;
				if (currentDepth > closestDepth + bias)
					shadow += 1.0f;     
		    }    
		}
		shadow /= pow((sampleRadius * 2 + 1), 2);

	
	}
		float specular = 0.0f;
	if (doReflect && doDirSpecularLight && diffuse != 0.0f){
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);

	vec3 halfwayVec = normalize(lightDirection + viewDirection);

	float specAmount = pow(max(dot(normal, halfwayVec), 0.0f), 32);
	specular = specAmount * dirSpecularLight;
	return (texture(texture_diffuse0, texCoord) * (diffuse + (1.0f - shadow) + directAmbient) + texture(texture_roughness0, texCoord).r * specular * (1.0f - shadow)) * vec4(directLightCol, 1.0f);
	}
	else{
	return (texture(texture_diffuse0, texCoord) * (diffuse + (1.0f - shadow) + directAmbient)) * vec4(directLightCol, 1.0f);
	}
}

vec4 pointLight(int iteration)
{	
	vec4 finalColour = vec4(0.0f);

	
	//vec3 lightVec = (Lights[iteration].position) - crntPos;
	vec3 lightVec = (Lights[iteration].position) - crntPos;

	// intensity of light with respect to distance
	float dist = length(lightVec);
	float a = 1.00f;
	float b = 0.70f;
	float inten = Lights[iteration].radius / (a * dist * dist + b * dist + 1.0);

	// ambient lighting
	//float ambient = 0.0f;

	// diffuse lighting
	//vec3 NormalTex = texture(normal0, texCoord).rgb; // Fetch normal from texture

    //vec3 normal = normalize(NormalTex * Normal);

	vec3 normal = normalize(Normal); 

	//vec3 unpackedNormal = normalize(texture(normal0, texCoord).xyz * 2.0f - vec3(1.0f));
	//vec3 normal = normalize(TBN * unpackedNormal);

	vec3 lightDirection = normalize(lightVec);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	float specular = 0.0f;
	if (doReflect && diffuse != 0.0f){
	// specular lighting
		//float specularLight = 0.50f;
		vec3 viewDirection = normalize( (camPos) - crntPos);
		vec3 reflectionDirection = reflect(-lightDirection, normal);

		vec3 halfwayVec = normalize(lightDirection + viewDirection);

		float specAmount = pow(max(dot(normal, halfwayVec), 0.1f), 16);
		specular = specAmount * specularLight;

		finalColour = finalColour + (texture(texture_diffuse0, texCoord) * (diffuse * inten + 0.0f) + texture(texture_roughness0, texCoord).r * specular * inten) * vec4(Lights[iteration].colour, 1.0 ) * inten;
	}
	else{
		finalColour = finalColour + (texture(texture_diffuse0, texCoord) * (diffuse * inten + 0.0f) * vec4(Lights[iteration].colour, 1.0) * inten);
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
	//vec3 unpackedNormal = normalize(texture(normal0, texCoord).xyz * 2.0f - vec3(1.0f));
	//vec3 normal = normalize(TBN * unpackedNormal);
	vec3 normal = normalize(Normal);
	//vec3 normal = normalize(texture(normal0, texCoord).xyz * 2.0f - 1.0f);

	vec3 lightDirection = normalize(Lights[iteration].position - crntPos);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// calculates the intensity of the crntPos based on its angle to the center of the light cone
	float angle = dot(Lights[iteration].rotation, -lightDirection); // direction
	float inten = clamp( (angle - outerCone) / (innerCone - outerCone), 0.0f, 1.0); 

	float specular = 0.0f;
	if (doReflect && diffuse != 0.0f){

		// specular lighting
	//float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);

	vec3 halfwayVec = normalize(lightDirection + viewDirection);

	float specAmount = pow(max(dot(normal, halfwayVec), 0.0f), 16);
	specular = specAmount * specularLight;

	finalColour = finalColour + (texture(texture_diffuse0, texCoord) * (diffuse * inten + 0.0f) + texture(texture_roughness0, texCoord).r * specular * inten) * vec4(Lights[iteration].colour, 1.0) * inten;

	}
	else{
	
	finalColour = finalColour + (texture(texture_diffuse0, texCoord) * (diffuse * inten + 0.0f) * vec4(Lights[iteration].colour, 1.0) * inten);
	//	finalColour = finalColour + (texture(diffuse0, texCoord) * (diffuse * inten + ambient) * vec4(Lights[iteration].colour, 1.0) * inten);
	}

	//finalColour = finalColour + (diffuse * inten + skyColor);

	return finalColour; 
}

vec4 lights(){
	vec4 diffuseTex = texture(texture_diffuse0, texCoord);
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
		//return finalColour;
		return (diffuseTex * skyColor) + finalColour;
} 

void main()
{
	FragColor = lights();
	//FragColor = texture(texture_diffuse0, texCoord);
	//FragColor = texture(texture_roughness0, texCoord);
	//FragColor = texture(texture_normal0, texCoord);
	
}
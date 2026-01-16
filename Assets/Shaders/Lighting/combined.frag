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
uniform sampler2D BlueNoiseTex;

// Gets the position of the light from the main function
//const vec3 lightPos = vec3(0.0, 5.0, 1.0);
uniform vec3 directLightPos;
uniform vec3 directLightCol;
uniform float directAmbient; // 0.20f
uniform float dirSpecularLight;
uniform bool doDirLight;
uniform bool doDirSpecularLight;
uniform bool doDirShadowMap;
uniform int FilterRadius;
uniform int NumberOfSamples;
uniform float DirSMMaxBias;
uniform float deltatime;
uniform float time;

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

uniform float FogNearPlane;
uniform float FogFarPlane;
uniform float DepthDistance;
uniform vec3 fogColour;
uniform bool doFog;

uniform float NearPlane;
uniform float FarPlane;
uniform vec3 camPositon;

// reflections
in vec3 reflectedVector;
in vec3 NviewVector;
uniform samplerCube skybox; 
uniform float smoothnessValue;

float linearizeDepth(float depth, float NP, float FP)
{
	return (2.0 * NP * FP) / (FP + NP - (depth * 2.0 - 1.0) * (FP - NP));
}

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

float random(vec3 seed) {

	vec4 seed4 = vec4(seed, 1.0);
	float dot_product = dot(seed4, vec4(12.9898, 78.233, 45.164, 94.673));
	return fract(sin(dot_product) * 43758.5453);
}

float CalcShadowFactorDIR(vec4 LightSpacePos, vec3 lightDirection, vec3 normal)
{
	// perform perspective divide
	vec3 lightCoords = LightSpacePos.xyz / LightSpacePos.w;
	float shadow = 0.0f;

	// shadow calculation
	if (lightCoords.z <= 1.0f)
	{

		// transform to [0,1] range
		lightCoords = (lightCoords + 1.0f) / 2.0f;
		// get the current depth
		float currentDepth = lightCoords.z;
		// calculate shadow bias
		float bias = max(DirSMMaxBias * (1.0f - dot(normal, lightDirection)), 0.0005f);
		// PCF
		int sampleRadius = FilterRadius; // FilterRadius // NumberOfSamples
		//vec2 pixelSize = (float(NumberOfSamples) * 0.1) / textureSize(shadowMap, 0);
		vec2 noiseUV = vec2(gl_FragCoord.xy) / vec2(textureSize(BlueNoiseTex, 0));
		vec2 pixelSize = 1.0 / textureSize(shadowMap, 0);
		for(int y = -sampleRadius; y <= sampleRadius; y++)
		{
		    for(int x = -sampleRadius; x <= sampleRadius; x++)
		    {
					float angle = texture(BlueNoiseTex, noiseUV).r * NumberOfSamples;
					vec2 offset = vec2(cos(angle), sin(angle));

					float closestDepth = texture(shadowMap, lightCoords.xy + (vec2(x, y) * offset) * pixelSize).r;
					if (currentDepth > closestDepth + bias)
						shadow += 1.0f;
     
		    }    
		}

		shadow /= pow((sampleRadius * 2 + 1), 2);


	}

return shadow;
}

vec4 direcLight()
{ // normals need to be recalculated based on rotation

	vec3 normal = CalcNewNormal();

	vec3 lightDirection = normalize(directLightPos); //vec3(1.0f, 1.0f, 0.0f)
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// shadow map 
	float shadow = 0.0f;

	if (doDirShadowMap)
	shadow = CalcShadowFactorDIR(fragPosLight, lightDirection, normal);
	// shadow map end

		float specular = 0.0f;
	if (doReflect && doDirSpecularLight && diffuse != 0.0f){
	vec3 viewDirection = normalize(camPositon - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);

	vec3 halfwayVec = normalize(lightDirection + viewDirection);

	float specAmount = pow(max(dot(normal, halfwayVec), 0.0f), 32);
	specular = specAmount * dirSpecularLight;

	return ((diffuse * (1.0f - shadow) + directAmbient) + texture(texture_roughness0, texCoord).r * specular * (1.0f - shadow)) * vec4(directLightCol, 1.0f); }
	else{ return ((diffuse * (1.0f - shadow) + directAmbient)) * vec4(directLightCol, 1.0f); }
}

vec4 pointLight(int iteration)
{	
	vec4 finalColour = vec4(0.0f);

	
	//vec3 lightVec = (Lights[iteration].position) - crntPos;
	vec3 lightVec = (Lights[iteration].position) - crntPos;

	// intensity of light with respect to distance
	float dist = length(lightVec);
	float a = 3.00f;
	float b = 0.70f;
	float inten = 1.0f / (a * dist * dist + b * dist + 1.0) * Lights[iteration].radius;

	// ambient lighting
	//float ambient = 0.0f;
	vec3 normal = CalcNewNormal();
	//vec3 normal = normalize(Normal); 

	vec3 lightDirection = normalize(lightVec);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	float specular = 0.0f;
	if (doReflect && diffuse != 0.0f){
	// specular lighting
		//float specularLight = 0.50f;
		vec3 viewDirection = normalize( (camPositon) - crntPos);
		vec3 reflectionDirection = reflect(-lightDirection, normal);

		vec3 halfwayVec = normalize(lightDirection + viewDirection);

		float specAmount = pow(max(dot(normal, halfwayVec), 0.1f), 16);
		specular = specAmount * specularLight;

		finalColour = finalColour + ((diffuse * inten + 0.0f) + texture(texture_roughness0, texCoord).r * specular * inten) * vec4(Lights[iteration].colour, 1.0 ) * inten;
	}
	else{
		finalColour = finalColour + ( (diffuse * inten + 0.0f) * vec4(Lights[iteration].colour, 1.0) * inten);
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
	vec3 normal = CalcNewNormal();

	vec3 lightDirection = normalize(Lights[iteration].position - crntPos);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// calculates the intensity of the crntPos based on its angle to the center of the light cone
	float angle = dot(Lights[iteration].rotation, -lightDirection); // direction
	float inten = clamp( (angle - outerCone) / (innerCone - outerCone), 0.0f, 1.0); 

	float specular = 0.0f;
	if (doReflect && diffuse != 0.0f){

		// specular lighting
	//float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPositon - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);

	vec3 halfwayVec = normalize(lightDirection + viewDirection);

	float specAmount = pow(max(dot(normal, halfwayVec), 0.0f), 16);
	specular = specAmount * specularLight;

	finalColour = finalColour + ((diffuse * inten + 0.0f) + texture(texture_roughness0, texCoord).r * specular * inten) * vec4(Lights[iteration].colour, 1.0) * inten;

	}
	else{
	
	finalColour = finalColour + ((diffuse * inten + 0.0f) * vec4(Lights[iteration].colour, 1.0) * inten);
	//	finalColour = finalColour + (texture(diffuse0, texCoord) * (diffuse * inten + ambient) * vec4(Lights[iteration].colour, 1.0) * inten);
	}

	//finalColour = finalColour + (diffuse * inten + skyColor);

	return finalColour; 
}

vec4 lights(){
	//vec4 diffuseTex = texture(texture_diffuse0, texCoord);
	vec4 finalColour = vec4(0.0);
    //return (diffuseTex * skyColor);
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

		///return vec4(finalColour.xyz, diffuseTex.a);
		return vec4(finalColour); // was xyz
		//return vec4((diffuseTex.xyz * skyColor.xyz) + finalColour.xyz, diffuseTex.a);
		//return (diffuseTex.xyz * skyColor.xyz) + finalColour.xyz;
} 

vec3 reflections()
{
	vec4 final;
	
	vec3 NreflectedVector = reflect(NviewVector, CalcNewNormal());

	float metalic = texture(texture_roughness0, texCoord).g; // metalic
	float smoothness = smoothnessValue * metalic;

	return texture(skybox, NreflectedVector).rgb; // * smoothness;

}

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898, 78.233))) * 43758.5453);
}

vec3 sampleHemisphere(vec3 normal, float random)
{
    float u = rand(vec2(gl_FragCoord.xy) + random); 
    float v = rand(vec2(u, random));

    float phi = 2.0 * 3.14159265 * u;
    float cosTheta = sqrt(1.0 - v);
    float sinTheta = sqrt(v);

    vec3 localDir = vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);

    vec3 helper = abs(normal.y) < 0.999 ? vec3(0, 1, 0) : vec3(1, 0, 0);
    vec3 tangent = normalize(cross(helper, normal));
    vec3 bitangent = cross(normal, tangent);

    return tangent * localDir.x + bitangent * localDir.y + normal * localDir.z;
}

vec3 indirectIBL(int samples)
{
	vec3 normal = CalcNewNormal();

	vec3 NreflectedVector = reflect(NviewVector, normal);

	vec3 indirectColour = vec3(0.0f);

	for (int i = 0; i < samples; i++)
	{
		vec3 randomDir = sampleHemisphere(normalize(NreflectedVector), i + time); // i thought it would be better to add time for a film grain look, it would also solve with taa

		indirectColour += texture(skybox, randomDir).rgb;
	}

	return indirectColour / samples;
}

void blueNoiseOpacity(float Threshold) // for fade out or opacity (cheap) (could fade out near farplane or nearplane)
{
	vec2 noiseUV = vec2(gl_FragCoord.xy) / vec2(textureSize(BlueNoiseTex, 0)); // new uvec2
	float noise = texture(BlueNoiseTex, noiseUV).r;

	// normal ranges should be 0.0f-1.0f;
	if (noise > Threshold) discard;
}

float logisticDepth(float depth, float steepness, float offset, float NearPlane, float FarPlane)
{
    float zVal = linearizeDepth(depth, NearPlane, FarPlane);
    float expVal = exp(clamp(-steepness * (zVal - offset), -10.0, 10.0));
    return 1.0 / (1.0 + expVal);
}

vec4 calculateFog(float near, float far, float depthDistance, vec3 Colour, vec4 originalImage)
{
	float linearizedFogDepth = linearizeDepth(gl_FragCoord.z, near, far);
	float logisticizedDepth = logisticDepth(gl_FragCoord.z, 0.1f, depthDistance, near, far);
	return originalImage * (1.0f - logisticizedDepth) + vec4(logisticizedDepth * vec4(Colour, 1.0f)); // fog
}

void main()
{
	float linearizedDepth = linearizeDepth(gl_FragCoord.z, NearPlane, FarPlane);
	//early z cutoff
	if (linearizedDepth > FarPlane)
	discard;


	float fadeDistance = 10.0;
	float distToFar = FarPlane - linearizedDepth;
	float farOpacity = distToFar / fadeDistance;
	farOpacity = clamp(farOpacity, 0.0, 1.0);


	blueNoiseOpacity(farOpacity);

	vec4 diffuseTex = texture(texture_diffuse0, texCoord);
	if (diffuseTex.a < 0.1)
	discard;

	vec3 specular = vec3(0.0f);
	if (doReflect)
	{
		specular = reflections();
	}
	vec3 direct = lights().rgb;

	vec3 indirect = indirectIBL(2);

	vec3 gi = (direct + indirect);

	// albedo * gi + spec + em
	vec4 final = diffuseTex * vec4(gi, 1.0f) + vec4(specular, 1.0f);

	if (doFog) final = calculateFog(FogNearPlane, FogFarPlane, DepthDistance, fogColour, final); // fog

	FragColor = final;
	//FragColor = vec4(specular , 1.0f);
	//FragColor = vec4(gi, 1.0f);
	//FragColor = vec4(indirect, 1.0f);
	//FragColor = vec4(direct, 1.0f);
	//FragColor = vec4(gi, 1.0f) + vec4(specular, 1.0f);
}
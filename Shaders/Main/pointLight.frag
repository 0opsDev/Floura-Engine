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
// Gets the position of the light from the main function
uniform vec3 lightPos;
// Gets the position of the camera from the main function
uniform vec3 camPos;
//cones size and intens
uniform vec3 InnerLight1;
//angle of spotlight
uniform vec3 spotLightRot;

// Uniforms to determine if the texture is unshaded
uniform int isUnshaded0;
uniform int isUnshaded1;
uniform int isUnshaded2;
uniform int isUnshaded3;

uniform int doReflect;
uniform int doFog;

//color of light from sky
uniform vec4 skyColor;
uniform float gamma;
uniform float DepthDistance;
uniform float FarPlane;
uniform float NearPlane;

vec4 pointLight()
{	
	vec4 diffuseColor = texture(diffuse0, texCoord);
	diffuseColor.rgb = pow(diffuseColor.rgb, vec3(gamma)); // Correct the gamma

	float specularColor = pow(texture(specular0, texCoord).r, 1.0 / gamma);
	// Sample the unshaded0 texture
    vec4 unshadedColor = texture(unshaded0, texCoord) * skyColor;

    // If the unshaded0 texture is being used, return its color directly
    if (isUnshaded0 == 1)
    {
        return unshadedColor;
    }
	// used in two variables so I calculate it here to not have to do it twice
	vec3 lightVec = lightPos - crntPos;

	// intensity of light with respect to distance
	float dist = length(lightVec);
	float a = 3.0;
	float b = 0.7;
	float inten = 1.0f / (a * dist * dist + b * dist + 1.0f);

	// ambient lighting
	float ambient = 0.20f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightVec);
	float diffuse = max(abs(dot(normal, lightDirection)), 0.0f);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	if (diffuseColor.a < 0.1)
	discard;

		switch (doReflect){
	case 0:
	return (diffuseColor * (diffuse * inten + ambient) + 0) * lightColor;
	break;
	case 1:
		return (diffuseColor * (diffuse * inten + ambient) + specularColor * specular * inten) * lightColor;
	break;
	}

}
//float near = 0.1f;
//float far = 100.0f;

float linearizeDepth(float depth)
{
	return (2.0 * NearPlane * FarPlane) / (FarPlane + NearPlane - (depth * 2.0 - 1.0) * (FarPlane - NearPlane));
}

//offset is distance from camera
float logisticDepth(float depth, float steepness, float offset)
{
    float zVal = linearizeDepth(depth);
    float expVal = exp(clamp(-steepness * (zVal - offset), -10.0, 10.0));
    return 1.0 / (1.0 + expVal);
}

void main()
{
    switch (doFog)
    {
        case 0:
        {
            FragColor = pointLight();
            break;
        }
        case 1:
        {
            float depth = logisticDepth(gl_FragCoord.z, 0.1f, DepthDistance);
            vec3 correctedFogColor = pow(fogColor, vec3(gamma)); // Apply gamma correction to fog
            FragColor = pointLight() * (1.0f - depth) + vec4(depth * correctedFogColor, 1.0f);
            break;
        }
    }
	// Apply final gamma correction before output
    FragColor.rgb = pow(FragColor.rgb, vec3(1.0 / gamma)); 
}

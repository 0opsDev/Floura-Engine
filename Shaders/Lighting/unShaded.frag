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
uniform sampler2D empty;
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
uniform float gamma;
uniform float DepthDistance;
uniform float FarPlane;
uniform float NearPlane;

vec4 unShaded()
{
	vec4 diffuseColor = texture(diffuse0, texCoord);
	diffuseColor.rgb = pow(diffuseColor.rgb, vec3(gamma)); // Correct the gamma

	float specularColor = pow(texture(specular0, texCoord).r, 1.0 / gamma);

    // If the unshaded0 texture is being used, return its color directly
	// ambient lighting
	float ambient = 0.20f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(vec3(1.0f, 1.0f, 0.0f));
	float diffuse = max(abs(dot(normal, lightDirection)), 0.0f);

	// specular lighting
	float specularLight = 0.1f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	if (diffuseColor.a < 0.1)
	discard;

	switch (doReflect){
	case 0:
	return (diffuseColor) * skyColor;
	break;
	case 1:
	return (diffuseColor + specularColor * specular) * skyColor;
	break;
	case 2:
	return (vec4(specularColor));
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
            FragColor = unShaded();
            break;
        }
        case 1:
        {
            float depth = logisticDepth(gl_FragCoord.z, 0.1f, DepthDistance);
            vec3 correctedFogColor = pow(fogColor, vec3(gamma)); // Apply gamma correction to fog
            FragColor = unShaded() * (1.0f - depth) + vec4(depth * correctedFogColor, 1.0f);
            break;
        }
    }
	// Apply final gamma correction before output
    FragColor.rgb = pow(FragColor.rgb, vec3(1.0 / gamma)); 
}

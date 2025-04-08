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

vec4 spotLight()
{
	vec4 diffuseColor = texture(diffuse0, texCoord);
	diffuseColor.rgb = pow(diffuseColor.rgb, vec3(gamma)); // Correct the gamma

	float specularColor = pow(texture(specular0, texCoord).r, gamma);
	//float specularColor = texture(specular0, texCoord).r;
    // Sample the unshaded0 texture
    vec4 unshadedColor = texture(unshaded0, texCoord) * skyColor;

    // If the unshaded0 texture is being used, return its color directly
    if (isUnshaded0 == 1)
    {
        return unshadedColor;
    }

	// controls how big the area that is lit up is
	float outerCone = InnerLight1.x;
	//0.95
	float innerCone = InnerLight1.y;

	// ambient lighting
	float ambient = 0.20f / -InnerLight1.z;
	//new 2025
	//float ambient = 0.20f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightPos - crntPos);
	float diffuse = max(abs(dot(normal, lightDirection)), 0.0f);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 256); //256 part is reflection size
	float specular = specAmount * specularLight;

	// calculates the intensity of the crntPos based on its angle to the center of the light cone
	float angle = dot(vec3(spotLightRot.x, spotLightRot.y, spotLightRot.z), -lightDirection);
	float inten = clamp((angle - outerCone) / (innerCone - outerCone), (0.0f), (0.0f + (InnerLight1.z)) );

	if (diffuseColor.a < 0.1)
	discard;
	//skyColor
	//(inten * lightColor) life saver
	// first part intensity, second part removes too brighht, third part makes sure inten wont invert
	//real life saver ((inten * lightColor ) - (inten * skyColor) * (lightColor) )                                                            doesnt add color it adds brightness         the number we take needs to be pos
	//																																				adds specular part

	vec4 finalColor;
	switch (doReflect){
	case 0:
		finalColor = diffuseColor *  ( (diffuse) * ((inten * lightColor ) - (inten * skyColor)) + (skyColor + (ambient)  ) );
		break;
	case 1:
		finalColor = diffuseColor *  ( (diffuse) * ((inten * lightColor ) - (inten * skyColor)) + (skyColor + (ambient)  ) ) + specularColor * specular * inten;
		break;
	}

	// Ensure alpha is set to 1
	finalColor.a = 1.0;
	return finalColor;
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
            FragColor = spotLight();
            break;
        }
        case 1:
        {
            float depth = logisticDepth(gl_FragCoord.z, 0.1f, DepthDistance);
            vec3 correctedFogColor = pow(fogColor, vec3(gamma)); // Apply gamma correction to fog
            FragColor = spotLight() * (1.0f - depth) + vec4(depth * correctedFogColor, 1.0f);
            break;
        }
    }

    // Apply final gamma correction before output
    FragColor.rgb = pow(FragColor.rgb, vec3(1.0 / gamma)); 
}
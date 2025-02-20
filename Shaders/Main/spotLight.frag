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

vec4 spotLight()
{
	vec4 diffuseColor = texture(diffuse0, texCoord);
	float specularColor = texture(specular0, texCoord).r;
    // Sample the unshaded0 texture
    vec4 unshadedColor = texture(unshaded0, texCoord) * skyColor;

    // If the unshaded0 texture is being used, return its color directly
    if (isUnshaded0 == 1)
    {
        return unshadedColor;
    }
	float InnerX = InnerLight1.x;
	float InnerY = InnerLight1.y;
	float ConeInten = InnerLight1.z;

	float sRotx = spotLightRot.x;
	float sRoty = spotLightRot.y;
	float sRotz = spotLightRot.z;
	// controls how big the area that is lit up is
	float outerCone = InnerX;
	//0.95
	float innerCone = InnerY;

	// ambient lighting
	float ambient = 0.20f / -ConeInten;
	//new 2025
	//float ambient = 0.20f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightPos - crntPos);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	// calculates the intensity of the crntPos based on its angle to the center of the light cone
	float angle = dot(vec3(sRotx, sRoty, sRotz), -lightDirection);
	float inten = clamp((angle - outerCone) / (innerCone - outerCone), (0.0f), (0.0f + (ConeInten)) );

	//skyColor
	//(inten * lightColor) life saver
	// first part intensity, second part removes too brighht, third part makes sure inten wont invert
	//real life saver ((inten * lightColor ) - (inten * skyColor) * (lightColor) )                                                            doesnt add color it adds brightness         the number we take needs to be pos
	//																																				adds specular part
	switch (doReflect){
	case 0:
	return (diffuseColor *  ( (skyColor + diffuse) *     ((inten * lightColor ) - (inten * skyColor)) + (skyColor + (ambient)  ) ) + 0) * (skyColor);
	break;
	case 1:
	return (diffuseColor *  ( (skyColor + diffuse) *     ((inten * lightColor ) - (inten * skyColor)) + (skyColor + (ambient)  ) ) + specularColor * specular * inten) * (skyColor);
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
            float depth = logisticDepth(gl_FragCoord.z, 0.1f, 100.0f);
            FragColor = spotLight() * (1.0f - depth) + vec4(depth * vec3(fogColor), 1.0f);
            break;
        }
    }
}
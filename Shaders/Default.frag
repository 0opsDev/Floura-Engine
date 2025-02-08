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

//color of light from sky
uniform vec4 skyColor;

vec4 pointLight()
{	
	vec4 diffuseColor = texture(diffuse0, texCoord);
	float specularColor = texture(specular0, texCoord).r;
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
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	return (diffuseColor * (diffuse * inten + ambient) + specularColor * specular * inten) * lightColor;
}

vec4 direcLight()
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

    float sRotx = spotLightRot.x;
    float sRoty = spotLightRot.y;
    float sRotz = spotLightRot.z;

    // Ambient lighting
    float ambient = 0.20f;

    // Diffuse lighting
    vec3 normal = normalize(Normal);
    vec3 lightDirection = normalize(vec3(sRotx, sRoty, sRotz));
    float diffuse = max(dot(normal, lightDirection), 0.0f);

    // Specular lighting
    float specularLight = 0.50f;
    vec3 viewDirection = normalize(camPos - crntPos);
    vec3 reflectionDirection = reflect(-lightDirection, normal);
    float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
    float specular = specAmount * specularLight;

    return (diffuseColor * (diffuse + ambient) + (specularColor + skyColor) * specular) * lightColor;
}

vec4 unShaded()
{
	vec4 diffuseColor = texture(diffuse0, texCoord);
	float specularColor = texture(specular0, texCoord).r;
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
	}
	
}

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
	return (diffuseColor *  ( (skyColor + diffuse) *     ((inten * lightColor ) - (inten * skyColor)) + (skyColor + (ambient)  ) ) + specularColor * specular * inten) * (skyColor);
}

void main()
{
	//colours seem to blend well, just have problems instancing objects and lights
	// outputs final color
	switch (ShaderNumber){
	case 0:
	FragColor = unShaded();
	break;
	case 1:
	//spotlight is mostly working
	FragColor = spotLight();
	break;
	case 2:
	//works perfect
	FragColor = direcLight();
	break;
	case 3:
	//semi broken and needs controls to move light location
	FragColor = pointLight();
	break;
	}
}

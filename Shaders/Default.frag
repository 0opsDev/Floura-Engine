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

//color of light from sky
vec4 skylightSpread;

vec4 pointLight()
{	
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

	return (texture(diffuse0, texCoord) * (diffuse * inten + ambient) + texture(specular0, texCoord).r * specular * inten) * lightColor;
}

vec4 direcLight()
{
    // Sample the unshaded0 texture
    vec4 unshadedColor = texture(unshaded0, texCoord) * lightColor;

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

    return (texture(diffuse0, texCoord) * (diffuse + ambient) + texture(specular0, texCoord).r * specular) * lightColor;
}

vec4 unShaded()
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

	return (texture(diffuse0, texCoord) + texture(specular0, texCoord).r * specular) * lightColor;
}

vec4 spotLight()
{
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

	//skylightSpread
	//(inten * lightColor) life saver
	// first part intensity, second part removes too brighht, third part makes sure inten wont invert
	//real life saver ((inten * lightColor ) - (inten * skylightSpread) * (lightColor) )                                                            doesnt add color it adds brightness         the number we take needs to be pos
	
	return (texture(diffuse0, texCoord) *  ( (skylightSpread + diffuse) *     ((inten * lightColor ) - (inten * skylightSpread) * (lightColor) )    + (skylightSpread + (ambient)  ) ) + texture(specular0, texCoord).r * specular * inten) * (skylightSpread + lightColor);
}

void main()
{
	//colours seem to blend well, just have problems instancing objects and lights
	// outputs final color

	//spotlight is broken
	//FragColor = spotLight();

	//works perfect
	FragColor = direcLight();

	//FragColor = unShaded();

	//semi broken and needs controls to move light location
	//FragColor = pointLight();
}

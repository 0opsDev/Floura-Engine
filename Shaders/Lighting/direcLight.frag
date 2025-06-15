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

vec4 direcLight()
{
    vec4 diffuseColor = texture(diffuse0, texCoord);
    diffuseColor.rgb = pow(diffuseColor.rgb, vec3(gamma)); // Correct the gamma

    float specularColor = pow(texture(specular0, texCoord).r, 1.0 / gamma);

    float sRotx = spotLightRot.x;
    float sRoty = spotLightRot.y;
    float sRotz = spotLightRot.z;

    float ambient = 0.20f;

    vec3 normal = normalize(Normal);
    vec3 lightDirection = normalize(vec3(sRotx, sRoty, sRotz));
    float diffuse = max(abs(dot(normal, lightDirection)), 0.0f);

    float specularLight = 0.50f;
    vec3 viewDirection = normalize(camPos - crntPos);
    vec3 reflectionDirection = reflect(-lightDirection, normal);
    float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
    float specular = specAmount * specularLight;

    if (diffuseColor.a < 0.1)
        discard;

    diffuseColor.a = 1.0; // Force opaque alpha for diffuse texture

    switch (doReflect){
        case 0:
            return (diffuseColor * (diffuse + ambient)) * skyColor;
        break;
        case 1:
            return (diffuseColor * (diffuse + ambient) + specularColor * specular) * skyColor;
        break;
    }    
}

float linearizeDepth(float depth)
{
    return (2.0 * NearPlane * FarPlane) / (FarPlane + NearPlane - (depth * 2.0 - 1.0) * (FarPlane - NearPlane));
}

float logisticDepth(float depth, float steepness, float offset)
{
    float zVal = linearizeDepth(depth);
    float expVal = exp(clamp(-steepness * (zVal - offset), -10.0, 10.0));
    return 1.0 / (1.0 + expVal);
}

void main()
{
    FragColor.a = 1.0; // Ensure output is fully opaque

    switch (doFog)
    {
        case 0:
        {
            FragColor = direcLight();
            FragColor.a = 1.0; // Force fully opaque output
            break;
        }
        case 1:
        {
            float depth = logisticDepth(gl_FragCoord.z, 0.1f, DepthDistance);
            vec3 correctedFogColor = pow(fogColor, vec3(gamma)); // Apply gamma correction to fog
            FragColor = direcLight() * (1.0f - depth) + vec4(depth * correctedFogColor, 1.0f);
            FragColor.a = 1.0; // Ensure fully opaque output even with fog blending
            break;
        }
    }

    FragColor.rgb = pow(FragColor.rgb, vec3(1.0 / gamma)); // Apply final gamma correction before output
}
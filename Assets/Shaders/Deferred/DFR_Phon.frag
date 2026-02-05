#version 460 core
out vec4 FragColor;

in vec2 texCoord;
in vec3 camPositon;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D depthMap;
uniform mat4 cameraMatrix;

uniform vec3 lightColor;
uniform vec3 fogColor;
uniform vec3 skyColor;

uniform bool DEFtoggle;
uniform bool doFog;
uniform vec2 screenSize;
uniform float time;

uniform float DepthDistance;
uniform float FarPlane;
uniform float NearPlane;

// temp fields
float specularLight = 0.50f;
bool  doReflect= true;


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



vec4 pointLight(int iteration, vec3 ARM, vec3 iNormal, vec3 iPosition)
{
    vec4 finalColour = vec4(0.0f);

    //vec3 lightVec = (Lights[iteration].position) - crntPos;
    vec3 lightVec = (Lights[iteration].position) - iPosition;

    // intensity of light with respect to distance
    float dist = length(lightVec);
    float a = 3.00f;
    float b = 0.70f;
    float inten = 1.0f / (a * dist * dist + b * dist + 1.0) * Lights[iteration].radius;

    // ambient lighting
    //float ambient = 0.0f;
    vec3 normal = normalize(iNormal);
    //vec3 normal = normalize(Normal); 

    vec3 lightDirection = normalize(lightVec);
    float diffuse = max(dot(normal, lightDirection), 0.0f);

    float specular = 0.0f;
    if (doReflect && diffuse != 0.0f){
        // specular lighting
        //float specularLight = 0.50f;
        vec3 viewDirection = normalize( (camPositon) - iPosition);
        vec3 reflectionDirection = reflect(-lightDirection, normal);

        vec3 halfwayVec = normalize(lightDirection + viewDirection);

        float specAmount = pow(max(dot(normal, halfwayVec), 0.1f), 16);
        specular = specAmount * specularLight;

        finalColour = finalColour + ((diffuse * inten + 0.0f) +ARM.b * specular * inten) * vec4(Lights[iteration].colour, 1.0 ) * inten;
    }
    else{
        finalColour = finalColour + ( (diffuse * inten + 0.0f) * vec4(Lights[iteration].colour, 1.0) * inten);
        //finalColour = finalColour + (texture(diffuse0, texCoord) * (diffuse * inten + ambient) * vec4(Lights[iteration].colour, 1.0) * inten);
    }

    return finalColour;
}

vec4 spotLight(int iteration, vec3 ARM, vec3 iNormal, vec3 iPosition)
{
    // controls how big the area that is lit up is
    float outerCone = 0.90f;
    float innerCone = 0.95f;

    // ambient lighting
    //float ambient = 0.0f;

    vec4 finalColour = vec4(0.0f);

    // diffuse lighting
    vec3 normal = normalize(iNormal);

    vec3 lightDirection = normalize(Lights[iteration].position - iPosition);
    float diffuse = max(dot(normal, lightDirection), 0.0f);

    // calculates the intensity of the crntPos based on its angle to the center of the light cone
    float angle = dot(Lights[iteration].rotation, -lightDirection); // direction
    float inten = clamp( (angle - outerCone) / (innerCone - outerCone), 0.0f, 1.0);

    float specular = 0.0f;
    if (doReflect && diffuse != 0.0f){

        // specular lighting
        //float specularLight = 0.50f;
        vec3 viewDirection = normalize(camPositon - iPosition);
        vec3 reflectionDirection = reflect(-lightDirection, normal);

        vec3 halfwayVec = normalize(lightDirection + viewDirection);

        float specAmount = pow(max(dot(normal, halfwayVec), 0.0f), 16);
        specular = specAmount * specularLight;

        finalColour = finalColour + ((diffuse * inten + 0.0f) + ARM.b * specular * inten) * vec4(Lights[iteration].colour, 1.0) * inten;

    }
    else{

        finalColour = finalColour + ((diffuse * inten + 0.0f) * vec4(Lights[iteration].colour, 1.0) * inten);
        //	finalColour = finalColour + (texture(diffuse0, texCoord) * (diffuse * inten + ambient) * vec4(Lights[iteration].colour, 1.0) * inten);
    }

    //finalColour = finalColour + (diffuse * inten + skyColor);

    return finalColour;
}

vec4 lights(vec3 ARM, vec3 iNormal, vec3 iPosition){
    //vec4 diffuseTex = texture(texture_diffuse0, texCoord);
    vec4 finalColour = vec4(0.0);
    //return (diffuseTex * skyColor);
    int maxLights = 64;
    for (int i = 0; i < min(lightCount, maxLights); i++)
    {
        if (Lights[i].type == 0){
            finalColour += spotLight(i, ARM, iNormal, iPosition);
        }

        if (Lights[i].type == 1){
            finalColour += pointLight(i, ARM, iNormal, iPosition);
        }

    }
    //return finalColour;
    //FragColor = direcLight(); doDirLight

    //if (doDirLight) // if direct light is enabled, add it to the final color
    //{
       // finalColour += direcLight(specSamp);
    //}

    ///return vec4(finalColour.xyz, diffuseTex.a);
    return vec4(finalColour); // was xyz
    //return vec4((diffuseTex.xyz * skyColor.xyz) + finalColour.xyz, diffuseTex.a);
    //return (diffuseTex.xyz * skyColor.xyz) + finalColour.xyz;
}

void main()
{
    vec4 albedo = texture(gAlbedoSpec, texCoord);

    if (albedo.a <= 0.0)
    discard;

    vec3 normal = texture(gNormal, texCoord).rgb;
    
    //                      AO  Rough  MAT
    vec3 ARM = vec3(0.0, 1.0, 0.0); // placeholder
    
    float displacement = texture(gNormal, texCoord).a;
    
    vec3 position = texture(gPosition, texCoord).rgb;
    
    // port spec over to def renderer
    
    // light calc
    
    vec3 direct = lights(ARM , normal, position).rgb;
    vec3 indirect = vec3(0.0f); // [placeholder
    
    vec3 GI = (direct + indirect);
    
    vec3 final = albedo.rgb *  GI;

    FragColor = vec4(final , 1.0f);
    
    //FragColor = vec4(vec3(displacement), 1.0);
    //FragColor = vec4(normal, 1.0);
    //FragColor = vec4(normalize(normal), 1.0);
    //FragColor = vec4(albedo.rgb, 1.0);
}
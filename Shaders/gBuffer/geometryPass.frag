#version 460 core

layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gAlbedoSpec;

in vec3 crntPos;
in vec3 Normal;
in vec3 gNormals;
in vec3 color;
in vec2 texCoord;

uniform sampler2D diffuse0;
uniform sampler2D specular0;
uniform sampler2D normal0;
//uniform sampler2D noiseMapTexture;

void main()
{
    gPosition = crntPos; // Output position as-is
    vec3 gNormalTex = texture(normal0, texCoord).rgb; // Fetch normal from texture

    gNormal = normalize(gNormalTex * Normal);
    if (gNormalTex == vec3(0,0,0)){
    gNormal = Normal; // Default normal if texture is black
    }
    //gNormal = normalize(gNormalTex);
    

    // Assign Albedo RGB from texture
    //gAlbedoSpec.rgb = texture(diffuse0, texCoord).rgb * (texture(noiseMapTexture, texCoord) * 5).rgb;
    gAlbedoSpec.rgb = texture(diffuse0, texCoord).rgb;

    // Ensure alpha is correctly fetched
    gAlbedoSpec.a = texture(specular0, texCoord).r;
}
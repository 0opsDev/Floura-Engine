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

void main()
{
    gPosition = crntPos; // Output position as-is

    gNormal = normalize(Normal); // Using the vertex normal directly instead of transformed

    // Assign Albedo RGB from texture
    gAlbedoSpec.rgb = texture(diffuse0, texCoord).rgb;

    // Ensure alpha is correctly fetched
    gAlbedoSpec.a = texture(specular0, texCoord).a; // Using `.a` instead of `.r`
}
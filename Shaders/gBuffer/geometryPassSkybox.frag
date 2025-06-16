#version 460 core

layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gAlbedoSpec;

//in vec3 crntPos;
in vec3 texCoords;

uniform samplerCube skybox;
uniform vec3 skyRGBA; // consider renaming to skyColor for clarity

void main()
{
    vec4 skyTEX = texture(skybox, texCoords);
    vec3 color = skyTEX.rgb * skyRGBA;

    //gPosition = crntPos;
    gPosition = vec3(0.0, 0.0, 0.0);

    // Hardcoded purple normal (for testing/debugging?)
    gNormal = vec3(1.0, 0.0, 1.0);

    //gAlbedoSpec = vec4(1.0, 0.0, 0.0, 1.0);
    gAlbedoSpec.rgb = color;
    gAlbedoSpec.a = 0.0;
}
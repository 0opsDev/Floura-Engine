#version 460 core

#extension GL_ARB_gpu_shader_int64 : enable
#extension GL_ARB_bindless_texture : require

layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gAlbedo;
layout(location = 3) out vec4 gSpecular;
layout(location = 4) out vec4 gVelocity;
layout(location = 5) out vec3 gEmission;

in vec3 crntPos;
in vec2 texCoord;

uniform sampler2D texture0;
uniform mat3 normalMatrix;
//uniform sampler2D noiseMapTexture;

void main()
{
    vec4 albedoTex = texture(texture0, texCoord);
        // Discard fragment if alpha is too low
    if (albedoTex.a < 0.1) // Adjust threshold if needed
    discard;

    gPosition = crntPos; // Output position as-is

    //purple
    gNormal = vec4(normalMatrix * vec3(1.0, 0, 1.0), 1.0f); // Default normal if texture is black

    gAlbedo.rgb = albedoTex.rgb;
   //Ensure alpha is correctly fetched
    gAlbedo.a = 1.0f;

   gSpecular = vec4(1.0f,1.0f,0.0f,1.0f);

    gVelocity = vec4(0.0, 0.0, 1.0, 0.0);

    gEmission = vec3(0.0);
}
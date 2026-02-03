#version 460 core

#extension GL_ARB_gpu_shader_int64 : enable
#extension GL_ARB_bindless_texture : require

layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gAlbedoSpec;
layout(location = 4) out vec4 gSpecular;

in vec3 crntPos;
in vec2 texCoord;

uniform sampler2D texture0;
//uniform sampler2D noiseMapTexture;

void main()
{
    vec4 albedoTex = texture(texture0, texCoord);
        // Discard fragment if alpha is too low
    if (albedoTex.a < 0.1) // Adjust threshold if needed
    discard;

    gPosition = crntPos; // Output position as-is

    //purple
    gNormal = vec3(1.0, 0, 1.0); // Default normal if texture is black

    gAlbedoSpec.rgb = albedoTex.rgb;

   //Ensure alpha is correctly fetched
   gAlbedoSpec.a = 0.0f;

   gSpecular = vec4(1.0f,1.0f,0.0f,1.0f);
}
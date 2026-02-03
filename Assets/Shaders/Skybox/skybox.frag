#version 460 core

#extension GL_ARB_gpu_shader_int64 : enable
#extension GL_ARB_bindless_texture : require

out vec4 FragColor;

in vec3 texCoords;



uniform samplerCube skybox;
uniform uint64_t skyboxHandle;
//color of light from sky
uniform vec3 skyRGBA;
uniform bool DoSbRGBA;

void main()
{
    samplerCube smSamp = samplerCube(skyboxHandle);

    vec4 skyTEX = texture(smSamp, texCoords);
    vec3 color = skyTEX.rgb;
    if (DoSbRGBA)
    {
    color *= skyRGBA;
    }
    FragColor = vec4(color, 1.0);
}
#version 460 core

#extension GL_ARB_gpu_shader_int64 : enable
#extension GL_ARB_bindless_texture : require

// just albedo
layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gAlbedoSpec;
layout(location = 3) out vec4 gSpecular;
layout(location = 4) out vec4 gVelocity;
layout(location = 5) out vec3 gEmission;

in highp vec3 texCoords;
in highp vec4 currentPos;
in highp vec4 previousPos;

uniform samplerCube skybox;
uniform uint64_t skyboxHandle;
//color of light from sky
uniform vec3 skyRGBA;
uniform bool DoSbRGBA;

uniform vec2 currentJitter;
uniform vec2 previousJitter;
uniform vec2 scaledCurrentJitter;
uniform vec2 scaledPreviousJitter;

void main()
{
    samplerCube smSamp = samplerCube(skyboxHandle);

    vec4 skyTEX = texture(smSamp, texCoords);
    vec3 color = skyTEX.rgb;
    
    if (DoSbRGBA)
    {color *= skyRGBA;}


    gAlbedoSpec = vec4(color, 1.0);
    gPosition = vec3(0);
    gNormal = vec4(0);
    gSpecular = vec4(0);

    //vec2 currentNDC = currentPos.xy / (currentPos.w + 1e-7);
    //vec2 previousNDC = previousPos.xy / (previousPos.w + 1e-7);
    highp vec2 currentNDC = currentPos.xy / (currentPos.w);
    highp vec2 previousNDC = previousPos.xy / (previousPos.w);
    highp vec2 velocity = (currentNDC + scaledCurrentJitter) - (previousNDC + scaledPreviousJitter);

    // elipson
    if (length(velocity) < 0.001) { velocity = vec2(0.0); }

    gVelocity = vec4(velocity * 0.5, 1.0, 1.0);
    gEmission = vec3(0.0);
    //FragColor = vec4(color, 1.0);
}
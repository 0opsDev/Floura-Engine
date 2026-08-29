#version 460 core

#extension GL_ARB_gpu_shader_int64 : enable
#extension GL_ARB_bindless_texture : require

// previous
layout(location = 0) out vec4 hColour;
layout(location = 2) out float hDepthTexture;

// current
uniform sampler2D screentexture;
uniform sampler2D currentDepth;

in vec2 texCoord;

void main()
{
    // set current as previous
    
    // ColourBuffer
    hColour = texture(screentexture, texCoord);
    //hColour = vec4(1.0,0.0,0.0,1.0);
    
    // normal
    // depth (ill leave this one undone for now, i think i need to linearize it first)
    hDepthTexture = texture(currentDepth, texCoord).r; // this now, if there is issues then look into it
}
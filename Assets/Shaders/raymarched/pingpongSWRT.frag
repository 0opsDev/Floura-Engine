#version 460 core

out vec4 FragColor;
in highp vec2 texCoord;

layout(location = 0) out vec4 oindirect;
layout(location = 2) out vec4 oemission;
//layout(location = 3) out vec4 oindirectSpecular;
//layout(location = 4) out vec4 oemissionSpecular;

uniform sampler2D dIndirect;
uniform sampler2D dEmission;
//uniform sampler2D dIndirectSpecular;
//uniform sampler2D dEmissionSpecular;

void main(){
    oindirect = texture(dIndirect, texCoord);
    oemission = texture(dEmission, texCoord);
    //oindirectSpecular = texture(dIndirectSpecular, texCoord);
    //oemissionSpecular = texture(dEmissionSpecular, texCoord);
}
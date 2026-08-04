#version 460 core

out vec4 FragColor;
in highp vec2 texCoord;

//presentImage

uniform sampler2D presentImage;
uniform sampler2D depthMap;

void main(){
    float gdepth = texture2D(depthMap, texCoord).r;
    if (gdepth >= 0.99999) discard;
    
    FragColor = texture(presentImage, texCoord);
}
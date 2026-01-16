#version 460 core

out vec4 FragColor;
in vec2 texCoords;

uniform sampler2D depthMap;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D screenTexture;
uniform mat4 cameraMatrix;

uniform float gamma;

void main() {

    FragColor.rgb = pow(texture(screenTexture, texCoords).rgb, vec3(gamma));

    //FragColor.rgb = texture(gNormal, texCoords).rgb;
}
#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;

out vec2 texCoord;

uniform mat4 camMatrix;
uniform mat4 model;

void main()
{
    texCoord = aTex;  // Pass texture coordinates to fragment shader
    
    gl_Position = vec4(aPos, 1.0);
    //gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
    //gl_Position = camMatrix * model * vec4(aPos, 1.0);
}
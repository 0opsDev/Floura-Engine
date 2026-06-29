#version 460 core
layout (location = 0) in vec3 aPos;

out highp vec3 texCoords;
out highp vec4 currentPos;
out highp vec4 previousPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 rotation;

uniform mat4 previousProjection;
uniform mat4 previousView;

void main()
{
    // view positions
    highp mat4 currentVP = projection * view * rotation;
    highp mat4 prevVP    = previousProjection * previousView * rotation;
    
    currentPos = currentVP * vec4(aPos, 1.0);
    previousPos = prevVP * vec4(aPos, 1.0);
    
    gl_Position = vec4(currentPos.x, currentPos.y, currentPos.w, currentPos.w);
    texCoords = vec3(aPos.x, aPos.y, aPos.z);
}
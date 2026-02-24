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

//rotation
void main()
{
    //vec4 pos = projection * view * rotation * vec4(aPos, 1.0f);
    
    // view positions
    highp mat4 currentVP = projection * view * rotation;
    highp mat4 prevVP    = previousProjection * previousView * rotation;
    
    currentPos = currentVP * vec4(aPos, 1.0);
    previousPos = prevVP * vec4(aPos, 1.0);
    
    // Having z equal w will always result in a depth of 1.0f
    gl_Position = vec4(currentPos.x, currentPos.y, currentPos.w, currentPos.w);
    // We want to flip the z axis due to the different coordinate systems (left hand vs right hand)
    texCoords = vec3(-aPos.x, aPos.y, -aPos.z);
}
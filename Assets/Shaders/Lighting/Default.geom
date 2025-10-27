#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out; // points looks cool

out vec3 Normal;
out vec3 color;
out vec2 texCoord;
out vec3 crntPos;
out vec4 fragPosLight;
out vec3 Normal0;
out vec3 Tangent0;
out vec3 Bitangent0;
//out mat3 TBN;
//out vec3 camPos;
//out vec3 directLightPos;

in DATA
{
    vec3 Normal;
    vec3 color;
    vec2 texCoord;
    mat4 projection;
    mat4 model;
    vec3 WorldPos;
    vec4 fragPosLight;
    vec3 Normal0;
    vec3 Tangent0;
    vec3 Bitangent0;
    //mat3 normalMatrix;
    //mat3 TBN;
    //vec3 camPos;
//    vec3 directLightPos;
} data_in[];

uniform mat3 normalMatrix;

void main()
{
    //vec3 vector0 = vec3(gl_in[0].gl_Position - gl_in[1].gl_Position); // 1
    //vec3 vector1 = vec3(gl_in[2].gl_Position - gl_in[1].gl_Position); // 1
    //vec4 surfaceNormal = vec4(normalize(cross(vector1, vector0)), 0.0); // 1

    for (int i = 0; i < 3; i++)
    {
    //gl_Position = data_in[i].projection * (gl_in[i].gl_Position + surfaceNormal); // 1
    gl_Position = data_in[i].projection * gl_in[i].gl_Position;
    crntPos =  data_in[i].WorldPos; 
    fragPosLight = data_in[i].fragPosLight;
    Normal = data_in[i].Normal;
    color = data_in[i].color;
    texCoord = data_in[i].texCoord;
    //TBN
    Normal0 = data_in[i].Normal0;
    Tangent0 = data_in[i].Tangent0;
    Bitangent0 = data_in[i].Bitangent0;
    EmitVertex();
    }


    EndPrimitive();
}
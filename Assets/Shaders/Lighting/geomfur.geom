#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

out vec3 Normal;
out vec3 color;
out vec2 texCoord;
out vec3 crntPos;

in DATA
{
    vec3 Normal;
    vec3 color;
    vec2 texCoord;
    mat4 projection;
    vec3 WorldPos;
} data_in[];

void main()
{

    for (int i = 0; i < 3; i++)
    {
    crntPos = vec3(data_in[i].projection * gl_in[i].gl_Position);
    gl_Position = data_in[i].projection * gl_in[i].gl_Position;
    crntPos = data_in[i].WorldPos; 
    Normal = data_in[i].Normal;
    color = data_in[i].color;
    texCoord = data_in[i].texCoord;
    EmitVertex();
    }
    /*
        crntPos = vec3(data_in[0].projection * gl_in[0].gl_Position);
    gl_Position = data_in[0].projection * gl_in[0].gl_Position;
    crntPos = data_in[0].WorldPos; 
    Normal = data_in[0].Normal;
    color = data_in[0].color;
    texCoord = data_in[0].texCoord;
    EmitVertex();

    crntPos = vec3(data_in[1].projection * gl_in[1].gl_Position);
    gl_Position = data_in[1].projection * gl_in[1].gl_Position;
    crntPos = data_in[1].WorldPos; 
    Normal = data_in[1].Normal;
    color = data_in[1].color;
    texCoord = data_in[1].texCoord;
    EmitVertex();

    crntPos = vec3(data_in[2].projection * gl_in[2].gl_Position);
    gl_Position = data_in[2].projection * gl_in[2].gl_Position;
    crntPos = data_in[2].WorldPos; 
    Normal = data_in[2].Normal;
    color = data_in[2].color;
    texCoord = data_in[2].texCoord;
    EmitVertex();
    */


    EndPrimitive();
}
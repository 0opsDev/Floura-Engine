#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out; // points looks cool

out vec3 Normal;
out vec3 color;
out vec2 texCoord;
out vec3 crntPos;
out vec4 fragPosLight;

in DATA
{
    vec3 Normal;
    vec3 color;
    vec2 texCoord;
    mat4 projection;
    vec3 WorldPos;
    vec4 fragPosLight;
} data_in[];

void main()
{
    //vec3 vector0 = vec3(gl_in[0].gl_Position - gl_in[1].gl_Position); // 1
    //vec3 vector1 = vec3(gl_in[2].gl_Position - gl_in[1].gl_Position); // 1
    //vec4 surfaceNormal = vec4(normalize(cross(vector1, vector0)), 0.0); // 1

    for (int i = 0; i < 3; i++)
    {
    //gl_Position = data_in[i].projection * (gl_in[i].gl_Position + surfaceNormal); // 1
    gl_Position = data_in[i].projection * gl_in[i].gl_Position;
    crntPos = data_in[i].WorldPos; 
    fragPosLight = data_in[i].fragPosLight;
    Normal = data_in[i].Normal;
    color = data_in[i].color;
    texCoord = data_in[i].texCoord;
    EmitVertex();
    }


    EndPrimitive();
}
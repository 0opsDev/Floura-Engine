#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in DATA
{
    vec3 crntPos;
    vec3 Normal;
    vec3 color;
    vec2 TexCoord;
    mat4 projection;
} data_in[];

out vec3 crntPos;
out vec3 Normal;
out vec3 color;
out vec2 TexCoord;

void main()
{
    for (int i = 0; i < 3; i++)
    {
        gl_Position = data_in[i].projection * vec4(data_in[i].crntPos, 1.0);
        crntPos = data_in[i].crntPos;
        Normal = data_in[i].Normal;
        color = data_in[i].color;
        TexCoord = data_in[i].TexCoord;
        EmitVertex();
    }
    EndPrimitive();
}

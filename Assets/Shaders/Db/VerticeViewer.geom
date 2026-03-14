#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out; // points looks cool

out vec2 texCoord;
out vec3 crntPos;
out vec3 Colour;
in DATA
{
	vec2 texCoord;
	mat4 projection;
    mat4 model;
    vec3 WorldPos;
} data_in[];

float random(vec3 seed) {
    return fract(sin(dot(seed, vec3(12.9898, 78.233, 45.164))) * 43758.5453);
}

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
        texCoord = data_in[i].texCoord;
    Colour = vec3( random(data_in[0].WorldPos), random(data_in[1].WorldPos) , random(data_in[2].WorldPos) );

    EmitVertex();
    }


    EndPrimitive();
}
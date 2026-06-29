#version 460 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out; // points looks cool

/*
out vec3 Normal;
out vec3 color;
out vec2 texCoord;
out vec3 crntPos;
out vec3 gNormals;
*/

out highp vec3 crntPos;
out vec3 Normal;
out vec3 gNormals;
out vec3 colour;
out vec2 texCoord;

//TBN
out vec3 Normal0;
out vec3 Tangent0;
out vec3 Bitangent0;

out highp vec4 currentPos;
out highp vec4 previousPos;

in DATA
{
    vec3 Normal;
    vec3 gNormals;
    vec3 colour;
    vec2 texCoord;
    vec3 Normal0;
    vec3 Tangent0;
    vec3 Bitangent0;
    highp vec3 crntPos;
    highp vec4 currentPos;
    highp vec4 previousPos;

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
        gl_Position = gl_in[i].gl_Position;
        
        Normal = data_in[i].Normal;
        gNormals = data_in[i].gNormals;
        //colour = data_in[i].colour;
        colour = vec3( random(data_in[0].crntPos), random(data_in[1].crntPos) , random(data_in[2].crntPos) );
        texCoord = data_in[i].texCoord;
        Normal0 = data_in[i].Normal0;
        Tangent0 = data_in[i].Tangent0;
        Bitangent0 = data_in[i].Bitangent0;
        crntPos = data_in[i].crntPos;
        currentPos = data_in[i].currentPos;
        previousPos = data_in[i].previousPos;

        
    EmitVertex();
    }


    EndPrimitive();
}

/*
//gl_Position = data_in[i].projection * (gl_in[i].gl_Position + surfaceNormal); // 1
gl_Position = data_in[i].projection * gl_in[i].gl_Position;
crntPos = data_in[i].WorldPos; 
Normal = data_in[i].Normal;
color = data_in[i].color;
texCoord = data_in[i].texCoord;
gNormals = data_in[i].gNormals;
*/
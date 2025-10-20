#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out; // points looks cool

out vec3 Normal;
out vec3 color;
out vec2 texCoord;
out vec3 crntPos;
out vec4 fragPosLight;
out mat3 TBN;
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

    // first move tangent to the cpu then, move this to the vert shader
    
    vec3 edge0 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 edge1 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec2 deltaUV0 = data_in[1].texCoord - data_in[0].texCoord;
    vec2 deltaUV1 = data_in[2].texCoord - data_in[0].texCoord;

    float invDet = 1.0f / (deltaUV0.x * deltaUV1.y - deltaUV1.x * deltaUV0.y);

    vec3 tangent = vec3(invDet * (deltaUV1.y * edge0 - deltaUV0.y * edge1));
    vec3 bitangent = vec3(invDet * (-deltaUV1.x * edge0 + deltaUV0.x * edge1));

    vec3 T = normalize(vec3(data_in[0].model * vec4(tangent, 0.0f)));
    vec3 B = normalize(vec3(data_in[0].model * vec4(bitangent, 0.0f)));
    vec3 N = normalize(vec3(data_in[0].model * vec4(cross(edge1, edge0), 0.0f)));

    //vec3 T = normalize(normalMatrix * tangent);
    //vec3 B = normalize(normalMatrix * bitangent);
    //vec3 N = normalize(normalMatrix * cross(edge1, edge0));

    mat3 NTBN = mat3(T, B, N);
    NTBN = transpose(NTBN);
    TBN = NTBN;


    for (int i = 0; i < 3; i++)
    {
    //gl_Position = data_in[i].projection * (gl_in[i].gl_Position + surfaceNormal); // 1
    gl_Position = data_in[i].projection * gl_in[i].gl_Position;
    crntPos =  data_in[i].WorldPos; 
    fragPosLight = data_in[i].fragPosLight;
    Normal = data_in[i].Normal;
    color = data_in[i].color;
    texCoord = data_in[i].texCoord;
    //camPos = data_in[i].camPos;
//    directLightPos = TBN * data_in[i].directLightPos;
    EmitVertex();
    }


    EndPrimitive();
}
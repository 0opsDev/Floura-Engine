#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTex; // UV
layout (location = 4) in vec3 aTangent;
layout (location = 5) in vec3 aBitangent;
layout(location = 6) in ivec4 boneIds;
layout(location = 7) in vec4 weights;

//out mat3 TBN;

uniform mat4 camMatrix;
uniform mat4 model; // Final model matrix combining all transformations
uniform mat3 normalMatrix;
uniform vec3 camPos;

out vec3 crntPos;
out vec3 Normal;
out vec3 color;
out vec2 texCoord;
out vec3 weightColour;

//TBN
out vec3 Normal0;
out vec3 Tangent0;
out vec3 Bitangent0;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

void main()
{
    vec4 totalPosition = vec4(0.0f);

    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
        if(boneIds[i] == -1)
        continue;
        if(boneIds[i] >=MAX_BONES)
        {
            totalPosition = vec4(aPos,1.0f);
            break;
        }
        vec4 localPosition = finalBonesMatrices[boneIds[i]] * vec4(aPos,1.0f);
        totalPosition += localPosition * weights[i];
        vec3 localNormal = mat3(finalBonesMatrices[boneIds[i]]) * aNormal;
    }
    
    crntPos = vec3(model * vec4(aPos, 1.0f));

    Normal0 = normalMatrix * aNormal;
    Tangent0 = normalMatrix * aTangent;
    Bitangent0 = normalMatrix * aBitangent;

    //int iID = gl_InstanceID;

    vec3 Norm = normalize(normalMatrix * aNormal);
    Normal = Norm;
    color = aColor;
    weightColour = weights.rgb;
    texCoord = aTex;

    //gl_Position = camMatrix * vec4(crntPos, 1.0f) * totalPosition;
    gl_Position = camMatrix * vec4(crntPos, 1.0f) ;
}
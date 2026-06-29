#version 460 core

//out vec4 FragColor;
//in highp vec2 texCoord;

layout(location = 0) out vec4 FragColor;
layout(location = 0) in highp vec2 texCoord;


//uniform vec2 screenSize;

//uniform mat4 u_ViewMatrix;
//uniform mat4 u_ProjectionMatrix;
//uniform mat4 inverseViewMatrix;
//uniform mat4 inverseProjectionMatrix;


layout(location = 0) uniform vec2 screenSize;

layout(location = 1) uniform mat4 u_ViewMatrix;
layout(location = 2) uniform mat4 u_ProjectionMatrix;
layout(location = 3) uniform mat4 inverseViewMatrix;
layout(location = 4) uniform mat4 inverseProjectionMatrix;


// day
//vec3 zenithColour = vec3(0.12, 0.45, 0.92);
//vec3 horizonColour = vec3(0.60, 0.78, 0.98);
//vec3 groundColour  = vec3(0.01, 0.01, 0.02);

// sunset/rise
vec3 zenithColour  = vec3(0.18, 0.52, 0.88);
vec3 horizonColour = vec3(0.98, 0.82, 0.60);
vec3 groundColour  = vec3(0.95, 0.45, 0.38);

// night
//vec3 zenithColour = vec3(0.01, 0.02, 0.05) * 0.8;
//vec3 horizonColour = vec3(0.05, 0.08, 0.15);
//vec3 groundColour = vec3(0.02, 0.03, 0.05);

// stars
float coverage = 0.005f;

vec3 rayDirfromCam(mat4 projection, mat4 view, vec2 uv)
{
    vec4 target = projection * vec4(uv.x, uv.y, -1.0, 1.0);
    vec3 rayDirView = normalize(target.xyz / target.w);
    vec3 rayDirWorld = normalize(mat3(view) * rayDirView);

    return normalize(rayDirWorld);
}

float random (vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
}

void main()
{
    ivec2 size = ivec2(screenSize);
    vec2 uv = (texCoord - 0.5) * 2.0;
    
    vec3 rayDir = rayDirfromCam(inverseProjectionMatrix, inverseViewMatrix, uv);
    
    // adds stars onto zenith (white noise)
    zenithColour += step(1.0 - coverage, random(rayDir.xy));
    
    float blend = clamp(rayDir.y, 0.0, 1.0);
    // mix the horizon with the zenith
    vec3 colour = mix(horizonColour, zenithColour, blend);
    
    // mix the ground with the horizon
    if (rayDir.y < 0.0) colour = mix(horizonColour, groundColour, clamp(-rayDir.y * 2.0, 0.0, 1.0));
    
    // return, clamp, divide my 2.2 gamma for a slightly deeper tone
    FragColor = vec4(clamp(colour / 2.2, 0.0, 1.0), 1.0);
}
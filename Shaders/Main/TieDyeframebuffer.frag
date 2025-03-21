#version 330 core

out vec4 FragColor;
in vec2 texCoords;

uniform sampler2D screenTexture;
uniform float time;  // Pass in a time uniform for animation

void main()
{
    // Center the coordinates around (0.5, 0.5)
    vec2 uv = texCoords - vec2(0);
    
    // Calculate the distance from the center
    float dist = length(uv);
    
    // Apply a swirl effect with time-based rotation
    float angle = atan(uv.y, uv.x) + time * 0.1 - dist * 4;
    vec2 swirlUV = vec2(cos(angle), sin(angle)) * dist;

    // Apply an infinite zoom effect
    float zoomFactor = 0.5 + 0.5 * sin(time * 0.5);
    swirlUV *= zoomFactor;

    // Re-center UV coordinates
    swirlUV += vec2(0.5);

    // Sample the texture
    vec4 color = texture(screenTexture, uv);

    // Enhance psychedelic effect by modifying colors
    color.rgb *= vec3(sin((time * 5) + color.r * 50.0), sin((time * 5) + color.g * 50.0), sin((time * 5) + color.b * 50.0));

    FragColor = color;
}

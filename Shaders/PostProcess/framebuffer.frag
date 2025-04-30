#version 460 core

out vec4 FragColor;
in vec2 texCoords;

uniform sampler2D screenTexture;
uniform float time;  // Pass in a time uniform for animation
uniform bool enableFB;
vec4 FragColorT;

// Function to quantize colors to a specific bit depth
vec3 quantizeColor(vec3 color, int bitDepth) {
    float maxVal = float((1 << bitDepth) - 1);  // Maximum value for the given bit depth
    return floor(color * maxVal) / maxVal;
}

// 1D Bayer matrix for dithering (4x4 flattened into 1D)
const float bayerMatrix[16] = float[16](
    0.0, 8.0, 2.0, 10.0,
    12.0, 4.0, 14.0, 6.0,
    3.0, 11.0, 1.0, 9.0,
    15.0, 7.0, 13.0, 5.0
);

void main()
{
    vec4 texColor = texture(screenTexture, texCoords);
    
    // Quantize each color channel to 4 bits (bitDepth = 4)
    vec3 quantizedColor = quantizeColor(texColor.rgb, 4);
    
    // Determine the pixel's position in the 4x4 Bayer matrix (flattened to 1D)
    ivec2 bayerPos = ivec2(int(mod(gl_FragCoord.x, 4.0)), int(mod(gl_FragCoord.y, 4.0)));
    int index = bayerPos.y * 4 + bayerPos.x; // Flatten the 2D index to 1D
    
    // Calculate the dither threshold based on the Bayer matrix
    float threshold = bayerMatrix[index] / 16.0;
    
    // Apply dithering by adjusting the quantized color based on the threshold
    vec3 ditheredColor = quantizedColor + (step(threshold, texColor.rgb) - 0.5);

    FragColorT = vec4(ditheredColor, texColor.a);
   
    if (!enableFB)
    {
        FragColorT = texColor;
    }

    FragColor = FragColorT;
}
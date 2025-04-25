#version 460 core

out vec4 FragColor;
in vec2 texCoords;

uniform sampler2D screenTexture;
uniform float time;  // Pass in a time uniform for animation

// Bayer matrix for 3x3 dithering
const float bayerMatrix[9] = float[9](
    0.0,  7.0,  3.0,
    6.0,  5.0,  2.0,
    4.0,  1.0,  8.0
);

void main()
{
    // Downsample factor
    float downsampleFactor = 0.33;

    // Calculate the new texture coordinates for downsampling
    vec2 downsampledCoords = floor(gl_FragCoord.xy * downsampleFactor) / (vec2(textureSize(screenTexture, 0)) * downsampleFactor);

    // Sample the downscaled texture
    vec4 color = texture(screenTexture, downsampledCoords);

    // Blur kernel size (3x3) - Apply blur to the downscaled image
    float kernelSize = 2.0 / (textureSize(screenTexture, 0).x * downsampleFactor); // Adjust for downsampled image size
    vec4 blurredColor = vec4(0.0);

    for(int x = -1; x <= 1; x++)
    {
        for(int y = -1; y <= 1; y++)
        {
            vec2 offset = vec2(float(x), float(y)) * kernelSize;
            blurredColor += texture(screenTexture, downsampledCoords + offset);
        }
    }
    blurredColor /= 9.0;

    // Apply dithering
    ivec2 pixelPos = ivec2(gl_FragCoord.xy) % 3;
    int index = pixelPos.y * 3 + pixelPos.x;
    float threshold = bayerMatrix[index] / 9.0;

    // Adjust color values based on the threshold
    blurredColor.r = (blurredColor.r > threshold) ? clamp(blurredColor.r + 0.1, 0.0, 1.0) : clamp(blurredColor.r - 0.1, 0.0, 1.0);
    blurredColor.g = (blurredColor.g > threshold) ? clamp(blurredColor.g + 0.1, 0.0, 1.0) : clamp(blurredColor.g - 0.1, 0.0, 1.0);
    blurredColor.b = (blurredColor.b > threshold) ? clamp(blurredColor.b + 0.1, 0.0, 1.0) : clamp(blurredColor.b - 0.1, 0.0, 1.0);

    FragColor = blurredColor;
}

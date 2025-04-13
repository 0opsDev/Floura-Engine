#version 330 core
out vec4 FragColor;
in vec2 texCoords;

uniform sampler2D screenTexture;  // Input texture
uniform float time;               // Time-based randomness for jitter
uniform bool enableMSAA;           // Toggle MSAA on/off
uniform int MSAAsamp;              // Number of MSAA samples (must be an integer for the loop)
uniform float sharpenStrength;    // Sharpening slider variable
uniform float texelSizeMulti;

float random(vec2 coord, float frameOffset) {
    return fract(sin(dot(coord.xy, vec2(12.9898, 78.233)) + frameOffset + time) * 43758.5453);
}

// Function to calculate luminance
float luminance(vec3 color) {
    return dot(color, vec3(0.2126, 0.7152, 0.0722)); // Standard luminance weights
}

void main() {
    // Step 1: Use input texture at its native resolution
    vec2 texelSize = texelSizeMulti / vec2(textureSize(screenTexture, 0)); // Texel size for edge detection
    vec4 originalFrame = texture(screenTexture, texCoords);    // Original frame

    // Initialize accumulation variables for MSAA
    vec4 accumulatedColor = vec4(0.0);
    float weight = 1.0 / float(MSAAsamp);

    // Step 2: Apply MSAA on the original image
    if (enableMSAA) {
        for (int i = 0; i < MSAAsamp; i++) {
            float sampleOffset = float(i);
            vec2 jitter = vec2(
                random(texCoords, sampleOffset), 
                random(texCoords * 2.0, sampleOffset)
            ) * texelSize;

            vec2 jitteredTexCoords = texCoords + jitter;
            vec4 jitteredFrame = texture(screenTexture, jitteredTexCoords);
            accumulatedColor += jitteredFrame * weight;
        }
        originalFrame = accumulatedColor; // Final MSAA output
    }

    // Step 3: Edge detection on the MSAA image
    vec4 north = texture(screenTexture, texCoords + vec2(0.0, texelSize.y));
    vec4 south = texture(screenTexture, texCoords - vec2(0.0, texelSize.y));
    vec4 east = texture(screenTexture, texCoords + vec2(texelSize.x, 0.0));
    vec4 west = texture(screenTexture, texCoords - vec2(texelSize.x, 0.0));

    float edgeStrength = 0.0;

    // Color-based edge detection
    edgeStrength += length(originalFrame.rgb - north.rgb);
    edgeStrength += length(originalFrame.rgb - south.rgb);
    edgeStrength += length(originalFrame.rgb - east.rgb);
    edgeStrength += length(originalFrame.rgb - west.rgb);

    // Luminance-based edge detection
    float texLuminance = luminance(originalFrame.rgb);
    edgeStrength += abs(texLuminance - luminance(north.rgb));
    edgeStrength += abs(texLuminance - luminance(south.rgb));
    edgeStrength += abs(texLuminance - luminance(east.rgb));
    edgeStrength += abs(texLuminance - luminance(west.rgb));

    // Step 4: Sharpening (applied to the image only if MSAA is enabled)
    vec4 finalColor = originalFrame; // Default to the original image

    if (enableMSAA) {
        // Apply sharpening logic
        vec4 blur = (north + south + east + west) / 4.0; // Simple box blur
        vec4 sharpened = originalFrame + sharpenStrength * (originalFrame - blur);

        // Combine sharpened result with edge detection
        float edgeThreshold = 0.1; // Adjustable threshold for edge presence
        if ((edgeStrength) > edgeThreshold) {
            finalColor = originalFrame; // Preserve the original MSAA image at edges
        } else {
            finalColor = sharpened; // Apply sharpening elsewhere
        }
    }

    // Final output
    FragColor = finalColor;
}

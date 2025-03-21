#version 330 core
out vec4 FragColor;
in vec2 texCoords;

uniform sampler2D screenTexture;  // Input texture
uniform float resolutionScale;    // Downscale factor (e.g., 0.75 = 75% native resolution)
uniform float time;               // Time-based randomness for jitter
uniform bool enableTAA;           // Toggle TAA on/off
uniform int TAAsamp;              // Number of TAA samples (must be an integer for the loop)

float random(vec2 coord, float frameOffset) {
    // Generate random noise using coordinates, time, and frame offset
    return fract(sin(dot(coord.xy, vec2(12.9898, 78.233)) + frameOffset + time) * 43758.5453);
}

void main() {
    // Step 1: Simulate lower resolution using nearest-neighbor downsampling
    vec2 nativeResolution = vec2(textureSize(screenTexture, 0)); // Retrieve native resolution
    vec2 scaledResolution = nativeResolution * resolutionScale;  // Calculate scaled resolution
    vec2 downscaledTexCoords = floor(texCoords * scaledResolution) / scaledResolution; // Nearest-neighbor UV mapping
    vec4 scaledFrame = texture(screenTexture, downscaledTexCoords); // Simulated lower-resolution frame

    // Initialize accumulation variables
    vec4 accumulatedColor = vec4(0.0);          // Accumulate samples for blending
    float weight = 1.0 / float(TAAsamp);        // Equal weight for each sample

    // Step 2: Apply TAA by sampling multiple jittered frames
    if (enableTAA) {
        for (int i = 0; i < TAAsamp; i++) {      // Iterate over the number of samples
            // Generate jitter for the current sample
            float sampleOffset = float(i);      // Unique offset for each sample
            vec2 jitter = vec2(
                random(downscaledTexCoords, sampleOffset), 
                random(downscaledTexCoords * 2.0, sampleOffset)
            ) * (1.0 / scaledResolution);       // Normalize jitter to resolution

            vec2 jitteredTexCoords = downscaledTexCoords + jitter;

            // Sample the texture with jittered UVs
            vec4 jitteredFrame = texture(screenTexture, jitteredTexCoords);

            // Accumulate the jittered sample
            accumulatedColor += jitteredFrame * weight;
        }

        // Final blended frame after accumulating samples
        scaledFrame = accumulatedColor;
    }

    // Step 3: Output the scaled frame (anti-aliased with TAA applied)
    FragColor = scaledFrame; // Directly output the result
}

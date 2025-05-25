#version 460 core

out vec4 FragColor;
in vec2 texCoords;

uniform sampler2D depthMap;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D screenTexture;
uniform mat4 cameraMatrix;

uniform bool enableFB;

const float near = 0.1;
const float far = 500.0;

// Depth conversion
float linearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0;
    return (near * far) / (far - z * (far - near)); 
}

// Normalize [-1, 1] → [0, 1]
vec3 remapVec3(vec3 v) {
    return v * 0.5 + 0.5;
}

// Map a matrix row to RGB
vec3 encodeMatrixRowRGB(mat4 mat, int rowIndex) {
    return remapVec3(vec3(mat[rowIndex][0], mat[rowIndex][1], mat[rowIndex][2]));
}

void main() {
    if (!enableFB) {
        FragColor = texture(screenTexture, texCoords);
        return;
    }

    const float colWidth = 1.0 / 3.0;
    const float rowHeight = 0.5;

    int col = int(texCoords.x / colWidth);
    int row = int(texCoords.y / rowHeight);

    vec2 localUV = vec2(
        (texCoords.x - float(col) * colWidth) / colWidth,
        (texCoords.y - float(row) * rowHeight) / rowHeight
    );

    vec4 outColor = vec4(1.0, 0.0, 1.0, 1.0); // magenta error color (fallback)

    if (row == 1) {
        // Top row
        if (col == 0) {
            float depth = texture(depthMap, localUV).r;
            float linearDepth = linearizeDepth(depth) / far;
            outColor = vec4(vec3(linearDepth), 1.0);
        } else if (col == 1) {
            vec3 pos = texture(gPosition, localUV).rgb;
            vec3 mapped = clamp(pos / 100.0 + 0.5, 0.0, 1.0);
            outColor = vec4(mapped, 1.0);
        } else if (col == 2) {
            vec3 norm = texture(gNormal, localUV).rgb;
            outColor = vec4(remapVec3(norm), 1.0);
        }
    } else if (row == 0) {
        if (col == 0) {
            vec3 albedo = texture(gAlbedoSpec, localUV).rgb;
            outColor = vec4(albedo, 1.0);
        } else if (col == 1) {
            outColor = texture(screenTexture, localUV);
        } else if (col == 2) {
            // Show cameraMatrix in vertical stripes
            float slice = localUV.x;
            int rowIndex = int(slice * 4.0); // 4 matrix rows
            vec3 encoded = encodeMatrixRowRGB(cameraMatrix, rowIndex);
            outColor = vec4(encoded, 1.0);
        }
    }

    FragColor = outColor;
}
#version 330 core
out vec4 FragColor;

uniform float DepthDistance; // Distance offset for depth adjustment
uniform float NearPlane;     // Near plane of the camera
uniform float FarPlane;      // Far plane of the camera

float linearizeDepth(float depth)
{
    return (2.0 * NearPlane * FarPlane) / (FarPlane + NearPlane - (depth * 2.0 - 1.0) * (FarPlane - NearPlane));
}

float logisticDepth(float depth, float steepness, float offset)
{
    float zVal = linearizeDepth(depth);
    float expVal = exp(clamp(-steepness * (zVal - offset), -10.0, 10.0));
    return 1.0 / (1.0 + expVal);
}

void main()
{
    // Calculate depth-based interpolation factor using the logisticDepth function
    float depth = logisticDepth(gl_FragCoord.z, 0.1f, DepthDistance);

    // Interpolate between purple (1.0, 0.0, 1.0) and cyan (0.0, 1.0, 1.0)
    vec3 color = mix(vec3(1.0, 0.0, 1.0), vec3(0.0, 1.0, 1.0), depth);

    // Set the fragment color
    FragColor = vec4(color, 1.0);
}

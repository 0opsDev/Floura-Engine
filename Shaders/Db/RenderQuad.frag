#version 460 core
out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

uniform vec3 lightColor;
uniform vec3 skyColor;

void main()
{
    vec3 albedo = texture(gAlbedoSpec, texCoord).rgb;
    vec3 normal = texture(gNormal, texCoord).rgb;
    vec3 position = texture(gPosition, texCoord).rgb;
    float spec = texture(gAlbedoSpec, texCoord).a;


    //FragColor = vec4(normal, 1.0f); // normal
    //FragColor = vec4(position, 1.0f); // position
    //FragColor = vec4(albedo, 1.0f); // albedo
    //FragColor = vec4(vec3(spec), 1.0f); // spec
    FragColor = vec4(albedo * skyColor, 1.0f); // final
}

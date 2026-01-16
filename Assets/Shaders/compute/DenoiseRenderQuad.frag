#version 460 core

out vec4 FragColor;
uniform sampler2D screen;
in vec2 texCoord;

//layout(rgba32f, binding = 0) uniform image2DArray outSignals;

//vec4 loadSingal(int layer, ivec2 fragpos)
//{
//    return imageLoad(outSignals, ivec3(fragpos.xy, layer));
//}

void main()
{
	vec4 finaltex = texture(screen, texCoord);

	FragColor = finaltex;
}
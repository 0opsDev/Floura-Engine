#version 460 core

out vec4 FragColor;
uniform sampler2D screen;
in vec2 texCoord;

void main()
{
	vec4 finaltex = texture(screen, texCoord);

//    if (finaltex.a < 0.1) { 
//        discard;
//    }

	FragColor = finaltex;
}
#ifndef RENDERQUAD_CLASS_H
#define RENDERQUAD_CLASS_H

#include <glad/gl.h>

class RenderQuad
{
public:

	static void init();

	static void draw();
	
private:
	static unsigned int VAO, VBO, EBO;
};

#endif // RENDERQUAD_CLASS_H

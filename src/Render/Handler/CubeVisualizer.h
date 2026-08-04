#ifndef CUBEVISUALIZER_CLASS_H
#define CUBEVISUALIZER_CLASS_H


#include <camera/Camera.h>

class CubeVisualizer
{
public:
	static unsigned int cubeVAO, cubeVBO, cubeEBO, instanceVBO;
	
	static void init();

	static void cleanup();

	struct drawCommand{
		glm::mat4 transformation;
		glm::vec3 colour; 
	};
	
	static void uploadDraws(glm::mat4 transformation, glm::vec3 colour, bool hasWireframe);
	
	static void queuedDraws(float thickness, bool fboveride);
	
	static void draw(glm::vec3 position,
		glm::vec3 scale, glm::vec3 colour, float thickness, bool hasWireframe, bool fboveride);
	
	static void draw(glm::mat4 transformation, glm::vec3 colour, float thickness, bool hasWireframe, bool fboveride);
	
private:
	
	static std::vector<drawCommand> drawsList;
	static std::vector<drawCommand> wireframeDrawList;
	static Shader boxShader;
	
	static const float s_Cube_Vertices[24];
	static const unsigned int s_Cube_Indices[36];
	
};

#endif // CUBEVISUALIZER_CLASS_H

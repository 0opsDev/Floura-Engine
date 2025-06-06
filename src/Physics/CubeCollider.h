#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <cmath>   // For abs()
#include <algorithm> // For min()
#include <camera/Camera.h>
#include <Render/Cube/CubeVisualizer.h>
#ifndef CubeCollider_H
#define CubeCollider_H


class CubeCollider
{
public:
	CubeVisualizer CubeVisualizerRenderObject;
	glm::vec3 colliderXYZ = glm::vec3(0,0,0);
	glm::vec3 colliderScale = glm::vec3(1,1,1);
	glm::vec3 lastHit;
	static bool CollideWithCamera;
	static bool showBoxCollider;
	bool checkcollide(glm::vec3 victimXYZ, glm::vec3 victimScale);
	void init();
	void update();
	void draw();
};

#endif // CubeCollider_H
#ifndef PROBE_HANDLER_CLASS_H	
#define PROBE_HANDLER_CLASS_H

#include <iostream>
#include <glm/glm.hpp>
#include <Systems/Physics/Collision.h>
#include <vector>
class ProbeHandler
{
public:

	struct probe
	{
		int size; // same on all sides, also whole number
		glm::ivec3 position; // also whole number
	};

	static bool viewProbes;
	static bool dirtyScene;
	static int sceneProveArea;
	static int probeCalculationMethod;
	static int indirectSamples; // sticking it here for now

	// should take all root nodes, and the scene bounds

	// basic one that takes scene bounds
	static std::vector<ProbeHandler::probe> SceneToProbeSpace(Collision::AABB SceneBounds, int probeArea);


	static std::vector<ProbeHandler::probe> aabbsSceneToProbeSpace(Collision::AABB SceneBounds, std::vector<Collision::AABB> aabbs, int probeArea);

	static std::vector<ProbeHandler::probe> aabbsToProbeSpace(Collision::AABB SceneBounds, std::vector<Collision::AABB> aabbs, int probeArea);

	static std::vector<ProbeHandler::probe> calculateProbesWithMethod(int calculationMethod, Collision::AABB SceneBounds, std::vector<Collision::AABB> aabbs, int probeArea);

private:

	static bool collideWithAnyRootNode(glm::vec3 p, glm::vec3 s, std::vector<Collision::AABB> aabbs);

};
#endif 

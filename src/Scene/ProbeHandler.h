#ifndef PROBE_HANDLER_CLASS_H	
#define PROBE_HANDLER_CLASS_H

#include <glm/glm.hpp>
#include <vector>
#include <Render/Object/texture3D.h>
class ProbeHandler
{
public:
	
	enum probeTypes{
		individualBaked = 0,
		gridBaked = 1,
		individualDynamic = 2,
		gridDynamic = 3
	};
	
	struct probeVolume{
		Texture3D* indirectVolume;
		Texture3D* emissionVolume;
		uint64_t uuid;
		glm::vec3 position = glm::vec3(0.0f);
		glm::vec3 scale = glm::vec3(10.0f);
	};
	
	struct volume{
		glm::vec3 position = glm::vec3(0.0f);
		glm::vec3 scale = glm::vec3(10.0f);
		uint64_t uuid;
		int updateRate = 1;
		
	};
	
	
	
	static GLuint probesSSBO;
	static std::vector<probeVolume> probeVolumes;
	
	static bool dirtyScene;
	static int indirectSamples; // sticking it here for now
	
	static void init();
	static void cleanup();
	
	static uint64_t createProbe(glm::vec3 p, glm::vec3 s, int w, int h, int d);
	static void deleteProbe(uint64_t UUID);
};
#endif 

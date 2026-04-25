#include <string>
#include <Render/Object/ModelAssimp.h>
#include <Render/Object/Billboard.h>
#include <Render/Shader/Material.h>
#include "Systems/Physics/Collision.h"
#include "Render/passes/lighting/raytracer.h"
#include <vector>
#include <Scripting/ScriptObject.h>
#include <Scene/ProbeHandler.h>
//#include <Scene/scene.h>

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#ifndef FE_OBJECT_H
#define FE_OBJECT_H

class entity
{
public:

	//structs
	struct material {
		Material Material;
		glm::vec2 uvScale = glm::vec2(1.0f, 1.0f);
	};

	struct flags {
		bool isStatic = false;
		bool castsShadow = true;
		bool render = true;
		bool doCulling = true;
		bool cullFrontFace = false;
		bool hasCollider = false;
		bool hasPhysics = false;

	};

	struct transformation {
		glm::vec3 position = glm::vec3(0.0f);
		glm::vec3 rotation = glm::vec3(0.0f);
		glm::vec3 scale = glm::vec3(1.0f);
	};

	struct systems {
		transformation previousTransformation;
		transformation transformation;
		material material;
	};

	
	struct physics { // should have physics handler

		float mass = 1.0f;
		// current motion
		glm::vec3 velocity = glm::vec3(0.0f);
		// applied motion
		glm::vec3 force = glm::vec3(0.0f);

		// flags
		bool affectedByGravity = false;
		bool hasRigidbody = false;
	};

	struct collider {
		std::vector<Collision::AABB> boxcolliders; // not used yet
		std::vector<Collision::AABB> rootnodes;
	};

	struct render {
		//Model* Model;
		std::string renderIDString;
		std::string instanceIDString;
			uint64_t renderID;
			uint64_t instanceUUID;
		BillBoard* BillBoard;
		float smoothnessValue = 0.0f;
		bool dirtyTransform = false;
		bool drawInstanced = false;
	};

	struct relationship
	{
		bool hasParent = false;
		uint64_t parentUUID;
		std::vector<uint64_t> childUUID;
	};

	struct components {
		flags flags;
		physics physics;
		collider collider;
		systems systems;
		render render;
		relationship relationship;
	};
	std::string name;

	enum ENT_TYPE_ENUM // i thought this casing style would match the other enums ive seen with the libs i use  
	{
		ENT_MODEL_TYPE	    =  0,
		ENT_BILLBOARD_TYPE  = 1,
		ENT_EMPTY_TYPE			=  2
	};
	
	ENT_TYPE_ENUM type;
	
	// replace type with int and enum
	//char type;
	std::string path;
	std::vector<ScriptObject*> ScriptObjects;

	//

	void createwUUID(uint64_t nUUID, ENT_TYPE_ENUM type, const std::string& name, const std::string& path, const std::string& materialPath);

	void create(ENT_TYPE_ENUM type, const std::string& name, const std::string& path, const std::string& materialPath);

	void LoadMaterial(std::string path);

	void addScript(std::string path, std::string name);

	void reloadScript(int index);

	void removeScript(int index);

	void updateScripts();

	void initScript(int index);

	void update();

	void draw();

	void drawShadowMap();

	void updateLights();

	void Delete();

	//void addParent();

	// aabb vs entity here
	Collision::HitResult AABBVsEntity(glm::vec3 pos, glm::vec3 scale);
	Collision::HitResult RayVsEntity(glm::vec3 rayPos, glm::vec3 rayDir);
	public:
		void updateCollision();
		void updateMeshAABBs();

	components component;
	uint64_t UUID;
	std::string UUIDstring;

	private:
	void createModel(const std::string& path, const std::string& materialPath);
	void createBillBoard(const std::string& path);

	public:

	// transformations
	glm::vec3 fetchPosition() {return component.systems.transformation.position;}
	glm::vec3 fetchRotation() {return component.systems.transformation.rotation;}
	glm::vec3 fetchScale() {return component.systems.transformation.scale;}
	
	void setPosition(const glm::vec3& position) {
		if (position == component.systems.transformation.position) return;
		component.systems.transformation.position = position;
		raytracer::RTGlobalTransformFlag = true;
		component.render.dirtyTransform = true;
		ProbeHandler::dirtyScene = true;
	}
	void setRotation(const glm::vec3& rotation) {
		if (rotation == component.systems.transformation.rotation) return;
		glm::vec3 nr = rotation;
		if (nr.x > 360) nr.x = 0.0f;
		if (nr.y > 360) nr.y = 0.0f;
		if (nr.z > 360) nr.z = 0.0f;
		component.systems.transformation.rotation = nr;
		raytracer::RTGlobalTransformFlag = true;
		component.render.dirtyTransform = true;
		ProbeHandler::dirtyScene = true;
	}
	void setScale(const glm::vec3& scale) {
		if (scale == component.systems.transformation.scale) return;
		component.systems.transformation.scale = scale;
		raytracer::RTGlobalTransformFlag = true;
		component.render.dirtyTransform = true;
		ProbeHandler::dirtyScene = true;
	}
	private:
		void sendEntityUniformsToScripts(ScriptObject* obj);
		void getEntityUniformsToScripts(ScriptObject* obj);
		void initEntityTables(ScriptObject* obj);
};

#endif
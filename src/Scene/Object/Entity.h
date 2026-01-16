#include <string>
#include <Render/Object/ModelAssimp.h>
#include <Render/Object/Billboard.h>
#include <Render/Shader/Material.h>
#include "Systems/Physics/Collision.h"
#include "Render/passes/lighting/raytracer.h"
#include <xhash>
#include <vector>

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
		char type; // b = box, s = sphere, m = mesh, c = capsule etc
		glm::vec3 colliderPosition = glm::vec3(0.0f);
		glm::vec3 colliderScale = glm::vec3(1.0f);

		std::vector<Collision::AABB> rootnodes;
	};

	struct render {
		//Model* Model;
		uint64_t renderID;
		uint64_t instanceUUID;
		std::string renderIDString;
		std::string instanceIDString;
		BillBoard* BillBoard;
		float smoothnessValue = 0.0f;
		bool dirtyTransform = false;
	};

	struct components {
		flags flags;
		physics physics;
		collider collider;
		systems systems;
		render renderHeads;
	};
	std::string name;
	char type;
	std::string path;

	//

	void createwUUID(uint64_t nUUID, const char& type, const std::string& name, const std::string& path, const std::string& materialPath);

	void create(const char& type, const std::string& name, const std::string& path, const std::string& materialPath);

	void LoadMaterial(std::string path);

	void update();

	void draw();

	void drawShadowMap();

	void updateLights();

	void Delete();

	Collision::HitResult RayVsTriangle(glm::vec3 rayPos, glm::vec3 rayDir);
	char fetchType() {return type;}
	std::string fetchPath() {return path;}
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
	glm::vec3 fetchPosition() {
		return component.systems.transformation.position;
	}
	glm::vec3 fetchRotation() {
		return component.systems.transformation.rotation;
	}
	glm::vec3 fetchScale() {
		return component.systems.transformation.scale;
	}
	void setPosition(const glm::vec3& position) {

		if (position == component.systems.transformation.position) return;
		component.systems.transformation.position = position;
		raytracer::RTGlobalTransformFlag = true;
		component.renderHeads.dirtyTransform = true;
	}
	void setRotation(const glm::vec3& rotation) {
		if (rotation == component.systems.transformation.rotation) return;
		component.systems.transformation.rotation = rotation;
		raytracer::RTGlobalTransformFlag = true;
		component.renderHeads.dirtyTransform = true;
	}
	void setScale(const glm::vec3& scale) {
		if (scale == component.systems.transformation.scale) return;
		component.systems.transformation.scale = scale;
		raytracer::RTGlobalTransformFlag = true;
		component.renderHeads.dirtyTransform = true;
	}
};

#endif
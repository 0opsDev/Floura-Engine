#include <string>
#include <Render/Object/ModelAssimp.h>
#include <Render/Object/Billboard.h>
#include <Render/Shader/Material.h>
#include <Scene/IdManager.h>
#include "Physics/Collision.h"


#ifndef FE_OBJECT_H
#define FE_OBJECT_H

/*
TYPES:
m = model
b = billboard
c = collider
p = particle
e = empty // could make d for dummy
s = sound
h = shape
*/


class entity
{
public:

	IdManager::ID ID;

	void create(const char& type, const std::string& name, const std::string& path, const std::string& materialPath);

	void LoadMaterial(std::string path);

	void update();

	void draw();

	void drawShadowMap();

	void updateLights();

	void Delete();

	std::string fetchName();
	void setName(const std::string& name);
	char fetchType();
	std::string fetchPath();


	static struct material {
		Material Material;
		glm::vec2 uvScale = glm::vec2(1.0f, 1.0f);
	};
	

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
		component.systems.transformation.position = position;
	}
	void setRotation(const glm::vec3& rotation) {
		component.systems.transformation.rotation = rotation;
	}
	void setScale(const glm::vec3& scale) {
		component.systems.transformation.scale = scale;
	}
	// billboard specific
	void setDoPitch(bool doPitch) {
		if (type == 'b') {
		component.renderHeads.BillBoard->doPitch = doPitch; // probably because the space in memory hasnt been created for the billboard yet
		}
	}
	bool FetchDoPitch() {
		
		return component.renderHeads.BillBoard->doPitch;
	}
	bool FetchCastsShadow() {
		return component.flags.castsShadow;
	}
	void SetCastsShadow(bool castShadow) {
		component.flags.castsShadow = castShadow;
	}
	bool fetchDoCulling() {
		return component.flags.doCulling;
	}
	void setDoCulling(bool doCulling) {
		component.flags.doCulling = doCulling;
	}
	glm::vec2 fetchUVScale() {
		return component.systems.material.uvScale;;
	}
	void setUVScale(const glm::vec2& uvScale) {
		component.systems.material.uvScale = uvScale;
	}


	private:

	std::string name;
	char type;
	std::string path;

	static struct flags {
		bool isStatic = false;
		bool castsShadow = true;
		bool render = true;
		bool doCulling = true;
		bool cullFrontFace = false;
		bool hasCollider = false;
		bool hasPhysics = false;

	};

	static struct transformation {
		glm::vec3 position = glm::vec3(0.0f);
		glm::vec3 rotation = glm::vec3(0.0f);
		glm::vec3 scale = glm::vec3(1.0f);
	};

	static struct systems {
		transformation transformation;
		material material;
	};

	static struct physics { // should have physics handler

		float mass = 1.0f;
		// current motion
		glm::vec3 velocity = glm::vec3(0.0f);
		// applied motion
		glm::vec3 force = glm::vec3(0.0f);

		// flags
		bool affectedByGravity = false;
		bool hasRigidbody = false;
	};

	static struct collider {
		char type; // b = box, s = sphere, m = mesh, c = capsule etc
		glm::vec3 colliderPosition = glm::vec3(0.0f);
		glm::vec3 colliderScale = glm::vec3(1.0f);
	};
	public:
		void updateCollision();
		void updateMeshAABBs();
	private:

	static struct render {
		Model* Model;
		BillBoard* BillBoard;
		float smoothnessValue = 0.0f;
	};

	public:
	static struct components {
		flags flags;
		physics physics;
		collider collider;
		systems systems;
		render renderHeads;
	};

	components component;

	private:
	void createModel(const std::string& path, const std::string& materialPath);

	void createBillBoard(const std::string& path);
};

#endif
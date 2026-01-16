#include "Entity.h"
#include <scene/scene.h>
#include <utils/FE_math.h>
#include <utils/logConsole.h>
#include <Scene/LightingHandler.h>
#include <Render/passes/geometry/geometryPass.h>
#include <Gameplay/Player.h>
#include "Systems/util/UUID.h"
#include "Render/passes/lighting/raytracer.h"
#include <Render/Handler/RenderHandler.h>

void entity::createwUUID(uint64_t nUUID, const char& type, const std::string& name, const std::string& path, const std::string& materialPath)
{
	entity::UUID = nUUID;
	UUIDstring = UUID::UUIDToString(UUID);
	//std::cout << UUIDstring << std::endl;

	// set type
	entity::type = type;
	// set name 
	entity::name = name;
	// set path 
	entity::path = path;

	switch (type)
	{
	case 'm': // model
		createModel(path, materialPath);
		break;

	case 'b': // billboard
		createBillBoard(path);
		break;
	default:
		LogConsole::print("Entity Create: Unknown type '" + std::string(1, type) + "' for entity: " + name);
		break;
	}
	//raytracer::RTGlobalTransformFlag = true;
}


void entity::create(const char& type, const std::string& name, const std::string& path, const std::string& materialPath)
{
	UUID = UUID::returnHandle();
	UUIDstring = UUID::UUIDToString(UUID);
	//std::cout << UUIDstring << std::endl;

	// set type
	entity::type = type;
	// set name 
	entity::name = name;
	// set path 
	entity::path = path;
	
	switch (type)
	{
	case 'm': // model
		createModel(path, materialPath);
		break;

	case 'b': // billboard
		createBillBoard(path);
		break;
	default:
		LogConsole::print("Entity Create: Unknown type '" + std::string(1, type) + "' for entity: " + name);
		break;
	}
	raytracer::RTGlobalTransformFlag = true;
}

void entity::LoadMaterial(std::string path)
{
	component.systems.material.Material.LoadMaterial(path);
}

void entity::update()
{
	entity::updateMeshAABBs();
	if (component.physics.hasRigidbody) // change name to hasdynamics
	{
		if (component.physics.affectedByGravity)
		{
			glm::vec3 gravity = glm::vec3(0.0f, -9.81f, 0.0f);

			component.physics.force += component.physics.mass * gravity; // applying foce
		}

		component.physics.velocity += component.physics.force / component.physics.mass * TimeUtil::deltatime;
		component.systems.transformation.position += component.physics.velocity * TimeUtil::deltatime;

		component.physics.force = glm::vec3(0.0f); // reset force at end
	}

	// update mesh positions here vv
	switch (type)
	{
	case 'm': // model
	{
		int index = RenderHandler::fetchModelIndex(component.renderHeads.renderID);
		if (index != -1)
		{
			RenderHandler::models[index].model->updatePosition(component.systems.transformation.position);
			RenderHandler::models[index].model->updateRotation(component.systems.transformation.rotation);
			RenderHandler::models[index].model->updateScale(component.systems.transformation.scale);
			RenderHandler::models[index].model->updateTranformation();
			RenderHandler::models[index].model->updateMeshAABBs();
			raytracer::updateboundingboxes(component.renderHeads.instanceUUID, component.collider.rootnodes);
			raytracer::modelMatrixUpdate(component.renderHeads.instanceUUID, RenderHandler::models[index].model->gModelMatrix);
		}

		break;
	}
	case 'b': // billboard
		component.renderHeads.BillBoard->updatePosition(component.systems.transformation.position);
		component.renderHeads.BillBoard->updateScale(component.systems.transformation.scale);
		break;
	default:
		break;
	}
}

void entity::updateLights()
{
	component.systems.material.Material.updateForwardLights();
}

void entity::Delete()
{
	switch (type)
	{
	case 'm': // model
	{
		// 
		RenderHandler::removeInstancewRenderID(component.renderHeads.renderID);

		int index = RenderHandler::fetchModelIndex(component.renderHeads.renderID);
		if (index != -1)
		{
			raytracer::removeFromRaytracer(component.renderHeads.instanceUUID);
		}
		//delete component.renderHeads.Model;
		//component.renderHeads.Model = nullptr;
		component.systems.material.Material.ClearMaterial();
		break;
	}
	case 'b': // billboard
		delete component.renderHeads.BillBoard;
		component.renderHeads.BillBoard = nullptr;
		break;
	}
	raytracer::RTGlobalTransformFlag = true;
}

Collision::HitResult entity::RayVsTriangle(glm::vec3 rayPos, glm::vec3 rayDir)
{
	Collision::HitResult finalResult;
	finalResult.isColliding = false;
	finalResult.distance = std::numeric_limits<float>::max();	
	int index = RenderHandler::fetchModelIndex(component.renderHeads.renderID);
	if (index != -1)
	{

		glm::mat4 gModelMatrix = FE_Math::composeMatrixWDegrees(RenderHandler::models[index].model->globalTransformation.position,
			RenderHandler::models[index].model->globalTransformation.scale, RenderHandler::models[index].model->globalTransformation.rotation);

		// for each mesh
		for (size_t x = 0; x < RenderHandler::models[index].model->meshes.size(); x++)
		{
			// final transformation
			glm::mat4 finalMatrix = gModelMatrix * RenderHandler::models[index].model->lModelMatrix[x]; // * by local transform

			// AABB to speed things up
			Collision::HitResult AABB = Collision::AABBvsRay(component.collider.rootnodes[x].position, component.collider.rootnodes[x].size, rayPos, rayDir);
			if (AABB.isColliding)
			{
				for (size_t y = 0; y < RenderHandler::models[index].model->meshes[x].indices.size(); y += 3)
				{
					unsigned int i0 = RenderHandler::models[index].model->meshes[x].indices[y];
					unsigned int i1 = RenderHandler::models[index].model->meshes[x].indices[y + 1];
					unsigned int i2 = RenderHandler::models[index].model->meshes[x].indices[y + 2];

					if (i0 >= RenderHandler::models[index].model->meshes[x].vertices.size() ||
						i1 >= RenderHandler::models[index].model->meshes[x].vertices.size() ||
						i2 >= RenderHandler::models[index].model->meshes[x].vertices.size()) {
						continue;
					}


					glm::vec3 a = RenderHandler::models[index].model->meshes[x].vertices[i0].position;
					glm::vec3 b = RenderHandler::models[index].model->meshes[x].vertices[i1].position;
					glm::vec3 c = RenderHandler::models[index].model->meshes[x].vertices[i2].position;

					FE_Math::transformPoint(a, finalMatrix);
					FE_Math::transformPoint(b, finalMatrix);
					FE_Math::transformPoint(c, finalMatrix);

					// run hit test and return result
					Collision::HitResult triHit = Collision::RayVsTriangle(rayPos, rayDir, a, b, c);

					if (triHit.isColliding && triHit.distance < finalResult.distance)
					{
						finalResult = triHit;
					}
				}
			}
		}
	}
	return finalResult;
}


void entity::draw()
{
	if (!component.flags.render) return;

	switch (type)
	{
	case 'm': // model
	{
		RenderHandler::renderQueueData newRenderData;
		newRenderData.RenderID = component.renderHeads.renderID;
		newRenderData.shaderUUID = component.systems.material.Material.modelShaderUUID;
		newRenderData.gpShaderUUID = component.systems.material.Material.modelGpassShaderUUID;
		newRenderData.castsShadow = component.flags.castsShadow;
		newRenderData.cullFrontFace = component.flags.cullFrontFace;
		newRenderData.doCulling = component.flags.doCulling;
		//newRenderData.isInstanced;
		newRenderData.position = component.systems.transformation.position;
		newRenderData.rotation = component.systems.transformation.rotation;
		newRenderData.scale = component.systems.transformation.scale;
		newRenderData.smoothnessValue = component.renderHeads.smoothnessValue;
		newRenderData.uvScale = component.systems.material.uvScale;
		RenderHandler::addToRenderQueue(newRenderData);

		int index = RenderHandler::fetchModelIndex(component.renderHeads.renderID);
		if (index != -1)
		{
			raytracer::uvScaleUpdate(component.renderHeads.instanceUUID, component.systems.material.uvScale);
		}
		for (size_t i = 0; i < component.collider.rootnodes.size(); i++)
		{
			if (Collision::showBoxCollider)
				RenderClass::WhiteCube->draw(component.collider.rootnodes[i].position,
					component.collider.rootnodes[i].size, glm::vec3(1.0f));
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		break;
	}
	case 'b': // billboard
	{
		component.renderHeads.BillBoard->draw();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		break;
	}
	}
}

void entity::drawShadowMap()
{
	if (!component.flags.castsShadow) return;
	switch (type)
	{
	case 'm': // model
		//LightingHandler::drawShadowMap(component.renderHeads.Model, component.systems.transformation.position, component.systems.transformation.rotation, component.systems.transformation.scale);
		break;
	case 'b': // billboard
		LightingHandler::drawShadowMapBillboard(component.renderHeads.BillBoard, component.systems.transformation.position, component.systems.transformation.scale);
		break;
	}
}

// fetch model matrix,
// fetch model data

void entity::updateCollision()
{
	entity::updateMeshAABBs();
	switch (type)
	{
	case 'm':
	{
		int index = RenderHandler::fetchModelIndex(component.renderHeads.renderID);
		if (index != -1)
		{

			// just for now do camera vs aabb from meshes, in future there should
		// be a collision handler, that does objects vs objects for aabbs
			for (size_t i = 0; i < component.collider.rootnodes.size(); i++)
			{
				// Calculate global position and scale

				// these two are bugged

				glm::vec3 globalPosition = component.collider.rootnodes[i].position;
				//glm::vec3 globalPosition = component.systems.transformation.position * component.renderHeads.Model->MeshAABBs[i].position;
				//glm::vec3 globalPosition = component.renderHeads.Model->MeshAABBs[i].position;
				//glm::vec3 globalPosition = component.systems.transformation.position;

				glm::vec3 globalSize = component.collider.rootnodes[i].size;


				//std::cout << "x" << component.renderHeads.Model->MeshAABBs[i].position.x << std::endl;

				// Collision check using global position and scale
				Collision::HitResult collisionData = Collision::AABBvsAABB(globalPosition, globalSize,
					glm::vec3(Scene::maincamera.Position.x, (Scene::maincamera.Position.y - (Player::cameraColliderScale.y / 2.0f)), Scene::maincamera.Position.z),
					Player::cameraColliderScale);

				// Handle collision logic
				if (collisionData.isColliding)
				{
					Scene::maincamera.Position = glm::vec3(collisionData.lastHit.x,
						(collisionData.lastHit.y + (Player::cameraColliderScale.y / 2.0f)),
						collisionData.lastHit.z);

					RenderClass::WhiteCube->draw(collisionData.lastHit, glm::vec3(0.1f), glm::vec3(1.0f, 0.0f, 0.0f));

					if (collisionData.collisionNormal == glm::vec3(0.0f, 1.0f, 0.0f)) // up or down
						Player::isColliding = true; // Set collision state
				}
			}
		}
		//RenderClass::line->draw(glm::vec3(1.0f, 0.0f, 0.0f));
		break;
	}

	default:
		break;
	}


}

void entity::updateMeshAABBs()
{
	if (component.renderHeads.dirtyTransform)
	{
		component.renderHeads.dirtyTransform = false;

		if (type == 'm')
		{
			int index = RenderHandler::fetchModelIndex(component.renderHeads.renderID);
			if (index != -1)
			{

				RenderHandler::models[index].model->updatePosition(component.systems.transformation.position);
				RenderHandler::models[index].model->updateRotation(component.systems.transformation.rotation);
				RenderHandler::models[index].model->updateScale(component.systems.transformation.scale);
				RenderHandler::models[index].model->updateTranformation();
				RenderHandler::models[index].model->updateMeshAABBs();
				component.collider.rootnodes = RenderHandler::models[index].model->rootnodes;
			}
			//component.renderHeads.Model->updateMeshAABBs();
		}
	}

}

void entity::createModel(const std::string& path, const std::string& materialPath)
{
	RenderHandler::batchOfUUID newBatchOfUUID = RenderHandler::addModel(path);
	component.renderHeads.renderID = newBatchOfUUID.RenderID;
	component.renderHeads.instanceUUID = newBatchOfUUID.instanceUUID;
	component.renderHeads.renderIDString = UUID::UUIDToString(newBatchOfUUID.RenderID);
	component.renderHeads.instanceIDString = UUID::UUIDToString(newBatchOfUUID.instanceUUID);
	component.renderHeads.dirtyTransform = true;
	entity::updateMeshAABBs();
	component.systems.material.Material.LoadMaterial(materialPath);

	int index = RenderHandler::fetchModelIndex(component.renderHeads.renderID);
	if (index != -1)
	{
		raytracer::uploadToRaytracer(newBatchOfUUID.instanceUUID);
	}
}

void entity::createBillBoard(const std::string& path)
{
	component.renderHeads.BillBoard = new BillBoard(path);
}
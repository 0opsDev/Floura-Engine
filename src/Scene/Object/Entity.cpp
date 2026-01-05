#include "Entity.h"
#include <scene/scene.h>
#include <utils/FE_math.h>
#include <utils/logConsole.h>
#include <Scene/LightingHandler.h>
#include <Render/passes/geometry/geometryPass.h>
#include <Gameplay/Player.h>
#include "Systems/util/UUID.h"
#include "Render/passes/lighting/raytracer.h"

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
}

void entity::LoadMaterial(std::string path)
{
	component.systems.material.Material.LoadMaterial(path);
}

void entity::update()
{
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
		component.renderHeads.Model->updatePosition(component.systems.transformation.position);
		component.renderHeads.Model->updateRotation(component.systems.transformation.rotation);
		component.renderHeads.Model->updateScale(component.systems.transformation.scale);
		component.renderHeads.Model->updateTranformation();
		component.renderHeads.Model->updateMeshAABBs();
		raytracer::updateboundingboxes(component.renderHeads.Model);
		raytracer::modelMatrixUpdate(component.renderHeads.Model->UUID, component.renderHeads.Model->gModelMatrix);

		break;
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
		// 
		raytracer::removeFromRaytracer(component.renderHeads.Model->UUID);
		delete component.renderHeads.Model;
		component.systems.material.Material.ClearMaterial();
		component.renderHeads.Model = nullptr;
		break;
	case 'b': // billboard
		delete component.renderHeads.BillBoard;
		component.renderHeads.BillBoard = nullptr;
		break;
	}
}

Collision::HitResult entity::RayVsTriangle(glm::vec3 rayPos, glm::vec3 rayDir)
{
	Collision::HitResult finalResult;
	finalResult.isColliding = false;
	finalResult.distance = std::numeric_limits<float>::max();	

	glm::mat4 gModelMatrix = FE_Math::composeMatrixWDegrees(component.renderHeads.Model->globalTransformation.position, 
		component.renderHeads.Model->globalTransformation.scale, component.renderHeads.Model->globalTransformation.rotation);

	// for each mesh
	for (size_t x = 0; x < component.renderHeads.Model->meshes.size(); x++)
	{
		// final transformation
		glm::mat4 finalMatrix = gModelMatrix * component.renderHeads.Model->lModelMatrix[x]; // * by local transform

		// AABB to speed things up
		Collision::HitResult AABB = Collision::AABBvsRay(component.renderHeads.Model->meshes[x].boxCollider.position, component.renderHeads.Model->meshes[x].boxCollider.size, rayPos, rayDir);
		if (AABB.isColliding)
		{
			for (size_t y = 0; y < component.renderHeads.Model->meshes[x].indices.size(); y += 3)
			{
				unsigned int i0 = component.renderHeads.Model->meshes[x].indices[y];
				unsigned int i1 = component.renderHeads.Model->meshes[x].indices[y + 1];
				unsigned int i2 = component.renderHeads.Model->meshes[x].indices[y + 2];

				if (i0 >= component.renderHeads.Model->meshes[x].vertices.size() ||
					i1 >= component.renderHeads.Model->meshes[x].vertices.size() ||
					i2 >= component.renderHeads.Model->meshes[x].vertices.size()) {
					continue;
				}


				glm::vec3 a = component.renderHeads.Model->meshes[x].vertices[i0].position;
				glm::vec3 b = component.renderHeads.Model->meshes[x].vertices[i1].position;
				glm::vec3 c = component.renderHeads.Model->meshes[x].vertices[i2].position;

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

	return finalResult;
}


void entity::draw()
{

	if (!component.flags.render) return;

	switch (type)
	{
	case 'm': // model
		Skybox::bind(5);
		Skybox::cubemapToShader(component.systems.material.Material.ModelShader, 5);
		component.systems.material.Material.update();

		if (!RenderClass::DoForwardLightingPass && !RenderClass::DoDeferredLightingPass) return; // Skip rendering if not in regular or lighting pass
		if (component.flags.doCulling == true && !FEImGuiWindow::isWireframe) glEnable(GL_CULL_FACE);
		else glDisable(GL_CULL_FACE);
		if (component.flags.cullFrontFace) glCullFace(GL_FRONT);
		else glCullFace(GL_BACK);

		if (FEImGuiWindow::isWireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Enable wireframe mode

		component.systems.material.Material.ModelShader.Activate();
		component.systems.material.Material.ModelShader.setFloat2("uvScale", component.systems.material.uvScale);
		component.systems.material.Material.ModelShader.setFloat("smoothnessValue", component.renderHeads.smoothnessValue);
		//smoothnessValue
		component.systems.material.Material.ModelGpassShader.Activate();
		component.systems.material.Material.ModelGpassShader.setFloat2("uvScale", component.systems.material.uvScale);

		if (RenderClass::DoForwardLightingPass) {
			glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::FBO);
			component.systems.material.Material.ModelShader.Activate();
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);
			component.renderHeads.Model->draw(component.systems.material.Material.ModelShader);
			// Draw the mesh bounding box for visualization
			for (size_t i = 0; i < component.renderHeads.Model->meshes.size(); i++)
			{
				if (Collision::showBoxCollider)
					RenderClass::WhiteCube->draw(component.renderHeads.Model->meshes[i].boxCollider.position,
						component.renderHeads.Model->meshes[i].boxCollider.size, glm::vec3(1.0f));
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}


		}
		component.systems.material.Material.ModelGpassShader.Activate();
		GeometryPass::gPassDraw(component.renderHeads.Model, component.systems.material.Material.ModelGpassShader);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Enable wireframe mode

		//glFrontFace(GL_CCW);
		glCullFace(GL_BACK); // Reset culling to default
		glDisable(GL_CULL_FACE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		Skybox::unbind();
		break;
	case 'b': // billboard
		component.renderHeads.BillBoard->draw();
		break;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

void entity::drawShadowMap()
{
	if (!component.flags.castsShadow) return;
	switch (type)
	{
	case 'm': // model
		LightingHandler::drawShadowMap(component.renderHeads.Model, component.systems.transformation.position, component.systems.transformation.rotation, component.systems.transformation.scale);
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
		// just for now do camera vs aabb from meshes, in future there should
	// be a collision handler, that does objects vs objects for aabbs
		for (size_t i = 0; i < component.renderHeads.Model->MeshAABBs.size(); i++)
		{
			// Calculate global position and scale

			// these two are bugged

			glm::vec3 globalPosition = component.renderHeads.Model->MeshAABBs[i].position;
			//glm::vec3 globalPosition = component.systems.transformation.position * component.renderHeads.Model->MeshAABBs[i].position;
			//glm::vec3 globalPosition = component.renderHeads.Model->MeshAABBs[i].position;
			//glm::vec3 globalPosition = component.systems.transformation.position;

			glm::vec3 globalSize = component.renderHeads.Model->MeshAABBs[i].size;


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
		//RenderClass::line->draw(glm::vec3(1.0f, 0.0f, 0.0f));
		break;

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
			component.renderHeads.Model->updateMeshAABBs();
	}

}

void entity::createModel(const std::string& path, const std::string& materialPath)
{
	component.systems.material.Material.LoadMaterial(materialPath);
	component.renderHeads.Model = new Model(path.c_str());

	component.renderHeads.Model->createMeshAABBs();
	//entity::updateMeshAABBs();

	// update transformation for raytracer
	component.renderHeads.Model->updatePosition(component.systems.transformation.position);
	component.renderHeads.Model->updateRotation(component.systems.transformation.rotation);
	component.renderHeads.Model->updateScale(component.systems.transformation.scale);
	component.renderHeads.Model->updateTranformation();
	raytracer::uploadToRaytracer(component.renderHeads.Model);
}

void entity::createBillBoard(const std::string& path)
{
	component.renderHeads.BillBoard = new BillBoard(path);
}
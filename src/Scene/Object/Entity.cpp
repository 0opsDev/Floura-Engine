#include "Entity.h"
#include <scene/scene.h>
#include <utils/FE_math.h>
#include <utils/logConsole.h>
#include <Scene/LightingHandler.h>
#include <Render/passes/geometry/geometryPass.h>
#include <Gameplay/Player.h>

void entity::create(const char& type, const std::string& name, const std::string& path, const std::string& materialPath)
{
	ID.ObjType = 'o';
	ID.index = Scene::entityObjects.size(); // fetch array size from scene based on type
	IdManager::AddID(ID);

	// set type
	entity::type = type;
	// set name 
	entity::name = name;
	// set path 
	entity::path = path;

	// load material
	

	/*
	TYPES:
	m = model
	b = billboard
	c = collider
	p = particle
	e = empty
	*/
	switch (type)
	{
	case 'm': // model
		createModel(path, materialPath);
		break;

	case 'b': // billboard
		createBillBoard(path);
		break;
	case 'e': // empty

		break;
	case 'c': // collider
		break;
	case 'p': // particle
		break;
	default:
		LogConsole::print("Entity Create: Unknown type '" + std::string(1, type) + "' for entity: " + name);
		break;
	}

	
}

void entity::LoadMaterial(std::string path)
{
	// "Assets/Material/Default.Material" example path
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

		component.physics.velocity += component.physics.force / component.physics.mass * TimeUtil::s_DeltaTime;
		component.systems.transformation.position += component.physics.velocity * TimeUtil::s_DeltaTime;

		component.physics.force = glm::vec3(0.0f); // reset force at end
	}
	updateCollision();

	// update mesh positions here vv
	switch (type)
	{
	case 'm': // model
		component.renderHeads.Model->updatePosition(component.systems.transformation.position);
		component.renderHeads.Model->updateRotation(component.systems.transformation.rotation);
		component.renderHeads.Model->updateScale(component.systems.transformation.scale);
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
		delete component.renderHeads.Model;
		component.systems.material.Material.ClearMaterial();
		component.renderHeads.Model = nullptr;
		break;
	case 'b': // billboard
		delete component.renderHeads.BillBoard;
		component.renderHeads.BillBoard = nullptr;
		break;
	}
	//update lowest free index
	if (ID.index < IdManager::lowestDeletedIndex.object || IdManager::lowestDeletedIndex.BillBoard == -1) {
		IdManager::lowestDeletedIndex.object = ID.index;
		LogConsole::print("Lowest Deleted object Index is now: " + std::to_string(IdManager::lowestDeletedIndex.object));
	}

	IdManager::RemoveID(ID);
	//IdManager::lowestBillBoardIndexSync(); // sync up the index after deletion because the array has now changed
}


void entity::draw()
{

	if (!component.flags.render)
	{
		return;
	}

	switch (type)
	{
	case 'm': // model
		Skybox::bind(5);
		Skybox::cubemapToShader(component.systems.material.Material.ModelShader, 5);
		component.systems.material.Material.update();

		if (!RenderClass::DoForwardLightingPass && !RenderClass::DoDeferredLightingPass) {
			return; // Skip rendering if not in regular or lighting pass
		}


		if (component.flags.doCulling == true && !FEImGuiWindow::isWireframe) { glEnable(GL_CULL_FACE); }
		else { glDisable(GL_CULL_FACE); }

		if (component.flags.cullFrontFace) { glCullFace(GL_FRONT); }
		else { glCullFace(GL_BACK); }

		if (FEImGuiWindow::isWireframe) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Enable wireframe mode
		}

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
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

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
	}
}

void entity::drawShadowMap()
{
	if (!component.flags.castsShadow)
	{
		return;
	}
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
// getters
std::string entity::fetchName()
{
	return name;
}
char entity::fetchType()
{
	return type;
}
std::string entity::fetchPath()
{
	return path;
}

// fetch model matrix,
// fetch model data

// setters
void entity::setName(const std::string& name)
{
	entity::name = name;
}

void entity::updateCollision()
{
	switch (type)
	{
	case 'm':
		// just for now do camera vs aabb from meshes, in future there should
	// be a collision handler, that does objects vs objects for aabbs
		for (size_t i = 0; i < component.renderHeads.Model->MeshAABBs.size(); i++)
		{
			// Calculate global position and scale

			// these two are bugged

			glm::vec3 globalPosition = component.systems.transformation.position + component.renderHeads.Model->MeshAABBs[i].position;
			//glm::vec3 globalPosition = component.systems.transformation.position * component.renderHeads.Model->MeshAABBs[i].position;
			//glm::vec3 globalPosition = component.renderHeads.Model->MeshAABBs[i].position;
			//glm::vec3 globalPosition = component.systems.transformation.position;

			glm::vec3 globalSize = component.renderHeads.Model->MeshAABBs[i].size *
				component.systems.transformation.scale;


			//std::cout << "x" << component.renderHeads.Model->MeshAABBs[i].position.x << std::endl;

			// Collision check using global position and scale
			Collision::HitResult collisionData = Collision::AABBvsAABB(globalPosition, globalSize,
				glm::vec3(Camera::Position.x, (Camera::Position.y - (Player::cameraColliderScale.y / 2.0f)), Camera::Position.z),
				Player::cameraColliderScale);

			// Draw the mesh bounding box for visualization
			RenderClass::WhiteCube->draw(globalPosition,
				globalSize, glm::vec3(1.0f));

			// Handle collision logic
			if (collisionData.isColliding)
			{
				Camera::Position = glm::vec3(collisionData.lastHit.x,
					(collisionData.lastHit.y + (Player::cameraColliderScale.y / 2.0f)),
					collisionData.lastHit.z);

				if (collisionData.collisionNormal == glm::vec3(-1.0f, 0.0f, 0.0f) || collisionData.collisionNormal == glm::vec3(1.0f, 0.0f, 0.0f)) // left or right
					RenderClass::line->translate(collisionData.lastHit, glm::vec3(1.0f), glm::vec3(90.0f, 0.0f, 0.0f));

				if (collisionData.collisionNormal == glm::vec3(0.0f, -1.0f, 0.0f) || collisionData.collisionNormal == glm::vec3(0.0f, 1.0f, 0.0f)) // up or down
					RenderClass::line->translate(collisionData.lastHit, glm::vec3(1.0f), glm::vec3(0.0f, 0.0f, 90.0f));

				if (collisionData.collisionNormal == glm::vec3(0.0f, 0.0f, -1.0f) || collisionData.collisionNormal == glm::vec3(0.0f, 0.0f, 1.0f)) // front or back
					RenderClass::line->translate(collisionData.lastHit, glm::vec3(1.0f), glm::vec3(0, 90.0f, 0.0f));

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
	if (type == 'm')
	{
		component.renderHeads.Model->updateMeshAABBs();
	}
}

void entity::createModel(const std::string& path, const std::string& materialPath)
{
	component.systems.material.Material.LoadMaterial(materialPath);
	component.renderHeads.Model = new Model(path.c_str());
	//entity::updateMeshAABBs();
}

void entity::createBillBoard(const std::string& path)
{
	component.renderHeads.BillBoard = new BillBoard(path);
}
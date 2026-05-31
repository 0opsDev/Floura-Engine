#include "FE_LAYER.h"
#include "Core/Render.h"
#include "camera/Camera.h"
#include "utils/FE_math.h"
#include "Render/window/WindowHandler.h"
#include <Scene/Object/Entity.h>
#include "Scene/scene.h"
#include "Render/Cube/CubeVisualizer.h"
#include <Render/Animated/animator.h>
#include "Render/Handler/RenderHandler.h"
#include "utils/timeUtil.h"
#include "render/procedural/terrain.h"
#include "render/procedural/ocean.h"
#include "Systems/Physics/physworld.h"


//uint64_t gunID = 0;
//uint64_t renderID = 0;

uint64_t pivotPointID = 0;
uint64_t victimPointID = 0;

ocean* t;
Shader temporaryTerrainShader;
int size = 50;

void FE_LAYER::init(){
	
	return;
	
	physworld::emitter estrogenEmitter;
	estrogenEmitter.enabled = true;
	estrogenEmitter.position = glm::vec3(0.0f, 5.0f, 0.0f);
	estrogenEmitter.UUID = UUID::returnHandle();
	estrogenEmitter.lifespan = 1000.0f;
	estrogenEmitter.maxDistance = 20.0f;
	estrogenEmitter.spawnTickrate = 30.0f;
	estrogenEmitter.limit = 64;
	estrogenEmitter.templatePhysicsObject.affectedByGravity = true;
	//estrogenEmitter.gravity  = glm::vec3(0.0, 5.81, 0.0f);
	
	physworld::uploadEmitter(estrogenEmitter);
	//return;
	int radius = 4;
	
	for (int x = -radius; x < radius; x+=2)
		for (int y = -radius; y < radius; y+=2)
		{
			estrogenEmitter.position = glm::vec3(x, 0.0f, y);	
			physworld::uploadEmitter(estrogenEmitter);
		}
	
	
	return;
	victimPointID = Scene::AddEntityObject(entity::ENT_MODEL_TYPE,"VictimPoint (FE_LAYER.CPP)", "Assets/Models/pdf_teto/scene.gltf", glm::vec3(0.0f, 0.0f, 0.0f),glm::vec3(0.100), glm::vec3(0.0f) );
	pivotPointID = Scene::AddEntityObject(entity::ENT_MODEL_TYPE,"PivPoint (FE_LAYER.CPP)", "Assets/Models/basic shapes/sphere.gltf", glm::vec3(0.0f, 5.0f, 0.0f),glm::vec3(1.0), glm::vec3(0.0f) );
	
	for (int i = 0; i < Scene::entityObjects.size(); ++i) 
		if (victimPointID == Scene::entityObjects[i]->UUID){
			Scene::entityObjects[i]->component.physobject.hasRigidbody = true;
			Scene::entityObjects[i]->component.physobject.affectedByGravity = true;
		}
	
	return;
	
	t = new ocean(20,size);
	
	temporaryTerrainShader.LoadShaderGeom("Assets/Shaders/Db/VerticeViewer.vert","Assets/Shaders/Db/VertexViewer.frag", "Assets/Shaders/Db/VerticeViewer.geom"); 
	//gunID = Scene::AddEntityObject(entity::ENT_MODEL_TYPE,"gun_loaded_from_cpp", "temp/Capoeira.fbx", glm::vec3(0.0f),glm::vec3(0.01), glm::vec3(0.0f) );

	//for (int i = 0; i < Scene::entityObjects.size(); ++i) if (gunID == Scene::entityObjects[i]->UUID) renderID = Scene::entityObjects[i]->component.render.renderID;
	
	//int modelIndex =RenderHandler::fetchModelIndex(renderID);
	//if (modelIndex != -1)
	//{
		
		// outside here one of these VV doesnt intergrade well, hangs ev everything
		
		
		//Animation danceAnimation("temp/Capoeira.fbx",RenderHandler::models[modelIndex].model);
		//Animator animator(&danceAnimation);
		//animator.PlayAnimation(&danceAnimation);
		//animator.UpdateAnimation(TimeUtil::deltatime);
		//Animator animator(&danceAnimation);
		
		//animator.init(&danceAnimation);
	//}
	
}

static void temporaryUpdateFunction(ocean* inputT, glm::vec3 p, glm::vec3 S, glm::vec3 R)
{
	inputT->updatePosition(p);
	inputT->updateScale(S);
	inputT->updateRotation(R);

	inputT->updateTranformation();
	
	inputT->tagChunkLODLevels(50.0f, Scene::maincamera.Position);
}

//glm::vec3 position = glm::vec3(0.0f, 0.0f, 2.0f);
void FE_LAYER::Update() {
	return;
	glm::vec3 pPos = glm::vec3(0.0f, 0.0f, 0.0f);
	
	for (int i = 0; i < Scene::entityObjects.size(); ++i)  if (pivotPointID == Scene::entityObjects[i]->UUID) pPos = Scene::entityObjects[i]->fetchPosition();
	
	for (int i = 0; i < Scene::entityObjects.size(); ++i) 
		if (victimPointID == Scene::entityObjects[i]->UUID){
			// advancedConstrainPoint
			Collision::HitResult HR = Collision::advancedConstrainPoint(Scene::entityObjects[i]->fetchPosition(),glm::vec3(pPos), 10.0f);
			Scene::entityObjects[i]->setPosition(HR.lastHit);
			
			//Scene::entityObjects[i]->setPosition(Collision::constrainPoint(Scene::entityObjects[i]->fetchPosition(),glm::vec3(pPos), 10.0f));
			
			if (HR.isColliding){
				glm::vec3& vel = Scene::entityObjects[i]->component.physobject.velocity;
				float dot = glm::dot(vel, HR.collisionNormal);
				
				if (dot < 0.0f) vel -= dot * HR.collisionNormal;

				Scene::entityObjects[i]->component.physobject.force += (-HR.collisionNormal * HR.depth * 50.0f);
			}
		}
	
	return;
	temporaryUpdateFunction(t, glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0), glm::vec3(0.0));
	
	/*
	glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::FBO);

	float closestDist = std::numeric_limits<float>::max();
	Collision::HitResult closestResult;
	bool hitAnything = false;

	glm::vec3 rayPos = Camera::Position;
	glm::vec3 rayDir = Camera::Orientation;

	//glm::vec3 rayPos = glm::vec3(0.0f, 10.0f, 0.0f);
	//glm::vec3 rayDir = glm::vec3(0.0f, -1.0f, 0.0f);

	for (size_t i = 0; i < Scene::entityObjects.size(); i++)
	{
		if (glfwGetMouseButton(windowHandler::window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{

			Collision::HitResult result = Scene::entityObjects[i]->RayVsTriangle(rayPos, rayDir);

			if (result.isColliding && result.distance < closestDist) {
				{
					closestDist = result.distance;
					closestResult = result;
					hitAnything = true;
					
				}
			}
		}

		if (hitAnything) {
			cube->draw(closestResult.lastHit, glm::vec3(0.1f), glm::vec3(1.0f, 0.0f, 0.0f));
			std::cout << "hit" << std::endl;
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	*/

}
static void temporaryPreviousUpdateFunction(ocean* inputT, glm::vec3 p, glm::vec3 S, glm::vec3 R)
{
	
	inputT->updatePrevPosition(p);
	inputT->updatePrevScale(S);
	inputT->updatePrevRotation(R);

	inputT->updatePrevTranformation();
}
void FE_LAYER::onBeginningOfFrame()
{
	return;
	temporaryUpdateFunction(t, glm::vec3(0.0), glm::vec3(1.0), glm::vec3(0.0));
}


static void temporaryDrawFunction(ocean* inputT)
{
	glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::FBO);
	
	inputT->draw(temporaryTerrainShader , Scene::maincamera);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}



void FE_LAYER::draw()
{
	physworld::debugDraw();
	return;
	temporaryDrawFunction(t);
	
	t->dbgChunkDraw();
	
//	t->pointDraw();
}

void FE_LAYER::Delete()
{
	//delete cube;
	//cube = nullptr;
}

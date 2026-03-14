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

//uint64_t gunID = 0;
//uint64_t renderID = 0;

ocean* t;
Shader temporaryTerrainShader;
int size = 50;

void FE_LAYER::init(){

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

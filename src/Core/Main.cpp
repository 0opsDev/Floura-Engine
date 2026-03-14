#include "Main.h"
#include "utils/timeUtil.h" 
#include <thread>
#include <chrono>
#include "File/File.h"
#include <Editor/UI/ImGui/ImGuiWindow.h>
#include "Render.h"
#include "scene/FE_LAYER.h"
#include "scene/scene.h"
#include <Gameplay/Player.h>
#include <Render/window/WindowHandler.h>
#include <Scene/LightingHandler.h>
#include "Editor/UI/ImGui/OpenSceneWindow.h"
#include "Sound/SoundRunner.h"

bool Main::sleepState = true;


int main()
{

	//CloseConsoleWindow();
	Main::sleepState = true;
	FileClass::loadSettings();
	SoundRunner::init();
	RenderClass::init(windowHandler::width, windowHandler::height);
	Scene::maincamera.InitCamera(windowHandler::width, windowHandler::height, Scene::initalCameraPos); 	// camera ratio pos

	Scene::init();
	Scene::loadScene(Scene::sceneName);

	//two classes to test stuff
	FE_LAYER::init();
	Player::init();

	while (!glfwWindowShouldClose(windowHandler::window)) // GAME LOOP
	{
		TimeUtil::update();
		
		// cam
		Scene::maincamera.saveLastMaticies();
		Scene::maincamera.updateHaltonJitter(); // jitter
		Scene::maincamera.Inputs(windowHandler::window);
		Scene::maincamera.updateMatrix(); // Update: fov, near and far plane
		Scene::maincamera.applyJitter = RenderClass::doTAA; // apply to projection matrix
	
		
		// scene
		Scene::onBeginningOfFrame(); // for velocity atm
		FE_LAYER::onBeginningOfFrame();
		
		Scene::Update();
		// misc
		FE_LAYER::Update();
		Player::update();
		// render
		RenderClass::Render(windowHandler::window, windowHandler::width, windowHandler::height);
	}
	// Cleanup: Delete all objects on close
	Main::sleepState = false;

	if (FEImGuiWindow::imGuiEnabled)
		ImGui_ImplOpenGL3_Shutdown(), ImGui_ImplGlfw_Shutdown(), ImGui::DestroyContext(); // Kill ImGui

	RenderClass::Cleanup();
	Skybox::Delete();
	Skybox::cleanup();
	FE_LAYER::Delete(); // Delete scene
	SoundRunner::Delete();
	Scene::Delete();
	LightingHandler::cleanup();
	glfwDestroyWindow(windowHandler::window), glfwTerminate(); // Kill opengl
	return 0;
}
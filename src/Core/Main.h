#ifndef MAIN_CLASS_H
#define MAIN_CLASS_H

#include <iostream>
#include <vector>
#include "Camera/Camera.h"
//#include <glad/glad.h>
#include "Editor/UI/ImGui/ImGuiInclude.h"

// nope

class Main
{
public:
	// flags
	static bool sleepState;
	// thread lock
	static bool lockGameThread; // big nono rn holds renderer
	static bool lockPhysicsThread;
	static bool lockWorkerThread;
	
	static void gameLoop(GLFWwindow* window);
	
	static void physicsAttemptThreadUnlock();
	
	static void physicsLoop(GLFWwindow* window);
	
	static void workerLoop(GLFWwindow* window);
private:
};
#endif
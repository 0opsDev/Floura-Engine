#ifndef MAIN_CLASS_H
#define MAIN_CLASS_H

#include <iostream>
#include <vector>
#include "Camera/Camera.h"
#include <glad/glad.h>
#include "UI/ImGui/ImGuiInclude.h"

// nope

class Main
{
public:

	static bool sleepState;
	static float cameraSettings[3]; // FOV, near, far
	static GLfloat CameraXYZ[3]; // Initial camera position

	//void Main::updateModelLua(std::vector<std::string> path, std::vector<std::string> modelName, std::vector<float> x, std::vector<float> y, std::vector<float> z)

	static void loadSettings();

	static void saveSettings();

private:
};
#endif
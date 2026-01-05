#ifndef MAIN_CLASS_H
#define MAIN_CLASS_H

#include <iostream>
#include <vector>
#include "Camera/Camera.h"
#include <glad/glad.h>
#include "Editor/UI/ImGui/ImGuiInclude.h"

// nope

class Main
{
public:

	static bool sleepState;
	static float cameraSettings[3]; // FOV, near, far

	//void Main::updateModelLua(std::vector<std::string> path, std::vector<std::string> modelName, std::vector<float> x, std::vector<float> y, std::vector<float> z)

private:
};
#endif
#ifndef MAIN_CLASS_H
#define MAIN_CLASS_H

#include <iostream>
#include <vector>
#include "Camera/Camera.h"
class Main
{
public:

	static int VertNum;
	static int FragNum;
	static int TempButton;
	static float cameraSettings[3]; // FOV, near, far
	static GLfloat CameraXYZ[3]; // Initial camera position

	static void updateModelLua(	
		std::vector<std::string> path,
		std::vector<std::string> modelName,
		std::vector<bool> isCulling,
		std::vector<float> Modelx,
		std::vector<float> Modely,
		std::vector<float> Modelz,
		std::vector<float> RotW,
		std::vector<float> RotX,
		std::vector<float> RotY,
		std::vector<float> RotZ,
		std::vector<float> ScaleX,
		std::vector<float> ScaleY,
		std::vector<float> ScaleZ);
	//void Main::updateModelLua(std::vector<std::string> path, std::vector<std::string> modelName, std::vector<float> x, std::vector<float> y, std::vector<float> z)

	static void LoadPlayerConfig();

	static void loadSettings();

	static void saveSettings();

	static void loadEngineSettings();

private:
};
#endif
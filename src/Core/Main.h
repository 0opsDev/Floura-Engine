#ifndef MAIN_CLASS_H
#define MAIN_CLASS_H

#include <iostream>
#include <vector>
#include "Render/Camera.h"
class Main
{
public:

	static int VertNum;
	static int FragNum;
	static int TempButton;

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

private:
};
#endif
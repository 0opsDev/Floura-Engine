#include "SettingsUtil.h"
#include <string>

SettingsUtils::SettingsUtils() 
{
}

std::string SettingsUtils::s_WindowTitle = "OpenGL Window";

std::string SettingsUtils::mapName = ""; // Map loading

float SettingsUtils::s_ViewportVerticies[24] = {
	// Coords,   Texture cords
	 1.0f, -1.0f,  1.0f, 0.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	-1.0f,  1.0f,  0.0f, 1.0f,

	 1.0f,  1.0f,  1.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	-1.0f,  1.0f,  0.0f, 1.0f
};

int SettingsUtils::tempWidth = 0;
int SettingsUtils::tempHeight = 0;
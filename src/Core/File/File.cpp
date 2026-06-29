#include "File.h"
#include <utils/logConsole.h>
#include "Camera/Camera.h"
#include <Render/window/WindowHandler.h>
#include "scene/scene.h"


std::string FileClass::currentPath = "";
std::string FileClass::Contents = "";

void FileClass::saveContents() {
	std::ofstream file(currentPath, std::ios::trunc);
	if (!file.is_open()) {
		LogConsole::print("Failed to open file for writing: " + currentPath);
		return;
	}
	file << Contents;
	file.close();
	LogConsole::print("Saved contents to: " + currentPath);
}
void FileClass::loadContents() {
	std::ifstream file(currentPath);
	if (!file.is_open()) {
		LogConsole::print("Failed to open file: " + currentPath);
		return;
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	Contents = buffer.str();
	file.close();
	LogConsole::print("Loaded contents from: " + currentPath);
}

std::pair<std::string, std::string> FileClass::getShaderPaths(int vertIndex, int fragIndex) {
	std::ifstream file("Shaders/ShaderList.json"); // turn into string
	if (!file.is_open()) {
		throw std::runtime_error("Failed to open file: Shaders/ShaderList.json");
	}

	json shaderData;
	file >> shaderData;
	file.close();

	std::string vertPath = shaderData[0]["Vert"].at(vertIndex); //check for paths (strings) in array at number index givin and return it
	std::string fragPath = shaderData[0]["Frag"].at(fragIndex);

	return { vertPath, fragPath };
}

void FileClass::loadShaderProgram(int VertNum, int FragNum, Shader& shaderProgram) { //shader program switcher

	shaderProgram.Delete();

	try {
		std::pair<std::string, std::string> shaderPaths = FileClass::getShaderPaths(VertNum, FragNum);
		std::string vertFile = shaderPaths.first;
		std::string fragFile = shaderPaths.second;

		LogConsole::print("Vert: " + vertFile + " Frag: " + fragFile);

		shaderProgram.LoadShader(vertFile.c_str(), fragFile.c_str());
	}
	catch (const std::exception& e) {
		std::cerr << "Error loading shader program: " << e.what() << std::endl;
	}
}

void FileClass::loadSettings() {
	// Load Settings.json
	std::ifstream settingsFile("Settings/Settings.json");
	if (settingsFile.is_open()) {
		json settingsData;
		settingsFile >> settingsData;
		settingsFile.close();


		if (settingsData[0].contains("Resolution")) {

			//Resolution
			// width
			windowHandler::width = settingsData[0]["Resolution"][0];
			renderTarget::tempWidth = settingsData[0]["Resolution"][0];

			// height
			windowHandler::height = settingsData[0]["Resolution"][1];
			renderTarget::tempHeight = settingsData[0]["Resolution"][1];

		}
		if (settingsData[0].contains("Sensitivity")) {
			Scene::maincamera.sensitivity = glm::vec2(settingsData[0]["Sensitivity"][0].get<float>(), settingsData[0]["Sensitivity"][1].get<float>());
		}
		else {
			Scene::maincamera.sensitivity = glm::vec2(100.0f, 100.0f);
		}

		windowHandler::doVsync = settingsData[0]["Vsync"];
		//Main::cameraSettings[0] = settingsData[0]["FOV"];
		Scene::sceneName = settingsData[0]["Scene"].get<std::string>();

		//FEImGuiWindow::imGuiEnabled = settingsData[0]["imGui"];

		std::cout << "Loaded settings from Settings.json" << std::endl;

	}
	else {
		std::cerr << "Failed to open Settings/Settings.json" << std::endl;
	}
}

void FileClass::saveSettings() {
	try {
		// Load the settings file
		std::ifstream settingsFile("Settings/Settings.json", std::ios::in);
		if (!settingsFile.is_open()) {
			std::cout << "Failed to open Settings/Settings.json" << std::endl;
		}

		json settingsData;
		settingsFile >> settingsData;
		settingsFile.close();

		settingsData[0]["Vsync"] = windowHandler::doVsync;
		//settingsData[0]["FOV"] = Main::cameraSettings[0];

		settingsData[0]["Sensitivity"][0] = Scene::maincamera.sensitivity.x;
		settingsData[0]["Sensitivity"][1] = Scene::maincamera.sensitivity.y;

		// Write back to file
		std::ofstream outFile("Settings/Settings.json", std::ios::out);
		if (!outFile.is_open()) {
			std::cout << ("Failed to write to Settings.json") << std::endl;
		}

		outFile << settingsData.dump(4);
		outFile.close();

		std::cout << "Successfully updated Settings.json" << std::endl;

	}
	catch (const std::exception& e) {
		std::cout << "Exception: " << e.what() << std::endl;
	}
}
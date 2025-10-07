#include "File.h"
#include <utils/logConsole.h>

std::string FileClass::currentPath = "";
std::string FileClass::Contents = "";

void FileClass::saveContents() {
	std::ofstream file(currentPath, std::ios::trunc);
	if (!file.is_open()) {
		if (init::LogALL || init::LogSystems) LogConsole::print("Failed to open file for writing: " + currentPath);
		return;
	}
	file << Contents;
	file.close();
	if (init::LogALL || init::LogSystems) LogConsole::print("Saved contents to: " + currentPath);
}
void FileClass::loadContents() {
	std::ifstream file(currentPath);
	if (!file.is_open()) {
		if (init::LogALL || init::LogSystems) LogConsole::print("Failed to open file: " + currentPath);
		return;
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	Contents = buffer.str();
	file.close();
	if (init::LogALL || init::LogSystems) LogConsole::print("Loaded contents from: " + currentPath);
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

		if (init::LogALL || init::LogSystems) LogConsole::print("Vert: " + vertFile + " Frag: " + fragFile);

		shaderProgram.LoadShader(vertFile.c_str(), fragFile.c_str());
	}
	catch (const std::exception& e) {
		std::cerr << "Error loading shader program: " << e.what() << std::endl;
	}
}
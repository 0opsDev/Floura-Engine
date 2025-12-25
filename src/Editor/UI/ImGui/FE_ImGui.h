#include <glm/ext/vector_float3.hpp>
#include <glm/vec2.hpp>
#include <glm/fwd.hpp>
#include <string>


#ifndef FE_IMGUI_CLASS_H
#define FE_IMGUI_CLASS_H


class FEImGui
{
public:

	static void DragVec3(const std::string& label, glm::vec3& values,
		glm::vec3 resetValue,
		float columWidth);
	//static void DragVec3(const std::string& label, glm::vec3& values, glm::vec3& resetValue = glm::vec3(0.0f, 0.0f, 0.0f), float columWidth = 100.0f);
	
	// ID, windowTitle, fileTypeFilters
	static void spawnFileWindow(const std::string& ID, const std::string& windowTitle, const std::string& fileTypeFilters = ".*", const std::string& rootPath = ".");

	static bool renderFileWindow(const std::string& label,
		glm::vec2 MinimumScale = glm::vec2(500.0f, 500.0f),
		glm::vec2 Maxscale = glm::vec2(500.0f, 500.0f));

	// becomes true on okay
	static std::string fetchFilePath(const std::string& FilePath);

	// becomes true on okay
	static std::string fetchFileNamePath(const std::string& fileNamePath);

	//static void DragVec2(const std::string& label, glm::vec2& values, glm::vec2& resetValue = glm::vec2(0.0f, 0.0f), float columWidth = 100.0f);

	//static void DragVec1(const std::string& label, float& values, float resetValue = 0.0f, float columWidth = 100.0f);

private:


};

#endif // SCREEN_UTILS_CLASS_H
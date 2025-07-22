#include "ImGuiWindow.h"
#include <Scripting/ScriptRunner.h>
#include <Physics/CubeCollider.h>
#include <Core/File/File.h>
#include <Sound/SoundRunner.h>
#include <Render/passes/lighting/LightingPass.h>
#include <Core/scene.h>
#include <Gameplay/Player.h>
#include <Render/window/WindowHandler.h>
#include <ImGuiFileDialog/ImGuiFileDialog.h>

bool ImGuiWindow::imGuiEnabled = false;
bool ImGuiWindow::showViewportIcons = true;
bool ImGuiWindow::imGuiPanels[] = { true, true, true, true, true, true, true, true, true, true, true }; // ImGui Panels

bool ImGuiWindow::enableFB = false; // Change this as needed
bool ImGuiWindow::enableDEF = true;

bool ImGuiWindow::isWireframe = false;

std::string ImGuiWindow::SelectedObjectType;
int ImGuiWindow::SelectedObjectIndex;

float ImGuiWindow::lPassTime = 0;
float ImGuiWindow::Render = 0;


std::vector<std::string> ImGuiWindow::ContentObjects;
std::vector<std::string> ImGuiWindow::ContentObjectNames;
std::vector<std::string> ImGuiWindow::ContentObjectPaths;
std::vector<std::string> ImGuiWindow::ContentObjectTypes;

Texture ImGuiWindow::logoIcon; // Icon for logo in ImGui
Texture ImGuiWindow::FolderIcon; // Icon for folder in ImGui
Texture ImGuiWindow::SaveIcon; // Icon for Save in ImGui
Texture ImGuiWindow::ModelIcon; // Icon for Object in ImGui
Texture ImGuiWindow::BillBoardIcon; // Icon for BillBoard in ImGui 
Texture ImGuiWindow::colliderIcon; // Icon for Collider in ImGui 
Texture ImGuiWindow::pointLightIcon; // Icon for pointLightIcon in ImGui
Texture ImGuiWindow::spotLightIcon; // Icon for spotLightIcon in ImGui 
Texture ImGuiWindow::arrowIcon; // Icon for arrow in ImGui 
Texture ImGuiWindow::crossIcon; // Icon for cross in ImGui 
Texture ImGuiWindow::plusIcon; // Icon for plus in ImGui
Texture ImGuiWindow::cameraIcon; // Icon for camera in ImGui
Texture ImGuiWindow::skyboxIcon; // Icon for skybox in ImGui
Texture ImGuiWindow::directLight; // Icon for directLight in ImGui

// collideicon.png

// Temporary buffer for path editing
static char pathBuffer[256]; // Ensure the size is appropriate

void ImGuiWindow::init() {
	logoIcon.createTexture("Assets/Icons/Icon2.png", "diffuse", 0);
	FolderIcon.createTexture("Assets/Icons/bindericon.png", "diffuse", 1);
	SaveIcon.createTexture("Assets/Icons/saveicon.png", "diffuse", 2);
	ModelIcon.createTexture("Assets/Icons/meshicon.png", "diffuse", 3);
	BillBoardIcon.createTexture("Assets/Icons/pictureIcon.png", "diffuse", 4);
	colliderIcon.createTexture("Assets/Icons/collideicon.png", "diffuse", 5);
	pointLightIcon.createTexture("Assets/Icons/point.png", "diffuse", 6);
	spotLightIcon.createTexture("Assets/Icons/spot.png", "diffuse", 7);
	arrowIcon.createTexture("Assets/Icons/arrow.png", "diffuse", 8);
	crossIcon.createTexture("Assets/Icons/cross.png", "diffuse", 9);
	plusIcon.createTexture("Assets/Icons/plus.png", "diffuse", 10);
	cameraIcon.createTexture("Assets/Icons/cameraIcon.png", "diffuse", 11);
	skyboxIcon.createTexture("Assets/Icons/skyboxIcon.png", "diffuse", 12);
	directLight.createTexture("Assets/Icons/directLight.png", "diffuse", 13);

}

void ImGuiWindow::loadContentObjects(std::string path) {
	std::ifstream file(path);
	if (!file.is_open()) {
		std::cout << "Failed to open file: " << path << std::endl;
		return;
	}
	json ContentObjectFileData;
	try {
		file >> ContentObjectFileData;
	}
	catch (const nlohmann::json::parse_error& e) {
		// This catch block specifically handles JSON parsing errors,
		// which gives more precise error information from the library.
		std::cout << "JSON Parse Error loading ContentObject data: " << e.what() << std::endl;
		std::cout << "Error byte position: " << e.byte << std::endl; // Specific to nlohmann::json
	}
	catch (const std::ios_base::failure& e) {
		// This catch block handles file I/O errors (e.g., file not found, permission issues).
		std::cout << "File I/O Error loading ContentObject data: " << e.what() << std::endl;
	}
	catch (const std::exception& e) {
		// A general catch-all for any other std::exception derived errors.
		std::cout << "An unexpected error occurred loading ContentObject data: " << e.what() << std::endl;
	}
	file.close();
	int iteration = 0;
	for (const auto& item : ContentObjectFileData) {
		std::string contentObject = item.at("ObjectType").get<std::string>();
		std::string contentObjectType = item.at("Type").get<std::string>();
		std::string contentObjectPath = item.at("Path").get<std::string>();
		std::string contentObjectName = item.at("Name").get<std::string>();

		ContentObjects.push_back(contentObject); // Add a new content object to the list
		ContentObjectTypes.push_back(contentObjectType); // Add a new content object type to the list
		ContentObjectPaths.push_back(contentObjectPath); // Add a new content object path to the list
		ContentObjectNames.push_back(contentObjectName); // Add a new content object name to the list

		iteration++;
	}

	if (init::LogALL || init::LogModel) std::cout << "Loaded Scene ContentObject from: " << path << std::endl;



}

void ImGuiWindow::saveContentObjects(std::string path) {
	try {
		json ContentObjectData = json::array();  // New JSON array to hold model data
		// Serialize each modelObject into JSON
		for (size_t i = 0; i < ContentObjects.size(); ++i) {
			json ContentObjectJson;
			ContentObjectJson["ObjectType"] = ContentObjects[i];
			ContentObjectJson["Type"] = ContentObjectTypes[i];
			ContentObjectJson["Path"] = ContentObjectPaths[i];
			ContentObjectJson["Name"] = ContentObjectNames[i];
			ContentObjectData.push_back(ContentObjectJson);
		}
		// Write to file
		std::ofstream outFile(path, std::ios::out);
		if (!outFile.is_open()) {
			if (init::LogALL || init::LogSystems) std::cout << "Failed to write to " << path << std::endl;
			return;
		}
		outFile << ContentObjectData.dump(4);  // Pretty-print with indentation
		outFile.close();
		if (init::LogALL || init::LogSystems) std::cout << "Successfully updated " << path << std::endl;
	}
	catch (const std::exception& e) {
		if (init::LogALL || init::LogSystems) std::cout << "Exception: " << e.what() << std::endl;
	}
}

void ImGuiWindow::SystemInfomation() {
	if (ImGui::TreeNode("System Infomation")) {
		const GLubyte* version = glGetString(GL_VERSION);
		const GLubyte* renderer = glGetString(GL_RENDERER);

		ImGui::Text("OpenGL Version: %s", version); // Display OpenGL version
		ImGui::Text("Renderer: %s", renderer);  // Display GPU renderer

		ImGui::Text((std::string("ViewportSize: ") + std::to_string(static_cast<int>(Framebuffer::ViewPortWidth)) + "*" + std::to_string(static_cast<int>(Framebuffer::ViewPortHeight))).c_str());

		ImGui::TreePop();// Ends The ImGui Window
	}

}

void ImGuiWindow::RenderWindow(GLFWwindow*& window, int windowedWidth, int windowedHeight) {
	ImGui::Checkbox("enableDEF", &ImGuiWindow::enableDEF);

	ImGui::Dummy(ImVec2(0.0f, 5.0f)); // Adds 5 pixels of vertical space
	if (ImGui::TreeNode("Framerate And Resolution")) {
		ImGui::Text("Framerate Limiters");
		ImGui::Checkbox("Vsync", &windowHandler::doVsync); // Set the value of doVsync (bool)
		// Screen
		ImGui::DragInt("Width", &SettingsUtils::tempWidth);
		ImGui::DragInt("Height", &SettingsUtils::tempHeight); // screen slider
		//enableLinearScaling
		ImGui::Checkbox("Enable FB shader", &ImGuiWindow::enableFB); // Set the value of enableFB (bool)

		if (ImGui::SmallButton("Apply Changes?")) { // apply button
			glViewport(0, 0, SettingsUtils::tempWidth, SettingsUtils::tempHeight); // real internal res
			glfwSetWindowSize(window, SettingsUtils::tempWidth, SettingsUtils::tempHeight);
			windowHandler::setVSync(windowHandler::doVsync); // Set Vsync to value of doVsync (bool)
			Framebuffer::updateFrameBufferResolution(SettingsUtils::tempWidth, SettingsUtils::tempHeight); // Update frame buffer resolution
		}
		if (ImGui::SmallButton("Toggle Fullscreen (WARNING WILL TOGGLE HDR OFF)"))
		{
			ScreenUtils::toggleFullscreen(window, windowedWidth, windowedHeight); //needs to be fixed //GLFWwindow* &window, GLFWmonitor* &monitor, int windowedWidth, int windowedHeight
		} //Toggle Fullscreen


		ImGui::TreePop();// Ends The ImGui Window
	}
}

void ImGuiWindow::ShaderWindow() {
	if (ImGui::TreeNode("Shaders")) {
		//Optimisation And Shaders
		ImGui::DragInt("Shader Number (Frag)", &Main::FragNum); // Shader Switching
		if (ImGui::SmallButton("Apply Shader?")) { FileClass::loadShaderProgram(0, Main::FragNum, RenderClass::shaderProgram); } // apply shader

		if (ImGui::SmallButton("Reload Shaders?")) RenderClass::initGlobalShaders();
		
		ImGui::DragFloat("Gamma", &RenderClass::gamma);
		ImGui::Checkbox("doReflections", &RenderClass::doReflections);
		ImGui::Checkbox("doFog", &RenderClass::doFog); 		//Toggles
		ImGui::DragInt("Sample Count", &LightingPass::samplecount);

		ImGui::Text("DepthBuffer Settings (FOG)");
		ImGui::DragFloat("Depth Distance (FOG)", &RenderClass::DepthDistance);
		ImGui::DragFloat2("Near and Far Depth Plane", RenderClass::DepthPlane);
		ImGui::TreePop();// Ends The ImGui Window
	}
}

void ImGuiWindow::CameraWindow() {

	if (ImGui::TreeNode("Transform Component")) {
		ImGui::Text("Transformations: ");

		ImGui::DragFloat3("Camera Position", &Camera::Position.x); // set cam pos
		ImGui::DragFloat3("inital Camera Position", &Scene::initalCameraPos.x); // set inital cam pos
		if (ImGui::SmallButton("Reset Camera Position")) {
			Camera::Position = Scene::initalCameraPos;
		} // reset cam pos
		ImGui::DragFloat("Camera Speed", &Camera::s_scrollSpeed); //Camera
		ImGui::DragFloat3("Camera Collider Scale", &Camera::cameraColliderScale.x);

		ImGui::TreePop();// Ends The ImGui Window
	}
	ImGui::Spacing();
	if (ImGui::TreeNode("Settings Component")) {
		ImGui::Text("Settings: ");
		//sensitivity
		ImGui::DragFloat2("Camera Sensitivity", &Camera::sensitivity.x);
		ImGui::Spacing();
		ImGui::DragFloat("FOV", &Main::cameraSettings[0], 0.1f, 160.0f); //FOV
		ImGui::DragFloat2("Near and Far Plane", &Main::cameraSettings[1]); // Near and FarPlane

		ImGui::TreePop();// Ends The ImGui Window
	}
	ImGui::Spacing();
	if (ImGui::TreeNode("Collision Component")) {
		ImGui::Text("Collisions: ");
		ImGui::Text(("Foot Collision: " + std::to_string(Player::isColliding)).c_str());
		ImGui::Checkbox("CollideWithCamera: ", &CubeCollider::CollideWithCamera);
		ImGui::Checkbox("DoGravity: ", &Camera::s_DoGravity);

		ImGui::TreePop();// Ends The ImGui Window
	}

}

void ImGuiWindow::PanelsWindow() {
	ImGui::Text("Settings (Press escape to use mouse)");
	if (ImGui::SmallButton("load")) { Main::loadSettings(); } // load settings button
	if (ImGui::SmallButton("save (just settings)")) { Main::saveSettings(); } // save settings button
	ImGui::Checkbox("Rendering", &ImGuiWindow::imGuiPanels[1]);
	ImGui::Checkbox("ViewPort", &ImGuiWindow::imGuiPanels[3]);
	ImGui::Checkbox("Scene Hierarchy", &ImGuiWindow::imGuiPanels[4]);
	ImGui::Checkbox("Preformance Profiler", &ImGuiWindow::imGuiPanels[6]);
	ImGui::Checkbox("Text Editor", &ImGuiWindow::imGuiPanels[7]);
	ImGui::Checkbox("Inspector", &ImGuiWindow::imGuiPanels[9]);
	ImGui::Checkbox("Content Folder", &ImGuiWindow::imGuiPanels[10]); 
}

void ImGuiWindow::PreformanceProfiler() {
	ImGui::Begin("Preformance Profiler"); // ImGUI window creation

	SystemInfomation();

	ImGui::Spacing();

	static float framerateValues[60] = { 0 };
	static int frValues_offset = 0;
	framerateValues[frValues_offset] = static_cast<float>(TimeUtil::s_frameRate);
	frValues_offset = (frValues_offset + 1) % IM_ARRAYSIZE(framerateValues);

	//Frame time graph
	static float frameTimeValues[90] = { 0 }; //stores 90 snapshots of frametime

	static int ftValues_offset = 0;
	frameTimeValues[ftValues_offset] = TimeUtil::s_DeltaTime * 1000.0f; // Convert to milliseconds
	ftValues_offset = (ftValues_offset + 1) % IM_ARRAYSIZE(frameTimeValues);
	std::string frametimes = "LAT: " + std::to_string(frameTimeValues[ftValues_offset] = TimeUtil::s_DeltaTime * 1000.0f) + " ms";
	ImGui::BeginGroup();
	ImGui::TextColored(ImVec4(1, 0, 0, 1), ("fps: " + std::to_string(static_cast<int>(TimeUtil::s_frameRate1hz))).c_str());
	//std::string stringFPS = "FPS: " + std::to_string(deltaTimeStr.frameRate1IHZ) + frametimes;
	ImGui::PlotLines("##FPS", framerateValues, (IM_ARRAYSIZE(framerateValues)), frValues_offset, nullptr, 0.0f, TimeUtil::s_frameRate * 1.5f, ImVec2(180, 50));
	ImGui::EndGroup();

	ImGui::SameLine();

	ImGui::BeginGroup();
	ImGui::TextColored(ImVec4(0, 1, 0, 1), frametimes.c_str());
	ImGui::PlotLines("##MS", frameTimeValues, IM_ARRAYSIZE(frameTimeValues), ftValues_offset, nullptr, 0.0f, 50.0f, ImVec2(180, 50));
	ImGui::EndGroup();


	ImGui::Spacing();

	ImGui::TextColored(ImVec4(1, 0, 0, 1), ("lPass: " + std::to_string(lPassTime) + " ms").c_str());
	ImGui::TextColored(ImVec4(0,1,0,1), ("Render: " + std::to_string(Render) + " ms").c_str());

	ImGui::End();


}

void ImGuiWindow::TextEditor() {
	ImGui::Begin("Text Editor"); // ImGUI window creation
	ImGui::Text("Text Editor");

	// Input for the path to the text file
	if (ImGui::InputText("##Path", pathBuffer, sizeof(pathBuffer))) {
		FileClass::currentPath = pathBuffer;
	}
	ImGui::SameLine();

	strncpy(pathBuffer, FileClass::currentPath.c_str(), sizeof(pathBuffer) - 1);
	pathBuffer[sizeof(pathBuffer) - 1] = '\0';

	if (ImGui::ImageButton("##FolderIcon", (ImTextureID)ImGuiWindow::FolderIcon.ID, ImVec2(15, 15))) {
		IGFD::FileDialogConfig config;
		config.path = ".";
		ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".*", config);
	}
	// display
	if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) {
		if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

			FileClass::currentPath = filePathName; // Update current path

			FileClass::loadContents();
		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}

	std::vector<char> contentBuffer(FileClass::Contents.size() + 256);
	strncpy(contentBuffer.data(), FileClass::Contents.c_str(), contentBuffer.size());
	contentBuffer[contentBuffer.size() - 1] = '\0';

	ImVec2 textSize = ImGui::CalcTextSize(FileClass::Contents.c_str());
	ImVec2 boxSize(std::max(textSize.x + 20, 300.0f), std::max(textSize.y + 50, 150.0f));

	// Main Text Editor Input
	if (ImGui::InputTextMultiline("Text Box", contentBuffer.data(), contentBuffer.size(), boxSize)) {
		FileClass::Contents = std::string(contentBuffer.data());
	}
	ImGui::Spacing();
	// Save and Load buttons
	if (ImGui::ImageButton("##SaveIcon", (ImTextureID)ImGuiWindow::SaveIcon.ID, ImVec2(20, 20))) {
		FileClass::saveContents(); // Save contents of text editor
	}
	ImGui::SameLine();
	if (ImGui::ImageButton("##arrowIcon", (ImTextureID)ImGuiWindow::arrowIcon.ID, ImVec2(20, 20))) {
		FileClass::loadContents(); // Load contents of text editor
	}

	ImGui::End();
}

void ImGuiWindow::viewport() {
	ImGui::Begin("ViewPort");
	float window_width = ImGui::GetContentRegionAvail().x;
	float window_height = ImGui::GetContentRegionAvail().y;
	ImGui::Image((ImTextureID)(uintptr_t)Framebuffer::frameBufferTexture2, ImVec2(window_width, window_height), ImVec2(0, 1), ImVec2(1, 0));

	//prevEnableLinearScaling
	ScreenUtils::UpdateViewportResize();
	if (ScreenUtils::isResizing == true) {
		//std::cout << "Resolution scale changed!" << std::endl;
		Framebuffer::updateFrameBufferResolution(window_width, window_height); // Update frame buffer resolution
		glViewport(0, 0, (window_width), (window_height));

		Camera::SetViewportSize(window_width, window_height);
		//std::cout << window_width << " " << camera.width << std::endl;
		//std::cout << window_height << " " << camera.height << std::endl;
	}

	ImGui::End();
}

bool addWindowBool = false;

void ImGuiWindow::create() {

	if (ImGui::ImageButton("##plusIcon", (ImTextureID)ImGuiWindow::plusIcon.ID, ImVec2(20, 20))) {
		addWindowBool = true;
	}
	if (addWindowBool) {
		ImGuiWindow::addWindow("hierarchy", addWindowBool);
	}


}

void ImGuiWindow::HierarchyList() {
	ImGui::Spacing();

	ImGui::Text("Constant Objects:");
	//cameraIcon
	ImGui::Image(
		(ImTextureID)(intptr_t)ImGuiWindow::cameraIcon.ID,
		ImVec2(20, 20)
	);
	ImGui::SameLine();
	if (ImGui::MenuItem("Camera")) {
		ImGuiWindow::SelectedObjectType = "Camera";
		ImGuiWindow::SelectedObjectIndex = 0;
	}
	ImGui::Image(
		(ImTextureID)(intptr_t)ImGuiWindow::directLight.ID,
		ImVec2(20, 20)
	);
	ImGui::SameLine();
	if (ImGui::MenuItem("DirectLight")) {
		ImGuiWindow::SelectedObjectType = "DirectLight";
		ImGuiWindow::SelectedObjectIndex = 0;
	}
	ImGui::Image(
		(ImTextureID)(intptr_t)ImGuiWindow::skyboxIcon.ID,
		ImVec2(20, 20),
		ImVec2(0, 0),
		ImVec2(1, 1),
		ImVec4(RenderClass::skyRGBA.r, RenderClass::skyRGBA.g, RenderClass::skyRGBA.b, 1.0f) // tint
	);
	ImGui::SameLine();
	if (ImGui::MenuItem("Skybox")) {
		ImGuiWindow::SelectedObjectType = "Skybox";
		ImGuiWindow::SelectedObjectIndex = 0;
	}
	ImGui::NewLine();

		for (size_t i = 0; i < Scene::modelObjects.size(); i++)
		{
			ImGui::BeginGroup();
			ImGui::Image(
				(ImTextureID)(intptr_t)ImGuiWindow::ModelIcon.ID,
				ImVec2(20, 20)
			);
			ImGui::SameLine();
			if (ImGui::MenuItem((Scene::modelObjects[i].ObjectName + "##" + std::to_string(i)).c_str())) {
				ImGuiWindow::SelectedObjectType = "Model";
				ImGuiWindow::SelectedObjectIndex = i;
			}
			ImGui::EndGroup();
		}
		for (size_t i = 0; i < Scene::BillBoardObjects.size(); i++) {
			ImGui::BeginGroup();
			ImGui::Image(
				(ImTextureID)(intptr_t)ImGuiWindow::BillBoardIcon.ID,
				ImVec2(20, 20)
			);
			ImGui::SameLine();
			if (ImGui::MenuItem((Scene::BillBoardObjects[i].ObjectName + "##" + std::to_string(i)).c_str())) {
				ImGuiWindow::SelectedObjectType = "BillBoard";
				ImGuiWindow::SelectedObjectIndex = i;
			}
			ImGui::EndGroup();
		}
		
	//if (ImGui::TreeNode("Sound")) {
		for (size_t i = 0; i < Scene::SoundObjects.size(); i++) {
			ImGui::BeginGroup();
			if (ImGui::MenuItem((Scene::SoundObjects[i].name + "##" + std::to_string(i)).c_str())) {
				ImGuiWindow::SelectedObjectType = "Sound";
				ImGuiWindow::SelectedObjectIndex = i;
			}
			ImGui::EndGroup();
		}

		//ImGui::TreePop();
	//}

	//if (ImGui::TreeNode("Collider")) {

		for (size_t i = 0; i < Scene::CubeColliderObject.size(); i++) {
			ImGui::BeginGroup();
			ImGui::Image(
				(ImTextureID)(intptr_t)ImGuiWindow::colliderIcon.ID, ImVec2(20, 20));
			ImGui::SameLine();
			if (ImGui::MenuItem((Scene::CubeColliderObject[i].name + "##" + std::to_string(i)).c_str())) {
				ImGuiWindow::SelectedObjectType = "Collider";
				ImGuiWindow::SelectedObjectIndex = i;
			}
			ImGui::EndGroup();
		}
		for (size_t i = 0; i < Scene::enabled.size(); i++) {
			ImGui::BeginGroup();
			if (Scene::lightType[i] == 0) { // spot light
				ImGui::Image(
					(ImTextureID)(intptr_t)ImGuiWindow::spotLightIcon.ID, ImVec2(20, 20), ImVec2(0,0), ImVec2(1, 1), ImVec4(Scene::colour[i].x, Scene::colour[i].y, Scene::colour[i].z, 1.0f));
			}
			else if (Scene::lightType[i] == 1) { // point light
				ImGui::Image(
					(ImTextureID)(intptr_t)ImGuiWindow::pointLightIcon.ID, ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1), ImVec4(Scene::colour[i].x, Scene::colour[i].y, Scene::colour[i].z, 1.0f));
			}
			ImGui::SameLine();
			if (ImGui::MenuItem(("Light X:" + std::to_string(static_cast<int>(Scene::position[i].x))
				+ " Y:" + std::to_string(static_cast<int>(Scene::position[i].y))
				+ " Z:" + std::to_string(static_cast<int>(Scene::position[i].z))
				+ ("##" + std::to_string(i) ) ).c_str() ) ) 
			{
				ImGuiWindow::SelectedObjectType = "Light";
				ImGuiWindow::SelectedObjectIndex = i;
			}
			ImGui::EndGroup();
		}

	//	ImGui::TreePop();
	//}

	ImGui::Spacing();

}


static const char* hierarchyItems[]{ "Models","BillBoards","Sound", "Collider", "Light" };
static int hierarchySelectedItem = 0; // Index of the selected item in the hierarchy combo box

static const char* contentItems[]{ "Models","BillBoards" };
static int contentSelecteditem = 0;

char name[32] = "Name";
char Path[64] = "Assets/";
bool type = false;

void pathInput() {
	ImGui::InputText("Path Input", Path, IM_ARRAYSIZE(Path));
	
}
void nameInput() {
	ImGui::InputText("Name Input", name, IM_ARRAYSIZE(name));
}

void ImGuiWindow::addWindow(std::string typeString, bool &isOpen) {
	ImGui::Begin("Add");
	if (typeString == "hierarchy") {
		ImGui::Combo("ObjectType", &hierarchySelectedItem, hierarchyItems, IM_ARRAYSIZE(hierarchyItems));

		ImGui::Spacing();
		if (hierarchySelectedItem == 0) {
			//ImGui::Text("Model");
			pathInput();
			nameInput();
			ImGui::Checkbox("LOD", &type);

			if (ImGui::ImageButton("##plusIcon", (ImTextureID)ImGuiWindow::plusIcon.ID, ImVec2(10, 10))) {
				if (type) Scene::AddSceneModelObject("LOD", Path, name);
				else Scene::AddSceneModelObject("Static", Path, name);
			}

		}
		else if (hierarchySelectedItem == 1) {
			//ImGui::Text("BillBoard");
			pathInput();
			nameInput();
			ImGui::Checkbox("Animated", &type);
			if (ImGui::ImageButton("##plusIcon", (ImTextureID)ImGuiWindow::plusIcon.ID, ImVec2(10, 10))) {
				if (type) Scene::AddSceneBillBoardObject(name, "animated", Path);
				else Scene::AddSceneBillBoardObject(name, "static", Path);
			}
		}
		else if (hierarchySelectedItem == 2) {
			ImGui::Text("Not functional rn");
			pathInput();
			nameInput();
			if (ImGui::ImageButton("##plusIcon", (ImTextureID)ImGuiWindow::plusIcon.ID, ImVec2(10, 10))) {
			}
		}
		else if (hierarchySelectedItem == 3) {
			//ImGui::Text("Collider");
			nameInput();
			if (ImGui::ImageButton("##plusIcon", (ImTextureID)ImGuiWindow::plusIcon.ID, ImVec2(10, 10))) {
				Scene::AddSceneColliderObject(name);
			}
		}
		else if (hierarchySelectedItem == 4) {
			//ImGui::Text("Light");
			if (ImGui::ImageButton("##plusIcon", (ImTextureID)ImGuiWindow::plusIcon.ID, ImVec2(10, 10))) {
				Scene::AddSceneLightObject();
			}
		}
		//AddSceneLightObject
		ImGui::SameLine();
		if (ImGui::ImageButton(("##crossIcon" + typeString).c_str(), (ImTextureID)ImGuiWindow::crossIcon.ID, ImVec2(10, 10))) {
			isOpen = false;
		}
		}
	if (typeString == "content") {
		ImGui::Combo("ObjectType", &contentSelecteditem, contentItems, IM_ARRAYSIZE(contentItems));
		ImGui::Spacing();
		if (contentSelecteditem == 0) {
			//ImGui::Text("Model");
			pathInput();
			nameInput();
			ImGui::Checkbox("LOD", &type);
			if (ImGui::ImageButton("##plusIcon", (ImTextureID)ImGuiWindow::plusIcon.ID, ImVec2(10, 10))) {
				ContentObjects.push_back("Model"); // Add a new content object to the list

				if (type) {
					ContentObjectTypes.push_back("LOD"); // Add a new content object type to the list
				}
				else {
					ContentObjectTypes.push_back("static"); // Add a new content object type to the list
				}
				
				ContentObjectPaths.push_back(Path); // Add a new content object path to the list
				ContentObjectNames.push_back(name);
			}
		}
		else if (contentSelecteditem == 1) {
			//ImGui::Text("BillBoard");
			pathInput();
			nameInput();
			ImGui::Checkbox("Animated", &type);
			if (ImGui::ImageButton("##plusIcon", (ImTextureID)ImGuiWindow::plusIcon.ID, ImVec2(10, 10))) {

				ContentObjects.push_back("BillBoard"); // Add a new content object to the list

				if (type) {
					ContentObjectTypes.push_back("animated"); // Add a new content object type to the list
				}
				else {
					ContentObjectTypes.push_back("static"); // Add a new content object type to the list
				}

				ContentObjectPaths.push_back(Path); // Add a new content object path to the list
				ContentObjectNames.push_back(name);
			}
		}


		ImGui::SameLine();
		if (ImGui::ImageButton(("##crossIcon" + typeString).c_str(), (ImTextureID)ImGuiWindow::crossIcon.ID, ImVec2(10, 10))) {
			isOpen = false;
		}
	}
	ImGui::End();
}

void ImGuiWindow::ModelWindow() {
	ImGui::Spacing();
	ImGui::Text((Scene::modelObjects[ImGuiWindow::SelectedObjectIndex].ObjectName).c_str());

	if (ImGui::TreeNode("Transform Component")) {
		ImGui::Text("Transformations: ");
		// position
		ImGui::DragFloat3("Position", &Scene::modelObjects[ImGuiWindow::SelectedObjectIndex].transform.x);
		// scale
		ImGui::DragFloat3("Scale", &Scene::modelObjects[ImGuiWindow::SelectedObjectIndex].scale.x);
		//rortation
		ImGui::DragFloat3("Rotation", &Scene::modelObjects[ImGuiWindow::SelectedObjectIndex].rotation.x);

		ImGui::TreePop();// Ends The ImGui Window
	}
	ImGui::Spacing();
	if (ImGui::TreeNode("Collider Component")) {
		ImGui::Checkbox("isCollider", &Scene::modelObjects[ImGuiWindow::SelectedObjectIndex].isCollider);

		ImGui::Spacing();
		ImGui::Text("Transformations: ");
		ImGui::DragFloat3("BoxColliderTransform", &Scene::modelObjects[ImGuiWindow::SelectedObjectIndex].BoxColliderTransform.x);
		ImGui::DragFloat3("BoxColliderScale", &Scene::modelObjects[ImGuiWindow::SelectedObjectIndex].BoxColliderScale.x);

		ImGui::TreePop();// Ends The ImGui Window
	}
	ImGui::Spacing();
	if (ImGui::TreeNode("Culling Component")) {

		ImGui::Checkbox("isBackFaceCulling", &Scene::modelObjects[ImGuiWindow::SelectedObjectIndex].DoCulling);
		ImGui::Checkbox("DoFrustumCull", &Scene::modelObjects[ImGuiWindow::SelectedObjectIndex].DoFrustumCull);

		ImGui::Spacing();
		ImGui::Text("Transformations: ");
		ImGui::DragFloat3("frustumBoxTransform", &Scene::modelObjects[ImGuiWindow::SelectedObjectIndex].frustumBoxTransform.x);
		ImGui::DragFloat3("frustumBoxScale", &Scene::modelObjects[ImGuiWindow::SelectedObjectIndex].frustumBoxScale.x);

		ImGui::TreePop();// Ends The ImGui Window
	}
	ImGui::Spacing();
	if (ImGui::TreeNode("General Infomation:")) {
		ImGui::Text(("	isLOD?: " + std::to_string(Scene::modelObjects[ImGuiWindow::SelectedObjectIndex].IsLod)).c_str());
		if (Scene::modelObjects[ImGuiWindow::SelectedObjectIndex].IsLod) {
			ImGui::Text(("	LOD count: " + std::to_string(Scene::modelObjects[ImGuiWindow::SelectedObjectIndex].LODModels.size())).c_str());
			for (size_t i = 0; i < (Scene::modelObjects[ImGuiWindow::SelectedObjectIndex].LODModels.size()); i++)
			{
				ImGui::Spacing();
				ImGui::Text(("		LOD " + std::to_string(i) + ": ").c_str());
				ImGui::Text(("			Meshs size: " + std::to_string(Scene::modelObjects[ImGuiWindow::SelectedObjectIndex].LODModels[i].meshes.size())).c_str());
				for (size_t z = 0; z < Scene::modelObjects[ImGuiWindow::SelectedObjectIndex].LODModels[i].meshes.size(); z++)
				{
					ImGui::Text(("			Mesh Vertice Count " + std::to_string(z) + ": " + std::to_string(Scene::modelObjects[ImGuiWindow::SelectedObjectIndex].LODModels[i].meshes[z].vertices.size())).c_str());
				}
			}
		}
		else {
			ImGui::Text(("	Meshs size: " + std::to_string(Scene::modelObjects[ImGuiWindow::SelectedObjectIndex].ModelSingle.meshes.size())).c_str());

			for (size_t z = 0; z < Scene::modelObjects[ImGuiWindow::SelectedObjectIndex].ModelSingle.meshes.size(); z++)
			{
				ImGui::Text(("		Mesh Vertice Count " + std::to_string(z) + ": " + std::to_string(Scene::modelObjects[ImGuiWindow::SelectedObjectIndex].ModelSingle.meshes[z].vertices.size())).c_str());
			}

			//ImGui::Text(("vertices size: " + std::to_string(Scene::modelObjects[ImGuiWindow::SelectedObjectIndex].ModelSingle.meshes)).c_str());

		}
		ImGui::TreePop();// Ends The ImGui Window
	}
	ImGui::Spacing();
	if (ImGui::SmallButton("Delete")) {
		Scene::modelObjects[ImGuiWindow::SelectedObjectIndex].Delete();
		Scene::modelObjects.erase(Scene::modelObjects.begin() + ImGuiWindow::SelectedObjectIndex);
		ImGuiWindow::SelectedObjectType = "";
	}
}

void ImGuiWindow::BillBoardWindow() {

	ImGui::Text((Scene::BillBoardObjects[ImGuiWindow::SelectedObjectIndex].ObjectName).c_str());
	if (ImGui::TreeNode("Transform Component")) {
		ImGui::Text("Transformations: ");
		ImGui::DragFloat3("Position", &Scene::BillBoardObjects[ImGuiWindow::SelectedObjectIndex].transform.x);
		ImGui::DragFloat3("Scale", &Scene::BillBoardObjects[ImGuiWindow::SelectedObjectIndex].scale.x);
		ImGui::TreePop();// Ends The ImGui Window
	}
	ImGui::Spacing();
	ImGui::Checkbox("doPitch", &Scene::BillBoardObjects[ImGuiWindow::SelectedObjectIndex].doPitch);
	if (Scene::BillBoardObjects[ImGuiWindow::SelectedObjectIndex].type == "animated" || Scene::BillBoardObjects[ImGuiWindow::SelectedObjectIndex].type == "Animated") {

		ImGui::DragInt("tickrate", &Scene::BillBoardObjects[ImGuiWindow::SelectedObjectIndex].tickrate);
		ImGui::Checkbox("doUpdateSequence", &Scene::BillBoardObjects[ImGuiWindow::SelectedObjectIndex].doUpdateSequence);
	}

	ImGui::Checkbox("isCollider", &Scene::BillBoardObjects[ImGuiWindow::SelectedObjectIndex].isCollider);
	ImGui::Checkbox("DoFrustumCull", &Scene::BillBoardObjects[ImGuiWindow::SelectedObjectIndex].DoFrustumCull);

	ImGui::Spacing();
	if (ImGui::SmallButton("Delete")) {
		Scene::BillBoardObjects[ImGuiWindow::SelectedObjectIndex].Delete();
		Scene::BillBoardObjects.erase(Scene::BillBoardObjects.begin() + ImGuiWindow::SelectedObjectIndex);
		ImGuiWindow::SelectedObjectType = "";
	}
}

void ImGuiWindow::ColliderWindow() {
	ImGui::Text((Scene::CubeColliderObject[ImGuiWindow::SelectedObjectIndex].name).c_str());

	if (ImGui::TreeNode("Transform Component")) {
		ImGui::Text("Transformations: ");
		// position
		ImGui::DragFloat3("Position", &Scene::CubeColliderObject[ImGuiWindow::SelectedObjectIndex].colliderXYZ.x);
		// scale
		ImGui::DragFloat3("Scale", &Scene::CubeColliderObject[ImGuiWindow::SelectedObjectIndex].colliderScale.x);
		
		ImGui::TreePop();// Ends The ImGui Window
	}
	ImGui::Checkbox("Enabled", &Scene::CubeColliderObject[ImGuiWindow::SelectedObjectIndex].enabled);

	if (ImGui::SmallButton("Delete")) {
		Scene::CubeColliderObject[ImGuiWindow::SelectedObjectIndex].Delete();
		Scene::CubeColliderObject.erase(Scene::CubeColliderObject.begin() + ImGuiWindow::SelectedObjectIndex);
		ImGuiWindow::SelectedObjectType = "";
	}
}

static const char* lightTypes[]{ "Spotlight","Pointlight" };
static int SelectedLight = 0;

void ImGuiWindow::LightWindow() {
	ImGui::DragFloat3("Position", &Scene::position[ImGuiWindow::SelectedObjectIndex].x);
	ImGui::ColorEdit3("Color", &Scene::colour[ImGuiWindow::SelectedObjectIndex].x);
	ImGui::DragFloat2("Range And Power", &Scene::radiusAndPower[ImGuiWindow::SelectedObjectIndex].x, 0.1f, 0.1f);
	ImGui::Combo("LightType", &Scene::lightType[ImGuiWindow::SelectedObjectIndex], lightTypes, IM_ARRAYSIZE(lightTypes));

	ImGui::Spacing();

	//ImGui::Checkbox("Enabled", &Scene::LightObjectList[ImGuiWindow::SelectedObjectIndex].enabled);
	if (Scene::enabled[ImGuiWindow::SelectedObjectIndex] == 0) {
		if (ImGui::SmallButton("Enable")) {
			Scene::enabled[ImGuiWindow::SelectedObjectIndex] = !Scene::enabled[ImGuiWindow::SelectedObjectIndex];
		}
	}
	else if (Scene::enabled[ImGuiWindow::SelectedObjectIndex] == 1) {
		if (ImGui::SmallButton("Disable")) {
			Scene::enabled[ImGuiWindow::SelectedObjectIndex] = !Scene::enabled[ImGuiWindow::SelectedObjectIndex];
		}
	}
	//std::cout << Scene::LightObjectList[ImGuiWindow::SelectedObjectIndex].enabled << std::endl;

	//ImGui::Checkbox("Enabled", &Scene::enabled[ImGuiWindow::SelectedObjectIndex]);

	if (ImGui::SmallButton("Delete")) {

		Scene::enabled.erase(Scene::enabled.begin() + ImGuiWindow::SelectedObjectIndex);
		Scene::position.erase(Scene::position.begin() + ImGuiWindow::SelectedObjectIndex);
		Scene::colour.erase(Scene::colour.begin() + ImGuiWindow::SelectedObjectIndex);
		Scene::radiusAndPower.erase(Scene::radiusAndPower.begin() + ImGuiWindow::SelectedObjectIndex);
		Scene::lightType.erase(Scene::lightType.begin() + ImGuiWindow::SelectedObjectIndex);



		ImGuiWindow::SelectedObjectIndex = 0; // reset index
		ImGuiWindow::SelectedObjectType = "";
	}
}

char SkyBoxPath[64] = "Assets/Skybox/";
void ImGuiWindow::SkyBoxWindow() {
	ImGui::ColorEdit3("Sky Colour", &RenderClass::skyRGBA.r);
	ImGui::Checkbox("Render Skybox", &RenderClass::renderSkybox);
	ImGui::InputText("Skybox Path", SkyBoxPath, IM_ARRAYSIZE(SkyBoxPath));
	if (ImGui::Button("Load Skybox")) { 
		Skybox::LoadSkyBoxTexture(SkyBoxPath);
	}


}
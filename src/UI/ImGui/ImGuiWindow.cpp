#include "ImGuiWindow.h"
#include <Scripting/ScriptRunner.h>
#include <Physics/CubeCollider.h>
#include <Core/File/File.h>
#include <Sound/SoundRunner.h>
#include <Render/passes/lighting/LightingPass.h>
#include <Scene/scene.h>
#include <Gameplay/Player.h>
#include <Render/window/WindowHandler.h>
#include <glm/gtx/euler_angles.hpp>
#include "ImGuiInclude.h"
#include "FE_ImGui.h"
#include <Scene/LightingHandler.h>
#include "utils/logConsole.h"
//#include <Instance.h>


bool FEImGuiWindow::imGuiEnabled = false;
bool FEImGuiWindow::showViewportIcons = true;
bool FEImGuiWindow::imGuiPanels[] = { true, true, true, true, true, true, true, true, true, true, true, true }; // ImGui Panels

bool FEImGuiWindow::enableFB = false; // Change this as needed
bool FEImGuiWindow::enableDEF = true;

bool FEImGuiWindow::isWireframe = false;

std::string FEImGuiWindow::SelectedObjectType;
int FEImGuiWindow::SelectedObjectIndex;

float FEImGuiWindow::lPassTime = 0;
float FEImGuiWindow::Render = 0;


std::vector<std::string> FEImGuiWindow::ContentObjects;
std::vector<std::string> FEImGuiWindow::ContentObjectNames;
std::vector<std::string> FEImGuiWindow::ContentObjectPaths;
std::vector<std::string> FEImGuiWindow::ContentObjectTypes;

std::vector<std::string> FEImGuiWindow::MaterialObjectPaths;
std::vector<const char*> FEImGuiWindow::MaterialObjecNames;
int FEImGuiWindow::MaterialSelectedIndex = 0;

Texture FEImGuiWindow::logoIcon; // Icon for logo in ImGui
Texture FEImGuiWindow::FolderIcon; // Icon for folder in ImGui
Texture FEImGuiWindow::SaveIcon; // Icon for Save in ImGui
Texture FEImGuiWindow::ModelIcon; // Icon for Object in ImGui
Texture FEImGuiWindow::BillBoardIcon; // Icon for BillBoard in ImGui 
Texture FEImGuiWindow::colliderIcon; // Icon for Collider in ImGui 
Texture FEImGuiWindow::pointLightIcon; // Icon for pointLightIcon in ImGui
Texture FEImGuiWindow::spotLightIcon; // Icon for spotLightIcon in ImGui 
Texture FEImGuiWindow::arrowIcon; // Icon for arrow in ImGui 
Texture FEImGuiWindow::crossIcon; // Icon for cross in ImGui 
Texture FEImGuiWindow::plusIcon; // Icon for plus in ImGui
Texture FEImGuiWindow::cameraIcon; // Icon for camera in ImGui
Texture FEImGuiWindow::skyboxIcon; // Icon for skybox in ImGui
Texture FEImGuiWindow::directLight; // Icon for directLight in ImGui
Texture FEImGuiWindow::materialIcon; // Icon for material in ImGui
Texture FEImGuiWindow::environmentIcon; // Icon for material in ImGui
Texture FEImGuiWindow::moveIcon; // Icon for move in ImGui
Texture FEImGuiWindow::scaleIcon; // Icon for scale in ImGui
Texture FEImGuiWindow::rotateIcon; // Icon for rotate in ImGui
Texture FEImGuiWindow::wirefameIcon; // Icon for wireframe in ImGui
Texture FEImGuiWindow::iIcon; // Icon for Icon in ImGui

// collideicon.png

// Temporary buffer for path editing
static char pathBuffer[256]; // Ensure the size is appropriate

void FEImGuiWindow::init() {
	logoIcon.createTexture("Assets/Icons/Icon.png", "diffuse", 0);
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
	materialIcon.createTexture("Assets/Icons/materialIcon.png", "diffuse", 14);
	environmentIcon.createTexture("Assets/Icons/Environment.png", "diffuse", 15);
	moveIcon.createTexture("Assets/Icons/move.png", "diffuse", 16);
	scaleIcon.createTexture("Assets/Icons/scale.png", "diffuse", 17);
	rotateIcon.createTexture("Assets/Icons/rotate.png", "diffuse", 18);
	wirefameIcon.createTexture("Assets/Icons/wireframeIcon.png", "diffuse", 19);
	iIcon.createTexture("Assets/Icons/iIcon.png", "diffuse", 20);
}

void FEImGuiWindow::loadContentObjects(std::string path) {
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


		if (contentObject != "") {
			ContentObjects.push_back(contentObject); // Add a new content object to the list
			ContentObjectTypes.push_back(contentObjectType); // Add a new content object type to the list
			ContentObjectPaths.push_back(contentObjectPath); // Add a new content object path to the list
			ContentObjectNames.push_back(contentObjectName); // Add a new content object name to the list	
		}

		iteration++;
	}
	FEImGuiWindow::MaterialIndexUpdate();

	if (init::LogALL || init::LogModel) std::cout << "Loaded Scene ContentObject from: " << path << std::endl;

}

void FEImGuiWindow::saveContentObjects(std::string path) {
	try {
		json ContentObjectData = json::array();  // New JSON array to hold model data
		// Serialize each modelObject into JSON

		FEImGuiWindow::MaterialIndexUpdate();

		for (size_t i = 0; i < ContentObjects.size(); ++i) {
			json ContentObjectJson;
			ContentObjectJson["ObjectType"] = ContentObjects[i];
			ContentObjectJson["Type"] = ContentObjectTypes[i];
			ContentObjectJson["Path"] = ContentObjectPaths[i];
			ContentObjectJson["Name"] = ContentObjectNames[i];
			if (ContentObjects[i] != "") {
				ContentObjectData.push_back(ContentObjectJson);	
			}

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

// 0 positon, 1 scale, 2 rotate
int SelectedTransform = 0; 

void FEImGuiWindow::Update() {
	//Tell Imgui a new frame is about to begin
//
	ImGui_ImplOpenGL3_NewFrame(); ImGui_ImplGlfw_NewFrame(); ImGui::NewFrame();
	ImGuizmo::BeginFrame();

	FEImGuiWindow::menuwindow();

	// Rendering panel
	if (FEImGuiWindow::imGuiPanels[1]) {
		FEImGuiWindow::RenderWindow();
	}

	if (FEImGuiWindow::imGuiPanels[3]) {
		FEImGuiWindow::viewport();
	}

	if (FEImGuiWindow::imGuiPanels[6]) {
		FEImGuiWindow::PreformanceProfiler();
	}

	if (FEImGuiWindow::imGuiPanels[7]) {
		FEImGuiWindow::TextEditor();
	}

	if (FEImGuiWindow::imGuiPanels[4]) {
		FEImGuiWindow::HierarchyList();

	}

	if (FEImGuiWindow::imGuiPanels[9]) {
		FEImGuiWindow::InspectorWindow();
	}

	if (FEImGuiWindow::imGuiPanels[10]) {
		FEImGuiWindow::SceneFolderWindow();
	}

	if (FEImGuiWindow::imGuiPanels[11])
	{
		FEImGuiWindow::ConsoleWindow();
	}

	//
	// imguizmo selection
	//
	if (FEImGuiWindow::SelectedObjectType != "") // needs guizmo enabled var // passes if object is selected
	{
		if (FEImGuiWindow::SelectedObjectType == "Model") {
			auto* selected = Scene::modelObjects[FEImGuiWindow::SelectedObjectIndex].get();

			glm::mat4 T = glm::translate(glm::mat4(1.0f), selected->transform);
			glm::mat4 R = glm::yawPitchRoll(glm::radians(selected->rotation.y),
				glm::radians(selected->rotation.x),
				glm::radians(selected->rotation.z));
			glm::mat4 S = glm::scale(glm::mat4(1.0f), selected->scale);

			glm::mat4 modelMatrix = T * R * S;

			if (SelectedTransform == 0)
			{
				ImGuizmo::Manipulate(glm::value_ptr(Camera::view), glm::value_ptr(Camera::projection),
					ImGuizmo::TRANSLATE, ImGuizmo::LOCAL,
					glm::value_ptr(modelMatrix));
			}
			else if (SelectedTransform == 1)
			{
				ImGuizmo::Manipulate(glm::value_ptr(Camera::view), glm::value_ptr(Camera::projection),
					ImGuizmo::SCALE, ImGuizmo::LOCAL,
					glm::value_ptr(modelMatrix));
			}
			else if (SelectedTransform == 2)
			{
				ImGuizmo::Manipulate(glm::value_ptr(Camera::view), glm::value_ptr(Camera::projection),
					ImGuizmo::ROTATE, ImGuizmo::LOCAL,
					glm::value_ptr(modelMatrix));
			}




			if (ImGuizmo::IsUsing()) {
				glm::vec3 translation, rotation, scale;
				ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(modelMatrix),
					glm::value_ptr(translation),
					glm::value_ptr(rotation),
					glm::value_ptr(scale));
				if (SelectedTransform == 0)
				{
					selected->transform = translation;
				}
				else if (SelectedTransform == 1)
				{
					selected->scale = scale;
				}
				else if (SelectedTransform == 2)
				{
					selected->rotation = rotation;
				}
			}
		}

		else if (FEImGuiWindow::SelectedObjectType == "Light") {

			glm::mat4 T = glm::translate(glm::mat4(1.0f), LightingHandler::Lights[FEImGuiWindow::SelectedObjectIndex].position);

				ImGuizmo::Manipulate(glm::value_ptr(Camera::view), glm::value_ptr(Camera::projection),
					ImGuizmo::TRANSLATE, ImGuizmo::LOCAL,
				glm::value_ptr(T));

			if (ImGuizmo::IsUsing()) {
				glm::vec3 translation, rotation, scale;
				ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(T),
					glm::value_ptr(translation),
					glm::value_ptr(rotation),
					glm::value_ptr(scale));

				LightingHandler::Lights[FEImGuiWindow::SelectedObjectIndex].position = translation;
				//selected->rotation = rotation;
				//selected->scale = scale;
			}

		}

		else if (FEImGuiWindow::SelectedObjectType == "BillBoard") {

			glm::mat4 T = glm::translate(glm::mat4(1.0f), Scene::BillBoardObjects[FEImGuiWindow::SelectedObjectIndex].transform);
			glm::mat4 S = glm::scale(glm::mat4(1.0f), Scene::BillBoardObjects[FEImGuiWindow::SelectedObjectIndex].scale);

			glm::mat4 modelMatrix = T * S;

			if (SelectedTransform == 0)
			{
				ImGuizmo::Manipulate(glm::value_ptr(Camera::view), glm::value_ptr(Camera::projection),
					ImGuizmo::TRANSLATE, ImGuizmo::LOCAL,
					glm::value_ptr(modelMatrix));
			}
			else if (SelectedTransform == 1)
			{
				ImGuizmo::Manipulate(glm::value_ptr(Camera::view), glm::value_ptr(Camera::projection),
					ImGuizmo::SCALE, ImGuizmo::LOCAL,
					glm::value_ptr(modelMatrix));
			}
			else if (SelectedTransform == 2)
			{
				ImGuizmo::Manipulate(glm::value_ptr(Camera::view), glm::value_ptr(Camera::projection),
					ImGuizmo::ROTATE, ImGuizmo::LOCAL,
					glm::value_ptr(modelMatrix));
			}

			if (ImGuizmo::IsUsing()) {
				glm::vec3 translation, rotation, scale;
				ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(modelMatrix),
					glm::value_ptr(translation),
					glm::value_ptr(rotation),
					glm::value_ptr(scale));
				if (SelectedTransform == 0)
				{
					Scene::BillBoardObjects[FEImGuiWindow::SelectedObjectIndex].transform = translation;
				}
				else if (SelectedTransform == 1)
				{
					Scene::BillBoardObjects[FEImGuiWindow::SelectedObjectIndex].scale = scale;
				}
			}
		}
	}

	//scene
	ImGui::Render(); // Renders the ImGUI elements
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void FEImGuiWindow::menuwindow()
{
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size);

	// Important flags for the main dockspace window
	ImGuiWindowFlags window_flags =
		ImGuiWindowFlags_MenuBar |
		ImGuiWindowFlags_NoDocking |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoNavFocus;

	// Begin the main docking window
	ImGui::Begin("Docking Window", nullptr, window_flags);

	if (ImGui::BeginMenuBar())
	{
		ImGui::Image(
			(ImTextureID)(intptr_t)FEImGuiWindow::logoIcon.ID,
			ImVec2(20, 20)
		);

		if (ImGui::BeginMenu("File"))
		{
			ImGui::MenuItem("New Scene");
			if (ImGui::MenuItem("Open Scene")) {
				IGFD::FileDialogConfig config;
				config.path = ".";
				ImGuiFileDialog::Instance()->OpenDialog("LoadScene", "Choose Scene", ".*", config);
			}

			ImGui::MenuItem("Save Scene As");
			if (ImGui::MenuItem("Save Scene")) Scene::SaveScene(Scene::sceneName);
			if (ImGui::MenuItem("Reload Scene")) Scene::LoadScene(Scene::sceneName);

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View"))
		{
			ImGui::Checkbox("Wireframe", &FEImGuiWindow::isWireframe);
			ImGui::Checkbox("showBoxCollider", &CubeCollider::showBoxCollider);
			ImGui::Checkbox("showViewportIcons", &FEImGuiWindow::showViewportIcons);

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Reload Shaders?")) RenderClass::initGlobalShaders();
			ImGui::Text("Scripts:");
			if (ImGui::SmallButton("Stop")) { ScriptRunner::clearScripts(); } // save settings button
			if (ImGui::SmallButton("Start")) { ScriptRunner::init(Scene::sceneName + "/LuaStartup.json"); } // save settings button
			if (ImGui::SmallButton("Restart")) { ScriptRunner::clearScripts(); ScriptRunner::init(Scene::sceneName + "/LuaStartup.json"); } // save settings button
			ImGui::Spacing();
			ImGui::Text("Volume");
			ImGui::SliderFloat("Global Volume", &SoundRunner::GlobalVolume, 0, 1);
			ImGui::SliderFloat("Music Volume", &SoundRunner::MusicVolume, 0, 1);
			ImGui::SliderFloat("Environment Volume", &SoundRunner::environmentVolume, 0, 1);
			ImGui::SliderFloat("Entity Volume", &SoundRunner::entityVolume, 0, 1);

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Panels")) {
			//panel
			FEImGuiWindow::PanelsWindow();
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Debug")) {
			ImGui::Checkbox("DoDeferredLightingPass", &RenderClass::DoDeferredLightingPass);
			ImGui::Checkbox("DoForwardLightingPass", &RenderClass::DoForwardLightingPass);
			ImGui::Checkbox("DoComputeLightingPass", &RenderClass::DoComputeLightingPass);
			ImGui::Checkbox("doPlayerBoxCollision: ", &CubeCollider::CollideWithCamera);
			ImGui::EndMenu();
		}
		ImGui::SameLine(ImGui::GetWindowWidth() - 100.0f);
		if (ImGui::ImageButton("##SaveIcon", (ImTextureID)FEImGuiWindow::SaveIcon.ID, ImVec2(10, 10))) {
			Scene::SaveScene(Scene::sceneName);
		}
		if (ImGui::ImageButton("##arrowIcon", (ImTextureID)FEImGuiWindow::arrowIcon.ID, ImVec2(10, 10))) {
			Scene::LoadScene(Scene::sceneName);
		}
		if (ImGui::ImageButton("##crossIcon", (ImTextureID)FEImGuiWindow::crossIcon.ID, ImVec2(10, 10))) {
			Scene::Delete();
		}

		ImGui::EndMenuBar();
	}

	// display
	if (ImGuiFileDialog::Instance()->Display("LoadScene")) {
		if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
			std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
			std::replace(filePath.begin(), filePath.end(), '\\', '/');
			Scene::sceneName = filePath;
			std::cout << filePath << std::endl;
			Scene::LoadScene(filePath);
		}
		ImGuiFileDialog::Instance()->Close();
	}

	ImGuiID dockspace_id = ImGui::GetID("MainDockspace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

	ImGui::End();
}

void FEImGuiWindow::SystemInfomation() {
	if (ImGui::TreeNode("System Infomation")) {

		ImGui::Text("OpenGL Version: %s", glGetString(GL_VERSION)); // Display OpenGL version
		ImGui::Text("Renderer: %s", glGetString(GL_RENDERER));  // Display GPU renderer

		ImGui::Text((std::string("Viewport Size: ") + std::to_string(static_cast<int>(Framebuffer::ViewPortWidth)) + "*" + std::to_string(static_cast<int>(Framebuffer::ViewPortHeight))).c_str());

		ImGui::TreePop();// Ends The ImGui Window
	}

}

void FEImGuiWindow::RenderWindow() {
	ImGui::Begin("Rendering"); // ImGUI window creation
	ImGui::Checkbox("enableDEF", &FEImGuiWindow::enableDEF);
	ImGui::Checkbox("Enable FB shader", &FEImGuiWindow::enableFB); // Set the value of enableFB (bool)
	if (ImGui::SmallButton("Reload Shaders?")) RenderClass::initGlobalShaders();

	ImGui::Dummy(ImVec2(0.0f, 5.0f)); // Adds 5 pixels of vertical space
		ImGui::Text("Framerate Limiters");
		ImGui::Checkbox("Vsync", &windowHandler::doVsync); // Set the value of doVsync (bool)
		// Screen
		ImGui::DragInt("Width", &Framebuffer::tempWidth);
		ImGui::DragInt("Height", &Framebuffer::tempHeight); // screen slider

		if (ImGui::SmallButton("Apply Changes?")) { // apply button
			glViewport(0, 0, Framebuffer::tempWidth, Framebuffer::tempHeight); // real internal res
			glfwSetWindowSize(windowHandler::window, Framebuffer::tempWidth, Framebuffer::tempHeight);
			windowHandler::setVSync(windowHandler::doVsync); // Set Vsync to value of doVsync (bool)
			Framebuffer::updateFrameBufferResolution(Framebuffer::tempWidth, Framebuffer::tempHeight); // Update frame buffer resolution
		}
		if (ImGui::SmallButton("Toggle Fullscreen (WARNING WILL TOGGLE HDR OFF)"))
		{
			ScreenUtils::toggleFullscreen(windowHandler::window, windowHandler::width, windowHandler::height); //needs to be fixed //GLFWwindow* &window, GLFWmonitor* &monitor, int windowedWidth, int windowedHeight
		} //Toggle Fullscreen

		ImGui::DragFloat("Gamma", &RenderClass::gamma);
		ImGui::DragInt("Sample Count", &LightingPass::samplecount);
		ImGui::End();
}

void FEImGuiWindow::CameraWindow() {

	if (ImGui::TreeNode("Transform Component")) {
		ImGui::Text("Transformations: ");


		FEImGui::DragVec3("Camera Position", Camera::Position, Scene::initalCameraPos);

		FEImGui::DragVec3("Inital Camera Position", Scene::initalCameraPos);

		//ImGui::DragFloat3("inital Camera Position", &Scene::initalCameraPos.x); // set inital cam pos
		if (ImGui::SmallButton("Reset Camera Position")) {
			Camera::Position = Scene::initalCameraPos;
		} // reset cam pos
		ImGui::DragFloat("Camera Speed", &Camera::s_scrollSpeed); //Camera

		FEImGui::DragVec3("Camera Collider Scale", Camera::cameraColliderScale);

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

void FEImGuiWindow::PanelsWindow() {
	ImGui::Text("Settings (Press escape to use mouse)");
	if (ImGui::SmallButton("load")) { Main::loadSettings(); } // load settings button
	if (ImGui::SmallButton("save (just settings)")) { Main::saveSettings(); } // save settings button
	ImGui::Checkbox("Rendering", &FEImGuiWindow::imGuiPanels[1]);
	ImGui::Checkbox("ViewPort", &FEImGuiWindow::imGuiPanels[3]);
	ImGui::Checkbox("Scene Hierarchy", &FEImGuiWindow::imGuiPanels[4]);
	ImGui::Checkbox("Preformance Profiler", &FEImGuiWindow::imGuiPanels[6]);
	ImGui::Checkbox("Text Editor", &FEImGuiWindow::imGuiPanels[7]);
	ImGui::Checkbox("Inspector", &FEImGuiWindow::imGuiPanels[9]);
	ImGui::Checkbox("Scene Folder", &FEImGuiWindow::imGuiPanels[10]);
	ImGui::Checkbox("Console", &FEImGuiWindow::imGuiPanels[11]);
}

void FEImGuiWindow::PreformanceProfiler() {
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

void FEImGuiWindow::TextEditor() {
	ImGui::Begin("Text Editor"); // ImGUI window creation

	float header_height = 25.0f;
	ImGui::BeginChild("HeaderBar", ImVec2(0, header_height), false, ImGuiWindowFlags_NoScrollbar);

	// Input for the path to the text file
	if (ImGui::InputText("##Path", pathBuffer, sizeof(pathBuffer))) {
		FileClass::currentPath = pathBuffer;
	}

	ImGui::SameLine();

	strncpy(pathBuffer, FileClass::currentPath.c_str(), sizeof(pathBuffer) - 1);
	pathBuffer[sizeof(pathBuffer) - 1] = '\0';

	if (ImGui::ImageButton("##FolderIcon", (ImTextureID)FEImGuiWindow::FolderIcon.ID, ImVec2(15, 15))) {
		FEImGui::spawnFileWindow("TextEditor", "Choose File", ".*", ".");
	}
	// display

	if (FEImGui::renderFileWindow("TextEditor")) {
		if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();

			FileClass::currentPath = filePathName; // Update current path

			FileClass::loadContents();
		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}

	ImGui::SameLine(ImGui::GetWindowWidth() - 95.0f);

	// Save and Load buttons
	if (ImGui::ImageButton("##SaveIcon", (ImTextureID)FEImGuiWindow::SaveIcon.ID, ImVec2(15, 15))) {
		FileClass::saveContents(); // Save contents of text editor
	}
	ImGui::SameLine();
	if (ImGui::ImageButton("##arrowIcon", (ImTextureID)FEImGuiWindow::arrowIcon.ID, ImVec2(15, 15))) {
		FileClass::loadContents(); // Load contents of text editor
	}
	if (ImGui::SameLine(), ImGui::ImageButton("##crossIcon", (ImTextureID)FEImGuiWindow::crossIcon.ID, ImVec2(15, 15))) {
		FileClass::Contents = ""; // Clear contents of text editor
	}

	ImGui::EndChild();

	ImGui::Separator();
	

	// text box area
	ImGui::BeginChild("TextBox", ImVec2(0, 0), false);

	std::vector<char> contentBuffer(FileClass::Contents.size() + 256);
	strncpy(contentBuffer.data(), FileClass::Contents.c_str(), contentBuffer.size());
	contentBuffer[contentBuffer.size() - 1] = '\0';

	ImVec2 textSize = ImGui::CalcTextSize(FileClass::Contents.c_str());
	ImVec2 boxSize(std::max(textSize.x + 20, ImGui::GetWindowWidth()), std::max(textSize.y + 50, ImGui::GetWindowHeight()));

	// Main Text Editor Input
	if (ImGui::InputTextMultiline("Text Box", contentBuffer.data(), contentBuffer.size(), boxSize)) {
		FileClass::Contents = std::string(contentBuffer.data());
	}

	ImGui::EndChild();

	ImGui::End();
}

void FEImGuiWindow::viewport() {
	ImGui::Begin("ViewPort");
	bool placeholderBool;

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

	ImVec2 viewportPos = ImGui::GetItemRectMin();    // top-left corner relative to window
	ImVec2 viewportSize = ImGui::GetItemRectSize(); // size of the image

	ImGuizmo::SetOrthographic(false);
	ImGuizmo::SetDrawlist();
	ImGuizmo::SetRect(viewportPos.x, viewportPos.y, viewportSize.x, viewportSize.y);

	float padding = 10.0f;
	ImVec2 iconPos = ImVec2(viewportPos.x + padding, viewportPos.y + padding);

	ImGui::SetCursorScreenPos(iconPos);

	ImGui::BeginGroup();
	if (ImGui::ImageButton("##moveIcon", (ImTextureID)FEImGuiWindow::moveIcon.ID, ImVec2(30, 30))) {
		SelectedTransform = 0;
	}
	if (ImGui::ImageButton("##scaleIcon", (ImTextureID)FEImGuiWindow::scaleIcon.ID, ImVec2(30, 30))) {
		SelectedTransform = 1;
	}
	if (ImGui::ImageButton("##rotateIcon", (ImTextureID)FEImGuiWindow::rotateIcon.ID, ImVec2(30, 30))) {
		SelectedTransform = 2;
	}
	ImGui::EndGroup();

	ImGui::SameLine(ImGui::GetWindowWidth() - (60.0f)); // ImGui::GetWindowWidth() - 
	if (ImGui::ImageButton("##wireframeIcon", (ImTextureID)FEImGuiWindow::wirefameIcon.ID, ImVec2(30, 30))) {
		FEImGuiWindow::isWireframe = !FEImGuiWindow::isWireframe;
	}
	ImGui::SameLine(ImGui::GetWindowWidth() - (107.0f)); // ImGui::GetWindowWidth() - 
	if (ImGui::ImageButton("##iIcon", (ImTextureID)FEImGuiWindow::iIcon.ID, ImVec2(30, 30))) {
		FEImGuiWindow::showViewportIcons = !FEImGuiWindow::showViewportIcons;
	}


	ImGui::SameLine(ImGui::GetWindowWidth() - (ImGui::GetWindowWidth() - 10.0f));
	ImGui::BeginGroup();
	ImGui::Dummy(ImVec2(0, ImGui::GetWindowHeight() - 135.0f));
	ImGui::TextColored(ImVec4(1, 0, 0, 1), "OpenGL Version: %s", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
	ImGui::TextColored(ImVec4(1, 0, 0, 1), "GPU: %s", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
	ImGui::TextColored(ImVec4(1, 0, 0, 1), (std::string("RES: ") + std::to_string(static_cast<int>(Framebuffer::ViewPortWidth)) + "*" + std::to_string(static_cast<int>(Framebuffer::ViewPortHeight))).c_str());
	ImGui::TextColored(ImVec4(1, 0, 0, 1), ("FPS: " + std::to_string(static_cast<int>(TimeUtil::s_frameRate1hz))).c_str());
	ImGui::TextColored(ImVec4(1, 0, 0, 1), ("MS: " + std::to_string(TimeUtil::s_DeltaTime * 1000.0f) ).c_str());

	ImGui::EndGroup();
	ImGui::End();
}

bool addWindowBool = false;

void FEImGuiWindow::create() {

	if (ImGui::ImageButton("##plusIcon", (ImTextureID)FEImGuiWindow::plusIcon.ID, ImVec2(20, 20))) {
		addWindowBool = true;
	}
	if (addWindowBool) {
		FEImGuiWindow::addWindow("hierarchy", addWindowBool);
	}


}

void FEImGuiWindow::HierarchyList() {
	ImGui::Begin("Scene hierarchy"); // ImGUI window creation

	float header_height = 30.0f;
	ImGui::BeginChild("AddBar", ImVec2(0, header_height), false, ImGuiWindowFlags_NoScrollbar);

	FEImGuiWindow::create();
	ImGui::EndChild();
	ImGui::Separator();
	ImGui::BeginChild("Objects", ImVec2(0, 0), false);

	ImGui::Text("Constant Objects:");
	//cameraIcon
	ImGui::Image(
		(ImTextureID)(intptr_t)FEImGuiWindow::cameraIcon.ID,
		ImVec2(20, 20)
	);
	ImGui::SameLine();
	if (ImGui::MenuItem("Camera")) {
		FEImGuiWindow::SelectedObjectType = "Camera";
		FEImGuiWindow::SelectedObjectIndex = 0;
	}
	ImGui::Image(
		(ImTextureID)(intptr_t)FEImGuiWindow::directLight.ID,
		ImVec2(20, 20), 
		ImVec2(0, 0),
		ImVec2(1, 1),
		ImVec4(LightingHandler::directLightCol.r, LightingHandler::directLightCol.g, LightingHandler::directLightCol.b, 1.0f), // tint
		ImVec4(0, 0, 0, 0) // no border
	);
	ImGui::SameLine();
	if (ImGui::MenuItem("DirectLight")) {
		FEImGuiWindow::SelectedObjectType = "DirectLight";
		FEImGuiWindow::SelectedObjectIndex = 0;
	}
	ImGui::Image(
		(ImTextureID)(intptr_t)FEImGuiWindow::skyboxIcon.ID,
		ImVec2(20, 20),
		ImVec2(0, 0),
		ImVec2(1, 1),
		ImVec4(RenderClass::skyRGBA.r, RenderClass::skyRGBA.g, RenderClass::skyRGBA.b, 1.0f), // tint
		ImVec4(0, 0, 0, 0) // no border
	);
	ImGui::SameLine();
	if (ImGui::MenuItem("Skybox")) {
		FEImGuiWindow::SelectedObjectType = "Skybox";
		FEImGuiWindow::SelectedObjectIndex = 0;
	}

	ImGui::Image(
		(ImTextureID)(intptr_t)FEImGuiWindow::environmentIcon.ID,
		ImVec2(20, 20)
	);
	ImGui::SameLine();
	if (ImGui::MenuItem("Environment")) {
		FEImGuiWindow::SelectedObjectType = "Environment";
		FEImGuiWindow::SelectedObjectIndex = 0;
	}
	ImGui::NewLine();

		for (size_t i = 0; i < Scene::modelObjects.size(); i++)
		{
			ImGui::BeginGroup();
			ImGui::Image(
				(ImTextureID)(intptr_t)FEImGuiWindow::ModelIcon.ID,
				ImVec2(20, 20)
			);
			ImGui::SameLine();
			if (ImGui::MenuItem((Scene::modelObjects[i]->ObjectName + "##" + std::to_string(i)).c_str())) {
				FEImGuiWindow::SelectedObjectType = "Model";
				FEImGuiWindow::SelectedObjectIndex = i;
			}
			ImGui::EndGroup();
		}
		for (size_t i = 0; i < Scene::BillBoardObjects.size(); i++) {
			ImGui::BeginGroup();
			ImGui::Image(
				(ImTextureID)(intptr_t)FEImGuiWindow::BillBoardIcon.ID,
				ImVec2(20, 20)
			);
			ImGui::SameLine();
			if (ImGui::MenuItem((Scene::BillBoardObjects[i].ObjectName + "##" + std::to_string(i)).c_str())) {
				FEImGuiWindow::SelectedObjectType = "BillBoard";
				FEImGuiWindow::SelectedObjectIndex = i;
			}
			ImGui::EndGroup();
		}
		for (size_t i = 0; i < Scene::SoundObjects.size(); i++) {
			ImGui::BeginGroup();
			if (ImGui::MenuItem((Scene::SoundObjects[i].name + "##" + std::to_string(i)).c_str())) {
				FEImGuiWindow::SelectedObjectType = "Sound";
				FEImGuiWindow::SelectedObjectIndex = i;
			}
			ImGui::EndGroup();
		}

		for (size_t i = 0; i < Scene::CubeColliderObject.size(); i++) {
			ImGui::BeginGroup();
			ImGui::Image(
				(ImTextureID)(intptr_t)FEImGuiWindow::colliderIcon.ID, ImVec2(20, 20));
			ImGui::SameLine();
			if (ImGui::MenuItem((Scene::CubeColliderObject[i].name + "##" + std::to_string(i)).c_str())) {
				FEImGuiWindow::SelectedObjectType = "Collider";
				FEImGuiWindow::SelectedObjectIndex = i;
			}
			ImGui::EndGroup();
		}
		for (size_t i = 0; i < LightingHandler::Lights.size(); i++) {
			ImGui::BeginGroup();
			if (LightingHandler::Lights[i].type == 0) { // spot light
				ImGui::Image(
					(ImTextureID)(intptr_t)FEImGuiWindow::spotLightIcon.ID, ImVec2(20, 20), ImVec2(0,0), ImVec2(1, 1), ImVec4(LightingHandler::Lights[i].colour.x, LightingHandler::Lights[i].colour.y, LightingHandler::Lights[i].colour.z, 1.0f), ImVec4(0, 0, 0, 0) );
			}
			else if (LightingHandler::Lights[i].type == 1) { // point light
				ImGui::Image(
					(ImTextureID)(intptr_t)FEImGuiWindow::pointLightIcon.ID, ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1), ImVec4(LightingHandler::Lights[i].colour.x, LightingHandler::Lights[i].colour.y, LightingHandler::Lights[i].colour.z, 1.0f), ImVec4(0, 0, 0, 0));
			}
			ImGui::SameLine();
			if (ImGui::MenuItem(("Light X:" + std::to_string(static_cast<int>(LightingHandler::Lights[i].position.x))
				+ " Y:" + std::to_string(static_cast<int>(LightingHandler::Lights[i].position.y))
				+ " Z:" + std::to_string(static_cast<int>(LightingHandler::Lights[i].position.z))
				+ ("##" + std::to_string(i) ) ).c_str() ) ) 
			{
				FEImGuiWindow::SelectedObjectType = "Light";
				FEImGuiWindow::SelectedObjectIndex = i;
			}
			ImGui::EndGroup();
		}

	//	ImGui::TreePop();
	//}

//	ImGui::Spacing();

	ImGui::EndChild();

	ImGui::End();
}


static const char* hierarchyItems[]{ "Models","BillBoards","Sound", "Collider", "Light" };
static int hierarchySelectedItem = 0; // Index of the selected item in the hierarchy combo box

static const char* contentItems[]{ "Models","BillBoards", "Sound", "Material"};
static int contentSelecteditem = 0;

char name[32] = "Name";
std::string Path = "Assets/";
bool type = false;

int InputTextCallback(ImGuiInputTextCallbackData* data) {
	if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
		// Resize string when needed
		std::string* str = static_cast<std::string*>(data->UserData);
		str->resize(data->BufTextLen);
		data->Buf = str->data();
	}
	return 0;
}

void pathInput() {
	ImGui::InputText("##PathInput", Path.data(), Path.capacity() + 1,
		ImGuiInputTextFlags_CallbackResize,
		InputTextCallback, static_cast<void*>(&Path));
	ImGui::SameLine();

	if (ImGui::ImageButton("##FolderIcon", (ImTextureID)FEImGuiWindow::FolderIcon.ID, ImVec2(15, 15))) {
		FEImGui::spawnFileWindow("ChooseAddPath", "Add Object", ".*", ".");
	}
	ImGui::SameLine();
	ImGui::Text("Path");
	// display
	if (ImGuiFileDialog::Instance()->Display("ChooseAddPath")) {
		if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK

			std::string fullPath = ImGuiFileDialog::Instance()->GetFilePathName();
			std::filesystem::path selectedPath(fullPath);
			std::filesystem::path currentDir = std::filesystem::current_path();

			// Make relative path
			std::filesystem::path relativePath = std::filesystem::relative(selectedPath, currentDir);

			// Normalize slashes to forward slashes
			std::string pathStr = relativePath.generic_string(); // gives "Assets/Models/fps/s"

			Path = pathStr;

		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}

	
}
void nameInput() {
	ImGui::InputText("Name Input", name, IM_ARRAYSIZE(name));
}

void FEImGuiWindow::addWindow(std::string typeString, bool &isOpen) {
	ImGui::Begin("Add");
	ImGui::Dummy(ImVec2((ImGui::GetWindowWidth() - 50.0f), 1));
	ImGui::SameLine();
	if (ImGui::ImageButton(("##crossIcon" + typeString).c_str(), (ImTextureID)FEImGuiWindow::crossIcon.ID, ImVec2(10, 10))) {
		isOpen = false;
	}
	if (typeString == "hierarchy") {
		ImGui::Combo("ObjectType", &hierarchySelectedItem, hierarchyItems, IM_ARRAYSIZE(hierarchyItems));

		ImGui::Spacing();
		if (hierarchySelectedItem == 0) {
			//ImGui::Text("Model");
			pathInput();
			nameInput();
			ImGui::Checkbox("LOD", &type);

			if (ImGui::ImageButton("##plusIcon", (ImTextureID)FEImGuiWindow::plusIcon.ID, ImVec2(10, 10))) {
				if (type) Scene::AddSceneModelObject("LOD", Path, name);
				else Scene::AddSceneModelObject("Static", Path, name);
			}

		}
		else if (hierarchySelectedItem == 1) {
			//ImGui::Text("BillBoard");
			pathInput();
			nameInput();
			ImGui::Checkbox("Animated", &type);
			if (ImGui::ImageButton("##plusIcon", (ImTextureID)FEImGuiWindow::plusIcon.ID, ImVec2(10, 10))) {
				if (type) Scene::AddSceneBillBoardObject(name, "animated", Path);
				else Scene::AddSceneBillBoardObject(name, "static", Path);
			}
		}
		else if (hierarchySelectedItem == 2) {
			ImGui::Text("Not functional rn");
			pathInput();
			nameInput();
			if (ImGui::ImageButton("##plusIcon", (ImTextureID)FEImGuiWindow::plusIcon.ID, ImVec2(10, 10))) {
			}
		}
		else if (hierarchySelectedItem == 3) {
			//ImGui::Text("Collider");
			nameInput();
			if (ImGui::ImageButton("##plusIcon", (ImTextureID)FEImGuiWindow::plusIcon.ID, ImVec2(10, 10))) {
				Scene::AddSceneColliderObject(name);
			}
		}
		else if (hierarchySelectedItem == 4) {
			//ImGui::Text("Light");
			if (ImGui::ImageButton("##plusIcon", (ImTextureID)FEImGuiWindow::plusIcon.ID, ImVec2(10, 10))) {
				LightingHandler::createLight();
			}
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
			if (ImGui::ImageButton("##plusIcon", (ImTextureID)FEImGuiWindow::plusIcon.ID, ImVec2(10, 10))) {
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
			if (ImGui::ImageButton("##plusIcon", (ImTextureID)FEImGuiWindow::plusIcon.ID, ImVec2(10, 10))) {

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
		// Sound
		else if (contentSelecteditem == 2) {
			pathInput();
			nameInput();
		}
		// material
		else if (contentSelecteditem == 3) {
			pathInput();
			nameInput();
			if (ImGui::ImageButton("##plusIcon", (ImTextureID)FEImGuiWindow::plusIcon.ID, ImVec2(10, 10))) {

				ContentObjects.push_back("Material"); // Add a new content object to the list
				ContentObjectTypes.push_back("NULL"); // Add a new content object type to the list
				ContentObjectPaths.push_back(Path); // Add a new content object path to the list
				ContentObjectNames.push_back(name);

				FEImGuiWindow::MaterialIndexUpdate();
			}
		}
	}
	ImGui::End();
}

void FEImGuiWindow::MaterialIndexUpdate()
{
	//clear index first
	if (MaterialObjectPaths.size() > 0){
		MaterialObjectPaths.clear();
		MaterialObjecNames.clear();
		MaterialSelectedIndex = 0;
	}

	// update index of Materials
	for (size_t i = 0; i < ContentObjects.size(); i++)
	{
		if (ContentObjects[i] == "Material")
		{
			//std::string path = ContentObjectPaths[i];
			//std::string name = ContentObjectNames[i];

			//std::cout << " Path:" << path << " Name:" << name << std::endl;
			MaterialObjectPaths.push_back(ContentObjectPaths[i]);
			MaterialObjecNames.push_back((ContentObjectNames[i]).c_str());
		}
	}
}

void FEImGuiWindow::ModelWindow() {
	ImGui::Spacing();
	ImGui::Text((Scene::modelObjects[FEImGuiWindow::SelectedObjectIndex]->ObjectName).c_str());
	//ID
	ImGui::Text(("ID: " + std::to_string(Scene::modelObjects[FEImGuiWindow::SelectedObjectIndex]->ID.ObjType) + "*" + std::to_string(Scene::modelObjects[FEImGuiWindow::SelectedObjectIndex]->ID.UniqueNumber)).c_str());
	// Index attached to ID
	ImGui::Text(("ID Attached Index: " + std::to_string(Scene::modelObjects[FEImGuiWindow::SelectedObjectIndex]->ID.index)).c_str());
	if (ImGui::TreeNode("Rendering Component")) {

		ImGui::Combo("Material", &MaterialSelectedIndex, MaterialObjecNames.data(), MaterialObjecNames.size());
		ImGui::Text(("Current Material: " + Scene::modelObjects[FEImGuiWindow::SelectedObjectIndex]->MaterialObject.materialPath).c_str());
		ImGui::Spacing();
		if (ImGui::SmallButton("Apply Material")) {
			Scene::modelObjects[FEImGuiWindow::SelectedObjectIndex]->LoadMaterial(MaterialObjectPaths[MaterialSelectedIndex]); // for now im just gonna load the wobbly shader
			LogConsole::print("Material Applied: " + MaterialObjectPaths[MaterialSelectedIndex]);
		}
		if (ImGui::SmallButton("Reload Current Material")) {
			Scene::modelObjects[FEImGuiWindow::SelectedObjectIndex]->LoadMaterial(Scene::modelObjects[FEImGuiWindow::SelectedObjectIndex]->MaterialObject.materialPath);
			LogConsole::print("Reloaded Material: " + Scene::modelObjects[FEImGuiWindow::SelectedObjectIndex]->MaterialObject.materialPath);
		}
		ImGui::TreePop();// Ends The ImGui Window
	}
	ImGui::Spacing();

	if (ImGui::TreeNode("Transform Component")) {
		ImGui::Text("Transformations: ");
		// position
		FEImGui::DragVec3("Position", Scene::modelObjects[FEImGuiWindow::SelectedObjectIndex]->transform);
		FEImGui::DragVec3("Scale", Scene::modelObjects[FEImGuiWindow::SelectedObjectIndex]->scale, glm::vec3(1.0f, 1.0f, 1.0f));
		FEImGui::DragVec3("Rotation", Scene::modelObjects[FEImGuiWindow::SelectedObjectIndex]->rotation);

		ImGui::TreePop();// Ends The ImGui Window
	}
	ImGui::Spacing();
	if (ImGui::TreeNode("Collider Component")) {
		ImGui::Checkbox("isCollider", &Scene::modelObjects[FEImGuiWindow::SelectedObjectIndex]->isCollider);

		ImGui::Spacing();
		ImGui::Text("Transformations: ");
		ImGui::DragFloat3("BoxColliderTransform", &Scene::modelObjects[FEImGuiWindow::SelectedObjectIndex]->BoxColliderTransform.x);
		ImGui::DragFloat3("BoxColliderScale", &Scene::modelObjects[FEImGuiWindow::SelectedObjectIndex]->BoxColliderScale.x);

		ImGui::TreePop();// Ends The ImGui Window
	}
	ImGui::Spacing();
	if (ImGui::TreeNode("Culling Component")) {

		ImGui::Checkbox("isBackFaceCulling", &Scene::modelObjects[FEImGuiWindow::SelectedObjectIndex]->DoCulling);
		ImGui::Checkbox("DoFrustumCull", &Scene::modelObjects[FEImGuiWindow::SelectedObjectIndex]->DoFrustumCull);

		ImGui::Spacing();
		ImGui::Text("Transformations: ");
		ImGui::DragFloat3("frustumBoxTransform", &Scene::modelObjects[FEImGuiWindow::SelectedObjectIndex]->frustumBoxTransform.x);
		ImGui::DragFloat3("frustumBoxScale", &Scene::modelObjects[FEImGuiWindow::SelectedObjectIndex]->frustumBoxScale.x);

		ImGui::TreePop();// Ends The ImGui Window
	}
	ImGui::Spacing();
	if (ImGui::TreeNode("General Infomation:")) {
		ImGui::Text(("	isLOD?: " + std::to_string(Scene::modelObjects[FEImGuiWindow::SelectedObjectIndex]->IsLod)).c_str());
		ImGui::Text(("	Material: " + Scene::modelObjects[FEImGuiWindow::SelectedObjectIndex]->MaterialObject.materialPath).c_str());
		if (Scene::modelObjects[FEImGuiWindow::SelectedObjectIndex]->IsLod) {
			ImGui::Text(("	LOD count: " + std::to_string(Scene::modelObjects[FEImGuiWindow::SelectedObjectIndex]->LODModels.size())).c_str());
			for (size_t i = 0; i < (Scene::modelObjects[FEImGuiWindow::SelectedObjectIndex]->LODModels.size()); i++)
			{
				ImGui::Spacing();
				ImGui::Text(("		LOD " + std::to_string(i) + ": ").c_str());
				ImGui::Text(("			Meshs size: " + std::to_string(Scene::modelObjects[FEImGuiWindow::SelectedObjectIndex]->LODModels[i].meshes.size())).c_str());
				for (size_t z = 0; z < Scene::modelObjects[FEImGuiWindow::SelectedObjectIndex]->LODModels[i].meshes.size(); z++)
				{
					ImGui::Text(("			Mesh Vertice Count " + std::to_string(z) + ": " + std::to_string(Scene::modelObjects[FEImGuiWindow::SelectedObjectIndex]->LODModels[i].meshes[z].vertices.size())).c_str());
				}
			}
		}
		else {
			ImGui::Text(("	Meshs size: " + std::to_string(Scene::modelObjects[FEImGuiWindow::SelectedObjectIndex]->ModelSingle.meshes.size())).c_str());

			for (size_t z = 0; z < Scene::modelObjects[FEImGuiWindow::SelectedObjectIndex]->ModelSingle.meshes.size(); z++)
			{
				ImGui::Text(("		Mesh Vertice Count " + std::to_string(z) + ": " + std::to_string(Scene::modelObjects[FEImGuiWindow::SelectedObjectIndex]->ModelSingle.meshes[z].vertices.size())).c_str());
			}

			//ImGui::Text(("vertices size: " + std::to_string(Scene::modelObjects[ImGuiWindow::SelectedObjectIndex].ModelSingle.meshes)).c_str());

		}
		ImGui::TreePop();// Ends The ImGui Window
	}
	ImGui::Spacing();
	if (ImGui::SmallButton("Delete")) {
		Scene::modelObjects[FEImGuiWindow::SelectedObjectIndex]->Delete();
		Scene::modelObjects.erase(Scene::modelObjects.begin() + FEImGuiWindow::SelectedObjectIndex);
		FEImGuiWindow::SelectedObjectType = "";
	}
}

void FEImGuiWindow::BillBoardWindow() {

	ImGui::Text((Scene::BillBoardObjects[FEImGuiWindow::SelectedObjectIndex].ObjectName).c_str());

	// ID
	ImGui::Text(("ID: " + std::to_string(Scene::BillBoardObjects[FEImGuiWindow::SelectedObjectIndex].ID.ObjType) + "*" + std::to_string(Scene::BillBoardObjects[FEImGuiWindow::SelectedObjectIndex].ID.UniqueNumber)).c_str());
	// Index attached to ID
	ImGui::Text(("ID Attached Index: " + std::to_string(Scene::BillBoardObjects[FEImGuiWindow::SelectedObjectIndex].ID.index)).c_str());
	if (ImGui::TreeNode("Transform Component")) {
		ImGui::Text("Transformations: ");
		ImGui::DragFloat3("Position", &Scene::BillBoardObjects[FEImGuiWindow::SelectedObjectIndex].transform.x);
		ImGui::DragFloat3("Scale", &Scene::BillBoardObjects[FEImGuiWindow::SelectedObjectIndex].scale.x);
		ImGui::TreePop();// Ends The ImGui Window
	}
	ImGui::Spacing();
	ImGui::Checkbox("doPitch", &Scene::BillBoardObjects[FEImGuiWindow::SelectedObjectIndex].doPitch);
	if (Scene::BillBoardObjects[FEImGuiWindow::SelectedObjectIndex].flag_isanimated) {

		ImGui::DragInt("tickrate", &Scene::BillBoardObjects[FEImGuiWindow::SelectedObjectIndex].tickrate);
		ImGui::Checkbox("doUpdateSequence", &Scene::BillBoardObjects[FEImGuiWindow::SelectedObjectIndex].doUpdateSequence);
	}

	ImGui::Checkbox("isCollider", &Scene::BillBoardObjects[FEImGuiWindow::SelectedObjectIndex].isCollider);
	ImGui::Checkbox("DoFrustumCull", &Scene::BillBoardObjects[FEImGuiWindow::SelectedObjectIndex].DoFrustumCull);

	ImGui::Spacing();
	if (ImGui::SmallButton("Delete")) {
		Scene::BillBoardObjects[FEImGuiWindow::SelectedObjectIndex].Delete();
		Scene::BillBoardObjects.erase(Scene::BillBoardObjects.begin() + FEImGuiWindow::SelectedObjectIndex);
		FEImGuiWindow::SelectedObjectType = "";
	}
}

void FEImGuiWindow::ColliderWindow() {
	ImGui::Text((Scene::CubeColliderObject[FEImGuiWindow::SelectedObjectIndex].name).c_str());

	// ID
	ImGui::Text(("ID: " + std::to_string(Scene::CubeColliderObject[FEImGuiWindow::SelectedObjectIndex].ID.ObjType) + "*" + std::to_string(Scene::CubeColliderObject[FEImGuiWindow::SelectedObjectIndex].ID.UniqueNumber)).c_str());
	// Index attached to ID
	ImGui::Text(("ID Attached Index: " + std::to_string(Scene::CubeColliderObject[FEImGuiWindow::SelectedObjectIndex].ID.index)).c_str());

	if (ImGui::TreeNode("Transform Component")) {
		ImGui::Text("Transformations: ");
		// position
		ImGui::DragFloat3("Position", &Scene::CubeColliderObject[FEImGuiWindow::SelectedObjectIndex].colliderXYZ.x);
		// scale
		ImGui::DragFloat3("Scale", &Scene::CubeColliderObject[FEImGuiWindow::SelectedObjectIndex].colliderScale.x);
		
		ImGui::TreePop();// Ends The ImGui Window
	}
	ImGui::Checkbox("Enabled", &Scene::CubeColliderObject[FEImGuiWindow::SelectedObjectIndex].enabled);

	if (ImGui::SmallButton("Delete")) {
		Scene::CubeColliderObject[FEImGuiWindow::SelectedObjectIndex].Delete();
		Scene::CubeColliderObject.erase(Scene::CubeColliderObject.begin() + FEImGuiWindow::SelectedObjectIndex);
		FEImGuiWindow::SelectedObjectType = "";
	}
}

static const char* lightTypes[]{ "Spotlight","Pointlight" };
static int SelectedLight = 0;

void FEImGuiWindow::LightWindow() {

	//ID
	ImGui::Text(("ID: " + std::to_string(LightingHandler::Lights[FEImGuiWindow::SelectedObjectIndex].ID.ObjType) + "*" + std::to_string(LightingHandler::Lights[FEImGuiWindow::SelectedObjectIndex].ID.UniqueNumber)).c_str());
	// Index attached to ID
	ImGui::Text(("ID Attached Index: " + std::to_string(LightingHandler::Lights[FEImGuiWindow::SelectedObjectIndex].ID.index)).c_str());

	FEImGui::DragVec3("Position", LightingHandler::Lights[FEImGuiWindow::SelectedObjectIndex].position);
	FEImGui::DragVec3("Rotation", LightingHandler::Lights[FEImGuiWindow::SelectedObjectIndex].rotation);
	ImGui::ColorEdit3("Color", &LightingHandler::Lights[FEImGuiWindow::SelectedObjectIndex].colour.x);
	ImGui::DragFloat("Radius", &LightingHandler::Lights[FEImGuiWindow::SelectedObjectIndex].radius, 0.1f);
	ImGui::Combo("LightType", &LightingHandler::Lights[FEImGuiWindow::SelectedObjectIndex].type, lightTypes, IM_ARRAYSIZE(lightTypes));
	ImGui::Spacing();

	//ImGui::Checkbox("Enabled", &Scene::LightObjectList[ImGuiWindow::SelectedObjectIndex].enabled);
	if (LightingHandler::Lights[FEImGuiWindow::SelectedObjectIndex].enabled == 0) {
		if (ImGui::SmallButton("Enable")) {
			LightingHandler::Lights[FEImGuiWindow::SelectedObjectIndex].enabled = !LightingHandler::Lights[FEImGuiWindow::SelectedObjectIndex].enabled;
		}
	}
	else if (LightingHandler::Lights[FEImGuiWindow::SelectedObjectIndex].enabled == 1) {
		if (ImGui::SmallButton("Disable")) {
			LightingHandler::Lights[FEImGuiWindow::SelectedObjectIndex].enabled = !LightingHandler::Lights[FEImGuiWindow::SelectedObjectIndex].enabled;
		}
	}
	//std::cout << Scene::LightObjectList[ImGuiWindow::SelectedObjectIndex].enabled << std::endl;

	//ImGui::Checkbox("Enabled", &Scene::enabled[ImGuiWindow::SelectedObjectIndex]);

	if (ImGui::SmallButton("Delete")) {
		LightingHandler::deleteLight(FEImGuiWindow::SelectedObjectIndex);
		//LightingHandler::Lights.erase(LightingHandler::Lights.begin() + FEImGuiWindow::SelectedObjectIndex);

		FEImGuiWindow::SelectedObjectIndex = 0; // reset index
		FEImGuiWindow::SelectedObjectType = "";
	}
}

char SkyBoxPath[64] = "Assets/Skybox/";
void FEImGuiWindow::SkyBoxWindow() {
	ImGui::ColorEdit3("Sky Colour", &RenderClass::skyRGBA.r);
	ImGui::Checkbox("Do Sky Colour", &Skybox::DoSbRGBA);
	ImGui::Checkbox("Render Skybox", &RenderClass::renderSkybox);
	ImGui::InputText("Skybox Path", SkyBoxPath, IM_ARRAYSIZE(SkyBoxPath));
	if (ImGui::Button("Load Skybox")) { 
		Skybox::DefaultSkyboxPath = SkyBoxPath;
		Skybox::LoadSkyBoxTexture(SkyBoxPath);
	}
}

void FEImGuiWindow::InspectorWindow() {

	ImGui::Begin("Inspector"); // ImGUI window creation
	ImGui::Text("Inspector");
	ImGui::Text(("Selected Object Type : " + FEImGuiWindow::SelectedObjectType).c_str());
	ImGui::Text(("Index: " + std::to_string(FEImGuiWindow::SelectedObjectIndex)).c_str());

	if (FEImGuiWindow::SelectedObjectType == "Camera") {
		FEImGuiWindow::CameraWindow();
	}
	else if (FEImGuiWindow::SelectedObjectType == "Model") {
		FEImGuiWindow::ModelWindow();
	}
	else if (FEImGuiWindow::SelectedObjectType == "BillBoard") {
		FEImGuiWindow::BillBoardWindow();
	}
	else if (FEImGuiWindow::SelectedObjectType == "Sound") {
	}
	else if (FEImGuiWindow::SelectedObjectType == "Collider") {
		FEImGuiWindow::ColliderWindow();
	}
	//std::cout << Scene::enabled.size() << std::endl;
	else if (FEImGuiWindow::SelectedObjectType == "Light") {
		FEImGuiWindow::LightWindow();
	}
	else if (FEImGuiWindow::SelectedObjectType == "DirectLight") {

		ImGui::Checkbox("Enabled", &LightingHandler::doDirLight);
		ImGui::Checkbox("Enabled Specular Light", &LightingHandler::doDirSpecularLight);
		ImGui::Spacing();
		FEImGui::DragVec3("Rotation", LightingHandler::dirLightRot);
		ImGui::Spacing();
		ImGui::DragFloat("Ambient Light", &LightingHandler::directAmbient);
		ImGui::DragFloat("Specular Light", &LightingHandler::dirSpecularLight);
		ImGui::ColorEdit3("Colour", &LightingHandler::directLightCol.r);	// sky and light

	}
	else if (FEImGuiWindow::SelectedObjectType == "Skybox") {
		FEImGuiWindow::SkyBoxWindow();
	}
	else if (FEImGuiWindow::SelectedObjectType == "Environment")
	{
		ImGui::Checkbox("doReflections", &RenderClass::doReflections);
		ImGui::Spacing();
		ImGui::Checkbox("doFog", &RenderClass::doFog); 		//Toggles
		ImGui::ColorEdit3("fog RGBA", &RenderClass::fogRGBA.r);	// sky and light
		ImGui::Spacing();
		ImGui::Text("DepthBuffer Settings (FOG)");
		ImGui::DragFloat("Depth Distance (FOG)", &RenderClass::DepthDistance);
		ImGui::DragFloat2("Near and Far Depth Plane", RenderClass::DepthPlane);
	}
	ImGui::End();
}

bool addContentBool = false;

void FEImGuiWindow::SceneFolderWindow()
{
	ImGui::Begin("Scene Folder"); // ImGUI window creation

	float header_height = 30.0f;
	ImGui::BeginChild("HeaderBar", ImVec2(0, header_height), false, ImGuiWindowFlags_NoScrollbar);

	if (ImGui::ImageButton("##plusIcon", (ImTextureID)FEImGuiWindow::plusIcon.ID, ImVec2(20, 20))) {
		addContentBool = true;
	}
	ImGui::EndChild();

	if (addContentBool) {
		FEImGuiWindow::addWindow("content", addContentBool);
	}

	ImGui::Separator();

	ImGui::BeginChild("ContentArea", ImVec2(0, 0), false);

	//if (FEImGuiWindow::ContentObjects.size() > 0) {
		//ImGui::Dummy(ImVec2(0, ImGui::GetWindowHeight() - 30.0f));
	//}
		for (size_t i = 0; i < FEImGuiWindow::ContentObjects.size(); i++)
		{
			if (FEImGuiWindow::ContentObjects[i] != "") {
				ImVec2 cursor_screen_pos = ImGui::GetCursorScreenPos(); // cursor pos at start of group
				std::string name = FEImGuiWindow::ContentObjectNames[i];
				ImVec2 textSize = ImGui::CalcTextSize(name.c_str());

				if (textSize.x > 85.0f) { // max width
					while (ImGui::CalcTextSize((name + "...").c_str()).x > 85.0f && !name.empty()) {
						name.pop_back();
					}
					name += "...";
				}

				ImGui::BeginGroup();
				if (FEImGuiWindow::ContentObjects[i] == "Model") {
					if (ImGui::ImageButton(("##ObjectIcon" + std::to_string(i)).c_str(), (ImTextureID)FEImGuiWindow::ModelIcon.ID, ImVec2(100, 100))) {
						if (FEImGuiWindow::ContentObjectTypes[i] == "static") {
							Scene::AddSceneModelObject(false, FEImGuiWindow::ContentObjectPaths[i], FEImGuiWindow::ContentObjectNames[i]);
						}
						else {
							Scene::AddSceneModelObject(true, FEImGuiWindow::ContentObjectPaths[i], FEImGuiWindow::ContentObjectNames[i]);
						}

					}
					if (ImGui::ImageButton(("##crossIcon" + std::to_string(i)).c_str(), (ImTextureID)FEImGuiWindow::crossIcon.ID, ImVec2(10, 10))) {
						FEImGuiWindow::ContentObjects.erase(FEImGuiWindow::ContentObjects.begin() + i);
						FEImGuiWindow::ContentObjectNames.erase(FEImGuiWindow::ContentObjectNames.begin() + i);
						FEImGuiWindow::ContentObjectPaths.erase(FEImGuiWindow::ContentObjectPaths.begin() + i);
						FEImGuiWindow::ContentObjectTypes.erase(FEImGuiWindow::ContentObjectTypes.begin() + i);
					}
					ImGui::SameLine();
					ImGui::Text("%s", name.c_str());
				}
				if (FEImGuiWindow::ContentObjects[i] == "BillBoard") {
					if (ImGui::ImageButton(("##BillBoardIcon" + std::to_string(i)).c_str(), (ImTextureID)FEImGuiWindow::BillBoardIcon.ID, ImVec2(100, 100))) {

						if (FEImGuiWindow::ContentObjectTypes[i] == "static") {
							Scene::AddSceneBillBoardObject(FEImGuiWindow::ContentObjectNames[i], false, FEImGuiWindow::ContentObjectPaths[i]);
						}
						else {
							Scene::AddSceneBillBoardObject(FEImGuiWindow::ContentObjectNames[i], true, FEImGuiWindow::ContentObjectPaths[i]);
						}
						//flag_isanimated

					}
					if (ImGui::ImageButton(("##crossIcon" + std::to_string(i)).c_str(), (ImTextureID)FEImGuiWindow::crossIcon.ID, ImVec2(10, 10))) {
						FEImGuiWindow::ContentObjects.erase(FEImGuiWindow::ContentObjects.begin() + i);
						FEImGuiWindow::ContentObjectNames.erase(FEImGuiWindow::ContentObjectNames.begin() + i);
						FEImGuiWindow::ContentObjectPaths.erase(FEImGuiWindow::ContentObjectPaths.begin() + i);
						FEImGuiWindow::ContentObjectTypes.erase(FEImGuiWindow::ContentObjectTypes.begin() + i);
					}
					ImGui::SameLine();
					ImGui::Text("%s", name.c_str());
				}
				if (FEImGuiWindow::ContentObjects[i] == "Material") {
					if (ImGui::ImageButton(("##materialIcon" + std::to_string(i)).c_str(), (ImTextureID)FEImGuiWindow::materialIcon.ID, ImVec2(100, 100))) {
						// when clicked on should summon material editor
					}

					if (ImGui::ImageButton(("##crossIcon" + std::to_string(i)).c_str(), (ImTextureID)FEImGuiWindow::crossIcon.ID, ImVec2(10, 10))) {
						FEImGuiWindow::ContentObjects.erase(FEImGuiWindow::ContentObjects.begin() + i);
						FEImGuiWindow::ContentObjectNames.erase(FEImGuiWindow::ContentObjectNames.begin() + i);
						FEImGuiWindow::ContentObjectPaths.erase(FEImGuiWindow::ContentObjectPaths.begin() + i);
						FEImGuiWindow::ContentObjectTypes.erase(FEImGuiWindow::ContentObjectTypes.begin() + i);

						FEImGuiWindow::MaterialIndexUpdate();
					}
					ImGui::SameLine();
					ImGui::Text("%s", name.c_str());

				}
				ImGui::EndGroup();

				ImVec2 window_pos = ImGui::GetWindowPos();
				float window_width = ImGui::GetWindowWidth();

				// Predict next item width (icon + margin)
				float next_item_width = 125.0f;

				if (cursor_screen_pos.x + next_item_width < window_pos.x + window_width)
				{
					ImGui::SameLine();
				}
			}
		
		}

		ImGui::EndChild();

		ImGui::End();
}

void FEImGuiWindow::ConsoleWindow()
{
	ImGui::Begin("Console");

	if (ImGui::Button("Clear")) {
		LogConsole::logs.clear();
	}

	ImGui::Separator();

	ImGui::BeginChild("LogScrollingRegion", ImVec2(0, -25.0f), false, ImGuiWindowFlags_HorizontalScrollbar);

	for (size_t i = 0; i < LogConsole::logs.size(); i++)
	{
		std::string log_line = "#" + std::to_string(i) + ": " + LogConsole::logs[i];
		ImGui::TextUnformatted(log_line.c_str());
	}

	ImGui::EndChild();

	ImGui::Separator();
	ImGui::Text("Text box should go here");

	ImGui::End();
}
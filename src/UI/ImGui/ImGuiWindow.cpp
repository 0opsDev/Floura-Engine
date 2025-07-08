#include "ImGuiWindow.h"
#include <Scripting/ScriptRunner.h>
#include <Physics/CubeCollider.h>
#include <Core/File/File.h>
#include <Sound/SoundRunner.h>
#include <Render/passes/lighting/LightingPass.h>
#include <Core/scene.h>
#include <Gameplay/Player.h>
bool ImGuiCamera::imGuiPanels[] = { true, true, true, true, true, true, true, true, true }; // ImGui Panels

bool ImGuiCamera::DebugPanels[] = { true, false }; // ImGui Panels

std::string ImGuiCamera::FileTabs = "Model";
bool ImGuiCamera::enableFB = false; // Change this as needed
bool ImGuiCamera::enableDEF = true;

char ImGuiCamera::UniformInput[64] = { "" }; // Zero-initialized buffer
float ImGuiCamera::UniformFloat[3] = {}; // Zero-initialized array

bool ImGuiCamera::isWireframe = false;

float ImGuiCamera::gPassTime = 0;
float ImGuiCamera::lPassTime = 0;
float ImGuiCamera::Render = 0;
float ImGuiCamera::physicsTime = 0;

// Temporary buffer for path editing
static char pathBuffer[256]; // Ensure the size is appropriate

void ImGuiCamera::SystemInfomation() {
	if (ImGui::TreeNode("System Infomation")) {
		const GLubyte* version = glGetString(GL_VERSION);
		const GLubyte* renderer = glGetString(GL_RENDERER);

		ImGui::Text("OpenGL Version: %s", version); // Display OpenGL version
		ImGui::Text("Renderer: %s", renderer);  // Display GPU renderer

		ImGui::Text((std::string("ViewportSize: ") + std::to_string(static_cast<int>(Framebuffer::ViewPortWidth)) + "*" + std::to_string(static_cast<int>(Framebuffer::ViewPortHeight))).c_str());

		ImGui::TreePop();// Ends The ImGui Window
	}

}

void ImGuiCamera::RenderWindow(GLFWwindow*& window, GLFWmonitor*& monitor, int windowedWidth, int windowedHeight) {

	ImGui::Checkbox("isWireframe", &ImGuiCamera::isWireframe);
	ImGui::Checkbox("enableDEF", &ImGuiCamera::enableDEF);

	ImGui::Dummy(ImVec2(0.0f, 5.0f)); // Adds 5 pixels of vertical space
	if (ImGui::TreeNode("Framerate And Resolution")) {
		ImGui::Text("Framerate Limiters");
		ImGui::Checkbox("Vsync", &ScreenUtils::doVsync); // Set the value of doVsync (bool)
		// Screen
		ImGui::DragInt("Width", &SettingsUtils::tempWidth);
		ImGui::DragInt("Height", &SettingsUtils::tempHeight); // screen slider
		//enableLinearScaling
		ImGui::Checkbox("Enable FB shader", &ImGuiCamera::enableFB); // Set the value of enableFB (bool)

		if (ImGui::SmallButton("Apply Changes?")) { // apply button
			glViewport(0, 0, SettingsUtils::tempWidth, SettingsUtils::tempHeight); // real internal res
			glfwSetWindowSize(window, SettingsUtils::tempWidth, SettingsUtils::tempHeight);
			ScreenUtils::setVSync(ScreenUtils::doVsync); // Set Vsync to value of doVsync (bool)
			Framebuffer::updateFrameBufferResolution(SettingsUtils::tempWidth, SettingsUtils::tempHeight); // Update frame buffer resolution
		}
		if (ImGui::SmallButton("Toggle Fullscreen (WARNING WILL TOGGLE HDR OFF)"))
		{
			ScreenUtils::toggleFullscreen(window, monitor, windowedWidth, windowedHeight); //needs to be fixed //GLFWwindow* &window, GLFWmonitor* &monitor, int windowedWidth, int windowedHeight
		} //Toggle Fullscreen


		ImGui::TreePop();// Ends The ImGui Window
	}
}

void ImGuiCamera::ShaderWindow() {
	if (ImGui::TreeNode("Shaders")) {
		//Optimisation And Shaders
		ImGui::DragInt("Shader Number (Vert)", &Main::VertNum);
		ImGui::DragInt("Shader Number (Frag)", &Main::FragNum); // Shader Switching
		if (ImGui::SmallButton("Apply Shader?")) { FileClass::loadShaderProgram(Main::VertNum, Main::FragNum, RenderClass::shaderProgram); } // apply shader

		if (ImGui::SmallButton("Reload Shaders?")) RenderClass::initGlobalShaders();

		
		ImGui::DragFloat("Gamma", &RenderClass::gamma);
		ImGui::Checkbox("doReflections", &RenderClass::doReflections);
		ImGui::Checkbox("doFog", &RenderClass::doFog); 		//Toggles
		ImGui::DragInt("Sample Count", &LightingPass::samplecount);

		ImGui::Text("DepthBuffer Settings (FOG)");
		ImGui::DragFloat("Depth Distance (FOG)", &RenderClass::DepthDistance);
		ImGui::DragFloat2("Near and Far Depth Plane", RenderClass::DepthPlane);

		ImGui::InputText("Uniform Input", ImGuiCamera::UniformInput, IM_ARRAYSIZE(ImGuiCamera::UniformInput));
		ImGui::DragFloat("UniformFloat", ImGuiCamera::UniformFloat);
		if (false & ImGuiCamera::UniformInput != NULL) { // Debug
			if (init::LogALL || init::LogSystems) std::cout << ImGuiCamera::UniformInput << std::endl;
		}
		ImGui::TreePop();// Ends The ImGui Window
	}
}

void ImGuiCamera::CameraWindow() {
	ImGui::Begin("Camera Settings"); // ImGUI window creation
	//std::to_string(footCollision)
	ImGui::Text(("Camera Position: x: " + std::to_string(Camera::Position.x) + "y: " + std::to_string(Camera::Position.y) + "z: " + std::to_string(Camera::Position.z)).c_str());
	glm::quat cameraQuat = Camera::Orientation; // Ensure Orientation is a quaternion
	glm::vec3 eulerAngles = glm::degrees(glm::eulerAngles(cameraQuat));
	ImGui::Text(("Camera Rotation: Yaw: " + std::to_string(eulerAngles.x) + " Pitch: " + std::to_string(eulerAngles.y) + " Roll: " + std::to_string(eulerAngles.z)).c_str());
	ImGui::Spacing();
	if (ImGui::TreeNode("Controls")) {
		ImGui::Text("Transform");
		if (ImGui::SmallButton("Reset Camera")) {
			Camera::Position = glm::vec3(0, 0, 0);
		} // reset cam pos
		ImGui::DragFloat3("Camera Transform", &Camera::Position.x); // set cam pos
		ImGui::DragFloat("Camera Speed", &Camera::s_scrollSpeed); //Camera

		ImGui::Spacing();
		ImGui::Text("Bindings");
		ImGui::DragFloat("Camera Sensitivity X", &Camera::s_sensitivityX);
		ImGui::DragFloat("Camera Sensitivity Y", &Camera::s_sensitivityY);
		ImGui::TreePop();
	}
	ImGui::Spacing();
	if (ImGui::TreeNode("Perspective")) {
		ImGui::Text("View");
		ImGui::SliderFloat("FOV", &Main::cameraSettings[0], 0.1f, 160.0f); //FOV
		ImGui::DragFloat2("Near and Far Plane", &Main::cameraSettings[1]); // Near and FarPlane
		ImGui::TreePop();
	}
	ImGui::Spacing();
	if (ImGui::TreeNode("Collision")) {
		ImGui::Text(("Foot Collision: " + std::to_string(Player::isColliding)).c_str());
		ImGui::Checkbox("doPlayerBoxCollision: ", &CubeCollider::CollideWithCamera);
		ImGui::TreePop();
	}
	ImGui::Spacing();
	ImGui::Checkbox("DoGravity: ", &Camera::s_DoGravity);

	ImGui::End();
}

void ImGuiCamera::LightWindow() {
	if (ImGui::TreeNode("Lighting")) {

		if (ImGui::TreeNode("Colour")) {
			ImGui::ColorEdit3("sky RGBA", &RenderClass::skyRGBA.r);
			ImGui::ColorEdit4("light RGBA", RenderClass::lightRGBA);
			ImGui::ColorEdit3("fog RGBA", &RenderClass::fogRGBA.r);	// sky and light
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Light Settings")) {
			ImGui::DragFloat3("Light Transform", RenderClass::LightTransform1);
			ImGui::DragFloat("light I", &RenderClass::ConeSI[2]);

			// cone settings
			ImGui::Text("cone size");
			ImGui::SliderFloat("cone Size (D: 0.95)", &RenderClass::ConeSI[1], 0.0f, 1.0f);
			ImGui::SliderFloat("cone Strength (D: 0.05)", &RenderClass::ConeSI[0], 0.0f, 0.90f);

			ImGui::Text("Light Angle");
			ImGui::DragFloat3("Cone Angle", RenderClass::ConeRot);

			ImGui::TreePop();
		}

		ImGui::TreePop();// Ends The ImGui Window
	}
}

void ImGuiCamera::PanelsWindow() {
	// Toggle ImGui Windows
	ImGui::Begin("Panels"); // ImGUI window creation
	ImGui::Text("Settings (Press escape to use mouse)");
	if (ImGui::SmallButton("load")) { Main::loadSettings(); } // load settings button
	if (ImGui::SmallButton("save (just settings)")) { Main::saveSettings(); } // save settings button
	ImGui::Text("Scripts:");
	if (ImGui::SmallButton("Stop")) { ScriptRunner::clearScripts(); } // save settings button
	if (ImGui::SmallButton("Start")) { ScriptRunner::init(SettingsUtils::sceneName + "LuaStartup.json"); } // save settings button
	if (ImGui::SmallButton("Restart")) { ScriptRunner::clearScripts(); ScriptRunner::init(SettingsUtils::sceneName + "LuaStartup.json"); } // save settings button
	ImGui::Checkbox("Rendering", &ImGuiCamera::imGuiPanels[1]);
	ImGui::Checkbox("Camera Settings", &ImGuiCamera::imGuiPanels[2]);
	ImGui::Checkbox("ViewPort", &ImGuiCamera::imGuiPanels[3]);
	ImGui::Checkbox("File Viewer", &ImGuiCamera::imGuiPanels[4]);
	ImGui::Checkbox("Physics Settings", &ImGuiCamera::imGuiPanels[5]);
	ImGui::Checkbox("Debug Window", &ImGuiCamera::imGuiPanels[6]);
	ImGui::Checkbox("Text Editor", &ImGuiCamera::imGuiPanels[7]);
	ImGui::Checkbox("Audio", &ImGuiCamera::imGuiPanels[8]); // Audio window
	ImGui::End();
}

void ImGuiCamera::PhysicsWindow() {
	ImGui::Begin("Physics"); // ImGUI window creation
	ImGui::Checkbox("showBoxCollider", &CubeCollider::showBoxCollider);
	if (ImGui::TreeNode("Collision")) {
		ImGui::Checkbox("doPlayerBoxCollision: ", &CubeCollider::CollideWithCamera);
		ImGui::TreePop();
	}

	ImGui::End();
}

void ImGuiCamera::DebugWindow() {
	ImGui::Begin("Debug Window"); // ImGUI window creation
	ImGui::Checkbox("Preformance Profiler", &DebugPanels[0]);
	if (DebugPanels[0]) { PreformanceProfiler(); }
	ImGui::Checkbox("DoDeferredLightingPass", &RenderClass::DoDeferredLightingPass);
	ImGui::Checkbox("DoForwardLightingPass", &RenderClass::DoForwardLightingPass);
	ImGui::Checkbox("DoComputeLightingPass", &RenderClass::DoComputeLightingPass);
	ImGui::End();
}

void ImGuiCamera::PreformanceProfiler() {
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

	ImGui::Text(("fps: " + std::to_string(static_cast<int>(TimeUtil::s_frameRate1hz))).c_str());
	ImGui::Text(frametimes.c_str());
	ImGui::Spacing();

	//std::string stringFPS = "FPS: " + std::to_string(deltaTimeStr.frameRate1IHZ) + frametimes;
	if (ImGui::TreeNode("FPS Graph"))
	{
		ImGui::PlotLines("Framerate (FPS) Graph", framerateValues, (IM_ARRAYSIZE(framerateValues)), frValues_offset, nullptr, 0.0f, TimeUtil::s_frameRate * 1.5f, ImVec2(0, 80));
		ImGui::PlotLines("Frame Times (ms) Graph", frameTimeValues, IM_ARRAYSIZE(frameTimeValues), ftValues_offset, nullptr, 0.0f, 50.0f, ImVec2(0, 80));

		ImGui::TreePop();// Ends The ImGui Window
	}
	ImGui::Spacing();

	ImGui::Text(("gPass: " + std::to_string(gPassTime) + " ms").c_str());
	ImGui::Text(("lPass: " + std::to_string(lPassTime) + " ms").c_str());
	ImGui::Text(("Render: " + std::to_string(Render) + " ms").c_str());
	ImGui::Text(("Physics: " + std::to_string(physicsTime) + " ms").c_str());
	float totalTime = ((Render + physicsTime));
	ImGui::Text(("Total (Render + physicsTime): " + std::to_string(totalTime) + " ms").c_str());

	ImGui::End();


}

void ImGuiCamera::TextEditor() {
	ImGui::Begin("Text Editor"); // ImGUI window creation
	ImGui::Text("Text Editor");

	strncpy(pathBuffer, FileClass::currentPath.c_str(), sizeof(pathBuffer) - 1);
	pathBuffer[sizeof(pathBuffer) - 1] = '\0';

	// Input for the path to the text file
	if (ImGui::InputText("Path", pathBuffer, sizeof(pathBuffer))) {
		FileClass::currentPath = pathBuffer;
	}

	if (ImGui::Button("Load")) {
		FileClass::loadContents(); // Load contents of text editor
	}
	if (ImGui::Button("Save")) {
		FileClass::saveContents(); // Save contents of text editor
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

	ImGui::End();
}

void ImGuiCamera::audio() {
	ImGui::Begin("audio window"); // ImGUI window creation
	ImGui::Checkbox("Visualize Audio", &SoundRunner::VisualizeSound);
	if (ImGui::TreeNode("Volume")) {
		ImGui::Spacing();
		ImGui::SliderFloat("Global Volume", &SoundRunner::GlobalVolume, 0, 1);
		ImGui::Spacing();
		ImGui::SliderFloat("Music Volume", &SoundRunner::MusicVolume, 0, 1);
		ImGui::Spacing();
		ImGui::SliderFloat("Environment Volume", &SoundRunner::environmentVolume, 0, 1);
		ImGui::Spacing();
		ImGui::SliderFloat("Entity Volume", &SoundRunner::entityVolume, 0, 1);

		ImGui::TreePop();
	}

	ImGui::End();
}

void ImGuiCamera::viewport() {
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

static const char* items[]{ "Models","BillBoards","Sound", "Collider"};
static int Selecteditem = 0;

char name[32] = "Name";
char Path[64] = "Assets/";
bool type = false;

void ImGuiCamera::create() {
	if (ImGui::TreeNode("Add New Object")) {
		ImGui::Spacing();
		ImGui::Combo("ObjectType", &Selecteditem, items, IM_ARRAYSIZE(items));
		ImGui::InputText("Path Input", Path, IM_ARRAYSIZE(Path));
		ImGui::InputText("Name Input", name, IM_ARRAYSIZE(name));
		ImGui::Spacing();
		if (Selecteditem == 0) {
			ImGui::Text("Model");
			ImGui::Checkbox("LOD", &type);

			if (ImGui::SmallButton("Create")) {
				if (type) Scene::AddSceneModelObject("LOD", Path, name);
				else Scene::AddSceneModelObject("Static", Path, name);
			}

		}
		else if (Selecteditem == 1) {
			ImGui::Text("BillBoard");

			ImGui::Checkbox("Animated", &type);
			if (ImGui::SmallButton("Create")) {
				if (type) Scene::AddSceneBillBoardObject(name, "animated", Path);
				else Scene::AddSceneBillBoardObject(name, "static", Path);
			}
			else if (Selecteditem == 2) {
				ImGui::Text("Sound");
			}
			else if (Selecteditem == 3) {
				ImGui::Text("Collider");
			}
		}
		ImGui::TreePop();
		//std::cout << Selecteditem << std::endl;
	}
}

void ImGuiCamera::ModelH() {
	if (ImGui::TreeNode("Models")) {
		
		ImGui::Spacing();

		for (size_t i = 0; i < Scene::modelObjects.size(); i++)
		{
			// would be better if i can select one of the many objects and ill have a properties plane to edit the data instead of this
			// also a add window would be nice for adding things
			if (ImGui::TreeNode((Scene::modelObjects[i].ObjectName).c_str())) {
				// position
				ImGui::DragFloat3("Position", &Scene::modelObjects[i].transform.x);

				// scale
				ImGui::DragFloat3("Scale", &Scene::modelObjects[i].scale.x);

				ImGui::DragFloat4("Rotation", &Scene::modelObjects[i].rotation.x);

				ImGui::Spacing();
				ImGui::Checkbox("isCollider", &Scene::modelObjects[i].isCollider);

				ImGui::DragFloat3("BoxColliderTransform", &Scene::modelObjects[i].BoxColliderTransform.x);

				ImGui::DragFloat3("BoxColliderScale", &Scene::modelObjects[i].BoxColliderScale.x);

				ImGui::Spacing();
				ImGui::Checkbox("isBackFaceCulling", &Scene::modelObjects[i].DoCulling);
				ImGui::Checkbox("DoFrustumCull", &Scene::modelObjects[i].DoFrustumCull);

				ImGui::DragFloat3("frustumBoxTransform", &Scene::modelObjects[i].frustumBoxTransform.x);

				ImGui::DragFloat3("frustumBoxScale", &Scene::modelObjects[i].frustumBoxScale.x);

				ImGui::Spacing();
				if (ImGui::SmallButton("Delete")) {
					Scene::modelObjects[i].Delete();
					Scene::modelObjects.erase(Scene::modelObjects.begin() + i);
				}

				ImGui::TreePop();
			}
		}

		ImGui::TreePop();
	}

}

void ImGuiCamera::BillBoardH() {
	if (ImGui::TreeNode("BillBoards")) {

		ImGui::Spacing();

		for (size_t i = 0; i < Scene::BillBoardObjects.size(); i++) {
			if (ImGui::TreeNode((Scene::BillBoardObjects[i].ObjectName).c_str())) {
				ImGui::DragFloat3("Position", &Scene::BillBoardObjects[i].transform.x);
				ImGui::DragFloat3("Scale", &Scene::BillBoardObjects[i].scale.x);

				ImGui::Spacing();
				ImGui::Checkbox("doPitch", &Scene::BillBoardObjects[i].doPitch);

				if (Scene::BillBoardObjects[i].type == "animated" || Scene::BillBoardObjects[i].type == "Animated") {

					ImGui::DragInt("tickrate", &Scene::BillBoardObjects[i].tickrate);
					ImGui::Checkbox("doUpdateSequence", &Scene::BillBoardObjects[i].doUpdateSequence);
				}

				ImGui::Checkbox("isCollider", &Scene::BillBoardObjects[i].isCollider);
				ImGui::Checkbox("DoFrustumCull", &Scene::BillBoardObjects[i].DoFrustumCull);

				ImGui::Spacing();
				if (ImGui::SmallButton("Delete")) {
					Scene::BillBoardObjects[i].Delete();
					Scene::BillBoardObjects.erase(Scene::BillBoardObjects.begin() + i);
				}

				ImGui::TreePop();
			}
		}

		ImGui::TreePop();
	}

}
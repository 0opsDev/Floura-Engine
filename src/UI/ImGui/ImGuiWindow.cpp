#include "ImGuiWindow.h"
#include <Scripting/ScriptRunner.h>
#include <Physics/CubeCollider.h>
#include <Core/File/File.h>
#include <Sound/SoundRunner.h>
bool ImGuiCamera::imGuiPanels[] = { true, true, true, true, true, true, true, true, true }; // ImGui Panels

bool ImGuiCamera::DebugPanels[] = { false, false}; // ImGui Panels

std::string ImGuiCamera::FileTabs = "Model";
bool ImGuiCamera::enableFB = false; // Change this as needed

char ImGuiCamera::UniformInput[64] = {""}; // Zero-initialized buffer
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

		ImGui::TreePop();// Ends The ImGui Window
	}

}

void ImGuiCamera::RenderWindow(GLFWwindow*& window, GLFWmonitor*& monitor, int windowedWidth, int windowedHeight) {

	SystemInfomation();

	ImGui::Checkbox("isWireframe", &ImGuiCamera::isWireframe);

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
		if (ImGui::SmallButton("Apply Shader?")) { Main::ApplyShader = true; } // apply shader
		ImGui::DragFloat("Gamma", &RenderClass::gamma);
		ImGui::Checkbox("doReflections", &RenderClass::doReflections);
		ImGui::Checkbox("doFog", &RenderClass::doFog); 		//Toggles

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

void ImGuiCamera::LightWindow() {
	if (ImGui::TreeNode("Lighting")) {

		if (ImGui::TreeNode("Colour")) {
			ImGui::ColorEdit4("sky RGBA", RenderClass::skyRGBA);
			ImGui::ColorEdit4("light RGBA", RenderClass::lightRGBA);
			ImGui::ColorEdit4("fog RGBA", RenderClass::fogRGBA);	// sky and light
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
	if (ImGui::SmallButton("load")) { Main::loadSettings(); Main::loadEngineSettings(); } // load settings button
	if (ImGui::SmallButton("save (just settings)")) { Main::saveSettings(); } // save settings button
	ImGui::Text("Scripts:");
	if (ImGui::SmallButton("Stop")) { ScriptRunner::clearScripts(); } // save settings button
	if (ImGui::SmallButton("Start")) { ScriptRunner::init(SettingsUtils::mapName + "LuaStartup.json"); } // save settings button
	if (ImGui::SmallButton("Restart")) { ScriptRunner::clearScripts(); ScriptRunner::init(SettingsUtils::mapName + "LuaStartup.json"); } // save settings button
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

void ImGuiCamera::PhysicsWindow(){
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
	if (DebugPanels[0]) { PreformanceProfiler();}
	ImGui::Checkbox("LightingPass", &RenderClass::LightingPass);
	ImGui::Checkbox("RegularPass", &RenderClass::RegularPass);
	ImGui::End();
}

void ImGuiCamera::PreformanceProfiler() {
	ImGui::Begin("Preformance Profiler"); // ImGUI window creation
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
#include "ImGuiWindow.h"
#include "Camera/Camera.h"
#include <imgui/imgui_impl_opengl3.h>

bool ImGuiCamera::imGuiPanels[4] = { true, true, true, true }; // ImGui Panels
bool ImGuiCamera::enableFB = false; // Change this as needed
bool ImGuiCamera::enableLinearScaling = false;

float ImGuiCamera::prevResolutionScale = 1.0f; // Initialize previous scale
float ImGuiCamera::prevEnableLinearScaling = false; // Initialize previous scale
float ImGuiCamera::resolutionScale = 1;

char ImGuiCamera::UniformInput[64] = {}; // Zero-initialized buffer
float ImGuiCamera::UniformFloat[3] = {}; // Zero-initialized array

bool ImGuiCamera::isWireframe = false;
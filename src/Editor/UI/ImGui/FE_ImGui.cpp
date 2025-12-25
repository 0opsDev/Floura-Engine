#include "FE_ImGui.h"

#include "ImGuiInclude.h"

void FEImGui::DragVec3(const std::string& label, glm::vec3& values,
	glm::vec3 resetValue, float columWidth)
{

	ImGui::PushID(label.c_str()); // unique label

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columWidth);
	ImGui::Text(label.c_str());
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0 , 0 });
	float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	ImVec2 ButtonSize{ lineHeight + 3.0f, lineHeight };


	//x
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.5f, 0.0f, 0.0f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.0f, 0.0f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f });
	if (ImGui::Button("X", ButtonSize)) // teleport vec3 to camkera
	{
		values.x = resetValue.x;
	}
	ImGui::PopStyleColor(3);

	//drag
	ImGui::SameLine();
	ImGui::DragFloat("##x", &values.x, 0.1f);
	ImGui::PopItemWidth();
	ImGui::SameLine();

	//y
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.0f, 0.5f, 0.0f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.0f, 0.9f, 0.0f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f });
	if (ImGui::Button("y", ButtonSize)) // teleport vec3 to camkera
	{
		values.y = resetValue.y;
	}
	ImGui::PopStyleColor(3);

	//drag
	ImGui::SameLine();
	ImGui::DragFloat("##y", &values.y, 0.1f);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	// z
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.0f, 0.0f, 0.5f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.0f, 0.0f, 0.9f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f });

	if (ImGui::Button("z", ButtonSize)) // teleport vec3 to camkera
	{
		values.z = resetValue.z;
	}
	ImGui::PopStyleColor(3);

	//drag
	ImGui::SameLine();
	ImGui::DragFloat("##z", &values.z, 0.1f);
	ImGui::PopItemWidth();

	ImGui::PopStyleVar();

	ImGui::Columns(1);

	ImGui::PopID();
}

// ID, windowTitle, fileTypeFilters
void FEImGui::spawnFileWindow(const std::string& ID, const std::string& windowTitle, const std::string& fileTypeFilters, const std::string& rootPath)
{
	IGFD::FileDialogConfig config;
	config.path = rootPath;
	ImGuiFileDialog::Instance()->OpenDialog(ID, windowTitle, fileTypeFilters.c_str(), config);
}

bool FEImGui::renderFileWindow(const std::string& label,
	glm::vec2 MinimumScale,
	glm::vec2 Maxscale)
{
	if (ImGuiFileDialog::Instance()->Display(label, 0, ImVec2(MinimumScale.x, MinimumScale.y), ImVec2(Maxscale.x, Maxscale.y))) {
	
		return true;
	}
	else {
		return false;
	}
}

// becomes true on okay
std::string FEImGui::fetchFilePath(const std::string& FilePath)
{
	if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
		std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
		return filePathName;
	}
}

// becomes true on okay
std::string FEImGui::fetchFileNamePath(const std::string& fileNamePath)
{
	if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
		std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
		return filePath;
	}
}
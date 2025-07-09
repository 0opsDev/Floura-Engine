#include "ModelObject.h"
#include <utils/VisibilityChecker.h>
#include <Render/passes/geometry/geometryPass.h>


void ModelObject::LODModelLoad(std::string path) {
	std::ifstream file(path);
	if (!file.is_open()) {
		std::cout << "Failed to open file: " << path << std::endl;
		Model TempLODModel;
		TempLODModel.init("Assets/Dependants/placeholder/placeholder.gltf");
		LODModels.push_back(TempLODModel);
		LodCount = LODModels.size();
		return;
	}

	json modelData;
	try {
		file >> modelData;
	}
	catch (const std::exception& e) {
		std::cout << "Error parsing JSON file: " << e.what() << std::endl;
		Model TempLODModel;
		TempLODModel.init("Assets/Dependants/placeholder/placeholder.gltf");
		LODModels.push_back(TempLODModel);
		LodCount = LODModels.size();
		return;

	}
	file.close();
	//TempLODModel.init((path).c_str());
	LodPath = modelData[0]["LodPath"];
	ModelFileNames = modelData[0]["ModelFileNames"];
	LodDistance = modelData[0]["LodDistance"];
	for (int i = 0; i < ModelFileNames.size(); i++)
	{
		Model TempLODModel;
		TempLODModel.init( (LodPath + ModelFileNames[i]).c_str());
		//std::cout << LodPath + ModelFileNames[i] << " " << i << std::endl;
		LODModels.push_back(TempLODModel);
		LodCount = i;
	}
	//std::cout << LODModels.size() << std::endl;
}

void ModelObject::SingleModelLoad(std::string path) {
	std::ifstream file(path);
	if (!file.is_open()) {
		std::cerr << "Failed to open file: " << path << std::endl;
		// << should return ? Model here
		ModelSingle.init("Assets/Dependants/placeholder/placeholder.gltf");
		return;
	}

	json modelData;
	try {
		file >> modelData;
	}
	catch (const std::exception& e) {
		std::cout << "Error parsing JSON file: " << e.what() << std::endl;
		ModelSingle.init("Assets/Dependants/placeholder/placeholder.gltf");
		return;
		
	}
	file.close();
	ModelSingle.init((path).c_str());
}
//init
void ModelObject::CreateObject(std::string type, std::string path, std::string ObjectNameT) {
	ObjectName = ObjectNameT;
	ModelPath = path;
	if (type == "LOD" || type == "lod" || type == "Lod") {
		CubeCollider.init();
		LODModelLoad(path);
		IsLod = true;
	}
	else if (type == "Static" || type == "STATIC") {
		CubeCollider.init();
		SingleModelLoad(path);
		IsLod = false;
	}
	else {
		std::cout << "ModelObject type: " << type << " not found" << std::endl;
	}

}

void ModelObject::UpdateCollider() {
	if (isCollider) {
		//std::cout << transform.x << " " << transform.y << " " << transform.z << std::endl;
		CubeCollider.colliderXYZ = transform + BoxColliderTransform;
		CubeCollider.colliderScale = BoxColliderScale;
	}
}

void ModelObject::UpdateCameraCollider() {
	if (isCollider) {
		CubeCollider.update();
	}
}

unsigned int CalculateLOD(const glm::vec3& cameraPos, const glm::vec3& transform, float LodDistance, unsigned int LodCount) {
	float distance = glm::distance(cameraPos, transform);
	float step = LodDistance / static_cast<float>(LodCount);

	// Clamp the LOD level to ensure it stays within bounds
	unsigned int lodLevel = std::min(LodCount, static_cast<unsigned int>(distance / step));

	return lodLevel;
}

void ModelObject::renderLogic(Shader& Shader) {
	if (!RenderClass::DoForwardLightingPass && !RenderClass::DoDeferredLightingPass) {
		return; // Skip rendering if not in regular or lighting pass
	}

	if (DoCulling == true && !ImGuiWindow::isWireframe) { glEnable(GL_CULL_FACE); }
	else { glDisable(GL_CULL_FACE); }

	//if (InsideFaceDirection) { glFrontFace(GL_CW); }
	//else { glFrontFace(GL_CCW); }

	if (CullFrontFace) { glCullFace(GL_FRONT); }
	else { glCullFace(GL_BACK); }

	if (ImGuiWindow::isWireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Enable wireframe mode
	}
	
	switch (IsLod) {
	case true: {
		for (size_t i = 0; i < LODModels.size(); i++)
		{			
			//std::cout << CalculateLOD(Camera::Position, transform, LodDistance, LodCount);
			if (i == CalculateLOD(Camera::Position, transform, LodDistance, LodCount)) {
				if (RenderClass::DoForwardLightingPass) {
					glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::FBO);
					glEnable(GL_DEPTH_TEST);
					glDepthFunc(GL_LESS);
					LODModels[i].Draw(Shader, glm::vec3(transform.x, transform.y, transform.z), rotation, scale);
					glBindFramebuffer(GL_FRAMEBUFFER, 0);
				}

				GeometryPass::gPassDraw(LODModels[i], glm::vec3(transform.x, transform.y, transform.z), rotation, scale);
			}
		}
		break;
	}
	case false: {
			if (RenderClass::DoForwardLightingPass) {
				glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::FBO);
				glEnable(GL_DEPTH_TEST);
				glDepthFunc(GL_LESS);
				ModelSingle.Draw(Shader, transform, rotation, scale);
				
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}
			GeometryPass::gPassDraw(ModelSingle, transform, rotation, scale);
		break;
	}
			  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Enable wireframe mode
	}
	
	//glFrontFace(GL_CCW);
	glCullFace(GL_BACK); // Reset culling to default
	glDisable(GL_CULL_FACE);
}

void ModelObject::draw(Shader &Shader) {
	if (DoFrustumCull) {
		if (Camera::isBoxInFrustum((frustumBoxTransform + transform), frustumBoxScale) || VisibilityChecker::isInRange((frustumBoxTransform + transform), Camera::Position, 1 + (0.1))) {
			renderLogic(Shader);
		}
	}
	else {
		renderLogic(Shader);
	}
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	if (isCollider) {
		CubeCollider.draw();
	}

}

void ModelObject::Delete() {
	if (IsLod) {
		for (size_t i = 0; i < LODModels.size(); i++)
		{LODModels[i].Delete();}
		LODModels.clear();

		ModelFileNames.clear();
	}
	else {
		ModelSingle.Delete();
	}
	CubeCollider.Delete();
}
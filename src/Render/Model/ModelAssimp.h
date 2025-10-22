#ifndef AMODEL_CLASS_H
#define AMODEL_CLASS_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <Render/Shader/shaderClass.h>
#include "string"
#include "iostream"
#include "Mesh.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

class aModel {
public:

	glm::vec3 gPosition = glm::vec3(0.0f), gRotation = glm::vec3(0.0f), gScale = glm::vec3(1.0f);

	void aModel::updatePosition(glm::vec3 Position);

	void aModel::updateRotation(glm::vec3 Rotation);

	void aModel::updateScale(glm::vec3 Scale);

	void create(const char* file);

	void draw(Shader &shader);

	void Delete();

private:

	std::vector<aMesh> meshes;
	std::vector<glm::vec3>lPosition;
	std::vector<glm::vec3>lScale;
	std::vector<glm::quat>lRotation;
	std::vector <glm::mat4> lModelMatrix;

	std::string directory;
	std::vector<std::string> loadedTexPath;
	std::vector<Texture> loadedTex;

	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	void processPositions(aiNode* node);
	aMesh processMesh(aiMesh* mesh, const aiScene* scene);

	std::vector<Texture> aloadMaterialTextures(aiMaterial* mat, aiTextureType type,
		std::string typeName);
};

#endif

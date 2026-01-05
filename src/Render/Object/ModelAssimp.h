#ifndef Model_CLASS_H
#define Model_CLASS_H

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
#include <xhash>

class Model {
public:

	struct transformation {
		glm::vec3 position = glm::vec3(0.0f);
		glm::vec3 rotation = glm::vec3(0.0f);
		glm::quat qRotation = glm::quat(0.0f, 0.0f, 0.0f, 0.0f);
		glm::vec3 scale = glm::vec3(1.0f);
	};

	transformation globalTransformation;
	glm::mat4 gModelMatrix = glm::mat4(1.0f);

	glm::mat4 modelMatrix = glm::mat4(1.0f);

	Model(const char* file);
	~Model();

	void draw(Shader &shader, Camera camera);

	std::vector<Mesh> meshes;
	std::vector<transformation>localTransformation;
	std::vector <glm::mat4> lModelMatrix;

private:
	std::string directory;
	std::vector<std::string> loadedTexPath;
	std::vector<Texture> loadedTex;

	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	void processPositions(aiNode* node);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);

	std::vector<Texture> aloadMaterialTextures(aiMaterial* mat, aiTextureType type,
		std::string typeName, int slot);
};

#endif

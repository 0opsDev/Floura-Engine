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
#include "core/Render.h"

class Model {
public:

	RenderClass::transformation globalTransformation;

	void updatePosition(glm::vec3 Position);

	void updateRotation(glm::vec3 Rotation);

	void updateScale(glm::vec3 Scale);

	void create(const char* file);

	void draw(Shader &shader);

	void Delete();

private:

	std::vector<Mesh> meshes;
	std::vector<RenderClass::transformation>localTransformation;
	std::vector <glm::mat4> lModelMatrix;

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

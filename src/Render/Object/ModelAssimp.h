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
#include "Systems/Physics/Collision.h"
#include <xhash>

class Model {
public:

	uint64_t UUID;

	RenderClass::transformation globalTransformation;
	glm::mat4 gModelMatrix = glm::mat4(1.0f);

	void updatePosition(glm::vec3 Position);

	void updateRotation(glm::vec3 Rotation);

	void updateScale(glm::vec3 Scale);

	void updateTranformation();

	Model(const char* file);
	~Model();

	void draw(Shader &shader);

	void createMeshAABBs();

	void updateMeshAABBs();

	// AABBS position stored in local space
	std::vector<Collision::AABB> MeshAABBs;
	std::vector<Mesh> meshes;
	std::vector<RenderClass::transformation>localTransformation;
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

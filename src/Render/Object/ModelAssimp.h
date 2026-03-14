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
#include <map>
#include <iterator>
#include "Render/Animated/animdata.h"
#include  <thread>

#define MAX_BONE_INFLUENCE 4

class Model {
public:

	uint64_t UUID;// modelID
	uint64_t renderID;
	std::vector<uint64_t>instanceUUIDs;

	struct transformation {
		glm::vec3 position = glm::vec3(0.0f);
		glm::vec3 rotation = glm::vec3(0.0f);
		glm::quat qRotation = glm::quat(0.0f, 0.0f, 0.0f, 0.0f);
		glm::vec3 scale = glm::vec3(1.0f);
	};

	transformation globalTransformation;
	transformation previousGlobalTransformation;
	glm::mat4 gModelMatrix = glm::mat4(1.0f);
	glm::mat4 pgModelMatrix = glm::mat4(1.0f);

	void updatePosition(glm::vec3 Position);
	void updateRotation(glm::vec3 Rotation);
	void updateScale(glm::vec3 Scale);
	void updateTranformation();
	
	void updatePrevPosition(glm::vec3 Position);
	void updatePrevRotation(glm::vec3 Rotation);
	void updatePrevScale(glm::vec3 Scale);
	void updatePrevTranformation();

	Model(const char* file);
	~Model();
	
	auto& GetBoneInfoMap() { return m_BoneInfoMap; }
	int& GetBoneCount() { return m_BoneCounter; }

	void draw(Shader &shader, Camera Camera);

	void drawInstance(Shader& shader, Camera Camera, int instanceCount);

	void createMeshAABBs();

	void updateMeshAABBs();

	// AABBS position stored in local space
	std::vector<Collision::AABB> rootnodes;
	std::vector<Mesh> meshes;
	std::vector<Collision::rubiksCubePoints> meshAabbPoints;

	std::vector<transformation> localTransformation;
	std::vector <glm::mat4> lModelMatrix;
	
	// informative
	int totalVertices = 0;
	int totalIndices = 0;
	int totalBones = 0;
	std::string directory;

private:
	std::vector<std::string> loadedTexPath;
	std::vector<Texture> loadedTex;
	
	// bones
	std::map<std::string, BoneInfo> m_BoneInfoMap;
	int m_BoneCounter = 0;
	
	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	void processPositions(aiNode* node);
	
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);

	void SetVertexBoneDataToDefault(Vertex& vertex);
	void SetVertexBoneData(Vertex& vertex, int boneID, float weight);
	void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);
	
	std::vector<Vertex> assembleVertices(aiMesh* mesh);
	std::vector<GLuint> assembleIndices(aiMesh* mesh);
	std::vector<Texture> assembleMaterials(aiMesh* mesh, const aiScene* scene);
	
	
	std::vector<Texture> aloadMaterialTextures(aiMaterial* mat, aiTextureType type,
		std::string typeName, int slot);
	
};

#endif

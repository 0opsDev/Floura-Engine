#ifndef BVH_H
#define BVH_H
#include "glm/glm.hpp"
#include <xhash>
#include <vector>
#include <Scene/scene.h>
#include <Systems/Physics/Collision.h>

class BVH
{
public:

	// move bvh and broadphase type functions here later

	struct AABB
	{
		glm::vec3 position;
		glm::vec3 scale;
	};

	struct root
	{
		AABB rootnode;
		uint64_t ModelUUID;
		uint64_t MeshUUID;
	};

	struct topNode
	{
		AABB Node;
		int layer; // layer count
		int floorNum; // count of encompased objects inside of node (floor below current layer)
		int offset;
	};

	struct tlas // calculated from all rootnodes up
	{
		int layers;
		int childrenPerNode;
		std::vector<root> rootnodes; // root layer (mesh)
		std::vector<topNode> tNodes; // above layers
	};

	struct blas // calculated from induvidual rootnode down
	{
	};

	static tlas nTlas;

	static void uploadSceneRootsToTlas(std::vector <std::unique_ptr<entity>> entityObjects);

	static void generateTlas(int childPerNodeSize, int layerCount);

	static Collision::AABB rootNodeFromRubixPoints(Collision::rubiksCubePoints points,
		glm::mat4 ModelMatrix); // returns rootnode

private:

};

#endif
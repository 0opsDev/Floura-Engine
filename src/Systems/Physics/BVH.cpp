#include "BVH.h"
#include "utils/FE_math.h"
#include <Render/Handler/RenderHandler.h>
BVH::tlas BVH::nTlas;

BVH::minmax BVH::returnMinMax(glm::vec3 p, glm::vec3 s)
{
    minmax newMinMax;
    newMinMax.min = p - (s * glm::vec3(0.5f)),
    newMinMax.max = p + (s * glm::vec3(0.5f));
    return newMinMax;
}

void BVH::uploadSceneRootsToTlas(std::vector<std::unique_ptr<entity>> entityObjects)
{
    nTlas.rootnodes.clear(); // clear prior nodes

    // get root nodes
    for (size_t i = 0; i < entityObjects.size(); i++)
    {
        int index = RenderHandler::fetchModelIndex(Scene::entityObjects[i]->component.render.renderID);
        if (index != -1)
        {
            for (size_t x = 0; x < RenderHandler::models[index].model->meshes.size(); x++) // each mesh
            {
                root newRootNode;
                newRootNode.ModelUUID = entityObjects[i]->UUID;
                newRootNode.MeshUUID = RenderHandler::models[index].model->meshes[x].UUID;
                newRootNode.rootnode.position = Scene::entityObjects[i]->component.collider.rootnodes[x].position;
                newRootNode.rootnode.scale = Scene::entityObjects[i]->component.collider.rootnodes[x].size;
                nTlas.rootnodes.push_back(newRootNode); // meshes
            }
        }
    }
}

void BVH::generateTlas(int childPerNodeSize, int layerCount)
{

    for (size_t i = 1; i < layerCount; i++) // layer
    {
        topNode newTopNode;
        if (i == 1) // i want one to pull data from root nodes
        {

            return;
        }
    }

}

Collision::AABB BVH::rootNodeFromRubixPoints(Collision::rubiksCubePoints points,
    glm::mat4 ModelMatrix)
{
    Collision::AABB newNode;

    //glm::mat4 newLMat = FE_Math::composeMatrixWDegrees(position, scale, rotation);
    //glm::mat4 newGMat = FE_Math::composeMatrixWDegrees(globalPosition, globalScale, globalRotation);

    //glm::mat4 newMat = newGMat * newLMat;

    Collision::rubiksCubePoints newpoints = Collision::transformRubiks(points, ModelMatrix);

    newNode = Collision::createAABBfromRubiksCubePoints(newpoints);
    newNode.size = FE_Math::pad(newNode.size, 0.1f);
    //boxCollider.position += finalGlobalPos + finalpos;

    return newNode;
}
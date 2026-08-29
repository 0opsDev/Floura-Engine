#include "SDF.h"
#include <utils/FE_math.h>

#include "Render/Handler/RenderHandler.h"
#include "utils/logConsole.h"
#include <utils/imageWrite.h>
#include <glm/gtx//matrix_decompose.hpp>

void flouraSDF::cacheSDF(const char* path, int hash, std::vector<Texture3D *>& meshSDFs){
	for (int i = 0; i < meshSDFs.size(); ++i){
		std::string nPath = path + std::to_string(hash) + "_" + std::to_string(i) +".SDF";
		FlouraImageWrite::writeImage3DToDiskJSON(meshSDFs[i]->ID,
			meshSDFs[i]->width, meshSDFs[i]->height, meshSDFs[i]->depth,
			nPath.c_str(), GL_RGB, GL_FLOAT, 3); // GB
	}
}

void flouraSDF::bakeMeshSDF(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, Collision::AABB root,
                            const int sliceSize, Texture3D& texture, GLuint slot){
    
    Collision::minmax mm = Collision::returnMinMax(root.position, root.size);
    glm::vec3 boundssize = mm.max - mm.min;
    
    // still here incase i change my mind
    const int sX = sliceSize;
    const int sY = sliceSize;
    const int sZ = sliceSize;

    //std::vector<float> sdfData(sX * sY * sZ);
    std::vector<float> sdfData(sX * sY * sZ * 3);

    for (int x = 0; x < sX; ++x)
        for (int y = 0; y < sY; ++y)
            for (int z = 0; z < sZ; ++z)
            {
                float u = (x + 0.5f) / sX;
                float v = (y + 0.5f) / sY;
                float w = (z + 0.5f) / sZ;
                
                glm::vec3 p = mm.min + glm::vec3(u, v, w) * boundssize;
                
                //float dist = Collision::distanceToClosestPointOnMeshSDF(vertices, indices, p);
                glm::vec3 dist_pUV = Collision::distanceToClosestPointOnMeshSDF_PlusUV(vertices, indices, p);
                
                //int index = x + (y * sX) + (z * sX * sY);
                //sdfData[index] = dist_pUV.x;
                
                int index = (x + (y * sX) + (z * sX * sY)) * 3; // one for each component
                sdfData[index] = dist_pUV.x;
                sdfData[index + 1] = dist_pUV.y;
                sdfData[index + 2] = dist_pUV.z;
            }
    
    texture.Delete();
    texture.path = "NULL";
    texture.type = "SDF";
    texture.slot = slot;
    texture.width = sX;
    texture.height = sY;
    texture.depth = sZ;
    
    glGenTextures(1, &texture.ID);
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_3D, texture.ID);
    
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // GL_NEAREST_MIPMAP_LINEAR
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    glTexImage3D(
    GL_TEXTURE_3D,
    0,
    GL_RGB32F, //     GL_R32F,
    sX,
    sY,
    sZ,
    0,
    GL_RGB, //GL_RED
    GL_FLOAT,
    sdfData.data()
    );
    glGenerateMipmap(GL_TEXTURE_3D);

    if (GLAD_GL_ARB_bindless_texture) {
        texture.handle = glGetTextureHandleARB(texture.ID);
        glMakeTextureHandleResidentARB(texture.handle);
    }
    glBindTexture(GL_TEXTURE_3D, 0);
}

void flouraSDF::bakeMeshSDFAccel(std::vector<Vertex>& vertices,
    std::vector<Collision::voxelAccel>& voxelAccel, Collision::AABB root, const int sliceSize, Texture3D& texture,
    GLuint slot){
    
    Collision::minmax mm = Collision::returnMinMax(root.position, root.size);
    glm::vec3 boundssize = mm.max - mm.min;
    
    // still here incase i change my mind
    const int sX = sliceSize;
    const int sY = sliceSize;
    const int sZ = sliceSize;

    //std::vector<float> sdfData(sX * sY * sZ);
    std::vector<float> sdfData(sX * sY * sZ * 3);

    for (int x = 0; x < sX; ++x)
        for (int y = 0; y < sY; ++y)
            for (int z = 0; z < sZ; ++z)
            {
                float u = (x + 0.5f) / sX;
                float v = (y + 0.5f) / sY;
                float w = (z + 0.5f) / sZ;
                
                glm::vec3 p = mm.min + glm::vec3(u, v, w) * boundssize;
                
                //float dist = Collision::distanceToClosestPointOnMeshSDF(vertices, indices, p);
                glm::vec3 dist_pUV = Collision::distanceToClosestPointOnMeshSDFAccel_PlusUV(vertices, voxelAccel, p);
                //glm::vec3 dist_pUV;
                //int index = x + (y * sX) + (z * sX * sY);
                //sdfData[index] = dist_pUV.x;
                
                int index = (x + (y * sX) + (z * sX * sY)) * 3; // one for each component
                sdfData[index] = dist_pUV.x;
                sdfData[index + 1] = dist_pUV.y;
                sdfData[index + 2] = dist_pUV.z;
            }
    
    texture.Delete();
    texture.path = "NULL";
    texture.type = "SDF";
    texture.slot = slot;
    texture.width = sX;
    texture.height = sY;
    texture.depth = sZ;
    
    glGenTextures(1, &texture.ID);
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_3D, texture.ID);
    
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // GL_NEAREST_MIPMAP_LINEAR
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    glTexImage3D(
    GL_TEXTURE_3D,
    0,
    GL_RGB32F, //     GL_R32F,
    sX,
    sY,
    sZ,
    0,
    GL_RGB, //GL_RED
    GL_FLOAT,
    sdfData.data()
    );
    glGenerateMipmap(GL_TEXTURE_3D);

    if (GLAD_GL_ARB_bindless_texture) {
        texture.handle = glGetTextureHandleARB(texture.ID);
        glMakeTextureHandleResidentARB(texture.handle);
    }
    glBindTexture(GL_TEXTURE_3D, 0);
}

void flouraSDF::bakeMeshSDFAccel(std::vector<Vertex>& vertices, std::vector<BVH::BVH_primitive>& prims,
	Collision::AABB root, const int sliceSize, Texture3D& texture, GLuint slot){
	        
    Collision::minmax mm = Collision::returnMinMax(root.position, root.size);
    glm::vec3 boundssize = mm.max - mm.min;
    
    // still here incase i change my mind
    const int sX = sliceSize;
    const int sY = sliceSize;
    const int sZ = sliceSize;

    //std::vector<float> sdfData(sX * sY * sZ);
    std::vector<float> sdfData(sX * sY * sZ * 3);

    for (int x = 0; x < sX; ++x)
        for (int y = 0; y < sY; ++y)
            for (int z = 0; z < sZ; ++z)
            {
                float u = (x + 0.5f) / sX;
                float v = (y + 0.5f) / sY;
                float w = (z + 0.5f) / sZ;
                
                glm::vec3 p = mm.min + glm::vec3(u, v, w) * boundssize;
                
                //float dist = Collision::distanceToClosestPointOnMeshSDF(vertices, indices, p);
                //glm::vec3 dist_pUV = Collision::distanceToClosestPointOnMeshSDFAccel_PlusUV(vertices, voxelAccel, p);
                
                glm::vec3 dist_pUV = distanceToClosestPointOnMeshSDFAccel_PlusUV(vertices, prims, p);
            	//dist_pUV = glm::vec3(dist_pUV.r);
                //int index = x + (y * sX) + (z * sX * sY);
                //sdfData[index] = dist_pUV.x;
                
                int index = (x + (y * sX) + (z * sX * sY)) * 3; // one for each component
                sdfData[index] = dist_pUV.x;
                sdfData[index + 1] = dist_pUV.y;
                sdfData[index + 2] = dist_pUV.z;
            }
    
    texture.Delete();
    texture.path = "NULL";
    texture.type = "SDF";
    texture.slot = slot;
    texture.width = sX;
    texture.height = sY;
    texture.depth = sZ;
    
    glGenTextures(1, &texture.ID);
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_3D, texture.ID);
    
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // GL_NEAREST_MIPMAP_LINEAR
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    glTexImage3D(
    GL_TEXTURE_3D,
    0,
    GL_RGB32F, //     GL_R32F,
    sX,
    sY,
    sZ,
    0,
    GL_RGB, //GL_RED
    GL_FLOAT,
    sdfData.data()
    );
    glGenerateMipmap(GL_TEXTURE_3D);

    if (GLAD_GL_ARB_bindless_texture) {
        texture.handle = glGetTextureHandleARB(texture.ID);
        glMakeTextureHandleResidentARB(texture.handle);
    }
    glBindTexture(GL_TEXTURE_3D, 0);
}

void flouraSDF::bakeMeshSDFAccel(std::vector<Vertex>& vertices, std::vector<BVH::leaf>& leaves,
                                 Collision::AABB root, const int sliceSize, Texture3D& texture, GLuint slot){
    Collision::minmax mm = Collision::returnMinMax(root.position, root.size);
    glm::vec3 boundssize = mm.max - mm.min;
    
    // still here incase i change my mind
    const int sX = sliceSize;
    const int sY = sliceSize;
    const int sZ = sliceSize;

    //std::vector<float> sdfData(sX * sY * sZ);
    std::vector<float> sdfData(sX * sY * sZ * 3);

    for (int x = 0; x < sX; ++x)
        for (int y = 0; y < sY; ++y)
            for (int z = 0; z < sZ; ++z)
            {
                float u = (x + 0.5f) / sX;
                float v = (y + 0.5f) / sY;
                float w = (z + 0.5f) / sZ;
                
                glm::vec3 p = mm.min + glm::vec3(u, v, w) * boundssize;
                
                //float dist = Collision::distanceToClosestPointOnMeshSDF(vertices, indices, p);
                //glm::vec3 dist_pUV = Collision::distanceToClosestPointOnMeshSDFAccel_PlusUV(vertices, voxelAccel, p);
                
                glm::vec3 dist_pUV = distanceToClosestPointOnMeshSDFAccel_PlusUV(vertices, leaves, p);
                //int index = x + (y * sX) + (z * sX * sY);
                //sdfData[index] = dist_pUV.x;
                
                int index = (x + (y * sX) + (z * sX * sY)) * 3; // one for each component
                sdfData[index] = dist_pUV.x;
                sdfData[index + 1] = dist_pUV.y;
                sdfData[index + 2] = dist_pUV.z;
            }
    
    texture.Delete();
    texture.path = "NULL";
    texture.type = "SDF";
    texture.slot = slot;
    texture.width = sX;
    texture.height = sY;
    texture.depth = sZ;
    
    glGenTextures(1, &texture.ID);
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_3D, texture.ID);
    
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // GL_NEAREST_MIPMAP_LINEAR
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    glTexImage3D(
    GL_TEXTURE_3D,
    0,
    GL_RGB32F, //     GL_R32F,
    sX,
    sY,
    sZ,
    0,
    GL_RGB, //GL_RED
    GL_FLOAT,
    sdfData.data()
    );
    glGenerateMipmap(GL_TEXTURE_3D);

    if (GLAD_GL_ARB_bindless_texture) {
        texture.handle = glGetTextureHandleARB(texture.ID);
        glMakeTextureHandleResidentARB(texture.handle);
    }
    glBindTexture(GL_TEXTURE_3D, 0);
}


// this function is broken


void flouraSDF::nearestPointBlasTraversal(std::vector<BVH::leaf>& leaves, std::vector<Vertex>& vertices, float &minDist, int &minIndex, int& closestPrimIndex, int cLeafIndex, glm::vec3& P){
    if (leaves.empty() || cLeafIndex < 0) return;
    
    // nearest point on node
	glm::vec3 np = Collision::nearestPointOnAABB(P, leaves[cLeafIndex].aabb.position, leaves[cLeafIndex].aabb.size);
    float nd = glm::distance(np, P); // get the distance
	
	if (nd >= minDist)return;
	
	if  (leaves[cLeafIndex].firstChildIndex <= -1 && leaves[cLeafIndex].secondChildIndex <= -1){
		//minIndex = cLeafIndex;
		//minDist = nd;
		
		float primMinDist = minDist;
		for (int i = 0; i < leaves[cLeafIndex].prims.size(); ++i){
			const glm::vec3 npp = Collision::nearestPointOnAABB(P, leaves[cLeafIndex].prims[i].extents.position, leaves[cLeafIndex].prims[i].extents.size);
			float npd = glm::distance(npp, P);
			if (npd < primMinDist){
				const unsigned int &i0 = leaves[cLeafIndex].prims[i].i0;
				const unsigned int &i1 = leaves[cLeafIndex].prims[i].i1;
				const unsigned int &i2 = leaves[cLeafIndex].prims[i].i2;
				
				if (i0 >= vertices.size() ||
					i1 >= vertices.size() ||
					i2 >= vertices.size())
					continue;
				
				const Vertex* cV1 = &vertices[i0];
				const Vertex* cV2 = &vertices[i1];
				const Vertex* cV3 = &vertices[i2];
	
				const glm::vec3 tnp = Collision::closestPointOnTriangle(P, cV1->position, cV2->position, cV3->position);
				float tnd = glm::distance(tnp, P);
				if (tnd < minDist){
					closestPrimIndex = i;
					minIndex = cLeafIndex;
					minDist = tnd;
				}
			}
		}
		return;
	}
	//minDist = nd;
	//minDistIndex = cLeafIndex;
	//std::cout << "index: " << cLeafIndex<< std::endl;
	int firstChildInd = leaves[cLeafIndex].firstChildIndex;
	int secondChildInd = leaves[cLeafIndex].secondChildIndex;
	
	float fnd = std::numeric_limits<float>::max();
	float snd = std::numeric_limits<float>::max();
	
	if (firstChildInd >= 0){
		glm::vec3 fnp = Collision::nearestPointOnAABB(P, leaves[firstChildInd].aabb.position, leaves[firstChildInd].aabb.size);
		fnd = glm::distance(fnp, P); // get the distance
	}
	if (secondChildInd >= 0){
		glm::vec3 snp = Collision::nearestPointOnAABB(P, leaves[secondChildInd].aabb.position, leaves[secondChildInd].aabb.size);
		snd = glm::distance(snp, P); // get the distance
	}
	
	if (fnd < snd){
		if (firstChildInd >= 0 && fnd < minDist)nearestPointBlasTraversal(leaves, vertices, minDist, minIndex, closestPrimIndex, leaves[cLeafIndex].firstChildIndex, P);
		if (secondChildInd >= 0 && snd < minDist)nearestPointBlasTraversal(leaves, vertices, minDist,minIndex, closestPrimIndex, leaves[cLeafIndex].secondChildIndex, P);
	}
	else{
		if (secondChildInd >= 0 && snd < minDist)nearestPointBlasTraversal(leaves, vertices, minDist,minIndex, closestPrimIndex, leaves[cLeafIndex].secondChildIndex, P);
		if (firstChildInd >= 0 && fnd < minDist)nearestPointBlasTraversal(leaves, vertices, minDist, minIndex, closestPrimIndex, leaves[cLeafIndex].firstChildIndex, P);
	}
}

void flouraSDF::nearestNeighbourPrims(std::vector<BVH::BVH_primitive>& prims, int& closestPrimIndex, glm::vec3& P){
	float minDist = std::numeric_limits<float>::max();
        
	for (int i = 0; i < prims.size(); ++i){
		const glm::vec3 np = Collision::nearestPointOnAABB(P, prims[i].extents.position, prims[i].extents.size);
		float nd = glm::distance(np, P);
		if (nd < minDist){
			minDist = nd;
			closestPrimIndex = i;
		}
	}
}

void flouraSDF::nearestNeighbourPrims(std::vector<Vertex>& vertices, std::vector<BVH::BVH_primitive>& prims,
	int& closestPrimIndex, glm::vec3& P){
	float minDist = std::numeric_limits<float>::max();
        
	for (int i = 0; i < prims.size(); ++i){
		const glm::vec3 np = Collision::nearestPointOnAABB(P, prims[i].extents.position, prims[i].extents.size);
		float nd = glm::distance(np, P);
		if (nd < minDist){
			//minDist = nd;
			
			const unsigned int &i0 = prims[i].i0;
			const unsigned int &i1 = prims[i].i1;
			const unsigned int &i2 = prims[i].i2;
			
			if (i0 >= vertices.size() ||
				i1 >= vertices.size() ||
				i2 >= vertices.size())
				continue;
			
			const Vertex* cV1 = &vertices[i0];
			const Vertex* cV2 = &vertices[i1];
			const Vertex* cV3 = &vertices[i2];
	
			const glm::vec3 tnp = Collision::closestPointOnTriangle(P, cV1->position, cV2->position, cV3->position);
			float tnd = glm::distance(tnp, P);
			if (tnd < minDist){
				closestPrimIndex = i;
				minDist = tnd;
			}
		}
	}
}

glm::vec3 flouraSDF::distanceToClosestPointOnMeshSDFAccel_PlusUV(std::vector<Vertex>& vertices,
                                                                 std::vector<BVH::leaf>& leaves, glm::vec3& P){
	if (vertices.empty()) return glm::vec3(0.0f);
	
	float minDist = std::numeric_limits<float>::max();
	int closestLeafIndex = -1;
	int closestPrimIndex = -1;
	
	nearestPointBlasTraversal(leaves, vertices, minDist, closestLeafIndex, closestPrimIndex, static_cast<int>(leaves.size()) - 1, P);
	
	//nearestPointBlasTraversal(leaves, minDist, closestLeafIndex, 0 , P);
	
	if (closestLeafIndex < 0){		
		std::cout <<  "closestLeafIndex below zero" << std::endl;
		return glm::vec3(0.0f);
	}
	
	//int closestPrimIndex = -1;
	//closestPrimIndex = closestLeafIndex;
	
	//nearestNeighbourPrims(leaves[closestLeafIndex].prims, closestPrimIndex, P);
	//nearestNeighbourPrims(vertices, leaves[closestLeafIndex].prims, closestPrimIndex, P);
	
	if (closestPrimIndex < 0){
		std::cout <<  "closestPrimIndex below zero" << std::endl;
		return glm::vec3(0.0f);
	}
	
	const unsigned int &i0 = leaves[closestLeafIndex].prims[closestPrimIndex].i0;
	const unsigned int &i1 = leaves[closestLeafIndex].prims[closestPrimIndex].i1;
	const unsigned int &i2 = leaves[closestLeafIndex].prims[closestPrimIndex].i2;
        
	if (i0 >= vertices.size() ||
	i1 >= vertices.size() ||
	i2 >= vertices.size())
		return glm::vec3(0.0f);
    
	minDist = std::numeric_limits<float>::max();
	glm::vec3 cP(0.0f);
	bool anyHit(false);
	
	const Vertex* cV1 = &vertices[i0];
	const Vertex* cV2 = &vertices[i1];
	const Vertex* cV3 = &vertices[i2];
	
	const glm::vec3 np = Collision::closestPointOnTriangle(P, cV1->position, cV2->position, cV3->position);
	float nd = glm::distance(np, P);
	if (nd < minDist){
		anyHit = true;
		minDist = nd;
		cP = np;
	}
	
	if (!anyHit){
		std::cout <<  "anyHit fail" << std::endl;
		return glm::vec3(0.0f);
	}

	// if directions are opossing, then flip ld (calc face normal here to cut down on calcs)
	//if (glm::dot(P - cP, FE_Math::faceNormalFromTriangle(cV1->position, cV2->position, cV3->position, cV1->normal, cV2->normal, cV3->normal)) < 0.0f)
	if (glm::dot(P - cP, FE_Math::faceNormalFromTriangle(cV1->position, cV2->position, cV3->position)) < 0.0f)
		minDist = -minDist;
	
	// calc uv with return instead of every cycle to cut down on calcs
	//return glm::vec3(minDist);
	return glm::vec3(minDist, FE_Math::uvPosFromVertexAndPoint(cV1->position,cV2->position,cV3->position, cV1->texUV, cV2->texUV, cV3->texUV, cP));
}

glm::vec3 flouraSDF::distanceToClosestPointOnMeshSDFAccel_PlusUV(std::vector<Vertex>& vertices,
	std::vector<BVH::BVH_primitive>& prims, glm::vec3& P){
	int closestPrimIndex = -1;
	
	nearestNeighbourPrims(vertices, prims, closestPrimIndex, P);
	
	if (closestPrimIndex < 0){
		std::cout <<  "closestPrimIndex below zero" << std::endl;
		return glm::vec3(0.0f);
	}
	
	// reset
	float minDist = std::numeric_limits<float>::max();
	glm::vec3 cP(0.0f);
	bool anyHit(false);
	
	const unsigned int &i0 = prims[closestPrimIndex].i0;
	const unsigned int &i1 = prims[closestPrimIndex].i1;
	const unsigned int &i2 = prims[closestPrimIndex].i2;
        
	if (i0 >= vertices.size() ||
	i1 >= vertices.size() ||
	i2 >= vertices.size())
		return glm::vec3(0.0f);
	
	const Vertex* cV1 = &vertices[i0];
	const Vertex* cV2 = &vertices[i1];
	const Vertex* cV3 = &vertices[i2];

	const glm::vec3 np = Collision::closestPointOnTriangle(P, cV1->position, cV2->position, cV3->position);
	float nd = glm::distance(np, P);
	if (nd < minDist){
		anyHit = true;
		minDist = nd;
		cP = np;
	}
	
	if (!anyHit){
		std::cout <<  "anyHit fail" << std::endl;
		return glm::vec3(0.0f);
	}

	// if directions are opossing, then flip ld (calc face normal here to cut down on calcs)
	if (glm::dot(P - cP, FE_Math::faceNormalFromTriangle(cV1->position, cV2->position, cV3->position)) < 0.0f)
		minDist = -minDist;
	
	// calc uv with return instead of every cycle to cut down on calcs
	return glm::vec3(minDist, FE_Math::uvPosFromVertexAndPoint(cV1->position,cV2->position,cV3->position, cV1->texUV, cV2->texUV, cV3->texUV, cP));
}

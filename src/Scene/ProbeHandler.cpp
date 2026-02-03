#include "ProbeHandler.h"
#include <utils/FE_math.h>


bool ProbeHandler::viewProbes = false;
bool ProbeHandler::dirtyScene = true;
int ProbeHandler::sceneProveArea = 5;
int ProbeHandler::probeCalculationMethod = 2;
int ProbeHandler::indirectSamples = 1;

std::vector<ProbeHandler::probe> ProbeHandler::SceneToProbeSpace(Collision::AABB SceneBounds, int probeArea)
{
	glm::ivec3 iPosition = static_cast<glm::ivec3>(SceneBounds.position);
	glm::ivec3 iScale = static_cast<glm::ivec3>(SceneBounds.size + glm::vec3(float(probeArea)));

	std::vector<ProbeHandler::probe> probeArray;

	for (int x = -iScale.x; x < iScale.x; x+= probeArea)
	{
		for (int y = -iScale.y; y < iScale.y; y+= probeArea)
		{
			for (int z = -iScale.z; z < iScale.z; z+= probeArea)
			{
				probe nProbe;

				nProbe.size = probeArea;
				// need to transform to fit in the centre

				glm::vec3 nPos = glm::vec3(float(x), float(y), float(z));

				glm::mat4 nGlobalMatrix = FE_Math::composeMatrixWDegrees(iPosition, glm::vec3(1.0f), glm::vec3(0.0f));
				FE_Math::transformPoint(nPos, nGlobalMatrix);
				nProbe.position = nPos;

				probeArray.push_back(nProbe);
			}
		}
	}


	return probeArray;
}

std::vector<ProbeHandler::probe> ProbeHandler::aabbsSceneToProbeSpace(Collision::AABB SceneBounds, std::vector<Collision::AABB> aabbs, int probeArea)
{
	glm::ivec3 iPosition = static_cast<glm::ivec3>(SceneBounds.position);
	glm::ivec3 iScale = static_cast<glm::ivec3>(SceneBounds.size + glm::vec3(float(probeArea)) );

	std::vector<ProbeHandler::probe> probeArray;

	for (int x = -iScale.x; x < iScale.x; x += probeArea)
	{
		for (int y = -iScale.y; y < iScale.y; y += probeArea)
		{
			for (int z = -iScale.z; z < iScale.z; z += probeArea)
			{
				probe nProbe;

				nProbe.size = probeArea;
				// need to transform to fit in the centre

				glm::vec3 nPos = glm::vec3(float(x), float(y), float(z));

				glm::mat4 nGlobalMatrix = FE_Math::composeMatrixWDegrees(iPosition, glm::vec3(1.0f), glm::vec3(0.0f));
				FE_Math::transformPoint(nPos, nGlobalMatrix);

				if (!collideWithAnyRootNode(nPos, glm::ivec3(probeArea), aabbs)) continue;

				nProbe.position = nPos;

				probeArray.push_back(nProbe);
			}
		}
	}


	return probeArray;
}

std::vector<ProbeHandler::probe> ProbeHandler::aabbsToProbeSpace(Collision::AABB SceneBounds,std::vector<Collision::AABB> aabbs, int probeArea)
{
	std::vector<ProbeHandler::probe> probeArray;

	glm::ivec3 isPosition = static_cast<glm::ivec3>(SceneBounds.position);

	for (size_t i = 0; i < aabbs.size(); i++)
	{
		glm::ivec3 iPosition = static_cast<glm::ivec3>(aabbs[i].position);
		glm::ivec3 iScale = static_cast<glm::ivec3>(aabbs[i].size + glm::vec3(float(probeArea)));

		for (int x = -iScale.x; x < iScale.x; x += probeArea)
		{
			for (int y = -iScale.y; y < iScale.y; y += probeArea)
			{
				for (int z = -iScale.z; z < iScale.z; z += probeArea)
				{
					probe nProbe;

					nProbe.size = probeArea;
					// need to transform to fit in the centre

					glm::vec3 nPos = glm::vec3(float(x), float(y), float(z));

					glm::mat4 nLocalMatrix = FE_Math::composeMatrixWDegrees(iPosition, glm::vec3(1.0f), glm::vec3(0.0f));

					//FE_Math::transformPoint(nPos, nGlobalMatrix * nLocalMatrix);
					FE_Math::transformPoint(nPos, nLocalMatrix);

					nProbe.position = nPos;

					bool skip = false;

					for (size_t i = 0; i < probeArray.size(); i++)
					{
						if (FE_Math::isInRange(probeArray[i].position, nPos, float(probeArea))) skip = true;

					}
					if (skip) continue;

					glm::mat4 nGlobalMatrix = FE_Math::composeMatrixWDegrees(isPosition, glm::vec3(1.0f), glm::vec3(0.0f));
					FE_Math::transformPoint(nPos, nGlobalMatrix);

					probeArray.push_back(nProbe);
				}
			}
		}
	}

	return probeArray;
}

std::vector<ProbeHandler::probe> ProbeHandler::calculateProbesWithMethod(int calculationMethod, Collision::AABB SceneBounds, std::vector<Collision::AABB> aabbs, int probeArea)
{
	switch (calculationMethod)
	{
	case 0:
		return SceneToProbeSpace(SceneBounds, probeArea);
	case 1:
		return aabbsSceneToProbeSpace(SceneBounds, aabbs, probeArea);
	case 2:
		return aabbsToProbeSpace(SceneBounds, aabbs, probeArea);
	default:
		break;
	}

	return std::vector<ProbeHandler::probe>();
}

bool ProbeHandler::collideWithAnyRootNode(glm::vec3 p, glm::vec3 s ,std::vector<Collision::AABB> aabbs)
{
	bool anyCollide = false;
	for (size_t i = 0; i < aabbs.size(); i++)
	{
		Collision::HitResult hit = Collision::AABBvsAABB(p, s, aabbs[i].position, aabbs[i].size * 2.0f);
		anyCollide = hit.isColliding;

		if (anyCollide) break;
	}
	return anyCollide;
}

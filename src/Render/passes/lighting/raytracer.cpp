#include "raytracer.h"
#include <Render/Shader/Framebuffer.h>
#include "Render/Object/RenderQuad.h"
#include <Render/passes/geometry/geometryPass.h>
#include <utils/FE_math.h>
#include"Render/passes/post/denoise.h"
#include "Core/Render.h"
#include "Scene/scene.h"
#include "Scene/LightingHandler.h"
#include <Render/Handler/RenderHandler.h>
bool raytracer::RTGlobalTransformFlag = false;
GLuint raytracer::raytracedOutput;
GLuint raytracer::directSignal;
GLuint raytracer::indirectSignal;
GLuint raytracer::specularSignal;
GLuint raytracer::specularIndirectSignal;
GLuint raytracer::emissionSignal;
GLuint raytracer::NoiseMask;
RenderQuad ComputeQuad;
Shader ComputeQuadShader;
Shader testCompute;
unsigned int CurrentWidth;
unsigned int CurrentHeight;
std::vector<raytracer::modelData> raytracer::modelArray;

GLuint raytracer::triangleSSBOID;
GLuint raytracer::meshSSBOID;
GLuint raytracer::quickSSBOID;
GLuint raytracer::bvhSSBO;
float raytracer::downscaleFactor = 0.77f;
float raytracer::maxDistance = 100.0f;
float raytracer::noiseThreshold = 0.3f;
float raytracer::reflectionDistance = 50.0f;
int raytracer::reflectionBounces = 2;
int raytracer::indirectBounces = 1;
int raytracer::indirectSamples = 1;
int raytracer::maxAccumulatedFrames = 32;
bool raytracer::doAccumulate = true;
bool raytracer::resetAccumulationOnDirty = true;

void raytracer::init() {
	// generate triangle buffer
	glGenBuffers(1, &triangleSSBOID);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, triangleSSBOID);
	// allocate 1024 bytes
	glBufferData(GL_SHADER_STORAGE_BUFFER, 1024, NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, triangleSSBOID); // 6
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // Unbind
	//meshSSBOID
	
	// generate triangle buffer
	glGenBuffers(1, &meshSSBOID);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, meshSSBOID);
	// allocate 1024 bytes
	glBufferData(GL_SHADER_STORAGE_BUFFER, 1024, NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, meshSSBOID); // 7
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // Unbind

	//quickSSBOID
	glGenBuffers(1, &quickSSBOID);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, quickSSBOID);
	// allocate 1024 bytes
	glBufferData(GL_SHADER_STORAGE_BUFFER, 1024, NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 9, quickSSBOID); // 8
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // Unbind

	//bvhSSBO
	glGenBuffers(1, &bvhSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, bvhSSBO);
	// allocate 1024 bytes
	glBufferData(GL_SHADER_STORAGE_BUFFER, 1024, NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, bvhSSBO); // 9
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // Unbind
}

void raytracer::reloadSceneToRaytracer()
{
	clearRaytracerData();
	uploadSceneToRaytracer();
}

void raytracer::uploadSceneToRaytracer()
{
	raytracer::RTGlobalTransformFlag = true;
	for (size_t i = 0; i < Scene::entityObjects.size(); i++)
	{
		int index = RenderHandler::fetchModelIndex(Scene::entityObjects[i]->component.render.renderID);
		if (index != -1)
		{
			uint64_t instanceUUID = Scene::entityObjects[i]->component.render.instanceUUID;
			for (size_t x = 0; x < modelArray.size(); x++)
			{
				if (modelArray[x].harddata.rayModel.instanceUUID == instanceUUID)
					continue;
			}
			uploadToRaytracer(instanceUUID);
		}
		UpdateModelBuffer();
		updateQuickModelData();
	}
}

void raytracer::clearRaytracerData()
{
	modelArray.clear();
	UpdateModelBuffer();
	updateQuickModelData();
}

void raytracer::uploadToRaytracer(uint64_t instanceUUID)
{

	uint64_t RenderUUID = RenderHandler::findRenderUUIDwIstanceUUID(instanceUUID);
	int index = RenderHandler::fetchModelIndex(RenderUUID);
	if (index != -1)
	{
		// modelHarddata
		modelData newCpuModel;

		rayModel rModel;
		// should checkk if alredy uuid already exisits in modelaray
		rModel.instanceUUID = instanceUUID;
		rModel.meshCount = RenderHandler::models[index].model->meshes.size();
		newCpuModel.quickdata.quickModel.instanceUUID = rModel.instanceUUID;

		// prevent duplicates
		if (!modelArray.empty())
		{
			for (size_t i = 0; i < modelArray.size(); i++)
			{
				if (modelArray[i].harddata.rayModel.instanceUUID == rModel.instanceUUID)
					return;
			}
		}

		glm::mat4 modelMatrix = FE_Math::composeMatrixWDegrees(
			RenderHandler::models[index].model->globalTransformation.position, RenderHandler::models[index].model->globalTransformation.scale,
			RenderHandler::models[index].model->globalTransformation.rotation);

		// push the raymodel into model array
		newCpuModel.harddata.rayModel = rModel;
		RenderHandler::models[index].model->createMeshAABBs();
		RenderHandler::models[index].model->updateMeshAABBs();

		for (size_t x = 0; x < RenderHandler::models[index].model->meshes.size(); x++)
		{

			// use texture arrays + meshindex

			glm::mat4 finalMatrix = RenderHandler::models[index].model->lModelMatrix[x]; // rModel.ModelMatrix *

			// 	component.renderHeads.Model->createMeshAABBs();
			rayMesh newMesh;
			newMesh.triangleCount = (int)RenderHandler::models[index].model->meshes[x].indices.size() / 3;

			newMesh.meshIndex = x;
			newMesh.instanceUUID = instanceUUID;
			newMesh.meshUUID = RenderHandler::models[index].model->meshes[x].UUID;

			newMesh.albedoHandle = 0;
			newMesh.specularHandle = 0;
			newMesh.normalHandle = 0;


			// i should probably get rid of my nested loopsat some stages, im just lazy so i keep using them
			auto& currentSourceMesh = RenderHandler::models[index].model->meshes[x];

			for (size_t x = 0; x < currentSourceMesh.textures.size(); x++)
			{
				std::string type = currentSourceMesh.textures[x].type;
				uint64_t handle = currentSourceMesh.textures[x].handle;

				if (type == "texture_diffuse")      newMesh.albedoHandle = handle;
				else if (type == "texture_normal")  newMesh.normalHandle = handle;
				else if (type == "texture_roughness") newMesh.specularHandle = handle;
			}

			//newMesh.albedoHandle = model->loadedTex

			//std::cout << newMesh.meshUUID << std::endl;
			//std::cout << newMesh.modelUUID << std::endl;
			for (size_t y = 0; y + 2 < RenderHandler::models[index].model->meshes[x].indices.size(); y += 3)
			{
				triangle newtriangle;
				unsigned int i0 = RenderHandler::models[index].model->meshes[x].indices[y];
				unsigned int i1 = RenderHandler::models[index].model->meshes[x].indices[y + 1];
				unsigned int i2 = RenderHandler::models[index].model->meshes[x].indices[y + 2];

				if (i0 >= RenderHandler::models[index].model->meshes[x].vertices.size() ||
					i1 >= RenderHandler::models[index].model->meshes[x].vertices.size() ||
					i2 >= RenderHandler::models[index].model->meshes[x].vertices.size()) {
					continue;
				}

				glm::vec3 a = RenderHandler::models[index].model->meshes[x].vertices[i0].position;
				glm::vec3 b = RenderHandler::models[index].model->meshes[x].vertices[i1].position;
				glm::vec3 c = RenderHandler::models[index].model->meshes[x].vertices[i2].position;

				FE_Math::transformPoint(a, finalMatrix);
				FE_Math::transformPoint(b, finalMatrix);
				FE_Math::transformPoint(c, finalMatrix);

				// position
				newtriangle.a = glm::vec4(a, 1.0f);
				newtriangle.b = glm::vec4(b, 1.0f);
				newtriangle.c = glm::vec4(c, 1.0f);
				// colour

				// texture uv;
				newtriangle.aTex = glm::vec4(glm::vec2(RenderHandler::models[index].model->meshes[x].vertices[i0].texUV), 1.0f, 1.0f);
				newtriangle.bTex = glm::vec4(glm::vec2(RenderHandler::models[index].model->meshes[x].vertices[i1].texUV), 1.0f, 1.0f);
				newtriangle.cTex = glm::vec4(glm::vec2(RenderHandler::models[index].model->meshes[x].vertices[i2].texUV), 1.0f, 1.0f);

				// normal
				glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(finalMatrix)));

				glm::vec3 nA = glm::normalize(normalMatrix * RenderHandler::models[index].model->meshes[x].vertices[i0].normal);
				glm::vec3 nB = glm::normalize(normalMatrix * RenderHandler::models[index].model->meshes[x].vertices[i1].normal);
				glm::vec3 nC = glm::normalize(normalMatrix * RenderHandler::models[index].model->meshes[x].vertices[i2].normal);

				newtriangle.aNormal = glm::vec4(nA, 1.0f);
				newtriangle.bNormal = glm::vec4(nB, 1.0f);
				newtriangle.cNormal = glm::vec4(nC, 1.0f);
				// tangents
				newtriangle.aTangent = glm::vec4(RenderHandler::models[index].model->meshes[x].vertices[i0].tangent, 1.0f);
				newtriangle.bTangent = glm::vec4(RenderHandler::models[index].model->meshes[x].vertices[i1].tangent, 1.0f);
				newtriangle.cTangent = glm::vec4(RenderHandler::models[index].model->meshes[x].vertices[i2].tangent, 1.0f);
				// bitangents
				newtriangle.aBiTangent = glm::vec4(RenderHandler::models[index].model->meshes[x].vertices[i0].biTangent, 1.0f);
				newtriangle.bBiTangent = glm::vec4(RenderHandler::models[index].model->meshes[x].vertices[i1].biTangent, 1.0f);
				newtriangle.cBiTangent = glm::vec4(RenderHandler::models[index].model->meshes[x].vertices[i2].biTangent, 1.0f);

				newCpuModel.harddata.tris.push_back(newtriangle);
			}
			newCpuModel.harddata.meshes.push_back(newMesh);
		}

		modelArray.push_back(newCpuModel);

		UpdateModelBuffer();

	}

}

void raytracer::removeFromRaytracer(uint64_t instanceUUID)
{
	for (size_t x = 0; x < modelArray.size(); x++)
	{
		if (modelArray[x].harddata.rayModel.instanceUUID == instanceUUID)
		{
			//std::cout << "Removing model from raytracer with UUID: " << modelUUID << std::endl;
			modelArray.erase(modelArray.begin() + x);
			break;
		}
	}
	UpdateModelBuffer();
}

void raytracer::UpdateModelBuffer()
{
	std::vector<triangle> newTriangleArray;
	std::vector<rayMesh> newMeshArray;

	for (size_t x = 0; x < modelArray.size(); x++)
	{
		for (size_t z = 0; z < modelArray[x].harddata.tris.size(); z++)
		{
			triangle newTriangle;
			newTriangle = modelArray[x].harddata.tris[z];
			newTriangleArray.push_back(newTriangle);
		}
		for (size_t z = 0; z < modelArray[x].harddata.meshes.size(); z++)
		{
			rayMesh newMesh;
			newMesh = modelArray[x].harddata.meshes[z];
			newMeshArray.push_back(newMesh);
		}
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, triangleSSBOID);
	glBufferData(GL_SHADER_STORAGE_BUFFER, newTriangleArray.size() * sizeof(triangle), newTriangleArray.data(), GL_STATIC_DRAW); // gl buffer data wipes whole array
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, triangleSSBOID);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	// meshSSBOID
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, meshSSBOID);
	glBufferData(GL_SHADER_STORAGE_BUFFER, newMeshArray.size() * sizeof(rayMesh), newMeshArray.data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, meshSSBOID);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

}

void raytracer::updateboundingboxes(uint64_t instanceUUID, std::vector<Collision::AABB> rootnodes)
{
	uint64_t RenderUUID = RenderHandler::findRenderUUIDwIstanceUUID(instanceUUID);
	int index = RenderHandler::fetchModelIndex(RenderUUID);
	{
		for (size_t i = 0; i < modelArray.size(); i++)
		{
			if (modelArray[i].harddata.rayModel.instanceUUID == instanceUUID)
			{
				// update root node
				if (modelArray[i].quickdata.meshAABBs.size() != RenderHandler::models[index].model->meshes.size())
					modelArray[i].quickdata.meshAABBs.resize(RenderHandler::models[index].model->meshes.size());

				for (size_t x = 0; x < rootnodes.size(); x++)
				{
					boxRootNode& node = modelArray[i].quickdata.meshAABBs[x];
					node.rootPos = glm::vec4(rootnodes[x].position, 1.0f);
					node.rootscale = glm::vec4(rootnodes[x].size, 1.0f);
					node.instanceUUID = instanceUUID;
					node.padding = 0;
				}
				break;
			}
		}
	}
}

void raytracer::modelMatrixUpdate(uint64_t instanceUUID, glm::mat4 newModelMatrix)
{
	for (size_t i = 0; i < modelArray.size(); i++)
	{
		if (modelArray[i].quickdata.quickModel.instanceUUID == instanceUUID) // move aabb bounds here too
		{
			modelArray[i].quickdata.quickModel.ModelMatrix = newModelMatrix;
			glm::mat3 model3x3 = glm::mat3(newModelMatrix);
			glm::mat3 normalMat3 = glm::transpose(glm::inverse(model3x3));
			modelArray[i].quickdata.quickModel.NormalMatrix = glm::mat4(normalMat3);
			break;
		}
	}
}

void raytracer::uvScaleUpdate(uint64_t instanceUUID, glm::vec2 scale)
{
	for (size_t i = 0; i < modelArray.size(); i++)
	{
		if (modelArray[i].quickdata.quickModel.instanceUUID == instanceUUID) // move aabb bounds here too
		{
			modelArray[i].quickdata.quickModel.uvScale = glm::vec4(scale.x, scale.y, 0.0f, 0.0f);
			break;
		}
	}
}

void raytracer::updateQuickModelData()
{
	std::vector<quickRayModel> newQuickDataArray;
	std::vector<boxRootNode> newRootNodeArray;

	for (size_t i = 0; i < modelArray.size(); i++)
	{
		newQuickDataArray.push_back(modelArray[i].quickdata.quickModel);
		for (size_t x = 0; x < modelArray[i].harddata.meshes.size(); x++)
		{
			boxRootNode meshBox;
			meshBox.rootPos = modelArray[i].quickdata.meshAABBs[x].rootPos;
			meshBox.rootscale = modelArray[i].quickdata.meshAABBs[x].rootscale;
			meshBox.instanceUUID = modelArray[i].harddata.rayModel.instanceUUID;
			meshBox.padding = 0;

			newRootNodeArray.push_back(meshBox);
		}
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, quickSSBOID);
	glBufferData(GL_SHADER_STORAGE_BUFFER, newQuickDataArray.size() * sizeof(quickRayModel), newQuickDataArray.data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 9, quickSSBOID);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	// bvhSSBO
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, bvhSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, newRootNodeArray.size() * sizeof(boxRootNode), newRootNodeArray.data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, bvhSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

}



void raytracer::initcomputeShader(unsigned int width, unsigned int height) {

	ComputeQuad.init();

	ComputeQuadShader.LoadShader("Assets/Shaders/Db/RenderQuad.vert", "Assets/Shaders/Db/ComputeRenderQuad.frag");

	// direct /0
	// indirect /1
	// specular /2 
	//specularIndirect /3
	// emission /4
	// noise mask / 6
	// denoisetex / 7
	glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &raytracedOutput);
	glTextureParameteri(raytracedOutput, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(raytracedOutput, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureStorage3D(raytracedOutput, 1, GL_RGBA32F, width, height, 6);
	glBindImageTexture(0, raytracedOutput, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	CurrentWidth = width;
	CurrentHeight = height;

	testCompute.LoadComputeShader("Assets/Shaders/compute/Raytrace.comp");

//	glEnable(GL_DEBUG_OUTPUT);
//	glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity,
//		GLsizei length, const GLchar* message, const void* userParam) {
//			std::cerr << "GL DEBUG: " << message << std::endl;
//		}, nullptr);

	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		std::cerr << "OpenGL error: " << err << std::endl;
	}

}

bool resized = false;

float downscaleFactor = 0.77f;

void raytracer::resizeTexture(unsigned int width, unsigned int height) {
    glDeleteTextures(1, &raytracedOutput); // Delete old texture 

	glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &raytracedOutput);
	glTextureParameteri(raytracedOutput, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(raytracedOutput, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureStorage3D(raytracedOutput, 1, GL_RGBA32F, width * downscaleFactor, height * downscaleFactor, 6);
	glBindImageTexture(0, raytracedOutput, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	CurrentWidth = width;
	CurrentHeight = height;

	resized = true;

	denoiser::resizeTexture(width * downscaleFactor, height * downscaleFactor);
}

glm::vec3 oldCampos = glm::vec3(0.0f);
glm::vec3 oldOrientation = glm::vec3(0.0f);
int framecount = 0;

void raytracer::render() {
	framecount++;
	testCompute.Activate();
	testCompute.setFloat4("u_BaseColour", glm::vec4(glm::vec3(RenderClass::gammaCorrect3(RenderClass::skyRGBA)), 1.0f) ); // glm::vec3(RenderClass::gammaCorrect3(RenderClass::skyRGBA)), 1.0f)
	testCompute.setMat4("u_ViewMatrix", Scene::maincamera.view);
	testCompute.setMat4("u_ProjectionMatrix", Scene::maincamera.projection);
	testCompute.setFloat3("cameraPosition", Scene::maincamera.Position);
	testCompute.setFloat3("orientation", Scene::maincamera.Orientation);
	testCompute.setFloat3("camUp", Scene::maincamera.Up);
	testCompute.setFloat("fov", Scene::maincamera.fov);
	testCompute.setFloat("time", glfwGetTime());
	testCompute.setFloat("deltatime", TimeUtil::deltatime);

	testCompute.setFloat("maxDistance", raytracer::maxDistance);
	testCompute.setFloat("noiseThreshold", raytracer::noiseThreshold);
	testCompute.setFloat("reflectionDistance", raytracer::reflectionDistance);
	testCompute.setInt("reflectionBounces", raytracer::reflectionBounces);
	testCompute.setFloat3("skycolour", RenderClass::gammaCorrect3(RenderClass::skyRGBA));
	
	testCompute.setInt("indirectBounces", raytracer::indirectBounces);
	testCompute.setInt("indirectSamples", raytracer::indirectSamples);
	testCompute.setInt("MaxAccumulatedFrames", raytracer::maxAccumulatedFrames);

	if (framecount > maxAccumulatedFrames +1 && maxAccumulatedFrames != 0)
		framecount = maxAccumulatedFrames +1;

		testCompute.setInt("frameCount", framecount);
	if (oldCampos != Scene::maincamera.Position && resetAccumulationOnDirty 
		|| oldOrientation != Scene::maincamera.Orientation && resetAccumulationOnDirty 
		|| resized && resetAccumulationOnDirty || RTGlobalTransformFlag)
	{
		oldCampos = Scene::maincamera.Position; oldOrientation = Scene::maincamera.Orientation; resized = false;
		framecount = 0;
		testCompute.setBool("doAccumulate", false);
	}
	else
	{
		testCompute.setBool("doAccumulate", doAccumulate);
	}
	testCompute.Activate();

	LightingHandler::update(testCompute);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, GeometryPass::depthTexture);
	testCompute.setInt("depthMap", 5);

	RenderClass::bluenoise->Bind();
	testCompute.setInt("BlueNoiseTex", 6);

	Skybox::bind(7);
	Skybox::cubemapToShader(testCompute, 7);
	testCompute.ActivateCompute((CurrentWidth + 7) / 8, (CurrentHeight + 3) / 4, 1);
	RenderClass::bluenoise->Unbind();
	Skybox::unbind();

	denoiser::render();
}
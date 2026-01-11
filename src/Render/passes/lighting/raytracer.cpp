#include "raytracer.h"
#include <Render/Shader/Framebuffer.h>
#include "Render/Object/RenderQuad.h"
#include <Render/passes/geometry/geometryPass.h>
#include <utils/FE_math.h>
#include"Render/passes/post/denoise.h"
#include "Core/Render.h"
#include "Scene/scene.h"
#include "Scene/LightingHandler.h"
bool raytracer::RTGlobalTransformFlag = false;
GLuint raytracer::directSignal;
GLuint raytracer::indirectSignal;
GLuint raytracer::specularSignal;
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
float raytracer::downscaleFactor = 0.58f;
float raytracer::maxDistance = 100.0f;
float raytracer::noiseThreshold = 0.3f;
float raytracer::reflectionDistance = 50.0f;
int raytracer::reflectionBounces = 2;
int raytracer::indirectBounces = 1;
int raytracer::indirectSamples = 1;
bool raytracer::doAccumulate = true;
bool raytracer::resetAccumulationOnDirty = true;
Texture* raytracer::bluenoise;

void raytracer::init() {
	// generate triangle buffer
	glGenBuffers(1, &triangleSSBOID);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, triangleSSBOID);
	// allocate 1024 bytes
	glBufferData(GL_SHADER_STORAGE_BUFFER, 1024, NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, triangleSSBOID); // 6
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // Unbind
	//meshSSBOID
	
	// generate triangle buffer
	glGenBuffers(1, &meshSSBOID);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, meshSSBOID);
	// allocate 1024 bytes
	glBufferData(GL_SHADER_STORAGE_BUFFER, 1024, NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, meshSSBOID); // 7
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // Unbind

	//quickSSBOID
	glGenBuffers(1, &quickSSBOID);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, quickSSBOID);
	// allocate 1024 bytes
	glBufferData(GL_SHADER_STORAGE_BUFFER, 1024, NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, quickSSBOID); // 8
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // Unbind

	//bvhSSBO
	glGenBuffers(1, &bvhSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, bvhSSBO);
	// allocate 1024 bytes
	glBufferData(GL_SHADER_STORAGE_BUFFER, 1024, NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 9, bvhSSBO); // 9
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // Unbind

	// load bluenoise texture
	bluenoise = new Texture();
	bluenoise->createTexture("Assets/Dependants/LDR_LLL1_0.png", "misc", 6);
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
		for (size_t x = 0; x < modelArray.size(); x++)
		{
			if (modelArray[x].harddata.rayModel.modelUUID == Scene::entityObjects[i]->component.renderHeads.Model->UUID)
				continue;
		}
		uploadToRaytracer(Scene::entityObjects[i]->component.renderHeads.Model);
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

void raytracer::uploadToRaytracer(Model* model)
{
	 // modelHarddata
	modelData newCpuModel;

	rayModel rModel;
	// should checkk if alredy uuid already exisits in modelaray
	rModel.modelUUID = model->UUID;
	rModel.meshCount = model->meshes.size();
	newCpuModel.quickdata.quickModel.modelUUID = rModel.modelUUID;

	// prevent duplicates
	if (!modelArray.empty())
	{
		for (size_t i = 0; i < modelArray.size(); i++)
		{
			if (modelArray[i].harddata.rayModel.modelUUID == rModel.modelUUID)
				return;
		}
	}
	
	 glm::mat4 modelMatrix = FE_Math::composeMatrixWDegrees(
		model->globalTransformation.position, model->globalTransformation.scale,
		model->globalTransformation.rotation);

	// push the raymodel into model array
	newCpuModel.harddata.rayModel = rModel;
	model->createMeshAABBs();
	model->updateMeshAABBs();

	for (size_t x = 0; x < model->meshes.size(); x++)
	{

		// use texture arrays + meshindex

		glm::mat4 finalMatrix = model->lModelMatrix[x]; // rModel.ModelMatrix *

		// 	component.renderHeads.Model->createMeshAABBs();
		rayMesh newMesh;
		newMesh.triangleCount = (int)model->meshes[x].indices.size() / 3;

		newMesh.meshIndex = x;
		newMesh.modelUUID = model->UUID;
		newMesh.meshUUID = model->meshes[x].UUID;

		//std::cout << newMesh.meshUUID << std::endl;
		//std::cout << newMesh.modelUUID << std::endl;
		for (size_t y = 0; y + 2 < model->meshes[x].indices.size(); y += 3)
		{
			triangle newtriangle;
			unsigned int i0 = model->meshes[x].indices[y];
			unsigned int i1 = model->meshes[x].indices[y + 1];
			unsigned int i2 = model->meshes[x].indices[y + 2];

			if (i0 >= model->meshes[x].vertices.size() ||
				i1 >= model->meshes[x].vertices.size() ||
				i2 >= model->meshes[x].vertices.size()) {
				continue;
			}

			glm::vec3 a = model->meshes[x].vertices[i0].position;
			glm::vec3 b = model->meshes[x].vertices[i1].position;
			glm::vec3 c = model->meshes[x].vertices[i2].position;

			FE_Math::transformPoint(a, finalMatrix);
			FE_Math::transformPoint(b, finalMatrix);
			FE_Math::transformPoint(c, finalMatrix);

			// just for now plug this into pos
			newtriangle.aColour = glm::vec4(model->meshes[x].vertices[i0].color, 1.0f);

			// position
			newtriangle.a = glm::vec4(a, newtriangle.aColour.x);
			newtriangle.b = glm::vec4(b, newtriangle.aColour.y);
			newtriangle.c = glm::vec4(c, newtriangle.aColour.z);
			// colour

			newtriangle.bColour = glm::vec4(model->meshes[x].vertices[i1].color, 1.0f);
			newtriangle.cColour = glm::vec4(model->meshes[x].vertices[i2].color, 1.0f);
			// texture uv;
			newtriangle.aTex = glm::vec4(glm::vec2(model->meshes[x].vertices[i0].texUV), 1.0f, 1.0f);
			newtriangle.bTex = glm::vec4(glm::vec2(model->meshes[x].vertices[i1].texUV), 1.0f, 1.0f);
			newtriangle.cTex = glm::vec4(glm::vec2(model->meshes[x].vertices[i2].texUV), 1.0f, 1.0f);

			// normal
			glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(finalMatrix)));

			glm::vec3 nA = glm::normalize(normalMatrix * model->meshes[x].vertices[i0].normal);
			glm::vec3 nB = glm::normalize(normalMatrix * model->meshes[x].vertices[i1].normal);
			glm::vec3 nC = glm::normalize(normalMatrix * model->meshes[x].vertices[i2].normal);

			newtriangle.aNormal = glm::vec4(nA, 1.0f);
			newtriangle.bNormal = glm::vec4(nB, 1.0f);
			newtriangle.cNormal = glm::vec4(nC, 1.0f);
			// tangents
			newtriangle.aTangent = glm::vec4(model->meshes[x].vertices[i0].tangent, 1.0f);
			newtriangle.bTangent = glm::vec4(model->meshes[x].vertices[i1].tangent, 1.0f);
			newtriangle.cTangent = glm::vec4(model->meshes[x].vertices[i2].tangent, 1.0f);
			// bitangents
			newtriangle.aBiTangent = glm::vec4(model->meshes[x].vertices[i0].biTangent, 1.0f);
			newtriangle.bBiTangent = glm::vec4(model->meshes[x].vertices[i1].biTangent, 1.0f);
			newtriangle.cBiTangent = glm::vec4(model->meshes[x].vertices[i2].biTangent, 1.0f);

			newCpuModel.harddata.tris.push_back(newtriangle);
		}
		newCpuModel.harddata.meshes.push_back(newMesh);
	}

	modelArray.push_back(newCpuModel);

	UpdateModelBuffer();

}

void raytracer::removeFromRaytracer(uint64_t modelUUID)
{
	for (size_t x = 0; x < modelArray.size(); x++)
	{
		if (modelArray[x].harddata.rayModel.modelUUID == modelUUID)
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
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, triangleSSBOID);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	// meshSSBOID
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, meshSSBOID);
	glBufferData(GL_SHADER_STORAGE_BUFFER, newMeshArray.size() * sizeof(rayMesh), newMeshArray.data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, meshSSBOID);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

}

void raytracer::updateboundingboxes(Model* model)
{
	for (size_t i = 0; i < modelArray.size(); i++)
	{
		if (modelArray[i].harddata.rayModel.modelUUID == model->UUID)
		{
			// update root node
			if (modelArray[i].quickdata.meshAABBs.size() != model->meshes.size())
				modelArray[i].quickdata.meshAABBs.resize(model->meshes.size());

			for (size_t x = 0; x < model->meshes.size(); x++)
			{
				boxRootNode& node = modelArray[i].quickdata.meshAABBs[x];
				node.rootPos = glm::vec4(model->meshes[x].boxCollider.position, 1.0f);
				node.rootscale = glm::vec4(model->meshes[x].boxCollider.size, 1.0f);
				node.modelUUID = model->UUID;
				node.padding = 0;
			}
			break;
		}
	}
}

void raytracer::modelMatrixUpdate(uint64_t modelUUID, glm::mat4 newModelMatrix)
{
	for (size_t i = 0; i < modelArray.size(); i++)
	{
		if (modelArray[i].quickdata.quickModel.modelUUID == modelUUID) // move aabb bounds here too
		{
			modelArray[i].quickdata.quickModel.ModelMatrix = newModelMatrix;
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
			meshBox.modelUUID = modelArray[i].harddata.rayModel.modelUUID;
			meshBox.padding = 0;

			newRootNodeArray.push_back(meshBox);
		}
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, quickSSBOID);
	glBufferData(GL_SHADER_STORAGE_BUFFER, newQuickDataArray.size() * sizeof(quickRayModel), newQuickDataArray.data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, quickSSBOID);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	// bvhSSBO
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, bvhSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, newRootNodeArray.size() * sizeof(boxRootNode), newRootNodeArray.data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 9, bvhSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

}



void raytracer::initcomputeShader(unsigned int width, unsigned int height) {

	ComputeQuad.init();

	ComputeQuadShader.LoadShader("Assets/Shaders/Db/RenderQuad.vert", "Assets/Shaders/Db/ComputeRenderQuad.frag");

	// direct /0
	glCreateTextures(GL_TEXTURE_2D, 1, &directSignal);
	glTextureParameteri(directSignal, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(directSignal, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(directSignal, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(directSignal, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureStorage2D(directSignal, 1, GL_RGBA32F, width, height);
	glBindImageTexture(0, directSignal, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	// indirect /1
	glCreateTextures(GL_TEXTURE_2D, 1, &indirectSignal);
	glTextureParameteri(indirectSignal, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(indirectSignal, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(indirectSignal, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(indirectSignal, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureStorage2D(indirectSignal, 1, GL_RGBA32F, width, height);
	glBindImageTexture(1, indirectSignal, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	// specular /2 
	glCreateTextures(GL_TEXTURE_2D, 1, &specularSignal);
	glTextureParameteri(specularSignal, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(specularSignal, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(specularSignal, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(specularSignal, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureStorage2D(specularSignal, 1, GL_RGBA32F, width, height);
	glBindImageTexture(2, specularSignal, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	// emission /3
	glCreateTextures(GL_TEXTURE_2D, 1, &emissionSignal);
	glTextureParameteri(emissionSignal, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(emissionSignal, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(emissionSignal, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(emissionSignal, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureStorage2D(emissionSignal, 1, GL_RGBA32F, width, height);
	glBindImageTexture(3, emissionSignal, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	// noise mask
	glCreateTextures(GL_TEXTURE_2D, 1, &NoiseMask); //5
	glTextureParameteri(NoiseMask, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(NoiseMask, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(NoiseMask, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(NoiseMask, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureStorage2D(NoiseMask, 1, GL_RGBA32F, width, height);
	glBindImageTexture(5, NoiseMask, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

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

float downscaleFactor = 0.58f;

void raytracer::resizeTexture(unsigned int width, unsigned int height) {
    glDeleteTextures(1, &directSignal); // Delete old texture 

	glCreateTextures(GL_TEXTURE_2D, 1, &directSignal);
	glTextureParameteri(directSignal, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(directSignal, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(directSignal, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(directSignal, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureStorage2D(directSignal, 1, GL_RGBA32F, width * downscaleFactor, height * downscaleFactor);
	glBindImageTexture(0, directSignal, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	glCreateTextures(GL_TEXTURE_2D, 1, &indirectSignal);
	glTextureParameteri(indirectSignal, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(indirectSignal, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(indirectSignal, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(indirectSignal, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureStorage2D(indirectSignal, 1, GL_RGBA32F, width * downscaleFactor, height * downscaleFactor);
	glBindImageTexture(1, indirectSignal, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	glCreateTextures(GL_TEXTURE_2D, 1, &specularSignal);
	glTextureParameteri(specularSignal, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(specularSignal, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(specularSignal, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(specularSignal, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureStorage2D(specularSignal, 1, GL_RGBA32F, width * downscaleFactor, height * downscaleFactor);
	glBindImageTexture(2, specularSignal, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	glCreateTextures(GL_TEXTURE_2D, 1, &emissionSignal);
	glTextureParameteri(emissionSignal, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(emissionSignal, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(emissionSignal, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(emissionSignal, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureStorage2D(emissionSignal, 1, GL_RGBA32F, width * downscaleFactor, height * downscaleFactor);
	glBindImageTexture(3, emissionSignal, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	glCreateTextures(GL_TEXTURE_2D, 1, &NoiseMask);
	glTextureParameteri(NoiseMask, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(NoiseMask, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(NoiseMask, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(NoiseMask, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureStorage2D(NoiseMask, 1, GL_RGBA32F, width * downscaleFactor, height * downscaleFactor);
	glBindImageTexture(5, NoiseMask, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

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
	testCompute.setInt("frameCount", framecount);
	testCompute.setInt("indirectBounces", raytracer::indirectBounces);
	testCompute.setInt("indirectSamples", raytracer::indirectSamples);
	
	if (oldCampos != Scene::maincamera.Position && resetAccumulationOnDirty 
		|| oldOrientation != Scene::maincamera.Orientation && resetAccumulationOnDirty 
		|| resized && resetAccumulationOnDirty || RTGlobalTransformFlag)
	{
		oldCampos = Scene::maincamera.Position; oldOrientation = Scene::maincamera.Orientation; resized = false;
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

	bluenoise->Bind();
	testCompute.setInt("BlueNoiseTex", 6);

	Skybox::bind(7);
	Skybox::cubemapToShader(testCompute, 7);
	testCompute.ActivateCompute((CurrentWidth + 7) / 8, (CurrentHeight + 3) / 4, 1);
	bluenoise->Unbind();
	Skybox::unbind();

	denoiser::render();
}
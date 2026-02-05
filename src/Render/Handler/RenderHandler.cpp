#include "RenderHandler.h"
#include "Systems/util/UUID.h"
#include <Scene/LightingHandler.h>
#include <Render/Handler/ShaderHandler.h>
#include <Core/Render.h>
#include <Render/Object/SkyBox.h>
#include <Render/passes/geometry/geometryPass.h>
#include <Scene/scene.h>
#include <Editor/UI/ImGui/ImGuiWindow.h>
#include <Render/Shader/Framebuffer.h>
#include <Gameplay/Player.h>
std::unordered_map<std::string, uint64_t> RenderHandler::pKeyHandleMapRender;
std::vector<RenderHandler::modelObject> RenderHandler::models;
std::vector<RenderHandler::renderQueueData> RenderHandler::renderQueueDataVector;

uint64_t RenderHandler::fetchHandle(std::string path)
{
	auto it = pKeyHandleMapRender.find(path);
	if (it != pKeyHandleMapRender.end()) {
		return it->second;
	}
	return 0;
}

int RenderHandler::fetchModelIndex(uint64_t RenderID)
{
	for (size_t i = 0; i < models.size(); i++)
	{
		if (models[i].RenderID == RenderID) return (int)i;
	}
	return -1;
}

RenderHandler::batchOfUUID RenderHandler::addModel(std::string path)
{
	uint64_t nUUID = fetchHandle(path);
	uint64_t nIUUID = UUID::returnHandle();
	if (nUUID == 0) // if equal to zero handle does not exist in array, we can create away
	{
		// assign new handle
		nUUID = UUID::returnHandle();
		pKeyHandleMapRender[path] = nUUID;
		modelObject newModelObject;
		newModelObject.path = path;
		newModelObject.RenderID = nUUID;
		newModelObject.instances = 1;
		newModelObject.model = new Model(path.c_str());
		newModelObject.model->createMeshAABBs();
		newModelObject.model->renderID = nUUID;
		newModelObject.model->instanceUUIDs.push_back(nIUUID);  // set
		models.push_back(newModelObject);
	}
	else
	{
		int index = fetchModelIndex(nUUID);
		if (index != -1)
		{
			models[index].model->instanceUUIDs.push_back(nIUUID); // add
			models[index].instances += 1;
		}
	}
	batchOfUUID nBatchOfUUIDS;
	nBatchOfUUIDS.instanceUUID = nIUUID;
	nBatchOfUUIDS.RenderID = nUUID;
	return nBatchOfUUIDS;
}

void RenderHandler::addToRenderQueue(renderQueueData data)
{
	renderQueueDataVector.push_back(data);
}

void RenderHandler::clearRenderQueue()
{
	renderQueueDataVector.clear();
}

void RenderHandler::render()
{
	regularDraw();

	instancedDraw();

	// after render clear render queue
	clearRenderQueue();
}

void RenderHandler::removeInstancewRenderID(uint64_t RenderID)
{
	int index = fetchModelIndex(RenderID);
	removeInstance(index);
}

void RenderHandler::removeInstance(int index)
{
	// bounds check
	if (index < 0 || index >= (int)models.size()) return;

	models[index].instances -= 1;

	if (models[index].instances <= 0)
	{
		// erase the item in map
		auto handleIt = pKeyHandleMapRender.find(models[index].path);
		if (handleIt != pKeyHandleMapRender.end()) {
			pKeyHandleMapRender.erase(handleIt);
		}
		// erase model
		delete models[index].model;
		models[index].model = nullptr;
		models.erase(models.begin() + index);
	}
}

uint64_t RenderHandler::findRenderUUIDwIstanceUUID(uint64_t InstanceUUID)
{

	for (size_t i = 0; i < RenderHandler::models.size(); i++)
	{
		for (size_t x = 0; x < RenderHandler::models[i].model->instanceUUIDs.size(); x++)
		{
			if (RenderHandler::models[i].model->instanceUUIDs[x] == InstanceUUID)
				return RenderHandler::models[i].RenderID;
		}
		
	}

	return uint64_t(0);
}

uint64_t RenderHandler::findModelUUIDwRenderUUID(uint64_t RenderID)
{
	int index = fetchModelIndex(RenderID);
	if (index != -1)
	{
		return RenderHandler::models[index].model->UUID;
	}

	return uint64_t(0);
}

uint64_t RenderHandler::findModelUUIDwInstanceUUID(uint64_t InstanceUUID)
{
	uint64_t renderUUID = findRenderUUIDwIstanceUUID(InstanceUUID);
	return findModelUUIDwRenderUUID(renderUUID);
}

void RenderHandler::init()
{
	tempCM = new Cubemap();
	//tempCM->loadCubeMap("Assets/Skybox/clearsky/Skybox.json"); // temp issue stems from this itself??
	cmShader.LoadShader("Assets/Shaders/Lighting/Default.vert", "Assets/Shaders/Lighting/reflection.frag");
}

glm::vec3 rqtargets[] = {
glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(-1.0f,  0.0f,  0.0f),
glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f),
glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f,  0.0f, -1.0f)
};

glm::vec3 rqups[] = {
	glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f),
	glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f,  0.0f, -1.0f),
	glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)
};


Shader RenderHandler::cmShader;
Cubemap* RenderHandler::tempCM;

void RenderHandler::cmDraw(std::vector<renderQueueData> rqdVector, Cubemap*& cm, Shader& shader, glm::vec2 resolution, glm::vec3 pos)
{

	tempCM->resizeCubeMap(resolution); // seems to remove the texture, keep an eye on this later
	
	// creation
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (GLAD_GL_ARB_bindless_texture && cm->handle != 0) {
		glMakeTextureHandleNonResidentARB(cm->handle);
	}

	if (cm->ID == 0) {

		glDeleteTextures(1, &cm->ID);
		// Creates the cubemap texture object
		glGenTextures(1, &cm->ID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cm->ID);

		glTexStorage2D(GL_TEXTURE_CUBE_MAP, 5, GL_RGBA8, (int)resolution.x, (int)resolution.y);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		// These are very important to prevent seams
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_CUBE_MAP_SEAMLESS, GL_TRUE);

	}



	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::cmFBO);


	int width = (int)resolution.x; int height = (int)resolution.y;
	glViewport(0, 0, width, height); glPixelStorei(GL_PACK_ALIGNMENT, 1);
	Framebuffer::smUpdateResolution(resolution); glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Cycles through all the textures and attaches them to the cubemap object
	for (unsigned int x = 0; x < 6; x++)
	{
		Camera nCamera;
		nCamera.InitCamera(resolution.x, resolution.y, pos); // Matching your Position
		nCamera.fov = 90.0f;
		nCamera.Orientation = rqtargets[x];
		nCamera.Up = rqups[x];
		nCamera.updateMatrix();

		glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::cmFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + x, cm->ID, 0);
		Framebuffer::clearsmbuffer();
		glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::cmFBO);

		if (!FEImGuiWindow::isWireframe && RenderClass::renderSkybox) // should add skybox.scene
			Skybox::draw(nCamera);



		for (size_t i = 0; i < renderQueueDataVector.size(); i++)
		{
			int index = fetchModelIndex(renderQueueDataVector[i].RenderID);
			if (index != -1 && !renderQueueDataVector[i].isInstanced)
			{

				//int modelShaderIndex = ShaderHandler::fetchShaderIndex(renderQueueDataVector[i].shaderUUID);

				// these are temp
				models[index].model->updatePosition(renderQueueDataVector[i].position);
				models[index].model->updateRotation(renderQueueDataVector[i].rotation);
				models[index].model->updateScale(renderQueueDataVector[i].scale);
				models[index].model->updateTranformation();

				LightingHandler::update(shader);

				RenderClass::bluenoise->Bind();
				shader.setInt("BlueNoiseTex", 6);

				shader.setHandleui64ARB("bayerMatrixHandle", RenderClass::bayermatrix->handle);

				Skybox::SkyboxCubemap->cubemapToUUIDShader("cmMainHandle", shader);

				// this would normally be in material
				shader.Activate();

				nCamera.Matrix(shader, "camMatrix");

				shader.Activate();
				shader.setFloat("deltatime", TimeUtil::deltatime);
				shader.setFloat("time", glfwGetTime());
				// this would normally be in material

				if (!RenderClass::DoForwardLightingPass && !RenderClass::DoDeferredLightingPass) continue; // Skip rendering if not in regular or lighting pass
				if (renderQueueDataVector[i].doCulling == true && !FEImGuiWindow::isWireframe) glEnable(GL_CULL_FACE);
				else glDisable(GL_CULL_FACE);
				if (renderQueueDataVector[i].cullFrontFace) glCullFace(GL_FRONT);
				else glCullFace(GL_BACK);

				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Enable wireframe mode


				shader.Activate();
				shader.setFloat2("uvScale", renderQueueDataVector[i].uvScale);
				shader.setFloat("smoothnessValue", renderQueueDataVector[i].smoothnessValue);
				shader.setInt("indirectSamples", 0);
				shader.setBool("doReflect", false);
				//raytracer::uvScaleUpdate(component.renderHeads.Model->UUID, component.systems.material.uvScale);

				if (RenderClass::DoForwardLightingPass) {


					shader.Activate();
					glEnable(GL_DEPTH_TEST);
					glDepthFunc(GL_LESS);

					// temp
					models[index].model->draw(shader, nCamera);
				}
				

				//glFrontFace(GL_CCW);
				glCullFace(GL_BACK); // Reset culling to default
				glDisable(GL_CULL_FACE);

				glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
			}
		}






		unsigned char* data = new unsigned char[width * height * 4];
		glBindFramebuffer(GL_READ_BUFFER, Framebuffer::cmFBO);
		//glReadBuffer(GL_COLOR_ATTACHMENT0);
		glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);



		glBindTexture(GL_TEXTURE_CUBE_MAP, cm->ID);
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + x,
			0,
			GL_RGBA,
			width,
			height,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			data
		);

		delete[] data;
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP, cm->ID);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	if (GLAD_GL_ARB_bindless_texture) {
		cm->handle = glGetTextureHandleARB(cm->ID);
		glMakeTextureHandleResidentARB(cm->handle);
	}


	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glViewport(0, 0, viewport[2], viewport[3]);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderHandler::regularDraw()
{

	// shadow pass
	for (size_t i = 0; i < renderQueueDataVector.size(); i++)
	{
		if (renderQueueDataVector[i].castsShadow && !renderQueueDataVector[i].isInstanced)
		{
			int index = fetchModelIndex(renderQueueDataVector[i].RenderID);
			if (index != -1)
			{
				// these are temp
				models[index].model->updatePosition(renderQueueDataVector[i].position);
				models[index].model->updateRotation(renderQueueDataVector[i].rotation);
				models[index].model->updateScale(renderQueueDataVector[i].scale);
				models[index].model->updateTranformation();
				LightingHandler::drawShadowMap(models[index].model);
			}
		}
	}

	if (RenderClass::doReflections || ProbeHandler::indirectSamples > 0)
	{
		//glm::vec3(Scene::maincamera.Position.x, Scene::maincamera.Position.y, Scene::maincamera.Position.z)
		cmDraw(renderQueueDataVector, tempCM, cmShader, glm::vec2(512), glm::vec3(Scene::maincamera.Position.x, Scene::maincamera.Position.y, Scene::maincamera.Position.z));
		// cmDraw(renderQueueDataVector, tempCM, cmShader, glm::vec2(256), glm::vec3(0.0f, 5.0f, 0.0f));
		Skybox::unbind();
	}

	// do gpass draw here

	for (size_t i = 0; i < renderQueueDataVector.size(); i++)
	{
		int index = fetchModelIndex(renderQueueDataVector[i].RenderID);
		if (index != -1 && !renderQueueDataVector[i].isInstanced)
		{
			int modelGPShaderIndex = ShaderHandler::fetchShaderIndex(renderQueueDataVector[i].gpShaderUUID);

			// these are temp
			models[index].model->updatePosition(renderQueueDataVector[i].position);
			models[index].model->updateRotation(renderQueueDataVector[i].rotation);
			models[index].model->updateScale(renderQueueDataVector[i].scale);
			models[index].model->updateTranformation();

			ShaderHandler::shaderObjects[modelGPShaderIndex].Shader.Activate();
			Scene::maincamera.Matrix(ShaderHandler::shaderObjects[modelGPShaderIndex].Shader, "camMatrix");

			ShaderHandler::shaderObjects[modelGPShaderIndex].Shader.Activate();
			ShaderHandler::shaderObjects[modelGPShaderIndex].Shader.setFloat("deltatime", TimeUtil::deltatime);
			ShaderHandler::shaderObjects[modelGPShaderIndex].Shader.setFloat("time", glfwGetTime());
			// this would normally be in material

			if (!RenderClass::DoForwardLightingPass && !RenderClass::DoDeferredLightingPass) continue; // Skip rendering if not in regular or lighting pass
			if (renderQueueDataVector[i].doCulling == true && !FEImGuiWindow::isWireframe) glEnable(GL_CULL_FACE);
			else glDisable(GL_CULL_FACE);
			if (renderQueueDataVector[i].cullFrontFace) glCullFace(GL_FRONT);
			else glCullFace(GL_BACK);

			if (FEImGuiWindow::isWireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Enable wireframe mode

			//smoothnessValue
			ShaderHandler::shaderObjects[modelGPShaderIndex].Shader.Activate();
			ShaderHandler::shaderObjects[modelGPShaderIndex].Shader.setFloat2("uvScale", renderQueueDataVector[i].uvScale);

			ShaderHandler::shaderObjects[modelGPShaderIndex].Shader.Activate();
			GeometryPass::gPassDraw(models[index].model, ShaderHandler::shaderObjects[modelGPShaderIndex].Shader, Scene::maincamera);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Enable wireframe mode

			//glFrontFace(GL_CCW);
			glCullFace(GL_BACK); // Reset culling to default
			glDisable(GL_CULL_FACE);
		}
	}

	if (RenderClass::DoForwardLightingPass) {
		// regular non instanced
		for (size_t i = 0; i < renderQueueDataVector.size(); i++)
		{
			int index = fetchModelIndex(renderQueueDataVector[i].RenderID);
			if (index != -1 && !renderQueueDataVector[i].isInstanced)
			{

				int modelShaderIndex = ShaderHandler::fetchShaderIndex(renderQueueDataVector[i].shaderUUID);

				// these are temp
				models[index].model->updatePosition(renderQueueDataVector[i].position);
				models[index].model->updateRotation(renderQueueDataVector[i].rotation);
				models[index].model->updateScale(renderQueueDataVector[i].scale);
				models[index].model->updateTranformation();

				LightingHandler::update(ShaderHandler::shaderObjects[modelShaderIndex].Shader);

				RenderClass::bluenoise->Bind();
				ShaderHandler::shaderObjects[modelShaderIndex].Shader.setInt("BlueNoiseTex", 6);

				ShaderHandler::shaderObjects[modelShaderIndex].Shader.setHandleui64ARB("bayerMatrixHandle", RenderClass::bayermatrix->handle);

				//Skybox::bind(5);
				//Skybox::cubemapToShader(ShaderHandler::shaderObjects[modelShaderIndex].Shader, 5);

				//glActiveTexture(GL_TEXTURE0 + 5);// + textureUnit
				//glBindTexture(GL_TEXTURE_CUBE_MAP, tempCM->ID);

				tempCM->cubemapToUUIDShader("cmMainHandle", ShaderHandler::shaderObjects[modelShaderIndex].Shader);

				ShaderHandler::shaderObjects[modelShaderIndex].Shader.Activate();
				ShaderHandler::shaderObjects[modelShaderIndex].Shader.setInt("skybox", 5);

				//tempCM

				// this would normally be in material
				ShaderHandler::shaderObjects[modelShaderIndex].Shader.Activate();
				Scene::maincamera.Matrix(ShaderHandler::shaderObjects[modelShaderIndex].Shader, "camMatrix"); // Send Camera Matrix To Shader Prog

				ShaderHandler::shaderObjects[modelShaderIndex].Shader.Activate();
				ShaderHandler::shaderObjects[modelShaderIndex].Shader.setFloat("deltatime", TimeUtil::deltatime);
				ShaderHandler::shaderObjects[modelShaderIndex].Shader.setFloat("time", glfwGetTime());
				// this would normally be in material

				if (!RenderClass::DoForwardLightingPass && !RenderClass::DoDeferredLightingPass) continue; // Skip rendering if not in regular or lighting pass
				if (renderQueueDataVector[i].doCulling == true && !FEImGuiWindow::isWireframe) glEnable(GL_CULL_FACE);
				else glDisable(GL_CULL_FACE);
				if (renderQueueDataVector[i].cullFrontFace) glCullFace(GL_FRONT);
				else glCullFace(GL_BACK);

				if (FEImGuiWindow::isWireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Enable wireframe mode


				ShaderHandler::shaderObjects[modelShaderIndex].Shader.Activate();
				ShaderHandler::shaderObjects[modelShaderIndex].Shader.setFloat2("uvScale", renderQueueDataVector[i].uvScale);
				ShaderHandler::shaderObjects[modelShaderIndex].Shader.setFloat("smoothnessValue", renderQueueDataVector[i].smoothnessValue);
				ShaderHandler::shaderObjects[modelShaderIndex].Shader.setInt("indirectSamples", ProbeHandler::indirectSamples);

				glActiveTexture(GL_TEXTURE7);
				glBindTexture(GL_TEXTURE_2D, GeometryPass::depthTexture);
				ShaderHandler::shaderObjects[modelShaderIndex].Shader.setInt("depthMap", 7);

				//smoothnessValue

				//raytracer::uvScaleUpdate(component.renderHeads.Model->UUID, component.systems.material.uvScale);


				glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::FBO);
				ShaderHandler::shaderObjects[modelShaderIndex].Shader.Activate();
				glEnable(GL_DEPTH_TEST);
				glDepthFunc(GL_LESS);

				// temp
				models[index].model->draw(ShaderHandler::shaderObjects[modelShaderIndex].Shader, Scene::maincamera);
				// Draw the mesh bounding box for visualization
				//for (size_t i = 0; i < component.renderHeads.Model->meshes.size(); i++)
				//{
				//	if (Collision::showBoxCollider)
				//		RenderClass::WhiteCube->draw(component.renderHeads.Model->meshes[i].boxCollider.position,
				//			component.renderHeads.Model->meshes[i].boxCollider.size, glm::vec3(1.0f));
				//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
				//}

				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Enable wireframe mode

				//glFrontFace(GL_CCW);
				glCullFace(GL_BACK); // Reset culling to default
				glDisable(GL_CULL_FACE);
				glBindFramebuffer(GL_FRAMEBUFFER, 0);

				glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
			}
		}
	}
}

void RenderHandler::instancedDraw()
{
	// needs to make batches of instanced data, do sep for both shadow and regular, shadow doesnt include shaders or uv
	for (size_t i = 0; i < renderQueueDataVector.size(); i++)
	{
		if (renderQueueDataVector[i].isInstanced)
		{

		}
	}
	// then needs to draw it
}
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

void RenderHandler::regularDraw()
{
	// positions
	for (size_t i = 0; i < renderQueueDataVector.size(); i++)
	{
		int index = fetchModelIndex(renderQueueDataVector[i].RenderID);
		if (index != -1)
		{
			// these are temp
			models[index].model->updatePosition(renderQueueDataVector[i].position);
			models[index].model->updateRotation(renderQueueDataVector[i].rotation);
			models[index].model->updateScale(renderQueueDataVector[i].scale);
			models[index].model->updateTranformation();
		}
	}
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
	// regular non instanced
	for (size_t i = 0; i < renderQueueDataVector.size(); i++)
	{
		int index = fetchModelIndex(renderQueueDataVector[i].RenderID);
		if (index != -1 && !renderQueueDataVector[i].isInstanced)
		{

			int modelShaderIndex = ShaderHandler::fetchShaderIndex(renderQueueDataVector[i].shaderUUID);
			int modelGPShaderIndex = ShaderHandler::fetchShaderIndex(renderQueueDataVector[i].gpShaderUUID);

			// these are temp
			models[index].model->updatePosition(renderQueueDataVector[i].position);
			models[index].model->updateRotation(renderQueueDataVector[i].rotation);
			models[index].model->updateScale(renderQueueDataVector[i].scale);
			models[index].model->updateTranformation();

			LightingHandler::update(ShaderHandler::shaderObjects[modelShaderIndex].Shader);

			RenderClass::bluenoise->Bind();
			ShaderHandler::shaderObjects[modelShaderIndex].Shader.setInt("BlueNoiseTex", 6);
			Skybox::bind(5);
			Skybox::cubemapToShader(ShaderHandler::shaderObjects[modelShaderIndex].Shader, 5);

			// this would normally be in material
			ShaderHandler::shaderObjects[modelShaderIndex].Shader.Activate();
			Scene::maincamera.Matrix(ShaderHandler::shaderObjects[modelShaderIndex].Shader, "camMatrix"); // Send Camera Matrix To Shader Prog
			ShaderHandler::shaderObjects[modelGPShaderIndex].Shader.Activate();
			Scene::maincamera.Matrix(ShaderHandler::shaderObjects[modelGPShaderIndex].Shader, "camMatrix");

			ShaderHandler::shaderObjects[modelShaderIndex].Shader.Activate();
			ShaderHandler::shaderObjects[modelShaderIndex].Shader.setFloat("deltatime", TimeUtil::deltatime);
			ShaderHandler::shaderObjects[modelShaderIndex].Shader.setFloat("time", glfwGetTime());

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


			ShaderHandler::shaderObjects[modelShaderIndex].Shader.Activate();
			ShaderHandler::shaderObjects[modelShaderIndex].Shader.setFloat2("uvScale", renderQueueDataVector[i].uvScale);
			ShaderHandler::shaderObjects[modelShaderIndex].Shader.setFloat("smoothnessValue", renderQueueDataVector[i].smoothnessValue);
			//smoothnessValue
			ShaderHandler::shaderObjects[modelGPShaderIndex].Shader.Activate();
			ShaderHandler::shaderObjects[modelGPShaderIndex].Shader.setFloat2("uvScale", renderQueueDataVector[i].uvScale);

			//raytracer::uvScaleUpdate(component.renderHeads.Model->UUID, component.systems.material.uvScale);

			if (RenderClass::DoForwardLightingPass) {
				glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::FBO);
				ShaderHandler::shaderObjects[modelShaderIndex].Shader.Activate();
				glEnable(GL_DEPTH_TEST);
				glDepthFunc(GL_LESS);
				models[index].model->draw(ShaderHandler::shaderObjects[modelShaderIndex].Shader);
				// Draw the mesh bounding box for visualization
				//for (size_t i = 0; i < component.renderHeads.Model->meshes.size(); i++)
				//{
				//	if (Collision::showBoxCollider)
				//		RenderClass::WhiteCube->draw(component.renderHeads.Model->meshes[i].boxCollider.position,
				//			component.renderHeads.Model->meshes[i].boxCollider.size, glm::vec3(1.0f));
				//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
				//}
			}

			ShaderHandler::shaderObjects[modelGPShaderIndex].Shader.Activate();
			GeometryPass::gPassDraw(models[index].model, ShaderHandler::shaderObjects[modelGPShaderIndex].Shader);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Enable wireframe mode

			//glFrontFace(GL_CCW);
			glCullFace(GL_BACK); // Reset culling to default
			glDisable(GL_CULL_FACE);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			Skybox::unbind();
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
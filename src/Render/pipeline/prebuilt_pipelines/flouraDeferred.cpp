#include "flouraDeferred.h"
#include <Render/pipeline/prebuilt_pipelines/geometryPass.h>
#include <Render/pipeline/prebuilt_pipelines/historyPass.h>
#include <Render/Shader/renderTarget.h>
#include <Render/Handler/RenderClass.h>
#include <Scene/LightingHandler.h>
#include <Scene/Scene.h>

#include "Render/Handler/RenderHandler.h"


void FlouraDeferred::init(){
}

void FlouraDeferred::createShaders(){
    DFL_Shader.LoadShader("Assets/Shaders/Deferred/DFR_Phon.vert", "Assets/Shaders/Deferred/DFR_Phon.frag");
    SSR_Shader.LoadShader("Assets/Shaders/Deferred/screenspaceReflections.vert", "Assets/Shaders/Deferred/screenspaceReflections.frag");
}

void FlouraDeferred::draw(){

}

void FlouraDeferred::Delete()
{
	DFL_Shader.Delete();
	SSR_Shader.Delete();
}

Shader FlouraDeferred::DFL_Shader;
Shader FlouraDeferred::SSR_Shader;

void FlouraDeferred::DeferredLightingPass(){
	glDisable(GL_CULL_FACE);
	glBindFramebuffer(GL_FRAMEBUFFER, renderTarget::FBO);
	DFL_Shader.Activate();
	// gPass textures bound to FB
	// send gPass textures to shader
	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, GeometryPass::gPosition);
	DFL_Shader.setInt("gPosition", 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, GeometryPass::gNormal);
	DFL_Shader.setInt("gNormal", 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, GeometryPass::gAlbedoSpec);
	DFL_Shader.setInt("gAlbedoSpec", 3);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, GeometryPass::depthTexture);
	DFL_Shader.setInt("depthMap", 5);
	
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, GeometryPass::gSpecular);
	DFL_Shader.setInt("gSpecular", 6);
	
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, GeometryPass::gVelocity);
	DFL_Shader.setInt("gVelocity", 7);
	// skip 8 because of shadow map (i really need to use bindless on these)
	
	
	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, GeometryPass::gEmission);
	glGenerateMipmap(GL_TEXTURE_2D);
	DFL_Shader.setInt("gEmission", 10);
	

	glActiveTexture(GL_TEXTURE11);
	glBindTexture(GL_TEXTURE_2D, HistoryPass::hColour);
	glGenerateMipmap(GL_TEXTURE_2D); // remove later
	DFL_Shader.setInt("hColour", 11);
	
	// reserve 10 for depth
	glActiveTexture(GL_TEXTURE12);
	glBindTexture(GL_TEXTURE_2D, HistoryPass::hDepthTexture);
	DFL_Shader.setInt("hDepthTexture", 12);
	
	// prior normals
	glActiveTexture(GL_TEXTURE13);
	glBindTexture(GL_TEXTURE_2D, HistoryPass::hNormal);
	DFL_Shader.setInt("hNormal", 13);
	
	glActiveTexture(GL_TEXTURE14);
	glBindTexture(GL_TEXTURE_2D, renderTarget::skyGradientTexture);
	DFL_Shader.setInt("skyGradientTexture", 14);
	
	DFL_Shader.setFloat("NearPlane", Scene::maincamera.nearFar.x);
	DFL_Shader.setFloat("FarPlane", Scene::maincamera.nearFar.y);

	DFL_Shader.setFloat("fogDepthDistance", RenderClass::DepthDistance);
	DFL_Shader.setFloat("fogNearPlane", RenderClass::DepthPlane[0]);
	DFL_Shader.setFloat("fogFarPlane", RenderClass::DepthPlane[1]);
	DFL_Shader.setBool("doFog", RenderClass::doFog);

	DFL_Shader.setFloat3("fogColor", RenderClass::fogRGBA);
	//GBLpass.Activate();
	//mat4
	DFL_Shader.setMat4("cameraMatrix", Scene::maincamera.cameraMatrix);
	DFL_Shader.setMat4("projectionMatrix", Scene::maincamera.projection);
	DFL_Shader.setMat4("viewMatrix", Scene::maincamera.view);
	DFL_Shader.setMat4("inverseViewMatrix",glm::inverse(Scene::maincamera.view));
	DFL_Shader.setMat4("inverseProjection", glm::inverse(Scene::maincamera.projection));

	glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(Scene::maincamera.cameraMatrix)));
	DFL_Shader.setMat3("normalMatrix", normalMatrix);

	DFL_Shader.setFloat3("orientation", Scene::maincamera.Orientation);
	DFL_Shader.setFloat3("cameraPos", Scene::maincamera.Position);
	DFL_Shader.setFloat3("cameraDirection", Scene::maincamera.Orientation);
	DFL_Shader.setFloat3("camPos", Scene::maincamera.Position);
	DFL_Shader.setInt("indirectSamples", ProbeHandler::indirectSamples);

	
	DFL_Shader.setHandleui64ARB("BlueNoiseHandle", RenderClass::bluenoise->handle);
	DFL_Shader.setHandleui64ARB("bayerMatrixHandle", RenderClass::bayermatrix->handle);

	//DFL_Shader.setBool("doSSR", RenderClass::doSSR);
	DFL_Shader.setBool("doContactShadows", RenderClass::doContactShadows);

	DFL_Shader.setTimeVariables();
	
	LightingHandler::sendToShader(DFL_Shader);
	
	//shader.
	RenderQuad::draw();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void FlouraDeferred::ssrPass(){
	glDisable(GL_CULL_FACE);
	glBindFramebuffer(GL_FRAMEBUFFER, renderTarget::FBO);
	SSR_Shader.Activate();
	// gPass textures bound to FB
	// send gPass textures to shader
	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, renderTarget::screentexture);
	glGenerateMipmap(GL_TEXTURE_2D);
	SSR_Shader.setInt("screentexture", 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, GeometryPass::gPosition);
	SSR_Shader.setInt("gPosition", 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, GeometryPass::gNormal);
	SSR_Shader.setInt("gNormal", 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, GeometryPass::gAlbedoSpec);
	SSR_Shader.setInt("gAlbedoSpec", 3);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, GeometryPass::depthTexture);
	SSR_Shader.setInt("depthMap", 5);
	
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, GeometryPass::gSpecular);
	SSR_Shader.setInt("gSpecular", 6);
	
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, GeometryPass::gVelocity);
	SSR_Shader.setInt("gVelocity", 7);
	// skip 8 because of shadow map (i really need to use bindless on these)
	
	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, GeometryPass::gEmission);
	glGenerateMipmap(GL_TEXTURE_2D);
	SSR_Shader.setInt("gEmission", 10);
	

	glActiveTexture(GL_TEXTURE11);
	glBindTexture(GL_TEXTURE_2D, HistoryPass::hColour);
	glGenerateMipmap(GL_TEXTURE_2D); // remove later
	SSR_Shader.setInt("hColour", 11);
	
	// reserve 10 for depth
	glActiveTexture(GL_TEXTURE12);
	glBindTexture(GL_TEXTURE_2D, HistoryPass::hDepthTexture);
	SSR_Shader.setInt("hDepthTexture", 12);
	
	// prior normals
	glActiveTexture(GL_TEXTURE13);
	glBindTexture(GL_TEXTURE_2D, HistoryPass::hNormal);
	SSR_Shader.setInt("hNormal", 13);
	
	
	SSR_Shader.setFloat("NearPlane", Scene::maincamera.nearFar.x);
	SSR_Shader.setFloat("FarPlane", Scene::maincamera.nearFar.y);
	
	//ssPass.Activate();
	//mat4
	SSR_Shader.setMat4("cameraMatrix", Scene::maincamera.cameraMatrix);
	SSR_Shader.setMat4("projectionMatrix", Scene::maincamera.projection);
	SSR_Shader.setMat4("viewMatrix", Scene::maincamera.view);
	SSR_Shader.setMat4("inverseViewMatrix",glm::inverse(Scene::maincamera.view));
	SSR_Shader.setMat4("inverseProjection", glm::inverse(Scene::maincamera.projection));

	glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(Scene::maincamera.cameraMatrix)));
	SSR_Shader.setMat3("normalMatrix", normalMatrix);

	SSR_Shader.setFloat3("orientation", Scene::maincamera.Orientation);
	SSR_Shader.setFloat3("cameraPos", Scene::maincamera.Position);
	SSR_Shader.setFloat3("cameraDirection", Scene::maincamera.Orientation);
	SSR_Shader.setFloat3("camPos", Scene::maincamera.Position);
	SSR_Shader.setInt("indirectSamples", ProbeHandler::indirectSamples);
	
	SSR_Shader.setHandleui64ARB("BlueNoiseHandle", RenderClass::bluenoise->handle);
	SSR_Shader.setHandleui64ARB("bayerMatrixHandle", RenderClass::bayermatrix->handle);

	SSR_Shader.setBool("doSSR", RenderClass::doSSR);
	SSR_Shader.setBool("doContactShadows", RenderClass::doContactShadows);
	SSR_Shader.setBool("doReflections", RenderClass::doReflections);
	
	SSR_Shader.setTimeVariables();
	//RenderHandler::tempCM->cubemapToUUIDShader("cmMainHandle", RenderClass::ssPass);
	
	Skybox::SkyboxCubemap->cubemapToUUIDShader("SB_Handle", SSR_Shader);
	if (RenderHandler::renderENV)  RenderHandler::tempCM->cubemapToUUIDShader("cmMainHandle", SSR_Shader);
	else Skybox::SkyboxCubemap->cubemapToUUIDShader("cmMainHandle", SSR_Shader);
	
	//shader.
	RenderQuad::draw();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

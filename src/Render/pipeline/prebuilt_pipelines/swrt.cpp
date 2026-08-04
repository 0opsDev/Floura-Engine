#include "swrt.h"
#include "Render/pipeline/prebuilt_pipelines/geometryPass.h"
#include "Scene/scene.h"
#include "Scene/LightingHandler.h"
#include <chrono>    

unsigned int FlouraSWRT::swrtBuffer;
unsigned int FlouraSWRT::indirect; 
unsigned int FlouraSWRT::specular; 
unsigned int FlouraSWRT::emission; 
unsigned int FlouraSWRT::indirectSpecular; 
unsigned int FlouraSWRT::emissionSpecular; 
unsigned int FlouraSWRT::direct;
unsigned int FlouraSWRT::filteredVariance;

// history
unsigned int FlouraSWRT::swrtHBuffer; 
unsigned int FlouraSWRT::hIndirect; 
unsigned int FlouraSWRT::hEmission; 
unsigned int FlouraSWRT::hIndirectSpecular; 
unsigned int FlouraSWRT::hEmissionSpecular; 
unsigned int FlouraSWRT::presentImage; 
unsigned int FlouraSWRT::swrtHDepth; 

Shader FlouraSWRT::raymarchRQShader;
Shader FlouraSWRT::raymarchCompShader;
Shader FlouraSWRT::denoisePrePassShader;
Shader FlouraSWRT::denoiseShader;
Shader FlouraSWRT::presentShader;


bool FlouraSWRT::doTemporalAccumulation = true;
bool FlouraSWRT::doDenoise = true;
bool FlouraSWRT::doDenoiseSplitDBGView;
int FlouraSWRT::denoiseRadius = 3;
float FlouraSWRT::temporalAccumulationBlendFactor = 0.9f;

bool FlouraSWRT::doHalfRes = false;
    
double FlouraSWRT::rmTime = 0.0f;

void FlouraSWRT::initShaders(){
    raymarchRQShader.LoadShader("Assets/Shaders/raymarched/raymarch.vert", "Assets/Shaders/raymarched/traceSWRT.frag");
	raymarchCompShader.LoadComputeShader("Assets/Shaders/raymarched/traceSWRT.comp");
	denoisePrePassShader.LoadShader("Assets/Shaders/raymarched/raymarch.vert", "Assets/Shaders/raymarched/denoisePrePassSWRT.frag");
	denoiseShader.LoadShader("Assets/Shaders/raymarched/raymarch.vert", "Assets/Shaders/raymarched/denoiseSWRT.frag");
	presentShader.LoadShader("Assets/Shaders/raymarched/raymarch.vert", "Assets/Shaders/raymarched/presentSWRT.frag");
}

void FlouraSWRT::cleanupShaders(){
	raymarchRQShader.Delete();
	raymarchCompShader.Delete();
	denoisePrePassShader.Delete();
	denoiseShader.Delete();
	presentShader.Delete();
}

void FlouraSWRT::setupSWRTbuffers(unsigned int width, unsigned int height){
	glGenFramebuffers(1, &swrtBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, swrtBuffer);

	glGenTextures(1, &indirect);
	glBindTexture(GL_TEXTURE_2D, indirect);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, indirect, 0);
	
	// needed to become vec4 for comp
	glGenTextures(1, &specular);
	glBindTexture(GL_TEXTURE_2D, specular);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, specular, 0);

	glGenTextures(1, &emission);
	glBindTexture(GL_TEXTURE_2D, emission);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, emission, 0);

	glGenTextures(1, &indirectSpecular);
	glBindTexture(GL_TEXTURE_2D, indirectSpecular);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, indirectSpecular, 0);
	
	glGenTextures(1, &emissionSpecular);
	glBindTexture(GL_TEXTURE_2D, emissionSpecular);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, emissionSpecular, 0);
	
	glGenTextures(1, &direct);
	glBindTexture(GL_TEXTURE_2D, direct);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, direct, 0);
	
	//filteredVariance
	glGenTextures(1, &filteredVariance);
	glBindTexture(GL_TEXTURE_2D, filteredVariance);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, GL_TEXTURE_2D, filteredVariance, 0);
	
	unsigned int attachments[7] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,
													GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5,
													GL_COLOR_ATTACHMENT6};
	glDrawBuffers(7, attachments);
	
	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete! (SWRT first set)" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	// H BUFFERRRRRRS
	
	glGenFramebuffers(1, &swrtHBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, swrtHBuffer);

	glGenTextures(1, &hIndirect);
	glBindTexture(GL_TEXTURE_2D, hIndirect);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hIndirect, 0);
	
	glGenTextures(1, &hEmission);
	glBindTexture(GL_TEXTURE_2D, hEmission);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, hEmission, 0);

	glGenTextures(1, &hIndirectSpecular);
	glBindTexture(GL_TEXTURE_2D, hIndirectSpecular);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, hIndirectSpecular, 0);

	glGenTextures(1, &hEmissionSpecular);
	glBindTexture(GL_TEXTURE_2D, hEmissionSpecular);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, hEmissionSpecular, 0);

	glGenTextures(1, &presentImage);
	glBindTexture(GL_TEXTURE_2D, presentImage);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, presentImage, 0);
	
	//swrtHDepth
	
	glGenTextures(1, &swrtHDepth);
	glBindTexture(GL_TEXTURE_2D, swrtHDepth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, swrtHDepth, 0);
	
	unsigned int hAttachments[6] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5};
	glDrawBuffers(6, hAttachments);
	
	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FlouraSWRT::updateSWRTbuffersResolution(unsigned int width, unsigned int height){
	if (doHalfRes){
		width = width / 2;
		height = height / 2;
	}
	glBindTexture(GL_TEXTURE_2D, indirect);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	//glBindTexture(GL_TEXTURE_2D, 0);
	
	glBindTexture(GL_TEXTURE_2D, specular);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	//glBindTexture(GL_TEXTURE_2D, 0);
	
	glBindTexture(GL_TEXTURE_2D, emission);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	//glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, indirectSpecular);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	//glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, emissionSpecular);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL); // third component is alpha masks
	//glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, direct);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	//glBindTexture(GL_TEXTURE_2D, 0);
	
	glBindTexture(GL_TEXTURE_2D, filteredVariance);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	//glBindTexture(GL_TEXTURE_2D, 0);
	// history
	
	glBindTexture(GL_TEXTURE_2D, hIndirect);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	//glBindTexture(GL_TEXTURE_2D, 0);
	
	glBindTexture(GL_TEXTURE_2D, hEmission);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	//glBindTexture(GL_TEXTURE_2D, 0);
	
	glBindTexture(GL_TEXTURE_2D, hIndirectSpecular);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	//glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, hEmissionSpecular);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	//glBindTexture(GL_TEXTURE_2D, 0);
	
	glBindTexture(GL_TEXTURE_2D, presentImage);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	
	glBindTexture(GL_TEXTURE_2D, swrtHDepth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
	
	glBindTexture(GL_TEXTURE_2D, 0);
}

void FlouraSWRT::cleanupSWRTbuffers(){
	glDeleteFramebuffers(1, &swrtBuffer);
	GLuint texs[] = {
		indirect,
		specular,
		emission,
		indirectSpecular,
		emissionSpecular,
		direct,
		filteredVariance
	};
	glDeleteTextures(sizeof(texs) / sizeof(GLuint), texs);
	
	// history
	
	glDeleteFramebuffers(1, &swrtHBuffer);
	GLuint hTexs[] = {
		hIndirect,
		hEmission,
		hEmissionSpecular,
		hIndirectSpecular,
		presentImage,
		swrtHDepth
	};
	glDeleteTextures(sizeof(hTexs) / sizeof(GLuint), hTexs);
}

void FlouraSWRT::draw(){
	rmTime = 0.0;
	if (doHalfRes){
		unsigned int w = renderTarget::ViewPortWidth / 2;	
		unsigned int h = renderTarget::ViewPortHeight / 2;
		glViewport(0, 0, w, h );
		rmPassRenderQuad(w, h);
		//rmPassCompute(w,h);
		denoisePrePass();
		denoisePass();
		w = renderTarget::ViewPortWidth;	
		h = renderTarget::ViewPortHeight;
		glViewport(0, 0, w, h );
	}
	else{
		unsigned int w = renderTarget::ViewPortWidth;	
		unsigned int h = renderTarget::ViewPortHeight;
		rmPassRenderQuad(w, h);
		//rmPassCompute(w,h);
		denoisePrePass();
		denoisePass();
	}
	presentPass();
}

void FlouraSWRT::rmPassRenderQuad(unsigned int w, unsigned int h){
    glDisable(GL_CULL_FACE);
    //glBindFramebuffer(GL_FRAMEBUFFER, renderTarget::FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, swrtBuffer);
	
    raymarchRQShader.Activate();
	
	raymarchRQShader.setFloat("NearPlane", Scene::maincamera.nearFar.x);
	raymarchRQShader.setFloat("FarPlane", Scene::maincamera.nearFar.y);
	
	raymarchRQShader.setBool("doTemporalAccumulation", doTemporalAccumulation);
	raymarchRQShader.setFloat("temporalAccumulationBlendFactor", temporalAccumulationBlendFactor);
	raymarchRQShader.setBool("doDenoiseSplitDBGView", doDenoiseSplitDBGView);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, GeometryPass::gVelocity);
	raymarchRQShader.setInt("gVelocity", 0);
	
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, GeometryPass::gPosition);
    raymarchRQShader.setInt("gPosition", 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, GeometryPass::gNormal);
    raymarchRQShader.setInt("gNormal", 2);
	
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, GeometryPass::gAlbedoSpec);
    raymarchRQShader.setInt("gAlbedoSpec", 3);
	
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, GeometryPass::depthTexture);
    raymarchRQShader.setInt("depthMap", 5);
	
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, GeometryPass::gSpecular);
    raymarchRQShader.setInt("gSpecular", 6);
	
    glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_2D, GeometryPass::gEmission);
    glGenerateMipmap(GL_TEXTURE_2D);
    raymarchRQShader.setInt("gEmission", 10);
	
	glActiveTexture(GL_TEXTURE11);
	glBindTexture(GL_TEXTURE_2D, swrtHDepth);
	raymarchRQShader.setInt("swrtHDepth", 11);
	
	// History
	glActiveTexture(GL_TEXTURE12);
	glBindTexture(GL_TEXTURE_2D, hIndirect);
	raymarchRQShader.setInt("hIndirect", 12);
	
	glActiveTexture(GL_TEXTURE13);
	glBindTexture(GL_TEXTURE_2D, hEmission);
	raymarchRQShader.setInt("hEmission", 14);
	
	glActiveTexture(GL_TEXTURE15);
	glBindTexture(GL_TEXTURE_2D, hIndirectSpecular);
	raymarchRQShader.setInt("hIndirectSpecular", 15);
	
	glActiveTexture(GL_TEXTURE16);
	glBindTexture(GL_TEXTURE_2D, hEmissionSpecular);
	raymarchRQShader.setInt("hEmissionSpecular", 16);
	
	glActiveTexture(GL_TEXTURE17);
	glBindTexture(GL_TEXTURE_2D, presentImage);
	raymarchRQShader.setInt("presentImage", 17);
	
	
    raymarchRQShader.setFloat2("screenSize", glm::vec2(w, h));
    raymarchRQShader.setMat4("viewMatrix", Scene::maincamera.view);
	raymarchRQShader.setMat4("invViewMatrix", glm::inverse(Scene::maincamera.view));
    raymarchRQShader.setMat4("projectionMatrix", Scene::maincamera.projection);
	raymarchRQShader.setMat4("invProjectionMatrix", glm::inverse(Scene::maincamera.projection));
	raymarchRQShader.setMat4("invHViewMatrix", glm::inverse(Scene::maincamera.hView));
	raymarchRQShader.setMat4("invHProjectionMatrix",glm::inverse(Scene::maincamera.hProjection));
	
    raymarchRQShader.setFloat3("cameraPosition", Scene::maincamera.Position);
    //raymarchShader.setFloat3("skycolour", skyRGBA);
    raymarchRQShader.setHandleui64ARB("BlueNoiseHandle", RenderClass::bluenoise->handle);
    Skybox::SkyboxCubemap->cubemapToUUIDShader("cmMainHandle", raymarchRQShader);
	
	raymarchRQShader.setFloat3("sceneBoundPos", Scene::SceneBounds.position);
	raymarchRQShader.setFloat3("sceneBoundScale", Scene::SceneBounds.size);
	
    raymarchRQShader.setTimeVariables();
	
    LightingHandler::sendToShader(raymarchRQShader);
	
    RenderQuad::draw();
    //glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glActiveTexture(0);
    glBindTexture(GL_TEXTURE_2D, 0);	
}

void FlouraSWRT::rmPassCompute(unsigned int w, unsigned int h){
	raymarchCompShader.Activate();
	
	glBindImageTexture(0, indirect, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F);
	glBindImageTexture(1, specular, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
	glBindImageTexture(2, emission, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F);
	glBindImageTexture(3, indirectSpecular, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F);
	glBindImageTexture(4, emissionSpecular, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F);
	glBindImageTexture(5, direct, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

	
	
	
	raymarchCompShader.setFloat("NearPlane", Scene::maincamera.nearFar.x);
	raymarchCompShader.setFloat("FarPlane", Scene::maincamera.nearFar.y);
	
	raymarchCompShader.setBool("doTemporalAccumulation", doTemporalAccumulation);
	raymarchCompShader.setFloat("temporalAccumulationBlendFactor", temporalAccumulationBlendFactor);
	raymarchCompShader.setBool("doDenoiseSplitDBGView", doDenoiseSplitDBGView);
	
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, GeometryPass::gVelocity);
	raymarchCompShader.setInt("gVelocity", 6);
	
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, GeometryPass::gPosition);
    raymarchCompShader.setInt("gPosition", 7);

	// shadow map at 8
    glActiveTexture(GL_TEXTURE9);
    glBindTexture(GL_TEXTURE_2D, GeometryPass::gNormal);
    raymarchCompShader.setInt("gNormal", 9);
	
    glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_2D, GeometryPass::gAlbedoSpec);
    raymarchCompShader.setInt("gAlbedoSpec", 10);
	
    glActiveTexture(GL_TEXTURE11);
    glBindTexture(GL_TEXTURE_2D, GeometryPass::depthTexture);
    raymarchCompShader.setInt("depthMap", 11);
	
    glActiveTexture(GL_TEXTURE12);
    glBindTexture(GL_TEXTURE_2D, GeometryPass::gSpecular);
    raymarchCompShader.setInt("gSpecular", 12);
	
    glActiveTexture(GL_TEXTURE13);
    glBindTexture(GL_TEXTURE_2D, GeometryPass::gEmission);
    glGenerateMipmap(GL_TEXTURE_2D);
    raymarchCompShader.setInt("gEmission", 13);
	
	glActiveTexture(GL_TEXTURE14);
	glBindTexture(GL_TEXTURE_2D, swrtHDepth);
	raymarchCompShader.setInt("swrtHDepth", 14);
	
	// History
	glActiveTexture(GL_TEXTURE15);
	glBindTexture(GL_TEXTURE_2D, hIndirect);
	raymarchCompShader.setInt("hIndirect", 15);
	
	glActiveTexture(GL_TEXTURE16);
	glBindTexture(GL_TEXTURE_2D, hEmission);
	raymarchCompShader.setInt("hEmission", 16);
	
	glActiveTexture(GL_TEXTURE17);
	glBindTexture(GL_TEXTURE_2D, hIndirectSpecular);
	raymarchCompShader.setInt("hIndirectSpecular", 17);
	
	glActiveTexture(GL_TEXTURE18);
	glBindTexture(GL_TEXTURE_2D, hEmissionSpecular);
	raymarchCompShader.setInt("hEmissionSpecular", 18);
	
	glActiveTexture(GL_TEXTURE19);
	glBindTexture(GL_TEXTURE_2D, presentImage);
	raymarchCompShader.setInt("presentImage", 19);
	
	
    raymarchCompShader.setFloat2("screenSize", glm::vec2(w, h));
    raymarchCompShader.setMat4("u_ViewMatrix", Scene::maincamera.view);
    //raymarchShader.setMat4("u_ProjectionMatrix", Scene::maincamera.projectionAlwaysUnjittered);
    raymarchCompShader.setMat4("u_ProjectionMatrix", Scene::maincamera.projection);
    raymarchCompShader.setFloat3("cameraPosition", Scene::maincamera.Position);
    //raymarchShader.setFloat3("skycolour", skyRGBA);
    raymarchCompShader.setHandleui64ARB("BlueNoiseHandle", RenderClass::bluenoise->handle);
    Skybox::SkyboxCubemap->cubemapToUUIDShader("cmMainHandle", raymarchCompShader);
	
	raymarchCompShader.setFloat3("sceneBoundPos", Scene::SceneBounds.position);
	raymarchCompShader.setFloat3("sceneBoundScale", Scene::SceneBounds.size);
	
    raymarchCompShader.setTimeVariables();
	
    LightingHandler::sendToShader(raymarchCompShader);
	
	glDispatchCompute((w + 7) / 8, (h + 3) / 4, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void FlouraSWRT::denoisePrePass(){
	glDisable(GL_CULL_FACE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, swrtBuffer);
	
	denoisePrePassShader.Activate();
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, GeometryPass::depthTexture);
	denoisePrePassShader.setInt("depthMap", 0);
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, GeometryPass::gNormal);
	denoisePrePassShader.setInt("gNormal", 1);
	
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, indirect);
	denoisePrePassShader.setInt("swrtIndirect", 2);
		
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, emission);
	denoisePrePassShader.setInt("swrtEmission", 3);
		
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, indirectSpecular);
	denoisePrePassShader.setInt("swrtIndirectSpecular", 4);
		
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, emissionSpecular);
	denoisePrePassShader.setInt("swrtEmissionSpecular", 5);
	
	denoisePrePassShader.setBool("doDenoise", doDenoise);
	denoisePrePassShader.setBool("doDenoiseSplitDBGView", doDenoiseSplitDBGView);
	denoisePrePassShader.setFloat("NearPlane", Scene::maincamera.nearFar.x);
	denoisePrePassShader.setFloat("FarPlane", Scene::maincamera.nearFar.y);

	
	RenderQuad::draw();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, 0);	
}

void FlouraSWRT::denoisePass(){
		glDisable(GL_CULL_FACE);
		glBindFramebuffer(GL_FRAMEBUFFER, swrtHBuffer);
		denoiseShader.Activate();
		
		denoiseShader.setBool("doDenoise", doDenoise);
		denoiseShader.setInt("denoiseRadius",denoiseRadius);
		denoiseShader.setBool("doDenoiseSplitDBGView", doDenoiseSplitDBGView);
		denoiseShader.setFloat("NearPlane", Scene::maincamera.nearFar.x);
		denoiseShader.setFloat("FarPlane", Scene::maincamera.nearFar.y);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specular);
		denoiseShader.setInt("swrtSpecular", 1);
		
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, direct);
		denoiseShader.setInt("swrtDbgColour", 2);
		
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, GeometryPass::gEmission);
		denoiseShader.setInt("gEmission", 3);
		
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, GeometryPass::gNormal);
		denoiseShader.setInt("gNormal", 4);
		
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, GeometryPass::gAlbedoSpec);
		denoiseShader.setInt("gAlbedoSpec", 5);
		
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, GeometryPass::depthTexture);
		denoiseShader.setInt("depthMap", 6);
		
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, indirect);
		denoiseShader.setInt("swrtIndirect", 7);
		
		glActiveTexture(GL_TEXTURE8);
		glBindTexture(GL_TEXTURE_2D, emission);
		denoiseShader.setInt("swrtEmission", 8);
		
		glActiveTexture(GL_TEXTURE9);
		glBindTexture(GL_TEXTURE_2D, indirectSpecular);
		denoiseShader.setInt("swrtIndirectSpecular", 9);
		
		glActiveTexture(GL_TEXTURE10);
		glBindTexture(GL_TEXTURE_2D, emissionSpecular);
		denoiseShader.setInt("swrtEmissionSpecular", 10);
	
		glActiveTexture(GL_TEXTURE11);
		glBindTexture(GL_TEXTURE_2D, filteredVariance);
		denoiseShader.setInt("filteredVariance", 11);
	
		glActiveTexture(GL_TEXTURE12);
		glBindTexture(GL_TEXTURE_2D, GeometryPass::gSpecular);
		denoiseShader.setInt("gSpecular", 12);
	
		RenderQuad::draw();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glActiveTexture(0);
		glBindTexture(GL_TEXTURE_2D, 0);	
}

void FlouraSWRT::presentPass(){
	glDisable(GL_CULL_FACE);
	glBindFramebuffer(GL_FRAMEBUFFER, renderTarget::FBO);
	
	presentShader.Activate();
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, presentImage);
	presentShader.setInt("presentImage", 1);
	
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, GeometryPass::depthTexture);
	presentShader.setInt("depthMap", 2);
	
	RenderQuad::draw();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, 0);	
}

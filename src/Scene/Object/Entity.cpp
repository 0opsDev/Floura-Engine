#include "Entity.h"
#include <scene/scene.h>
#include <Math/FE_math.h>
#include <utils/logConsole.h>
#include <Scene/LightingHandler.h>
#include <Render/passes/geometry/geometryPass.h>

void entity::create(const char& type, const std::string& name, const std::string& path, const std::string& materialPath)
{
	ID.ObjType = 'o';
	ID.index = Scene::entityObjects.size(); // fetch array size from scene based on type
	IdManager::AddID(ID);

	// set type
	entity::type = type;
	// set name 
	entity::name = name;
	// set path 
	entity::path = path;

	// load material
	

	/*
	TYPES:
	m = model
	b = billboard
	c = collider
	p = particle
	e = empty
	*/
	switch (type)
	{
	case 'm': // model
		createModel(path, materialPath);
		break;

	case 'b': // billboard
		createBillBoard(path);
		break;
	case 'e': // empty

		break;
	case 'c': // collider
		break;
	case 'p': // particle
		break;
	default:
		LogConsole::print("Entity Create: Unknown type '" + std::string(1, type) + "' for entity: " + name);
		break;
	}

	
}

void entity::LoadMaterial(std::string path)
{
	// "Assets/Material/Default.Material" example path
	component.systems.material.Material.LoadMaterial(path);

}


void entity::update()
{
	
}

void entity::updateLights()
{
	component.systems.material.Material.updateForwardLights();
}

void entity::Delete()
{
	switch (type)
	{
	case 'm': // model
		delete component.renderHeads.Model;
		component.systems.material.Material.ClearMaterial();
		component.renderHeads.Model = nullptr;
		break;
	case 'b': // billboard
		delete component.renderHeads.BillBoard;
		component.renderHeads.BillBoard = nullptr;
		break;
	}
	//update lowest free index
	if (ID.index < IdManager::lowestDeletedIndex.object || IdManager::lowestDeletedIndex.BillBoard == -1) {
		IdManager::lowestDeletedIndex.object = ID.index;
		LogConsole::print("Lowest Deleted object Index is now: " + std::to_string(IdManager::lowestDeletedIndex.object));
	}

	IdManager::RemoveID(ID);
	//IdManager::lowestBillBoardIndexSync(); // sync up the index after deletion because the array has now changed
}


void entity::draw()
{
	switch (type)
	{
	case 'm': // model

		component.systems.material.Material.update();

		component.renderHeads.Model->updatePosition(component.systems.transformation.position);
		component.renderHeads.Model->updateRotation(component.systems.transformation.rotation);
		component.renderHeads.Model->updateScale(component.systems.transformation.scale);


		if (!RenderClass::DoForwardLightingPass && !RenderClass::DoDeferredLightingPass) {
			return; // Skip rendering if not in regular or lighting pass
		}

		if (component.flags.doCulling == true && !FEImGuiWindow::isWireframe) { glEnable(GL_CULL_FACE); }
		else { glDisable(GL_CULL_FACE); }

		if (component.flags.cullFrontFace) { glCullFace(GL_FRONT); }
		else { glCullFace(GL_BACK); }

		if (FEImGuiWindow::isWireframe) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Enable wireframe mode
		}

		component.systems.material.Material.ModelShader.Activate();
		component.systems.material.Material.ModelShader.setFloat2("uvScale", component.systems.material.uvScale.x, component.systems.material.uvScale.y);
		component.systems.material.Material.ModelGpassShader.Activate();
		component.systems.material.Material.ModelGpassShader.setFloat2("uvScale", component.systems.material.uvScale.x, component.systems.material.uvScale.y);

		if (RenderClass::DoForwardLightingPass) {
			glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::FBO);
			component.systems.material.Material.ModelShader.Activate();
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);
			component.renderHeads.Model->draw(component.systems.material.Material.ModelShader);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

		}
		component.systems.material.Material.ModelGpassShader.Activate();
		GeometryPass::gPassDraw(component.renderHeads.Model, component.systems.material.Material.ModelGpassShader);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Enable wireframe mode

		//glFrontFace(GL_CCW);
		glCullFace(GL_BACK); // Reset culling to default
		glDisable(GL_CULL_FACE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		break;
	case 'b': // billboard
		component.renderHeads.BillBoard->updatePosition(component.systems.transformation.position);
		component.renderHeads.BillBoard->updateScale(component.systems.transformation.scale);
		component.renderHeads.BillBoard->draw();
		break;
	}
}

void entity::drawShadowMap()
{
	if (!component.flags.castsShadow)
	{
		return;
	}
	switch (type)
	{
	case 'm': // model
		LightingHandler::drawShadowMap(component.renderHeads.Model, component.systems.transformation.position, component.systems.transformation.rotation, component.systems.transformation.scale);
		break;
	}
}
// getters
std::string entity::fetchName()
{
	return name;
}
char entity::fetchType()
{
	return type;
}
std::string entity::fetchPath()
{
	return path;
}

// fetch model matrix,
// fetch model data

// setters
void entity::setName(const std::string& name)
{
	entity::name = name;
}


void entity::createModel(const std::string& path, const std::string& materialPath)
{
	component.systems.material.Material.LoadMaterial(materialPath);
	component.renderHeads.Model = new Model(path.c_str());
}

void entity::createBillBoard(const std::string& path)
{
	component.renderHeads.BillBoard = new BillBoard(path);
}
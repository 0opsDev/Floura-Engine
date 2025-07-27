#include "FE_Object.h"
#include <Core/scene.h>
#include <Math/FE_math.h>

void FE_Object::Create(const std::string& type, const std::string& name, const std::string& path, const std::string& Material)
{
	// set type
	FE_Object::type = type;
	// set name 
	FE_Object::name = name;
	// set path 
	FE_Object::path = path;

	if (FE_Object::type == "model") {
		// should remove name, its not needed, and LOD should become bool and a flag set before creating
		ModelObject.IsLod = flag_ModelIsLOD;
		ModelObject.CreateObject(path, name, Material);
	}
	else if (FE_Object::type == "billboard") {
		BillBoardObject.flag_isanimated = flag_BillBoardAnimated;
		BillBoardObject.CreateObject(path, name);

	}

}

void FE_Object::Update()
{

}

void FE_Object::UpdateLights() 
{
	
		if (FE_Object::type == "model")
		{
			ModelObject.updateForwardLights(Scene::colour, Scene::position, Scene::radiusAndPower, Scene::lightType, Scene::enabled);
		}


}

void FE_Object::Draw()
{
	// come up with a faster check in the future
	if (FE_Object::type == "model")
	{
		ModelObject.draw();
	}
	else if (FE_Object::type == "billboard")
	{
		BillBoardObject.draw();
	}
}
// getters
std::string FE_Object::fetchName()
{
	return name;
}
std::string FE_Object::fetchType()
{
	return type;
}
std::string FE_Object::fetchPath()
{
	return path;
}
// fetch model matrix,
// fetch model data

// setters
void FE_Object::setName(const std::string& name)
{
	FE_Object::name = name;
}

void FE_Object::setFlagIsLOD(bool flag)
{
	flag_ModelIsLOD = flag;
}

void FE_Object::setFlagIsAnimated(bool flag)
{
	flag_BillBoardAnimated = flag;
}
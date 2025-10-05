#include "FE_Object.h"
#include <Core/scene.h>
#include <Math/FE_math.h>

void FE_Object::create(const std::string& type, const std::string& name, const std::string& path, const std::string& Material)
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

void FE_Object::update()
{
	if (FE_Object::type == "model")
	{
		// Update transformation
		ModelObject.transform = FE_Object::gPosition;
		ModelObject.scale = FE_Object::gScale;
		ModelObject.rotation = FE_Object::gRotation;

	}
	else if (FE_Object::type == "billboard")
	{
		BillBoardObject.UpdateCollider();
		BillBoardObject.UpdateCameraCollider();
	}
}

void FE_Object::updateLights() 
{
		if (FE_Object::type == "model")
		{
			ModelObject.updateForwardLights();
		}
}

void FE_Object::Delete()
{
	if (FE_Object::type == "model")
	{
		ModelObject.Delete();
	}
	else if (FE_Object::type == "billboard")
	{
		BillBoardObject.Delete();
	}
}

void FE_Object::draw()
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

bool FE_Object::fetchDoFrustumCull()
{
	if (FE_Object::type == "model")
	{
		return ModelObject.DoFrustumCull;
	}
	else if (FE_Object::type == "billboard")
	{
		return BillBoardObject.DoFrustumCull;
	}

}

bool FE_Object::fetchIsCollider()
{
	if (FE_Object::type == "model")
	{
		return ModelObject.isCollider;
	}
	else if (FE_Object::type == "billboard")
	{
		return BillBoardObject.isCollider;
	}
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

void FE_Object::setDoFrustumCull(bool flag)
{
	if (FE_Object::type == "model")
	{
		ModelObject.DoFrustumCull = flag;
	}
	else if (FE_Object::type == "billboard")
	{
		BillBoardObject.DoFrustumCull = flag;
	}
}

void FE_Object::setIsCollider(bool flag)
{
	if (FE_Object::type == "model")
	{
		ModelObject.isCollider = flag;
	}
	else if (FE_Object::type == "billboard")
	{
		BillBoardObject.isCollider = flag;
	}
}
#ifndef ECS_INSPECTOR_CLASS_H
#define ECS_INSPECTOR_CLASS_H

class EcsInspector
{
public:

	static char NameBuffer[256];
	
	static void InspectorWindow();

	static void ModelWindow();

	static void BillBoardWindow();

	static void SoundWindow();

	static void LightWindow();

	static void SkyBoxWindow();

	static void CameraWindow();
	
	static void EmptyWindow();
	
	static void MaterialOnHit();
	
	static void MaterialWindow();
	
private:

	static void entityScriptPane();
	
	static void entityPhysicsPane();
	
	static void entityTransformPane();
	
	static void entityRelationsPane();
	
};

#endif 
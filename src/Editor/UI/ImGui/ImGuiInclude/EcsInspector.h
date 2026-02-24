#ifndef ECS_INSPECTOR_CLASS_H
#define ECS_INSPECTOR_CLASS_H

class EcsInspector
{
public:

	static void InspectorWindow();

	static void ModelWindow();

	static void BillBoardWindow();

	static void SoundWindow();

	static void LightWindow();

	static void SkyBoxWindow();

	static void CameraWindow();
	
	static void EmptyWindow();
	
	static void VolumeWindow();
	
private:

	static void entityScriptPane();
	
	static void entityPhysicsPane();
	
	static void entityTransformPane();
	
	static void entityRelationsPane();
	
};

#endif 
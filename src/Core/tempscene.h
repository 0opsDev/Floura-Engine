#ifndef TEMP_SCENE_CLASS_H	
#define TEMP_SCENE_CLASS_H

#include <iostream>
#include "Gameplay/ModelObject.h"
#include "Gameplay/BillboardObject.h"

//testclass
class TempScene
{
public:
	// might be nice to not have these static, so you can swap scenes, load then create them whatever with other ones loaded
	static void init();

	static void Update();

	static void Delete();
private:
};
#endif
#ifndef TEMP_SCENE_CLASS_H	
#define TEMP_SCENE_CLASS_H

#include <iostream>
//testclass
class FE_LAYER // FE_LAYER is a good rename
{
public:
	// might be nice to not have these static, so you can swap scenes, load then create them whatever with other ones loaded
	static void init();

	static void Update();
	
	static void onBeginningOfFrame(); 
	
	static void draw();

	static void Delete();
private:
};
#endif
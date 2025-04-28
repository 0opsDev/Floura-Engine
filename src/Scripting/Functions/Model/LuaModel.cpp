#include "LuaModel.h"

std::vector<float> Modelx2, Modely2, Modelz2, RotW2, RotX2, RotY2, RotZ2, ScaleX2, ScaleY2, ScaleZ2;
std::vector<std::string> modelName2, Path2;
std::vector<bool>isCulling2;
std::unordered_set<std::string> existingNames; // Track model names to avoid duplicates

void luaModel::DrawModel(sol::state& luaState) {
	luaState["ModelDraw"] = [](std::string Path, std::string modelName, bool isCulling, float Modelx, float Modely, float Modelz, float RotW, float RotX, float RotY, float RotZ, float ScaleX, float ScaleY, float ScaleZ) {
		//std::cout << "VALID" << std::endl;
		if (existingNames.find(modelName) == existingNames.end()) { // Only add if it's not a duplicate
			Modelx2.push_back(Modelx);
			Modely2.push_back(Modely);
			Modelz2.push_back(Modelz);
			RotW2.push_back(RotW);
			RotX2.push_back(RotX);
			RotY2.push_back(RotY);
			RotZ2.push_back(RotZ);
			ScaleX2.push_back(ScaleX);
			ScaleY2.push_back(ScaleY);
			ScaleZ2.push_back(ScaleZ);
			modelName2.push_back(modelName);
			Path2.push_back(Path);
			isCulling2.push_back(isCulling);
			existingNames.insert(modelName); // Mark the modelName as added
		}
		Main::updateModelLua(Path2, modelName2, isCulling2, Modelx2, Modely2, Modelz2, RotW2, RotX2, RotY2, RotZ2, ScaleX2, ScaleY2, ScaleZ2);
		return 0;
		}; // needs to be made so it doesnt clear when re ran
}
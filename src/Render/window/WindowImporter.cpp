#include "windowImporter.h"
#include "Scene/scene.h"
#include "utils/FE_math.h"
 #include <algorithm>

void windowImporter::importCall(std::string path)
{
 	std::replace(path.begin(), path.end(), '\\', '/');

    // i need to remove everything before the root dir
    	
    //ends_with()
    if (path.ends_with(".gltf") || path.ends_with(".glb")
        || path.ends_with(".fbx") || path.ends_with(".dae")
        || path.ends_with(".obj") || path.ends_with(".3ds")
        || path.ends_with(".stl") || path.ends_with(".ply")
        || path.ends_with(".abc") || path.ends_with(".blend"))
    {
        if (Scene::spawnNearCamera) Scene::AddEntityObject(entity::ENT_MODEL_TYPE, "drop_imported", path, 
    Scene::maincamera.Position - ( FE_Math::getForwardFromViewMatrix(Scene::maincamera.cameraMatrix * 5.0f) ) , glm::vec3(1.0f), glm::vec3(0.0f) );
        else Scene::AddEntityObject(entity::ENT_MODEL_TYPE, "drop_imported", path, glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f));
    }
}

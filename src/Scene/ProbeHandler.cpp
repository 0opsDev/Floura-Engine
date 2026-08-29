#include "ProbeHandler.h"
#include "Systems/util/UUID.h"

GLuint ProbeHandler::probesSSBO;
std::vector<ProbeHandler::probeVolume> ProbeHandler::probeVolumes;
bool ProbeHandler::dirtyScene = false;
int ProbeHandler::indirectSamples = 1;

void ProbeHandler::init(){
    glGenBuffers(1, &probesSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, probesSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 1024, NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 12, probesSSBO); 
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void ProbeHandler::cleanup(){
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    glDeleteBuffers(1, &probesSSBO);
}

uint64_t ProbeHandler::createProbe(glm::vec3 p, glm::vec3 s, int w, int h, int d){
    uint64_t nUUID = UUID::returnHandle();
    
    probeVolume nVolume;
    nVolume.uuid = nUUID;
    nVolume.position = p;
    nVolume.scale = s;
    
    probeVolumes.push_back(nVolume);
    probeVolumes.back().indirectVolume = new Texture3D();
    probeVolumes.back().emissionVolume = new Texture3D();
    probeVolumes.back().indirectVolume->createImage3D(w,h,d, "probe", 1, GL_RGBA16F);
    probeVolumes.back().emissionVolume->createImage3D(w,h,d, "probe", 1, GL_RGBA16F);
    
    return nUUID;
}

void ProbeHandler::deleteProbe(uint64_t UUID){
    for (int i = 0; i < probeVolumes.size(); ++i){
        if (UUID == probeVolumes[i].uuid){
            probeVolumes[i].indirectVolume->Delete();
            probeVolumes[i].emissionVolume->Delete();
            probeVolumes.erase(probeVolumes.begin() + i);
            break;
        }
    }
}

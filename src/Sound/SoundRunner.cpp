#include "SoundRunner.h"
#include "SoundProgram.h"

float SoundRunner::GlobalVolume = 1;

float SoundRunner::MusicVolume = 1;

float SoundRunner::environmentVolume = 1;

float SoundRunner::entityVolume = 1;

void SoundRunner::init() {
    // Open default device and create context
    SoundProgram::device = alcOpenDevice(nullptr);
    if (!SoundProgram::device) {
        std::cerr << "Failed to open OpenAL device" << std::endl;
        //return -1;
    }
    SoundProgram::context = alcCreateContext(SoundProgram::device, nullptr);
    alcMakeContextCurrent(SoundProgram::context);

}

void SoundRunner::Delete() {
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(SoundProgram::context);
    alcCloseDevice(SoundProgram::device);
}
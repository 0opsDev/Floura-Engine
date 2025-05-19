#include "SoundProgram.h"
#include <utils/init.h>


ALCdevice* SoundProgram::device;
ALCcontext* SoundProgram::context;

void SoundProgram::PlaySound(float Volume) {
    isPlay = true;
    alSourcef(source, AL_GAIN, Volume); // set volume
    std::thread([this]() {
        alSourcePlay(source);
        if (init::LogSound) std::cout << "Sound played" << std::endl;
        do {
            alGetSourcei(source, AL_SOURCE_STATE, &state);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Sleep to avoid busy-waiting
        } while (state == AL_PLAYING && Main::sleepState);
        if(init::LogSound) std::cout << "Sound stopped" << std::endl;
        isPlay = false;
        }).detach();  // Detach the thread so it runs independently
}

void SoundProgram::SetVolume(float Volume) {
    alSourcef(source, AL_GAIN, Volume); // set volume
}

void SoundProgram::SetSoundPosition(float x, float y, float z) {
    alSource3f(source, AL_POSITION, x, y, z);
}

void SoundProgram::SetListenerPosition(float x, float y, float z) {
    alListener3f(AL_POSITION, x, y, z);
    alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
    ALfloat orientation[] = { 0.0f, 0.0f, -1.0f,  // Forward
                              0.0f, 1.0f, 0.0f }; // Up
    alListenerfv(AL_ORIENTATION, orientation);
}

void SoundProgram::updateCameraPosition() {
    // Update listener's position
    SetListenerPosition(Camera::Position.x, Camera::Position.y, Camera::Position.z);

    // Use Camera orientation and up vector for 3D audio orientation
    ALfloat orientation[] = {
        Camera::PubOrientation.x, Camera::PubOrientation.y, Camera::PubOrientation.z,  // Forward
        Camera::PubUp.x,         Camera::PubUp.y,         Camera::PubUp.z              // Up
    };
    alListenerfv(AL_ORIENTATION, orientation);
}


void SoundProgram::StopSound() {
    if (source && isPlay) {
        alSourceStop(source);
        if (init::LogSound) std::cout << "StopSound: Sound stopped" << std::endl;
    }
    else if (!isPlay) {
        if (init::LogSound) std::cout << "StopSound: No Sound Playing" << std::endl;
    }
}


void SoundProgram::CreateSound(std::string Path) {
    //Load Sound, Generate source and attach buffer
    ChangeSound(Path);
}

void SoundProgram::ChangeSound(std::string path) {
    // Delete previous source and buffer if they exist
    if (source) {
        alDeleteSources(1, &source);
        source = 0;
    }
    if (buffer) {
        alDeleteBuffers(1, &buffer);
        buffer = 0;
    }

    // Load buffer
    loadWavFile(path, &buffer);
    alGenSources(1, &source);
    alSourcei(source, AL_BUFFER, buffer);
}

void SoundProgram::DeleteSound() {
    StopSound();
    isPlay = false;
    state = 0;
    alDeleteSources(1, &source);
    alDeleteBuffers(1, &buffer);
}

// Helper function to load WAV file
bool SoundProgram::loadWavFile(const std::string& filename, ALuint* buffer) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open WAV file: " << filename << std::endl;
        return false;
    }
    else {
        if (init::LogSound) std::cout << "Loaded WAV file: " << filename << std::endl;
    }

    char chunkId[4];
    file.read(chunkId, 4); // "RIFF"
    file.seekg(22);

    short channels;
    file.read(reinterpret_cast<char*>(&channels), 2);

    if (channels > 1) {
        if (init::LogSound) std::cout << "Directional sound only works with mono WAV files." << std::endl;
    }

    int sampleRate;
    file.read(reinterpret_cast<char*>(&sampleRate), 4);
    file.seekg(34);

    short bitsPerSample;
    file.read(reinterpret_cast<char*>(&bitsPerSample), 2);

    file.seekg(40);
    int dataSize;
    file.read(reinterpret_cast<char*>(&dataSize), 4);

    std::vector<char> data(dataSize);
    file.read(data.data(), dataSize);

    ALenum format;
    if (channels == 1) {
        format = (bitsPerSample == 8) ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16;
    }
    else {
        format = (bitsPerSample == 8) ? AL_FORMAT_STEREO8 : AL_FORMAT_STEREO16;
    }

    alGenBuffers(1, buffer);
    alBufferData(*buffer, format, data.data(), dataSize, sampleRate);

    return true;
}
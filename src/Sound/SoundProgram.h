#ifndef SOUND_CLASS_H
#define SOUND_CLASS_H

#include <AL/al.h>
#include <AL/alc.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include "camera/Camera.h"

class SoundProgram
{
public:

	static ALCdevice* device;
	static ALCcontext* context;

	glm::vec3 SoundPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	float currentvolume;

	bool isPlay = false;

	void PlaySound();

	void SetPitch(float pitch);

	void SetVolume(float Volume);

	void Set3D(bool is3D);
	
	void SetSoundPosition(float x, float y, float z);

	void SetListenerPosition(float x, float y, float z);

	void updateCameraPosition();

	void StopSound();

	void CreateSound(std::string Path);

	void ChangeSound(std::string path);

	void DeleteSound();

private:

	ALuint buffer;
	ALuint source;
	ALint state;

	bool loadWavFile(const std::string& filename, ALuint* buffer);

};

#endif //SOUND_CLASS_H
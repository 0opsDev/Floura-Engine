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
	
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
	std::string name;
	std::string path;
	float currentvolume = 1.0f;
	float pitch = 1.0f;

	bool queuedPlay = false; // is queued to play
	bool isPlay = false; // is playing
	bool loop = false; // is looping
	bool is3D = true; // is 3D sound

	void PlaySound();

	void SetPitch(float pitch);

	void SetVolume(float Volume);

	void Set3D(bool is3D);
	
	void SetSoundPosition(glm::vec3 position);

	void SetListenerPosition(glm::vec3 position);

	void updateCameraPosition();

	void StopSound();

	void CreateSound(std::string Path, std::string name);

	void ChangeSound(std::string path);

	void DeleteSound();

private:

	ALuint buffer;
	ALuint source;
	ALint state;

	bool loadWavFile(const std::string& filename, ALuint* buffer);

};

#endif //SOUND_CLASS_H
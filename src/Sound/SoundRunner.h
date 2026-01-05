#ifndef SOUND_RUNNER_CLASS_H
#define SOUND_RUNNER_CLASS_H

#include <AL/al.h>
#include <AL/alc.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include "camera/Camera.h"

class SoundRunner
{
public:

	static float GlobalVolume;

	static float MusicVolume;

	static float environmentVolume;

	static float entityVolume;

	static void init();

	static void Delete();

private:

};

#endif //SOUND_RUNNER_CLASS_H
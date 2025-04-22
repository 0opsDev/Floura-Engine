#include "Core/Render/RenderLoop.h"
#include"Render/Model/Model.h"
#include "Render/Camera.h"
#include "Main.h"
#include "Systems/utils/UF.h"
#include "Systems/utils/Init.h"
#include "Systems/utils/screenutils.h" 
#include <glm/gtx/string_cast.hpp>
#include "Systems/utils/timeUtil.h" 
#include "Systems/utils/inputUtil.h"
#include <thread>
#include <chrono>
#include <Systems/utils/SettingsUtil.h>
#include "Systems/utils/ScriptEngine.h"
//temorary
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <OpenAL/efx.h>
#include <OpenAL/efx-presets.h>
#define STB_PERLIN_IMPLEMENTATION
#include <stb/stb_perlin.h>
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#pragma comment(lib, "lua54.lib")

void RenderLoop::Loop() {
}
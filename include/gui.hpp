#pragma once
#include <cstdio>
#include <sstream>
#include <string>
#include <iomanip>
#include <bitset>

#include <SDL2/SDL.h>
#include <gl3w.h>

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include "cpu.hpp"

namespace GBEMU::GUI {

    bool InitInterface();
    void ImGuiFrameRender(GBEMU::Cpu* cpu, uint8_t* memory);
    bool ShouldDestroy(SDL_Event event);
    void DestroyInterface();

}

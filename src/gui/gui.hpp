#pragma once
#include <cstdio>
#include <sstream>
#include <string>
#include <iomanip>
#include <bitset>
#include <memory>

#include <SDL2/SDL.h>
#include <gl3w.h>

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include "../cpu/cpu.hpp"

namespace Gui {

    bool InitInterface();
    void ImGuiFrameRender(Cpu::Cpu* const cpu, uint8_t* const memory, std::shared_ptr<Cpu::Operations::Instruction> instruction);
    bool ShouldDestroy(SDL_Event event);
    void DestroyInterface();

}

#pragma once
#include <cstdio>

#include <SDL2/SDL.h>
#include <gl3w.h>

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

namespace GBEMU::GUI {

    bool InitInterface();
    void ImGuiFrameRender();
    bool ShouldDestroy(SDL_Event event);
    void DestroyInterface();

}

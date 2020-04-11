#include "emu.hpp"

namespace GBEMU {

    Emu::Emu() {
        GUI::InitInterface();
    }

    int Emu::play() {
        int out = 0;

        bool done = false;
        while (!done) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                ImGui_ImplSDL2_ProcessEvent(&event);
                if (GUI::ShouldDestroy(event))
                    done = true;
            }

            GUI::ImGuiFrameRender();
        }

        GUI::DestroyInterface();

        return out;
    }

}

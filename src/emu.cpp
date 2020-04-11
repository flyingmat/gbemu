#include "emu.hpp"

namespace GBEMU {

    Emu::Emu() {
        this->memory = new uint8_t [4096];  // arbitrary size
        this->cpu = new Cpu(this->memory);
    }

    Emu::~Emu() {
        delete[] this->memory;
        delete this->cpu;
        this->memory = nullptr;
        this->cpu = nullptr;
    }

    int Emu::play() {
        int out = 0;

        bool done = false;
        while (!done) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                ImGui_ImplSDL2_ProcessEvent(&event);
                done = GUI::ShouldDestroy(event);
            }

            GUI::ImGuiFrameRender();
        }

        GUI::DestroyInterface();

        return out;
    }

}

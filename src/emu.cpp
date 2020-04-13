#include "emu.hpp"

namespace GBEMU {

    Emu::Emu() {
        this->memory = new uint8_t [32768];  // arbitrary size

        this->cpu = new Cpu(this->memory);

        // testing
        for (uint16_t i = 0; i < 0xFF; i++)
            this->memory[i] = i+1;
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

                if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_n)
                    this->cpu->cycle();
            }

            GUI::ImGuiFrameRender(this->cpu, this->memory);
        }

        GUI::DestroyInterface();

        return out;
    }

}

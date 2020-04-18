#include "emu.hpp"

Emu::Emu() {
    this->memory = new uint8_t [32768];  // arbitrary size

    this->cpu = std::make_unique<Cpu::Cpu>(&this->memory);

    // testing
    memory[0] = 0x18;
    memory[1] = 0xFF;
}

Emu::~Emu() {
    delete[] this->memory;
    this->memory = nullptr;
    this->cpu = nullptr;
}

int Emu::Play() {
    int error = 0;

    bool done = false;
    std::shared_ptr<Cpu::Operations::Instruction> instruction = nullptr;
    while (!done) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            done = Gui::ShouldDestroy(event);

            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_n)
                instruction = this->cpu->Cycle();
        }

        Gui::ImGuiFrameRender(this->cpu.get(), this->memory, instruction);
    }

    Gui::DestroyInterface();

    return error;
}

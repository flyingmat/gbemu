#include "emu.hpp"

Emu::Emu() {
    this->memory = new uint8_t [32768];  // arbitrary size

    this->cpu = std::make_unique<Cpu::Cpu>(&this->memory);

    // testing
    this->memory[0] = 0x0C;
    this->memory[1] = 0x41;

    for (size_t i = 0; i < 10; i++)
        printf("%02x\n", this->memory[i]);
}

Emu::~Emu() {
    delete[] this->memory;
    this->memory = nullptr;
    this->cpu = nullptr;
}

int Emu::Play() {
    int error = 0;

    bool done = false;
    std::shared_ptr<Cpu::Operations::Operation> operation = nullptr;
    while (!done) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            done = Gui::ShouldDestroy(event);

            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_n)
                operation = this->cpu->Cycle();
        }

        Gui::ImGuiFrameRender(this->cpu.get(), this->memory, operation);
    }

    Gui::DestroyInterface();

    return error;
}

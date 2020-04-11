#pragma once
#include <SDL2/SDL.h>
#include "gui.hpp"
#include "cpu.hpp"

namespace GBEMU {

    class Emu {
    private:
        uint8_t* memory;
        Cpu* cpu;
    public:
        Emu();
        ~Emu();
        int play();
    };

}

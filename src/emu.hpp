#pragma once
#include <cstdint>
#include <cstdio>
#include <memory>
#include <SDL2/SDL.h>
#include "gui/gui.hpp"
#include "cpu/cpu.hpp"

class Emu {
private:
    uint8_t* memory;
    std::unique_ptr<Cpu::Cpu> cpu = nullptr;
public:
    Emu();
    ~Emu();
    int Play();
};

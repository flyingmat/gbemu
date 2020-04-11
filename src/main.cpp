#include <cstdio>
#include <cstdint>

#include "emu.hpp"
#include "gui.hpp"
#include "cpu.hpp"

int main() {
    GBEMU::GUI::InitInterface();
    GBEMU::Emu emulator = GBEMU::Emu();
    emulator.play();
    return 0;
}

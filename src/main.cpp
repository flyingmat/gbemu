#include <cstdio>
#include <cstdint>
#include <memory>

#include "emu.hpp"

int main() {
    Gui::InitInterface();
    std::unique_ptr<Emu> emulator = std::make_unique<Emu>();
    emulator->Play();
    return 0;
}

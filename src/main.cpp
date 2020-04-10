#include <cstdio>
#include <cstdint>
#include "cpu.hpp"

int main() {
    uint8_t* memory = new uint8_t [16];
    GBEMU::Cpu cpu = GBEMU::Cpu(memory);
    return 0;
}

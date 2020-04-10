#include "cpu.hpp"

int main() {
    unsigned char* memory = new unsigned char [16];
    GBEMU::Cpu cpu = GBEMU::Cpu(memory);
    return 0;
}

#pragma once
#include <cstdint>

namespace Cpu {
    class Cpu;
}

namespace Cpu::Helpers {
    /// Concatenates two bytes to form a 16-bit unsigned integer.
    uint16_t JoinBytes(uint8_t upper_byte, uint8_t lower_byte);

    /// Returns a pointer to the byte stored in memory at address HL.
    uint8_t* DereferenceHL(Cpu* const cpu);

    uint8_t GetArgsNumber(uint8_t opcode);
}

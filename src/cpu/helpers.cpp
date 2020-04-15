#include "helpers.hpp"
#include "cpu.hpp"

namespace GB_Cpu::Helpers {
    uint16_t JoinBytes(uint8_t upper_byte, uint8_t lower_byte) {
        return (uint16_t) ((upper_byte << 8) | lower_byte);
    }

    uint8_t* DereferenceHL(Cpu* const cpu) {
        return *cpu->memory + JoinBytes(cpu->H, cpu->L);
    }

    uint8_t GetArgsNumber(uint8_t opcode) {
        return 0;
    }
}

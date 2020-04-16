#include "helpers.hpp"
#include "cpu.hpp"

namespace Cpu::Helpers {
    uint16_t JoinBytes(uint8_t upper_byte, uint8_t lower_byte) {
        return (uint16_t) ((upper_byte << 8) | lower_byte);
    }

    uint8_t* DereferenceHL(Cpu* const cpu) {
        return *cpu->memory + JoinBytes(cpu->H, cpu->L);
    }

    uint8_t GetArgsNumber(uint8_t opcode) {
        if (opcode < 0x40) {
            if ((opcode & 0x0F) % 8 == 0x06)
                return 1;
            else
                return 0;
        } else if (opcode < 0x80) {
            return 0;
        }

        return 0;
    }
}

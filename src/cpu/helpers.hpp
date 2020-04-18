#pragma once
#include <cstdint>

namespace Cpu {
    class Cpu;
}

namespace Cpu::Helpers {
    
    enum PostOperation : uint8_t {
        Increase,
        Decrease,
        None
    };

    /// Concatenates two bytes to form a 16-bit unsigned integer.
    uint16_t JoinBytes(const uint8_t upper_byte, const uint8_t lower_byte);

    uint8_t* DereferenceDoubleByte(Cpu* const cpu, uint8_t& upper_byte, uint8_t& lower_byte, PostOperation post_op);
    uint8_t* DereferenceBC(Cpu* const cpu);
    uint8_t* DereferenceDE(Cpu* const cpu);
    /// Returns a pointer to the byte stored in memory at address HL.
    uint8_t* DereferenceHL(Cpu* const cpu, PostOperation post_op);

    uint8_t GetArgsNumber(uint8_t opcode);
}

#pragma once
#include <cstdint>
#include <memory>
#include "parser.hpp"
#include "helpers.hpp"
#include "operations.hpp"

namespace Cpu {
    enum class Flag : uint8_t {
        z = 0x80,
        n = 0x40,
        h = 0x20,
        c = 0x10
    };

    class Cpu {
    public:
        // temporarily public for testing purposes
        uint8_t A, F;
        uint8_t B, C;
        uint8_t D, E;
        uint8_t H, L;
        uint8_t S, P;
        uint16_t PC;
        uint8_t** const memory;
        std::unique_ptr<Parser> parser;

        Cpu(uint8_t** const memory);
        bool GetFlag(const Flag flag);
        void SetFlag(const Flag flag, const bool flag_value);
        std::shared_ptr<Operations::Instruction> Cycle();
    };
}

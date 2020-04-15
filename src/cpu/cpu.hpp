#pragma once
#include <cstdint>
#include <memory>
#include "parser.hpp"
#include "helpers.hpp"
#include "operations.hpp"

namespace GB_Cpu {
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
        uint8_t SP;
        uint8_t PC;
        uint8_t** const memory;
        std::unique_ptr<Parser> parser;

        Cpu(uint8_t** const memory);
        void SetFlag(Flag flag, bool flag_value);
        void Cycle();
    };
}

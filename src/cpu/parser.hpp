#pragma once
#include <cstdint>
#include <memory>
#include "helpers.hpp"
#include "operations.hpp"

namespace GB_Cpu {
    class Cpu;

    class Parser {
    public:
        Cpu* const cpu;

        Parser(Cpu* const cpu);
        uint8_t* ChooseOperandByte(const uint8_t index);
        std::unique_ptr<Operations::Operation> Parse(const uint8_t opcode, uint8_t* const args);
    };
}

#pragma once
#include <cstdint>
#include <memory>
#include "helpers.hpp"
#include "operations.hpp"

namespace Cpu {
    class Cpu;

    class Parser {
    public:
        Cpu* const cpu;

        Parser(Cpu* const cpu);
        uint8_t* ChooseOperandByte(const uint8_t index);
        uint8_t* ChooseOperandDoubleByte(const uint8_t index);
        uint8_t* ChooseDereference(const uint8_t index);
        std::shared_ptr<Operations::Operation> ChooseAluOperation(const uint8_t index, const uint8_t operand);
        Flag ChooseFlag(const uint8_t index);
        std::shared_ptr<Operations::Instruction> Parse(const uint8_t opcode, std::shared_ptr<uint8_t[]> args);
    };
}

#include "parser.hpp"
#include "cpu.hpp"

namespace GB_Cpu {
    Parser::Parser(Cpu* const cpu) : cpu(cpu) {}

    uint8_t* Parser::ChooseOperandByte(const uint8_t index) {
        switch (index) {
            case 0: return &this->cpu->B;
            case 1: return &this->cpu->C;
            case 2: return &this->cpu->D;
            case 3: return &this->cpu->E;
            case 4: return &this->cpu->H;
            case 5: return &this->cpu->L;
            case 6: return Helpers::DereferenceHL(cpu);
            case 7: return &this->cpu->A;
            default: return nullptr;
        }
    }

    std::unique_ptr<Operations::Operation> Parser::Parse(const uint8_t opcode, uint8_t* const args) {
        uint8_t upper_hb = (opcode & 0xF0) >> 8;
        uint8_t lower_hb = opcode & 0x0F;
        if (upper_hb < 0x04) {
            switch (lower_hb % 8) {
                case 0x04:
                    return std::make_unique<Operations::IncreaseByte>(
                        this->cpu, opcode, *this->ChooseOperandByte((opcode - 4) >> 3));
                case 0x05:
                    return std::make_unique<Operations::DecreaseByte>(
                        this->cpu, opcode, *this->ChooseOperandByte((opcode - 5) >> 3));
                case 0x07:
                    if (upper_hb < 0x01)
                        return std::make_unique<Operations::RotateByte>(
                            this->cpu, opcode, this->cpu->A, static_cast<Operations::ShiftDirection>((opcode - 7) % 7 % 2), 1);
            }
        }

        return nullptr;
    }
}

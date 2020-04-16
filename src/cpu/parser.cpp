#include "parser.hpp"
#include "cpu.hpp"

namespace Cpu {
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

    std::shared_ptr<Operations::Operation> Parser::Parse(const uint8_t opcode, uint8_t* const args) {
        uint8_t upper_hb = opcode >> 4;
        uint8_t lower_hb = opcode & 0x0F;
        printf("%02x %02x %02x\n", opcode, upper_hb, lower_hb);
        if (upper_hb < 0x04) {
            printf("here\n");
            switch (lower_hb % 8) {
                case 0x04:
                    return std::make_shared<Operations::IncreaseByte>(
                        this->cpu, opcode, *this->ChooseOperandByte((opcode - 4) >> 3));
                case 0x05:
                    return std::make_shared<Operations::DecreaseByte>(
                        this->cpu, opcode, *this->ChooseOperandByte((opcode - 5) >> 3));
                case 0x06:
                    return std::make_shared<Operations::LoadByte>(
                        this->cpu, opcode, *this->ChooseOperandByte((opcode - 6) >> 3), args[0], 1);
                case 0x07:
                    if (upper_hb < 0x01)
                        return std::make_shared<Operations::RotateByte>(
                            this->cpu, opcode, this->cpu->A, static_cast<Operations::ShiftDirection>((opcode - 7) % 7 % 2), 1);
            }
        } else if (upper_hb < 0x08 && opcode != 0x76) {
            return std::make_shared<Operations::LoadByte>(
                this->cpu, opcode, *this->ChooseOperandByte(((opcode - (lower_hb % 8)) >> 3) % 8), *this->ChooseOperandByte(lower_hb % 8), 0);
        } else if (opcode == 0x76) {
            // HALT: to be implemented
        } else if (upper_hb < 0x0C) {
            // bitwise ops
        }

        return nullptr;
    }
}

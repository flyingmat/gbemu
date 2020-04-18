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
            case 6: return Helpers::DereferenceHL(this->cpu, Helpers::PostOperation::None);
            case 7: return &this->cpu->A;
            default: return nullptr;
        }
    }

    uint8_t* Parser::ChooseOperandDoubleByte(const uint8_t index) {
        switch (index) {
            case 0: return &this->cpu->B;
            case 1: return &this->cpu->C;
            case 2: return &this->cpu->D;
            case 3: return &this->cpu->E;
            case 4: return &this->cpu->H;
            case 5: return &this->cpu->L;
            case 6: return &this->cpu->S;
            case 7: return &this->cpu->P;
            default: return nullptr;
        }
    }

    uint8_t* Parser::ChooseDereference(const uint8_t index) {
        switch (index) {
            case 0: return Helpers::DereferenceBC(this->cpu);
            case 1: return Helpers::DereferenceDE(this->cpu);
            case 2: return Helpers::DereferenceHL(this->cpu, Helpers::PostOperation::Increase);
            case 3: return Helpers::DereferenceHL(this->cpu, Helpers::PostOperation::Decrease);
        }
    }

    std::shared_ptr<Operations::Instruction> Parser::Parse(const uint8_t opcode, uint8_t* const args) {
        uint8_t upper_hb = opcode >> 4;
        uint8_t lower_hb = opcode & 0x0F;
        printf("%02x %01x %01x\n", opcode, upper_hb, lower_hb);

        if (upper_hb < 0x04) {
            switch (lower_hb) {
                // LD (rr),A
                case 0x02:
                    return std::make_shared<Operations::Instruction>(
                        std::make_shared<Operations::LoadByte>(
                            this->cpu,
                            *this->ChooseDereference(upper_hb),
                            this->cpu->A),
                        opcode, 1);
                // LD A,(rr)
                case 0x0A:
                    return std::make_shared<Operations::Instruction>(
                        std::make_shared<Operations::LoadByte>(
                            this->cpu,
                            this->cpu->A,
                            *this->ChooseDereference(upper_hb)),
                        opcode, 1);
                // INC rr
                case 0x03:
                    return std::make_shared<Operations::Instruction>(
                        std::make_shared<Operations::IncreaseDoubleByte>(
                            this->cpu,
                            *this->ChooseOperandDoubleByte(2 * upper_hb),
                            *this->ChooseOperandDoubleByte(2 * upper_hb + 1)),
                        opcode, 0);
                // DEC rr
                case 0x0B:
                    return std::make_shared<Operations::Instruction>(
                        std::make_shared<Operations::DecreaseDoubleByte>(
                            this->cpu,
                            *this->ChooseOperandDoubleByte(2 * upper_hb),
                            *this->ChooseOperandDoubleByte(2 * upper_hb + 1)),
                        opcode, 0);
            }
            switch (lower_hb % 8) {
                // INC r
                case 0x04:
                    return std::make_shared<Operations::Instruction>(
                        std::make_shared<Operations::IncreaseByte>(
                            this->cpu,
                            *this->ChooseOperandByte((opcode - 4) >> 3)),
                        opcode, 2 * (opcode == 0x34));
                // DEC r
                case 0x05:
                    return std::make_shared<Operations::Instruction>(
                        std::make_shared<Operations::DecreaseByte>(
                            this->cpu,
                            *this->ChooseOperandByte((opcode - 5) >> 3)),
                        opcode, 2 * (opcode == 0x35));
                // LD r,u8
                case 0x06:
                    return std::make_shared<Operations::Instruction>(
                        std::make_shared<Operations::LoadByte>(
                            this->cpu,
                            *this->ChooseOperandByte((opcode - 6) >> 3),
                            args[0]),
                        opcode, opcode == 0x36);
                case 0x07:
                    // RLCA / RRCA
                    if (upper_hb < 0x01)
                        return std::make_shared<Operations::Instruction>(
                            std::make_shared<Operations::RotateByte>(
                                this->cpu,
                                this->cpu->A,
                                static_cast<Operations::ShiftDirection>(lower_hb == 0x07),
                                1),
                            opcode, 0);
            }
        } else if (upper_hb < 0x08 && opcode != 0x76) {
            // LD r,r
            return std::make_shared<Operations::Instruction>(
                std::make_shared<Operations::LoadByte>(
                    this->cpu,
                    *this->ChooseOperandByte(((opcode - (lower_hb % 8)) >> 3) % 8),
                    *this->ChooseOperandByte(lower_hb % 8)),
                opcode, ((lower_hb % 8) == 6) || (upper_hb == 0x07 && lower_hb < 0x08));
        } else if (opcode == 0x76) {
            // HALT: to be implemented
        } else if (upper_hb < 0x0C) {
            // bitwise ops
        }

        return nullptr;
    }
}

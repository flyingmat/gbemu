#include "cpu.hpp"

namespace GBEMU {

    Cpu::Cpu(uint8_t*& memory) {
        this->A = 0;
        this->F = 0;
        this->B = 0;
        this->C = 0;
        this->D = 0;
        this->E = 0;
        this->H = 0;
        this->L = 0;
        this->SP = 0;
        this->PC = 0;
        this->memory = &memory;
    }

    Cpu::~Cpu() {
        this->memory = nullptr;
    }

    uint16_t Cpu::joinBytes(uint8_t r1, uint8_t r2) {
        return (r1 << 8) | r2;
    }

    uint16_t Cpu::readBC() { return this->joinBytes(this->B, this->C); }
    uint16_t Cpu::readDE() { return this->joinBytes(this->D, this->E); }
    uint16_t Cpu::readHL() { return this->joinBytes(this->H, this->L); }

    bool Cpu::getFlag(Flag flag) {
        return (this->F & (uint8_t) flag) != 0;
    }

    void Cpu::setFlag(Flag flag, bool value) {
        if (value)
            this->F |= (uint8_t) flag;
        else
            this->F &= ((uint8_t) flag ^ 0xFF);
    }

    void Cpu::setFlags(uint8_t flags) {
        this->F |= flags;
    }

    void Cpu::resetFlags(uint8_t flags) {
        this->F &= (flags ^ 0xFF);
    }

    void Cpu::adx_8b_SetFlags(uint8_t r, uint16_t real) {
        this->setFlag(Flag::zf, r == 0x00);
        this->setFlag(Flag::n, 0);
        this->setFlag(Flag::h, r == 0x10);
        this->setFlag(Flag::cy, real > 0xFF);
    }

    void Cpu::sub_8b_SetFlags(uint8_t r, int real) {
        this->setFlag(Flag::zf, r == 0x00);
        this->setFlag(Flag::n, 1);
        this->setFlag(Flag::h, r == 0x0F);
        this->setFlag(Flag::cy, real < 0);
    }

    void Cpu::inc_8b(uint8_t& r) {
        r++;
        this->setFlag(Flag::zf, r == 0x00);
        this->setFlag(Flag::n, 0);
        this->setFlag(Flag::h, r == 0x10);
    }

    void Cpu::dec_8b(uint8_t& r) {
        r--;
        this->setFlag(Flag::zf, r == 0xFF);
        this->setFlag(Flag::n, 1);
        this->setFlag(Flag::h, r == 0x0F);
    }

    void Cpu::rl_8b(uint8_t& r) {
        bool msb = r >> 7;
        r = ((r << 1) & 0xFF) | this->getFlag(Flag::cy);
        this->setFlag(Flag::cy, msb);
        this->resetFlags((uint8_t) Flag::zf | (uint8_t) Flag::n | (uint8_t) Flag::h);
    }

    void Cpu::rr_8b(uint8_t& r) {
        bool lsb = r & 0x01;
        r = (r >> 1) | (this->getFlag(Flag::cy) << 7);
        this->setFlag(Flag::cy, lsb);
        this->resetFlags((uint8_t) Flag::zf | (uint8_t) Flag::n | (uint8_t) Flag::h);
    }

    void Cpu::rlc_8b(uint8_t& r) {
        this->setFlag(Flag::cy, r >> 7);
        r = ((r << 1) & 0xFF) | (r >> 7);
        this->resetFlags((uint8_t) Flag::zf | (uint8_t) Flag::n | (uint8_t) Flag::h);
    }

    void Cpu::rrc_8b(uint8_t& r) {
        this->setFlag(Flag::cy, r & 0x01);
        r = (r >> 1) | ((r & 0x01) << 7);
        this->resetFlags((uint8_t) Flag::zf | (uint8_t) Flag::n | (uint8_t) Flag::h);
    }

    void Cpu::add_8b(uint8_t& r1, uint8_t r2) {
        uint16_t real = r1 + r2;
        r1 = real & 0x00FF;
        this->adx_8b_SetFlags(r1, real);
    }

    void Cpu::adc_8b(uint8_t& r1, uint8_t r2) {
        uint16_t real = r1 + r2 + this->getFlag(Flag::cy);
        r1 = real & 0x00FF;
        this->adx_8b_SetFlags(r1, real);
    }

    void Cpu::sub_8b(uint8_t& r1, uint8_t r2) {
        int real = r1 - r2;
        uint8_t result = r1 - r2;
        r1 = result;
        this->sub_8b_SetFlags(r1, real);
    }

    void Cpu::sbc_8b(uint8_t& r1, uint8_t r2) {
        int real = r1 - (r2 + this->getFlag(Flag::cy));
        uint8_t result = r1 - (uint8_t) (r2 + this->getFlag(Flag::cy));
        r1 = result;
        this->sub_8b_SetFlags(r1, real);
    }

    void Cpu::and_8b(uint8_t& r1, uint8_t r2) {
        r1 &= r2;
        this->setFlag(Flag::zf, r1 == 0x00);
        this->resetFlags((uint8_t) Flag::n | (uint8_t) Flag::cy);
        this->setFlag(Flag::h, 1);
    }

    void Cpu::xor_8b(uint8_t& r1, uint8_t r2) {
        r1 ^= r2;
        this->setFlag(Flag::zf, r1 == 0x00);
        this->resetFlags((uint8_t) Flag::n | (uint8_t) Flag::h | (uint8_t) Flag::cy);
    }

    void Cpu::or_8b(uint8_t& r1, uint8_t r2) {
        r1 |= r2;
        this->setFlag(Flag::zf, r1 == 0x00);
        this->resetFlags((uint8_t) Flag::n | (uint8_t) Flag::h | (uint8_t) Flag::cy);
    }

    void Cpu::cp_8b(uint8_t r1, uint8_t r2) {
        int real = r1 - r2;
        uint8_t result = r1 - r2;
        this->sub_8b_SetFlags(result, real);
    }

    void Cpu::inc_16b2(uint8_t& r1, uint8_t& r2) {
        r2++;
        if (!r2)
            r1++;
    }

    void Cpu::dec_16b2(uint8_t& r1, uint8_t& r2) {
        r2--;
        if (r2 == 0xFF)
            r1--;
    }

    void Cpu::mov_16bmem(uint16_t addr, uint16_t value) {
        (*this->memory)[addr] = value >> 8;
        (*this->memory)[addr+1] = value & 0x00FF;
    }

    void Cpu::add_16b2x2(uint8_t& r1, uint8_t& r2, uint8_t r3, uint8_t r4) {
        uint32_t real = this->joinBytes(r1, r2) + this->joinBytes(r3, r4);
        uint16_t result = real & 0xFFFF;
        r1 = result >> 8;
        r2 = result & 0x00FF;
        this->setFlag(Flag::n, 0);
        this->setFlag(Flag::h, result == 0x0100);
        this->setFlag(Flag::cy, result == (real > 0xFFFF));
    }

    uint8_t Cpu::jr_conditional(Flag flag, bool value, int8_t jvalue) {
        if (this->getFlag(flag) == value) {
            this->PC += jvalue;
            return 12;
        }
        return 8;
    }

    uint8_t Cpu::executeInstruction(bool cb, uint8_t opcode, uint8_t* args) {
        if (!cb) {
            switch (opcode) {
            // NOP
            case 0x00:
                return 4;
            // LD BC,u16
            case 0x01:
                this->B = args[0];
                this->C = args[1];
                return 12;
            // LD (BC),A
            case 0x02:
                (*this->memory)[this->readBC()] = this->A;
                return 8;
            // INC BC
            case 0x03:
                this->inc_16b2(this->B, this->C);
                return 8;
            // INC B
            case 0x04:
                this->inc_8b(this->B);
                return 4;
            // DEC B
            case 0x05:
                this->dec_8b(this->B);
                return 4;
            // LD B,u8
            case 0x06:
                this->B = args[0];
                return 8;
            // RLCA
            case 0x07:
                this->rlc_8b(this->A);
                return 4;
            // LD (u16),SP
            case 0x08:
                this->mov_16bmem(this->joinBytes(args[0], args[1]), this->SP);
                return 20;
            // ADD HL,BC
            case 0x09:
                this->add_16b2x2(this->H, this->L, this->B, this->C);
                return 8;
            // LD A,(BC)
            case 0x0A:
                this->A = (*this->memory)[this->readBC()];
                return 8;
            // DEC BC
            case 0x0B:
                this->dec_16b2(this->B, this->C);
                return 8;
            // INC C
            case 0x0C:
                this->inc_8b(this->C);
                return 4;
            // DEC C
            case 0x0D:
                this->dec_8b(this->C);
                return 4;
            // LD C,u8
            case 0x0E:
                this->C = args[0];
                return 8;
            // RRCA
            case 0x0F:
                this->rrc_8b(this->A);
                return 4;
            // STOP
            case 0x10:
                // to be implemented
                return 4;
            // LD DE,u16
            case 0x11:
                this->D = args[0];
                this->E = args[1];
                return 12;
            // LD (DE),A
            case 0x12:
                (*this->memory)[this->readDE()] = this->A;
                return 8;
            // INC DE
            case 0x13:
                this->inc_16b2(this->D, this->E);
                return 8;
            // INC D
            case 0x14:
                this->inc_8b(this->D);
                return 4;
            // DEC D
            case 0x15:
                this->dec_8b(this->D);
                return 4;
            // LD D,u8
            case 0x16:
                this->D = args[0];
                return 8;
            // RLA
            case 0x17:
                this->rl_8b(this->A);
                return 4;
            // JR i8
            case 0x18:
                this->PC += (int8_t) args[0];  // should work to get signed byte back
                return 12;
            // ADD HL,DE
            case 0x19:
                this->add_16b2x2(this->H, this->L, this->D, this->E);
                return 8;
            // LD A,(DE)
            case 0x1A:
                this->A = (*this->memory)[this->readDE()];
                return 8;
            // DEC DE
            case 0x1B:
                this->dec_16b2(this->D, this->E);
                return 8;
            // INC E
            case 0x1C:
                this->inc_8b(this->E);
                return 4;
            // DEC E
            case 0x1D:
                this->dec_8b(this->E);
                return 4;
            // LD E,u8
            case 0x1E:
                this->E = args[0];
                return 8;
            // RRA
            case 0x1F:
                this->rr_8b(this->A);
                return 4;
            // JR NZ,i8
            case 0x20:
                return this->jr_conditional(Flag::zf, 0, (int8_t) args[0]);  // 8 / 12
            // LD HL,u16
            case 0x21:
                this->H = args[0];
                this->L = args[1];
                return 12;
            // LD (HL+),A
            case 0x22:
                (*this->memory)[this->readHL()] = this->A;
                this->inc_16b2(this->H, this->L);
                return 8;
            // INC HL
            case 0x23:
                this->inc_16b2(this->H, this->L);
                return 8;
            // INC H
            case 0x24:
                this->inc_8b(this->H);
                return 4;
            // DEC H
            case 0x25:
                this->dec_8b(this->H);
                return 4;
            // LD H,u8
            case 0x26:
                this->H = args[0];
                return 8;
            // DAA
            case 0x27:
                if (!this->getFlag(Flag::n)) {
                    if (this->getFlag(Flag::cy) || this->A > 0x99) {
                        this->A += 0x60;
                        this->setFlag(Flag::cy, 1);
                    }
                    if (this->getFlag(Flag::h) || (this->A & 0x0F) > 0x09)
                        this->A += 0x06;
                } else {
                    if (this->getFlag(Flag::cy))
                        this->A -= 0x60;
                    if (this->getFlag(Flag::h))
                        this->A -= 0x06;
                }
                this->setFlag(Flag::zf, this->A == 0);
                this->setFlag(Flag::h, 0);
                return 4;
            // JR Z,i8
            case 0x28:
                return this->jr_conditional(Flag::zf, 1, (int8_t) args[0]);  // 8 / 12
            // ADD HL,HL
            case 0x29:
                this->add_16b2x2(this->H, this->L, this->H, this->L);
                return 8;
            // LD A,(HL+)
            case 0x2A:
                this->A = (*this->memory)[this->readHL()];
                this->inc_16b2(this->H, this->L);
                return 8;
            // DEC HL
            case 0x2B:
                this->dec_16b2(this->H, this->L);
                return 8;
            // INC L
            case 0x2C:
                this->inc_8b(this->L);
                return 4;
            // DEC L
            case 0x2D:
                this->dec_8b(this->L);
                return 4;
            // LD L,u8
            case 0x2E:
                this->L = args[0];
                return 8;
            // CPL
            case 0x2F:
                this->A ^= 0xFF;
                this->setFlags((uint8_t) Flag::n | (uint8_t) Flag::h);
                return 4;
            // JR NC,i8
            case 0x30:
                return this->jr_conditional(Flag::cy, 0, (int8_t) args[0]);
            // LD SP,u16
            case 0x31:
                this->SP = this->joinBytes(args[0], args[1]);
                return 12;
            // LD (HL-),A
            case 0x32:
                (*this->memory)[this->readHL()] = this->A;
                this->dec_16b2(this->H, this->L);
                return 8;
            // INC SP
            case 0x33:
                this->SP++;
                return 8;
            // INC (HL)
            case 0x34:
                this->inc_8b((*this->memory)[this->readHL()]);
                return 12;
            // DEC (HL)
            case 0x35:
                this->dec_8b((*this->memory)[this->readHL()]);
                return 12;
            // LD (HL),u8
            case 0x36:
                (*this->memory)[this->readHL()] = args[0];
                return 12;
            // SCF
            case 0x37:
                this->resetFlags((uint8_t) Flag::n | (uint8_t) Flag::h);
                this->setFlag(Flag::cy, 1);
                return 4;
            // JR C,i8
            case 0x38:
                return this->jr_conditional(Flag::cy, 1, (int8_t) args[0]);
            // ADD HL,SP
            case 0x39:
                this->add_16b2x2(this->H, this->L, (this->SP >> 8), (this->SP & 0x00FF));
                return 8;
            // LD A,(HL-)
            case 0x3A:
                this->A = (*this->memory)[this->readHL()];
                this->dec_16b2(this->H, this->L);
                return 8;
            // DEC SP
            case 0x3B:
                this->SP--;
                return 8;
            // INC A
            case 0x3C:
                this->inc_8b(this->A);
                return 4;
            // DEC A
            case 0x3D:
                this->dec_8b(this->A);
                return 4;
            // LD A,u8
            case 0x3E:
                this->A = args[0];
                return 8;
            // CCF
            case 0x3F:
                this->setFlag(Flag::cy, !this->getFlag(Flag::cy));
                this->resetFlags((uint8_t) Flag::n | (uint8_t) Flag::h);
                return 4;
            // ADD A,B
            case 0x80:
                this->add_8b(this->A, this->B);
                return 4;
            // ADD A,C
            case 0x81:
                this->add_8b(this->A, this->C);
                return 4;
            // ADD A,D
            case 0x82:
                this->add_8b(this->A, this->D);
                return 4;
            // ADD A,E
            case 0x83:
                this->add_8b(this->A, this->E);
                return 4;
            // ADD A,H
            case 0x84:
                this->add_8b(this->A, this->H);
                return 4;
            // ADD A,L
            case 0x85:
                this->add_8b(this->A, this->L);
                return 4;
            // ADD A,(HL)
            case 0x86:
                this->add_8b(this->A, (*this->memory)[this->readHL()]);
                return 8;
            // ADD A,A
            case 0x87:
                this->add_8b(this->A, this->A);
                return 4;
            // ADC A,B
            case 0x88:
                this->adc_8b(this->A, this->B);
                return 4;
            // ADC A,C
            case 0x89:
                this->adc_8b(this->A, this->C);
                return 4;
            // ADC A,D
            case 0x8A:
                this->adc_8b(this->A, this->D);
                return 4;
            // ADC A,E
            case 0x8B:
                this->adc_8b(this->A, this->E);
                return 4;
            // ADC A,H
            case 0x8C:
                this->adc_8b(this->A, this->H);
                return 4;
            // ADC A,L
            case 0x8D:
                this->adc_8b(this->A, this->L);
                return 4;
            // ADC A,(HL)
            case 0x8E:
                this->adc_8b(this->A, (*this->memory)[this->readHL()]);
                return 8;
            // ADC A,A
            case 0x8F:
                this->adc_8b(this->A, this->A);
                return 4;
            // SUB A,B
            case 0x90:
                this->sub_8b(this->A, this->B);
                return 4;
            // SUB A,C
            case 0x91:
                this->sub_8b(this->A, this->C);
                return 4;
            // SUB A,D
            case 0x92:
                this->sub_8b(this->A, this->D);
                return 4;
            // SUB A,E
            case 0x93:
                this->sub_8b(this->A, this->E);
                return 4;
            // SUB A,H
            case 0x94:
                this->sub_8b(this->A, this->H);
                return 4;
            // SUB A,L
            case 0x95:
                this->sub_8b(this->A, this->L);
                return 4;
            // SUB A,(HL)
            case 0x96:
                this->sub_8b(this->A, (*this->memory)[this->readHL()]);
                return 8;
            // SUB A,A
            case 0x97:
                this->sub_8b(this->A, this->A);
                return 4;
            // SBC A,B
            case 0x98:
                this->sbc_8b(this->A, this->B);
                return 4;
            // SBC A,C
            case 0x99:
                this->sbc_8b(this->A, this->C);
                return 4;
            // SBC A,D
            case 0x9A:
                this->sbc_8b(this->A, this->D);
                return 4;
            // SBC A,E
            case 0x9B:
                this->sbc_8b(this->A, this->E);
                return 4;
            // SBC A,H
            case 0x9C:
                this->sbc_8b(this->A, this->H);
                return 4;
            // SBC A,L
            case 0x9D:
                this->sbc_8b(this->A, this->L);
                return 4;
            // SBC A,(HL)
            case 0x9E:
                this->sbc_8b(this->A, (*this->memory)[this->readHL()]);
                return 8;
            // SBC A,A
            case 0x9F:
                this->sbc_8b(this->A, this->A);
                return 4;
            // AND A,B
            case 0xA0:
                this->and_8b(this->A, this->B);
                return 4;
            // AND A,C
            case 0xA1:
                this->and_8b(this->A, this->C);
                return 4;
            // AND A,D
            case 0xA2:
                this->and_8b(this->A, this->D);
                return 4;
            // AND A,E
            case 0xA3:
                this->and_8b(this->A, this->E);
                return 4;
            // AND A,H
            case 0xA4:
                this->and_8b(this->A, this->H);
                return 4;
            // AND A,L
            case 0xA5:
                this->and_8b(this->A, this->L);
                return 4;
            // AND A,(HL)
            case 0xA6:
                this->and_8b(this->A, (*this->memory)[this->readHL()]);
                return 8;
            // AND A,A
            case 0xA7:
                this->and_8b(this->A, this->A);
                return 4;
            // XOR A,B
            case 0xA8:
                this->xor_8b(this->A, this->B);
                return 4;
            // XOR A,C
            case 0xA9:
                this->xor_8b(this->A, this->C);
                return 4;
            // XOR A,D
            case 0xAA:
                this->xor_8b(this->A, this->D);
                return 4;
            // XOR A,E
            case 0xAB:
                this->xor_8b(this->A, this->E);
                return 4;
            // XOR A,H
            case 0xAC:
                this->xor_8b(this->A, this->H);
                return 4;
            // XOR A,L
            case 0xAD:
                this->xor_8b(this->A, this->L);
                return 4;
            // XOR A,(HL)
            case 0xAE:
                this->xor_8b(this->A, (*this->memory)[this->readHL()]);
                return 8;
            // XOR A,A
            case 0xAF:
                this->xor_8b(this->A, this->A);
                return 4;
            // OR A,B
            case 0xB0:
                this->or_8b(this->A, this->B);
                return 4;
            // OR A,C
            case 0xB1:
                this->or_8b(this->A, this->C);
                return 4;
            // OR A,D
            case 0xB2:
                this->or_8b(this->A, this->D);
                return 4;
            // OR A,E
            case 0xB3:
                this->or_8b(this->A, this->E);
                return 4;
            // OR A,H
            case 0xB4:
                this->or_8b(this->A, this->H);
                return 4;
            // OR A,L
            case 0xB5:
                this->or_8b(this->A, this->L);
                return 4;
            // OR A,(HL)
            case 0xB6:
                this->or_8b(this->A, (*this->memory)[this->readHL()]);
                return 8;
            // OR A,A
            case 0xB7:
                this->or_8b(this->A, this->A);
                return 4;
            // CP A,B
            case 0xB8:
                this->cp_8b(this->A, this->B);
                return 4;
            // CP A,C
            case 0xB9:
                this->cp_8b(this->A, this->C);
                return 4;
            // CP A,D
            case 0xBA:
                this->cp_8b(this->A, this->D);
                return 4;
            // CP A,E
            case 0xBB:
                this->cp_8b(this->A, this->E);
                return 4;
            // CP A,H
            case 0xBC:
                this->cp_8b(this->A, this->H);
                return 4;
            // CP A,L
            case 0xBD:
                this->cp_8b(this->A, this->L);
                return 4;
            // CP A,(HL)
            case 0xBE:
                this->cp_8b(this->A, (*this->memory)[this->readHL()]);
                return 8;
            // CP A,A
            case 0xBF:
                this->cp_8b(this->A, this->A);
                return 4;
            }
        }

        return 0;
    }
}

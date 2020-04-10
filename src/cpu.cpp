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

    uint16_t Cpu::getBC() { return this->joinBytes(this->B, this->C); }
    uint16_t Cpu::getDE() { return this->joinBytes(this->D, this->E); }
    uint16_t Cpu::getHL() { return this->joinBytes(this->H, this->L); }

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

    void Cpu::inc8b(uint8_t& r) {
        r++;
        this->setFlag(Flag::zf, r == 0x00);
        this->setFlag(Flag::n, 0);
        this->setFlag(Flag::h, r == 0x10);
    }

    void Cpu::dec8b(uint8_t& r) {
        r--;
        this->setFlag(Flag::zf, r == 0xFF);
        this->setFlag(Flag::n, 1);
        this->setFlag(Flag::h, r == 0x0F);
    }

    void Cpu::rl8b(uint8_t& r) {
        bool msb = r >> 7;
        r = ((r << 1) & 0xFF) | this->getFlag(Flag::cy);
        this->setFlag(Flag::cy, msb);
        this->resetFlags((uint8_t) Flag::zf | (uint8_t) Flag::n | (uint8_t) Flag::h);
    }

    void Cpu::rr8b(uint8_t& r) {
        bool lsb = r & 0x01;
        r = (r >> 1) | (this->getFlag(Flag::cy) << 7);
        this->setFlag(Flag::cy, lsb);
        this->resetFlags((uint8_t) Flag::zf | (uint8_t) Flag::n | (uint8_t) Flag::h);
    }

    void Cpu::rlc8b(uint8_t& r) {
        this->setFlag(Flag::cy, r >> 7);
        r = ((r << 1) & 0xFF) | (r >> 7);
        this->resetFlags((uint8_t) Flag::zf | (uint8_t) Flag::n | (uint8_t) Flag::h);
    }

    void Cpu::rrc8b(uint8_t& r) {
        this->setFlag(Flag::cy, r & 0x01);
        r = (r >> 1) | ((r & 0x01) << 7);
        this->resetFlags((uint8_t) Flag::zf | (uint8_t) Flag::n | (uint8_t) Flag::h);
    }

    void Cpu::add8b(uint8_t& r1, uint8_t r2) {
        uint16_t real = r1 + r2;
        r1 = real & 0x00FF;
        this->setFlag(Flag::zf, r1 == 0x00);
        this->setFlag(Flag::n, 0);
        this->setFlag(Flag::h, r1 == 0x10);
        this->setFlag(Flag::cy, real > 0xFF);
    }

    void Cpu::inc16b2(uint8_t& r1, uint8_t& r2) {
        r2++;
        if (!r2)
            r1++;
    }

    void Cpu::dec16b2(uint8_t& r1, uint8_t& r2) {
        r2--;
        if (r2 == 0xFF)
            r1--;
    }

    void Cpu::mov16bmem(uint16_t addr, uint16_t value) {
        (*this->memory)[addr] = value >> 8;
        (*this->memory)[addr+1] = value & 0x00FF;
    }

    void Cpu::add16b2x2(uint8_t& r1, uint8_t& r2, uint8_t r3, uint8_t r4) {
        uint32_t real = this->joinBytes(r1, r2) + this->joinBytes(r3, r4);
        uint16_t result = real & 0xFFFF;
        r1 = result >> 8;
        r2 = result & 0x00FF;
        this->setFlag(Flag::n, 0);
        this->setFlag(Flag::h, result == 0x0100);
        this->setFlag(Flag::cy, result == (real > 0xFFFF));
    }

    uint8_t Cpu::jrConditional(Flag flag, bool value, int8_t jvalue) {
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
                (*this->memory)[this->getBC()] = this->A;
                return 8;
            // INC BC
            case 0x03:
                this->inc16b2(this->B, this->C);
                return 8;
            // INC B
            case 0x04:
                this->inc8b(this->B);
                return 4;
            // DEC B
            case 0x05:
                this->dec8b(this->B);
                return 4;
            // LD B,u8
            case 0x06:
                this->B = args[0];
                return 8;
            // RLCA
            case 0x07:
                this->rlc8b(this->A);
                return 4;
            // LD (u16),SP
            case 0x08:
                this->mov16bmem(this->joinBytes(args[0], args[1]), this->SP);
                return 20;
            // ADD HL,BC
            case 0x09:
                this->add16b2x2(this->H, this->L, this->B, this->C);
                return 8;
            // LD A,(BC)
            case 0x0A:
                this->A = (*this->memory)[this->getBC()];
                return 8;
            // DEC BC
            case 0x0B:
                this->dec16b2(this->B, this->C);
                return 8;
            // INC C
            case 0x0C:
                this->inc8b(this->C);
                return 4;
            // DEC C
            case 0x0D:
                this->dec8b(this->C);
                return 4;
            // LD C,u8
            case 0x0E:
                this->C = args[0];
                return 8;
            // RRCA
            case 0x0F:
                this->rrc8b(this->A);
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
                (*this->memory)[this->getDE()] = this->A;
                return 8;
            // INC DE
            case 0x13:
                this->inc16b2(this->D, this->E);
                return 8;
            // INC D
            case 0x14:
                this->inc8b(this->D);
                return 4;
            // DEC D
            case 0x15:
                this->dec8b(this->D);
                return 4;
            // LD D,u8
            case 0x16:
                this->D = args[0];
                return 8;
            // RLA
            case 0x17:
                this->rl8b(this->A);
                return 4;
            // JR i8
            case 0x18:
                this->PC += (int8_t) args[0];  // should work to get signed byte back
                return 12;
            // ADD HL,DE
            case 0x19:
                this->add16b2x2(this->H, this->L, this->D, this->E);
                return 8;
            // LD A,(DE)
            case 0x1A:
                this->A = (*this->memory)[this->getDE()];
                return 8;
            // DEC DE
            case 0x1B:
                this->dec16b2(this->D, this->E);
                return 8;
            // INC E
            case 0x1C:
                this->inc8b(this->E);
                return 4;
            // DEC E
            case 0x1D:
                this->dec8b(this->E);
                return 4;
            // LD E,u8
            case 0x1E:
                this->E = args[0];
                return 8;
            // RRA
            case 0x1F:
                this->rr8b(this->A);
                return 4;
            // JR NZ,i8
            case 0x20:
                return this->jrConditional(Flag::zf, 0, (int8_t) args[0]);  // 8 / 12
            // LD HL,u16
            case 0x21:
                this->H = args[0];
                this->L = args[1];
                return 12;
            // LD (HL+),A
            case 0x22:
                (*this->memory)[this->getHL()] = this->A;
                this->inc16b2(this->H, this->L);
                return 8;
            // INC HL
            case 0x23:
                this->inc16b2(this->H, this->L);
                return 8;
            // INC H
            case 0x24:
                this->inc8b(this->H);
                return 4;
            // DEC H
            case 0x25:
                this->dec8b(this->H);
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
                return this->jrConditional(Flag::zf, 1, (int8_t) args[0]);  // 8 / 12
            // ADD HL,HL
            case 0x29:
                this->add16b2x2(this->H, this->L, this->H, this->L);
                return 8;
            // LD A,(HL+)
            case 0x2A:
                this->A = (*this->memory)[this->getHL()];
                this->inc16b2(this->H, this->L);
                return 8;
            // DEC HL
            case 0x2B:
                this->dec16b2(this->H, this->L);
                return 8;
            // INC L
            case 0x2C:
                this->inc8b(this->L);
                return 4;
            // DEC L
            case 0x2D:
                this->dec8b(this->L);
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
                return this->jrConditional(Flag::cy, 0, (int8_t) args[0]);
            // LD SP,u16
            case 0x31:
                this->SP = this->joinBytes(args[0], args[1]);
                return 12;
            // LD (HL-),A
            case 0x32:
                (*this->memory)[this->getHL()] = this->A;
                this->dec16b2(this->H, this->L);
                return 8;
            // INC SP
            case 0x33:
                this->SP++;
                return 8;
            // INC (HL)
            case 0x34:
                this->inc8b((*this->memory)[this->getHL()]);
                return 12;
            // DEC (HL)
            case 0x35:
                this->dec8b((*this->memory)[this->getHL()]);
                return 12;
            // LD (HL),u8
            case 0x36:
                (*this->memory)[this->getHL()] = args[0];
                return 12;
            // SCF
            case 0x37:
                this->resetFlags((uint8_t) Flag::n | (uint8_t) Flag::h);
                this->setFlag(Flag::cy, 1);
                return 4;
            // JR C,i8
            case 0x38:
                return this->jrConditional(Flag::cy, 1, (int8_t) args[0]);
            // ADD HL,SP
            case 0x39:
                this->add16b2x2(this->H, this->L, (this->SP >> 8), (this->SP & 0x00FF));
                return 8;
            // LD A,(HL-)
            case 0x3A:
                this->A = (*this->memory)[this->getHL()];
                this->dec16b2(this->H, this->L);
                return 8;
            // DEC SP
            case 0x3B:
                this->SP--;
                return 8;
            // INC A
            case 0x3C:
                this->inc8b(this->A);
                return 4;
            // DEC A
            case 0x3D:
                this->dec8b(this->A);
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
                this->add8b(this->A, this->B);
                return 4;
            // ADD A,C
            case 0x81:
                this->add8b(this->A, this->C);
                return 4;
            // ADD A,D
            case 0x82:
                this->add8b(this->A, this->D);
                return 4;
            // ADD A,E
            case 0x83:
                this->add8b(this->A, this->E);
                return 4;
            // ADD A,H
            case 0x84:
                this->add8b(this->A, this->H);
                return 4;
            // ADD A,L
            case 0x85:
                this->add8b(this->A, this->L);
                return 4;
            // ADD A,(HL)
            case 0x86:
                this->add8b(this->A, (*this->memory)[this->getHL()]);
                return 4;
            // ADD A,A
            case 0x87:
                this->add8b(this->A, this->A);
                return 4;
            }
        }

        return 0;
    }
}

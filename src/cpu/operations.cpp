#include "operations.hpp"
#include "cpu.hpp"

namespace Cpu::Operations {
    Instruction::Instruction(const std::shared_ptr<Operation> operation, const uint8_t opcode, const std::shared_ptr<uint8_t[]> args, const uint8_t extra_steps)
        : operation(operation), opcode(opcode), args(args), extra_steps(extra_steps) { this->extra_step_i = 0; }

    bool Instruction::Step() {
        if (this->extra_step_i++ < this->extra_steps)
            return false;
        else
            return this->operation->Step();
    }

    void Instruction::Execute() {
        while (this->extra_step_i++ < this->extra_steps) {}
        while (!this->operation->Step()) {}
    }

    Operation::Operation(Cpu* const cpu)
        : cpu(cpu) { this->step_i = 0; }

    IncreaseByte::IncreaseByte(Cpu* const cpu, uint8_t& byte)
        : Operation(cpu), byte(byte) {}

    // set the z, n, h flags
    void IncreaseByte::SetFlags() {
        // zero flag is only set if 11111111 -> 00000000
        this->cpu->SetFlag(Flag::z, this->byte == 0x00);
        // negative flag is always reset
        this->cpu->SetFlag(Flag::n, 0);
        // when increasing, half carry is set if 00001111 -> 00010000
        this->cpu->SetFlag(Flag::h, this->byte == 0x10);
    }

    bool IncreaseByte::Step() {
        switch (this->step_i++) {
            // only one step: the referenced byte is increased and flags are set
            case 0:
                this->byte++;
                this->SetFlags();
                return true;
            default:
                return true;
        }
    }

    DecreaseByte::DecreaseByte(Cpu* const cpu, uint8_t& byte)
        : Operation(cpu), byte(byte) {}

    // set the z, n, h flags
    void DecreaseByte::SetFlags() {
        // zero flag is only set if 00000001 -> 00000000
        this->cpu->SetFlag(Flag::z, this->byte == 0x00);
        // negative flag is always set
        this->cpu->SetFlag(Flag::n, 1);
        // when increasing, half carry is set if 00010000 -> 00001111
        this->cpu->SetFlag(Flag::h, this->byte == 0x0F);
    }

    bool DecreaseByte::Step() {
        switch (this->step_i++) {
            // only one step: the referenced byte is decreased and flags are set
            case 0:
                this->byte--;
                this->SetFlags();
                return true;
            default:
                return true;
        }
    }

    RotateByte::RotateByte(Cpu* const cpu, uint8_t& byte, const ShiftDirection direction, const bool fast)
        : Operation(cpu), byte(byte), direction(direction), fast(fast) {}

    void RotateByte::SetFlags() {
        // the zero flag is only set if fast is not set
        this->cpu->SetFlag(Flag::z, (this->byte == 0x00) & !this->fast);
        // n and h are always reset
        this->cpu->SetFlag(Flag::n, 0);
        this->cpu->SetFlag(Flag::h, 0);
        // the carry flag is set to the bit going out left (now bit zero)
        if (this->direction == ShiftDirection::Left)
            this->cpu->SetFlag(Flag::c, this->byte & 0x01);
        else
            this->cpu->SetFlag(Flag::c, this->byte >> 7);
    }

    bool RotateByte::Step() {
        // if fast is set, only case 1 will be executed
        // step_i could also be initialized to 1 if flag is set
        switch (this->step_i++ ^ (uint8_t) this->fast) {
            case 0:
                return false;
            case 1:
                // rotate the byte to the left and set flags
                if (this->direction == ShiftDirection::Left)
                    byte = ((byte << 1) & 0xFF) | (byte >> 7);
                else
                    byte = (byte >> 1) | ((byte & 0x01) << 7);
                this->SetFlags();
                return true;
            default:
                return true;
        }
    }

    LoadByte::LoadByte(Cpu* const cpu, uint8_t& dst, const uint8_t src)
        : Operation(cpu), dst(dst), src(src) {}

    bool LoadByte::Step() {
        switch (this->step_i++) {
            case 0:
                this->dst = this->src;
                return true;
            default:
                return true;
        }
    }

    AddByte::AddByte(Cpu* const cpu, uint8_t& dst, const uint8_t src)
        : Operation(cpu), dst(dst), src(src) {}

    void AddByte::SetFlags() {
        this->cpu->SetFlag(Flag::z, this->dst == 0x00);
        this->cpu->SetFlag(Flag::n, 0);
    }

    bool AddByte::Step() {
        switch (this->step_i++) {
            case 0: {
                uint16_t tmp = this->dst + this->src;
                this->cpu->SetFlag(Flag::h, (this->dst < 0x10) && (tmp >= 0x10));
                this->cpu->SetFlag(Flag::c, tmp > 0xFF);
                this->dst = tmp & 0xFF;
                this->SetFlags();
                return true;
            }
            default:
                return true;
        }
    }

    AdcByte::AdcByte(Cpu* const cpu, uint8_t& dst, const uint8_t src)
        : Operation(cpu), dst(dst), src(src) {}

    void AdcByte::SetFlags() {
        this->cpu->SetFlag(Flag::z, this->dst == 0x00);
        this->cpu->SetFlag(Flag::n, 0);
    }

    bool AdcByte::Step() {
        switch (this->step_i++) {
            case 0: {
                uint16_t tmp = this->dst + this->src + this->cpu->GetFlag(Flag::c);
                this->cpu->SetFlag(Flag::h, (this->dst < 0x10) && (tmp >= 0x10));
                this->cpu->SetFlag(Flag::c, tmp > 0xFF);
                this->dst = tmp & 0xFF;
                this->SetFlags();
                return true;
            }
            default:
                return true;
        }
    }

    SubByte::SubByte(Cpu* const cpu, uint8_t& dst, const uint8_t src)
        : Operation(cpu), dst(dst), src(src) {}

    void SubByte::SetFlags() {
        this->cpu->SetFlag(Flag::z, this->dst == 0x00);
        this->cpu->SetFlag(Flag::n, 1);
    }

    bool SubByte::Step() {
        switch (this->step_i++) {
            case 0: {
                int16_t tmp = this->dst - this->src;
                this->cpu->SetFlag(Flag::h, (this->dst > 0x0F) && (tmp <= 0x0F));
                this->cpu->SetFlag(Flag::c, tmp < 0);
                this->dst = static_cast<uint8_t>(tmp);
                this->SetFlags();
                return true;
            }
            default:
                return true;
        }
    }

    SbcByte::SbcByte(Cpu* const cpu, uint8_t& dst, const uint8_t src)
        : Operation(cpu), dst(dst), src(src) {}

    void SbcByte::SetFlags() {
        this->cpu->SetFlag(Flag::z, this->dst == 0x00);
        this->cpu->SetFlag(Flag::n, 1);
    }

    bool SbcByte::Step() {
        switch (this->step_i++) {
            case 0: {
                int16_t tmp = this->dst - (this->src + this->cpu->GetFlag(Flag::c));
                this->cpu->SetFlag(Flag::h, (this->dst > 0x0F) && (tmp <= 0x0F));
                this->cpu->SetFlag(Flag::c, tmp < 0);
                this->dst = static_cast<uint8_t>(tmp);
                this->SetFlags();
                return true;
            }
            default:
                return true;
        }
    }

    AndByte::AndByte(Cpu* const cpu, uint8_t& dst, const uint8_t src)
        : Operation(cpu), dst(dst), src(src) {}

    void AndByte::SetFlags() {
        this->cpu->SetFlag(Flag::z, this->dst == 0x00);
        this->cpu->SetFlag(Flag::n, 0);
        this->cpu->SetFlag(Flag::h, 1);
        this->cpu->SetFlag(Flag::c, 0);
    }

    bool AndByte::Step() {
        switch (this->step_i++) {
            case 0:
                this->dst &= this->src;
                this->SetFlags();
                return true;
            default:
                return true;
        }
    }

    XorByte::XorByte(Cpu* const cpu, uint8_t& dst, const uint8_t src)
        : Operation(cpu), dst(dst), src(src) {}

    void XorByte::SetFlags() {
        this->cpu->SetFlag(Flag::z, this->dst == 0x00);
        this->cpu->SetFlag(Flag::n, 0);
        this->cpu->SetFlag(Flag::h, 0);
        this->cpu->SetFlag(Flag::c, 0);
    }

    bool XorByte::Step() {
        switch (this->step_i++) {
            case 0:
                this->dst ^= this->src;
                this->SetFlags();
                return true;
            default:
                return true;
        }
    }

    OrByte::OrByte(Cpu* const cpu, uint8_t& dst, const uint8_t src)
        : Operation(cpu), dst(dst), src(src) {}

    void OrByte::SetFlags() {
        this->cpu->SetFlag(Flag::z, this->dst == 0x00);
        this->cpu->SetFlag(Flag::n, 0);
        this->cpu->SetFlag(Flag::h, 0);
        this->cpu->SetFlag(Flag::c, 0);
    }

    bool OrByte::Step() {
        switch (this->step_i++) {
            case 0:
                this->dst |= this->src;
                this->SetFlags();
                return true;
            default:
                return true;
        }
    }

    CpByte::CpByte(Cpu* const cpu, const uint8_t dst, const uint8_t src)
        : Operation(cpu), dst(dst), src(src) {}

    bool CpByte::Step() {
        switch (this->step_i++) {
            case 0: {
                int16_t tmp = this->dst - this->src;
                this->cpu->SetFlag(Flag::z, tmp == 0x00);
                this->cpu->SetFlag(Flag::n, 1);
                this->cpu->SetFlag(Flag::h, (this->dst > 0x0F) && (tmp <= 0x0F));
                this->cpu->SetFlag(Flag::c, tmp < 0);
                return true;
            }
            default:
                return true;
        }
    }

    IncreaseDoubleByte::IncreaseDoubleByte(Cpu* const cpu, uint8_t& upper_byte, uint8_t& lower_byte)
        : Operation(cpu), upper_byte(upper_byte), lower_byte(lower_byte) {}

    bool IncreaseDoubleByte::Step() {
        switch (this->step_i++) {
            case 0:
                this->lower_byte++;
                return false;
            case 1:
                if (!this->lower_byte)
                    this->upper_byte++;
                return true;
            default:
                return true;
        }
    }

    DecreaseDoubleByte::DecreaseDoubleByte(Cpu* const cpu, uint8_t& upper_byte, uint8_t& lower_byte)
        : Operation(cpu), upper_byte(upper_byte), lower_byte(lower_byte) {}

    bool DecreaseDoubleByte::Step() {
        switch (this->step_i++) {
            case 0:
                this->lower_byte--;
                return false;
            case 1:
                if (this->lower_byte == 0xFF)
                    this->upper_byte--;
                return true;
            default:
                return true;
        }
    }

    LoadDoubleByte::LoadDoubleByte(Cpu* const cpu, uint8_t& upper_dst, uint8_t& lower_dst, const uint8_t upper_src, const uint8_t lower_src)
        : Operation(cpu), upper_dst(upper_dst), lower_dst(lower_dst), upper_src(upper_src), lower_src(lower_src) {}

    bool LoadDoubleByte::Step() {
        switch (step_i++) {
            case 0:
                lower_dst = lower_src;
                return false;
            case 1:
                upper_dst = upper_src;
                return true;
            default:
                return true;
        }
    }

    PopDoubleByte::PopDoubleByte(Cpu* const cpu, uint8_t& upper_dst, uint8_t& lower_dst)
        : Operation(cpu), upper_dst(upper_dst), lower_dst(lower_dst) {}

    bool PopDoubleByte::Step() {
        switch (this->step_i++) {
            case 0:
                lower_dst = *Helpers::DereferenceSP(this->cpu, Helpers::DoubleByteOperation::Increase);
                return false;
            case 1:
                upper_dst = *Helpers::DereferenceSP(this->cpu, Helpers::DoubleByteOperation::Increase);
                return true;
            default:
                return true;
        }
    }

    PushDoubleByte::PushDoubleByte(Cpu* const cpu, const uint8_t upper_src, const uint8_t lower_src)
        : Operation(cpu), upper_src(upper_src), lower_src(lower_src) {}

    bool PushDoubleByte::Step() {
        switch (this->step_i++) {
            // lol ugly code
            case 0:
                *Helpers::DereferenceSP(this->cpu, Helpers::DoubleByteOperation::Decrease) = this->upper_src;
                return false;
            case 1:
                *Helpers::DereferenceSP(this->cpu, Helpers::DoubleByteOperation::Decrease) = this->lower_src;
                return true;
            default:
                return true;
        }
    }

    JumpRelative::JumpRelative(Cpu* const cpu, const uint8_t jump_offset)
        : Operation(cpu), jump_offset(jump_offset) {}

    bool JumpRelative::Step() {
        switch (this->step_i++) {
            case 0:
                this->cpu->PC += this->jump_offset - 128 - 2;
                return true;
            default:
                return true;
        }
    }

    JumpRelativeConditional::JumpRelativeConditional(Cpu* const cpu, const uint8_t jump_offset, const Flag flag, const bool flag_value)
        : Operation(cpu), jump_offset(jump_offset), flag(flag), flag_value(flag_value) { branch = false; }

    bool JumpRelativeConditional::Step() {
        switch(this->step_i++) {
            case 0:
                if (this->cpu->GetFlag(flag) == flag_value) {
                    this->branch = true;
                    return false;
                } else { return true; }
            case 1:
                if (this->branch)
                    this->cpu->PC += this->jump_offset - 128 - 2;
                return true;
            default:
                return true;
        }
    }
}

#pragma once
#include <cstdint>

namespace GB_Cpu {
    enum class Flag : uint8_t {
        z = 0x08,
        n = 0x04,
        h = 0x02,
        c = 0x01
    };

    class Cpu {
    public:
        Cpu();
        void SetFlag(Flag flag, bool flag_value);
    };
}

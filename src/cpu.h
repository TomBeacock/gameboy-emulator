#pragma once

#include <cstdint>

namespace Gameboy
{
    class CPU
    {
      private:
        inline uint8_t get_reg8(size_t reg) const { return registers[reg]; }
        inline void set_reg8(size_t reg, uint8_t val) { registers[reg] = val; }

        inline uint16_t get_reg16(size_t hi, size_t lo) const
        {
            return (uint16_t)registers[hi] << 8 | registers[lo];
        }

        inline void set_reg16(size_t hi, size_t lo, uint16_t value)
        {
            registers[hi] = (value & 0xFF00) >> 8;
            registers[lo] = value & 0x00FF;
        }

        inline uint16_t get_af() const { return get_reg16(Reg_A, Reg_F); }
        inline void set_af(uint16_t value) { set_reg16(Reg_A, Reg_F, value); };

        inline uint16_t get_bc() const { return get_reg16(Reg_B, Reg_C); }
        inline void set_bc(uint16_t value) { set_reg16(Reg_B, Reg_C, value); };

        inline uint16_t get_de() const { return get_reg16(Reg_D, Reg_E); }
        inline void set_de(uint16_t value) { set_reg16(Reg_D, Reg_E, value); };

        inline uint16_t get_hl() const { return get_reg16(Reg_H, Reg_L); }
        inline void set_hl(uint16_t value) { set_reg16(Reg_H, Reg_L, value); };

        inline bool get_flag(size_t flag) const
        {
            return registers[Reg_F] >> flag & 1;
        }

        inline void set_flag(size_t flag)
        {
            registers[Reg_F] |= 1 << flag;
        }

        inline void clear_flag(size_t flag)
        {
            registers[Reg_F] &= ~(1 << flag);
        }

      private:
        uint8_t registers[8];
        uint16_t sp;
        uint16_t pc;

      private:
        static constexpr size_t Reg_A = 0;
        static constexpr size_t Reg_B = 1;
        static constexpr size_t Reg_C = 2;
        static constexpr size_t Reg_D = 3;
        static constexpr size_t Reg_E = 4;
        static constexpr size_t Reg_F = 5;
        static constexpr size_t Reg_H = 6;
        static constexpr size_t Reg_L = 7;

        static constexpr size_t Flag_Zero = 7;
        static constexpr size_t Flag_Carry = 4;
        static constexpr size_t Flag_Sub = 6;
        static constexpr size_t Flag_HalfCarry = 7;
    };
} // namespace Gameboy
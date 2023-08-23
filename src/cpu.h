#pragma once

#include <cstdint>
#include <utility>

namespace Gameboy
{
    class Memory;
    class Display;

    class CPU
    {
      private:
        typedef uint8_t Register8;
        typedef uint16_t Address;

        struct Instruction {
            uint8_t encoding;
            bool prefixed;

            Instruction(uint8_t encoding, bool prefixed) : encoding(encoding), prefixed(prefixed) {}
        };

        struct ExecuteResult {
            Address next_pc;
            unsigned int cycles;

            ExecuteResult(Address next_pc, unsigned int cycles) : next_pc(next_pc), cycles(cycles)
            {
            }
        };

        class Register16
        {
          public:
            Register16() : reg_hi(0), reg_lo(0) {}
            Register16(uint16_t value) : reg_hi(value >> 8), reg_lo(value) {}

            Register8 &hi() { return reg_hi; }
            Register8 hi() const { return reg_hi; }
            Register8 &lo() { return reg_lo; }
            Register8 lo() const { return reg_lo; }

            operator uint16_t() const { return (uint16_t)reg_hi << 8 | reg_lo; }
            Register16 &operator=(uint16_t value)
            {
                reg_hi = value >> 8;
                reg_lo = value;
                return *this;
            }

          private:
            Register8 reg_hi;
            Register8 reg_lo;
        };

      public:
        CPU(Memory *memory, Display *display);

        unsigned int cycle();

      private:
        Instruction fetch();
        ExecuteResult decode(const Instruction &encoding);

        ExecuteResult load_reg_reg(Register8 &dst, Register8 src);
        ExecuteResult load_reg_n(Register8 &dst);
        ExecuteResult load_reg_addr(Register8 &dst, Address src);
        ExecuteResult load_addr_reg(Address dst, Register8 src);
        ExecuteResult load_hl_n();
        ExecuteResult load_a_nn();
        ExecuteResult load_nn_a();
        ExecuteResult load_a_io_n();
        ExecuteResult load_io_n_a();
        ExecuteResult load_a_io_c();
        ExecuteResult load_io_c_a();
        ExecuteResult load_incr_hl_a();
        ExecuteResult load_incr_a_hl();
        ExecuteResult load_decr_hl_a();
        ExecuteResult load_decr_a_hl();

        ExecuteResult load_reg_nn(Register16 &dst);
        ExecuteResult load_nn_sp();
        ExecuteResult load_sp_hl();
        ExecuteResult push_reg(Register16 src);
        ExecuteResult pop_reg(Register16 &dst);

        ExecuteResult add_a_reg(Register8 op);
        ExecuteResult add_a_n();
        ExecuteResult add_a_hl();
        ExecuteResult add_a_reg_carry(Register8 op);
        ExecuteResult add_a_n_carry();
        ExecuteResult add_a_hl_carry();
        ExecuteResult sub_a_reg(Register8 op);
        ExecuteResult sub_a_n();
        ExecuteResult sub_a_hl();
        ExecuteResult sub_a_reg_carry(Register8 op);
        ExecuteResult sub_a_n_carry();
        ExecuteResult sub_a_hl_carry();

        uint8_t read_next_8() const;
        uint16_t read_next_16() const;
        uint8_t add8(uint8_t a, uint8_t b);
        uint8_t add8_carry(uint8_t a, uint8_t b);
        uint8_t sub8(uint8_t a, uint8_t b);
        uint8_t sub8_carry(uint8_t a, uint8_t b);

        bool get_zero_flag() const { return get_flag(7); }
        void set_zero_flag(bool value) { value ? set_flag(7) : clear_flag(7); }
        bool get_subtract_flag() const { return get_flag(6); }
        void set_subtract_flag(bool value) { value ? set_flag(6) : clear_flag(6); }
        bool get_half_carry_flag() const { return get_flag(5); }
        void set_half_carry_flag(bool value) { value ? set_flag(5) : clear_flag(5); }
        bool get_carry_flag() const { return get_flag(4); }
        void set_carry_flag(bool value) { value ? set_flag(4) : clear_flag(4); }

        bool get_flag(size_t flag) const { return af.lo() >> flag & 1; }
        void set_flag(size_t flag) { af.lo() |= 1 << flag; }
        void clear_flag(size_t flag) { af.lo() &= ~(1 << flag); }

      private:
        Register16 af, bc, de, hl;
        Register16 sp, pc;
        Memory *memory;
        Display *display;
    };
} // namespace Gameboy
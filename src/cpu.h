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

        // 8-bit load instructions
        ExecuteResult ld_r_r(Register8 &dst, Register8 src);
        ExecuteResult ld_r_n(Register8 &dst);
        ExecuteResult ld_r_adr(Register8 &dst, Address src);
        ExecuteResult ld_adr_r(Address dst, Register8 src);
        ExecuteResult ld_hl_n();
        ExecuteResult ld_a_nn();
        ExecuteResult ld_nn_a();
        ExecuteResult ld_a_n();
        ExecuteResult ld_n_a();
        ExecuteResult ld_a_c();
        ExecuteResult ld_c_a();
        ExecuteResult ldi_hl_a();
        ExecuteResult ldi_a_hl();
        ExecuteResult ldd_hl_a();
        ExecuteResult ldd_a_hl();

        // 16-bit load instructions
        ExecuteResult ld_rr_nn(Register16 &dst);
        ExecuteResult ld_nn_sp();
        ExecuteResult ld_sp_hl();
        ExecuteResult push_rr(Register16 src);
        ExecuteResult pop_rr(Register16 &dst);

        // 8-bit arithmetic/logic instructions
        ExecuteResult add_a_r(Register8 op);
        ExecuteResult add_a_n();
        ExecuteResult add_a_hl();
        ExecuteResult adc_a_r(Register8 op);
        ExecuteResult adc_a_n();
        ExecuteResult adc_a_hl();
        ExecuteResult sub_a_r(Register8 op);
        ExecuteResult sub_a_n();
        ExecuteResult sub_a_hl();
        ExecuteResult sbc_a_r(Register8 op);
        ExecuteResult sbc_a_n();
        ExecuteResult sbc_a_hl();
        ExecuteResult and_a_r(Register8 op);
        ExecuteResult and_a_n();
        ExecuteResult and_a_hl();
        ExecuteResult xor_a_r(Register8 op);
        ExecuteResult xor_a_n();
        ExecuteResult xor_a_hl();
        ExecuteResult or_a_r(Register8 op);
        ExecuteResult or_a_n();
        ExecuteResult or_a_hl();
        ExecuteResult cp_a_r(Register8 op);
        ExecuteResult cp_a_n();
        ExecuteResult cp_a_hl();
        ExecuteResult inc_r(Register8& reg);
        ExecuteResult inc_hl();
        ExecuteResult dec_r(Register8 &reg);
        ExecuteResult dec_hl();
        ExecuteResult daa();
        ExecuteResult cpl();

        // 16-bit arithmetic/logic instructions
        ExecuteResult add_hl_rr(Register16 rr);
        ExecuteResult inc_rr(Register16 &rr);
        ExecuteResult dec_rr(Register16 &rr);
        ExecuteResult add_sp_dd();
        ExecuteResult ld_hl_sp_dd();

        uint8_t read_next_8() const;
        uint16_t read_next_16() const;
        uint8_t add_f(uint8_t a, uint8_t b);
        uint16_t add_f(uint16_t a, uint16_t b);
        uint8_t adc_f(uint8_t a, uint8_t b);
        uint8_t sub_f(uint8_t a, uint8_t b);
        uint8_t sbc_f(uint8_t a, uint8_t b);
        uint8_t and_f(uint8_t a, uint8_t b);
        uint8_t xor_f(uint8_t a, uint8_t b);
        uint8_t or_f(uint8_t a, uint8_t b);
        void cp_f(uint8_t a, uint8_t b);
        uint8_t inc_f(uint8_t a);
        uint8_t dec_f(uint8_t a);

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
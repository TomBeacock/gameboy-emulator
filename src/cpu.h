#pragma once

#include "types.h"

#include <optional>

namespace Gameboy
{
    class MMU;
    class Display;

    class CPU
    {
      private:
        typedef u8 Register8;
        typedef u16 Address;
        typedef u8 Instruction;

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
            Register16(u16 value) : reg_hi((u8)(value >> 8)), reg_lo((u8)value) {}

            Register8 &hi() { return reg_hi; }
            Register8 hi() const { return reg_hi; }
            Register8 &lo() { return reg_lo; }
            Register8 lo() const { return reg_lo; }

            operator u16() const { return (u16)reg_hi << 8 | reg_lo; }
            Register16 &operator=(u16 value)
            {
                reg_hi = (u8)(value >> 8);
                reg_lo = (u8)value;
                return *this;
            }

          private:
            Register8 reg_hi;
            Register8 reg_lo;
        };

      public:
        CPU(MMU *memory, Display *display);

        unsigned int step();

      private:
        Instruction fetch(bool &prefixed);
        ExecuteResult decode_8bit(Instruction instruction);
        ExecuteResult decode_16bit(Instruction instruction);

        std::optional<u8> check_interrupts();
        unsigned int interrupt_service_routine(u8 interupt);

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
        ExecuteResult inc_r(Register8 &reg);
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

        // Rotate and shift instructions
        ExecuteResult rlca();
        ExecuteResult rla();
        ExecuteResult rrca();
        ExecuteResult rra();
        ExecuteResult rlc_r(Register8 &r);
        ExecuteResult rlc_hl();
        ExecuteResult rl_r(Register8 &r);
        ExecuteResult rl_hl();
        ExecuteResult rrc_r(Register8 &r);
        ExecuteResult rrc_hl();
        ExecuteResult rr_r(Register8 &r);
        ExecuteResult rr_hl();
        ExecuteResult sla_r(Register8 &r);
        ExecuteResult sla_hl();
        ExecuteResult sra_r(Register8 &r);
        ExecuteResult sra_hl();
        ExecuteResult srl_r(Register8 &r);
        ExecuteResult srl_hl();
        ExecuteResult swap_r(Register8 &r);
        ExecuteResult swap_hl();

        // Single-bit operation instructions
        ExecuteResult bit_n_r(u8 n, Register8 r);
        ExecuteResult bit_n_hl(u8 n);
        ExecuteResult set_n_r(u8 n, Register8 &r);
        ExecuteResult set_n_hl(u8 n);
        ExecuteResult res_n_r(u8 n, Register8 &r);
        ExecuteResult res_n_hl(u8 n);

        // CPU control instructions
        ExecuteResult ccf();
        ExecuteResult scf();
        ExecuteResult nop();
        ExecuteResult halt();
        ExecuteResult stop();
        ExecuteResult di();
        ExecuteResult ei();

        // Jump instructions
        ExecuteResult jp_nn();
        ExecuteResult jp_hl();
        ExecuteResult jp_f_nn(u8 flag, bool value);
        ExecuteResult jr_dd();
        ExecuteResult jr_f_dd(u8 flag, bool value);
        ExecuteResult call_nn();
        ExecuteResult call_f_nn(u8 flag, bool value);
        ExecuteResult ret();
        ExecuteResult ret_f(u8 flag, bool value);
        ExecuteResult reti();
        ExecuteResult rst_n(u8 n);

        u8 read_next_8() const;
        u16 read_next_16() const;
        u8 add_f(u8 a, u8 b);
        u16 add_f(u16 a, u16 b);
        u8 adc_f(u8 a, u8 b);
        u8 sub_f(u8 a, u8 b);
        u8 sbc_f(u8 a, u8 b);
        u8 and_f(u8 a, u8 b);
        u8 xor_f(u8 a, u8 b);
        u8 or_f(u8 a, u8 b);
        void cp_f(u8 a, u8 b);
        u8 inc_f(u8 a);
        u8 dec_f(u8 a);

        bool get_flag_z() const { return get_flag(7); }
        void set_flag_z(bool value) { set_flag(7, value); }
        bool get_flag_n() const { return get_flag(6); }
        void set_flag_n(bool value) { set_flag(6, value); }
        bool get_flag_h() const { return get_flag(5); }
        void set_flag_h(bool value) { set_flag(5, value); }
        bool get_flag_c() const { return get_flag(4); }
        void set_flag_c(bool value) { set_flag(4, value); }

        bool get_flag(u8 flag) const { return af.lo() >> flag & 1; }
        void set_flag(u8 flag, bool value)
        {
            af.lo() = (af.lo() & ~(1 << flag)) | (value << flag);
        }

      private:
        Register16 af, bc, de, hl;
        Register16 sp, pc;
        bool halted = false;
        bool ime = true;
        MMU *memory;
        Display *display;
    };
} // namespace Gameboy
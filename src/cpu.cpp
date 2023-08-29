#include "cpu.h"

#include "display.h"
#include "mmu.h"

#define A af.hi()
#define F af.lo()
#define B bc.hi()
#define C bc.lo()
#define D de.hi()
#define E de.lo()
#define H hl.hi()
#define L hl.lo()

#define FLAG_Z 7
#define FLAG_N 6
#define FLAG_H 5
#define FLAG_C 4

#define I_VBLANK 0
#define I_LCD_STAT 1
#define I_TIMER 2
#define I_SERIAL 3
#define I_JOYPAD 4

#define BIT_0(n) (n & 0b00000001)
#define BIT_7(n) (n & 0b10000000)

#define HI(n) (n & 0xF0)
#define LO(n) (n & 0x0F)

#define FLAG_SET(value, n) (value & 1 << n)

namespace Gameboy
{
    CPU::CPU(MMU *memory, Display *display) : memory(memory), display(display) {}

    unsigned int CPU::step()
    {
        unsigned int cycles = 0;

        // Fetch-decode-execute
        bool prefixed;
        Instruction instruction = fetch(prefixed);
        ExecuteResult result = prefixed ? decode_16bit(instruction) : decode_8bit(instruction);
        pc = result.next_pc;
        cycles += result.cycles;

        // Interrupts
        if (auto interrupt = check_interrupts()) {
            cycles += interrupt_service_routine(*interrupt);
        }

        return cycles;
    }

    CPU::Instruction CPU::fetch(bool &prefixed)
    {
        u8 inst = memory->read(pc);
        prefixed = inst == 0xCB;
        if (prefixed) {
            inst = memory->read(pc + 1);
        }
        return inst;
    }

    CPU::ExecuteResult CPU::decode_8bit(Instruction instruction)
    {
        switch (instruction) {
            case 0x00: return nop();
            case 0x01: return ld_rr_nn(bc);
            case 0x02: return ld_adr_r(bc, A);
            case 0x03: return inc_rr(bc);
            case 0x04: return inc_r(B);
            case 0x05: return dec_r(B);
            case 0x06: return ld_r_n(B);
            case 0x07: return rlca();
            case 0x08: return ld_nn_sp();
            case 0x09: return add_hl_rr(bc);
            case 0x0A: return ld_r_adr(A, bc);
            case 0x0B: return dec_rr(bc);
            case 0x0C: return inc_r(C);
            case 0x0D: return dec_r(C);
            case 0x0E: return ld_r_n(C);
            case 0x0F: return rrca();
            case 0x10: return stop();
            case 0x11: return ld_rr_nn(de);
            case 0x12: return ld_adr_r(de, A);
            case 0x13: return inc_rr(de);
            case 0x14: return inc_r(D);
            case 0x15: return dec_r(D);
            case 0x16: return ld_r_n(D);
            case 0x17: return rla();
            case 0x18: return jr_dd();
            case 0x19: return add_hl_rr(de);
            case 0x1A: return ld_r_adr(A, de);
            case 0x1B: return dec_rr(de);
            case 0x1C: return inc_r(E);
            case 0x1D: return dec_r(E);
            case 0x1E: return ld_r_n(E);
            case 0x1F: return rra();
            case 0x20: return jr_f_dd(FLAG_Z, false);
            case 0x21: return ld_rr_nn(hl);
            case 0x22: return ldi_hl_a();
            case 0x23: return inc_rr(hl);
            case 0x24: return inc_r(H);
            case 0x25: return dec_r(H);
            case 0x26: return ld_r_n(H);
            case 0x27: return daa();
            case 0x28: return jr_f_dd(FLAG_Z, true);
            case 0x29: return add_hl_rr(hl);
            case 0x2A: return ldi_a_hl();
            case 0x2B: return dec_rr(hl);
            case 0x2C: return inc_r(L);
            case 0x2D: return dec_r(L);
            case 0x2E: return ld_r_n(L);
            case 0x2F: return cpl();
            case 0x30: return jr_f_dd(FLAG_C, false);
            case 0x31: return ld_rr_nn(sp);
            case 0x32: return ldd_hl_a();
            case 0x33: return inc_rr(sp);
            case 0x34: return inc_hl();
            case 0x35: return dec_hl();
            case 0x36: return ld_hl_n();
            case 0x37: return scf();
            case 0x38: return jr_f_dd(FLAG_C, true);
            case 0x39: return add_hl_rr(sp);
            case 0x3A: return ldd_a_hl();
            case 0x3B: return dec_rr(sp);
            case 0x3C: return inc_r(A);
            case 0x3D: return dec_r(A);
            case 0x3E: return ld_r_n(A);
            case 0x3F: return ccf();
            case 0x40: return ld_r_r(B, B);
            case 0x41: return ld_r_r(B, C);
            case 0x42: return ld_r_r(B, D);
            case 0x43: return ld_r_r(B, E);
            case 0x44: return ld_r_r(B, H);
            case 0x45: return ld_r_r(B, L);
            case 0x46: return ld_r_adr(B, hl);
            case 0x47: return ld_r_r(B, A);
            case 0x48: return ld_r_r(C, B);
            case 0x49: return ld_r_r(C, C);
            case 0x4A: return ld_r_r(C, D);
            case 0x4B: return ld_r_r(C, E);
            case 0x4C: return ld_r_r(C, H);
            case 0x4D: return ld_r_r(C, L);
            case 0x4E: return ld_r_adr(C, hl);
            case 0x4F: return ld_r_r(C, A);
            case 0x50: return ld_r_r(D, B);
            case 0x51: return ld_r_r(D, C);
            case 0x52: return ld_r_r(D, D);
            case 0x53: return ld_r_r(D, E);
            case 0x54: return ld_r_r(D, H);
            case 0x55: return ld_r_r(D, L);
            case 0x56: return ld_r_adr(D, hl);
            case 0x57: return ld_r_r(D, A);
            case 0x58: return ld_r_r(E, B);
            case 0x59: return ld_r_r(E, C);
            case 0x5A: return ld_r_r(E, D);
            case 0x5B: return ld_r_r(E, E);
            case 0x5C: return ld_r_r(E, H);
            case 0x5D: return ld_r_r(E, L);
            case 0x5E: return ld_r_adr(E, hl);
            case 0x5F: return ld_r_r(E, A);
            case 0x60: return ld_r_r(H, B);
            case 0x61: return ld_r_r(H, C);
            case 0x62: return ld_r_r(H, D);
            case 0x63: return ld_r_r(H, E);
            case 0x64: return ld_r_r(H, H);
            case 0x65: return ld_r_r(H, L);
            case 0x66: return ld_r_adr(H, hl);
            case 0x67: return ld_r_r(H, A);
            case 0x68: return ld_r_r(L, B);
            case 0x69: return ld_r_r(L, C);
            case 0x6A: return ld_r_r(L, D);
            case 0x6B: return ld_r_r(L, E);
            case 0x6C: return ld_r_r(L, H);
            case 0x6D: return ld_r_r(L, L);
            case 0x6E: return ld_r_adr(L, hl);
            case 0x6F: return ld_r_r(L, A);
            case 0x70: return ld_adr_r(hl, B);
            case 0x71: return ld_adr_r(hl, C);
            case 0x72: return ld_adr_r(hl, D);
            case 0x73: return ld_adr_r(hl, E);
            case 0x74: return ld_adr_r(hl, H);
            case 0x75: return ld_adr_r(hl, L);
            case 0x76: return halt();
            case 0x77: return ld_adr_r(hl, A);
            case 0x78: return ld_r_r(A, B);
            case 0x79: return ld_r_r(A, C);
            case 0x7A: return ld_r_r(A, D);
            case 0x7B: return ld_r_r(A, E);
            case 0x7C: return ld_r_r(A, H);
            case 0x7D: return ld_r_r(A, L);
            case 0x7E: return ld_r_adr(A, hl);
            case 0x7F: return ld_r_r(A, A);
            case 0x80: return add_a_r(B);
            case 0x81: return add_a_r(C);
            case 0x82: return add_a_r(D);
            case 0x83: return add_a_r(E);
            case 0x84: return add_a_r(H);
            case 0x85: return add_a_r(L);
            case 0x86: return add_a_hl();
            case 0x87: return add_a_r(A);
            case 0x88: return adc_a_r(B);
            case 0x89: return adc_a_r(C);
            case 0x8A: return adc_a_r(D);
            case 0x8B: return adc_a_r(E);
            case 0x8C: return adc_a_r(H);
            case 0x8D: return adc_a_r(L);
            case 0x8E: return adc_a_hl();
            case 0x8F: return adc_a_r(A);
            case 0x90: return sub_a_r(B);
            case 0x91: return sub_a_r(C);
            case 0x92: return sub_a_r(D);
            case 0x93: return sub_a_r(E);
            case 0x94: return sub_a_r(H);
            case 0x95: return sub_a_r(L);
            case 0x96: return sub_a_hl();
            case 0x97: return sub_a_r(A);
            case 0x98: return sbc_a_r(B);
            case 0x99: return sbc_a_r(C);
            case 0x9A: return sbc_a_r(D);
            case 0x9B: return sbc_a_r(E);
            case 0x9C: return sbc_a_r(H);
            case 0x9D: return sbc_a_r(L);
            case 0x9E: return sbc_a_hl();
            case 0x9F: return sbc_a_r(A);
            case 0xA0: return and_a_r(B);
            case 0xA1: return and_a_r(C);
            case 0xA2: return and_a_r(D);
            case 0xA3: return and_a_r(E);
            case 0xA4: return and_a_r(H);
            case 0xA5: return and_a_r(L);
            case 0xA6: return and_a_hl();
            case 0xA7: return and_a_r(A);
            case 0xA8: return xor_a_r(B);
            case 0xA9: return xor_a_r(C);
            case 0xAA: return xor_a_r(D);
            case 0xAB: return xor_a_r(E);
            case 0xAC: return xor_a_r(H);
            case 0xAD: return xor_a_r(L);
            case 0xAE: return xor_a_hl();
            case 0xAF: return xor_a_r(A);
            case 0xB0: return or_a_r(B);
            case 0xB1: return or_a_r(C);
            case 0xB2: return or_a_r(D);
            case 0xB3: return or_a_r(E);
            case 0xB4: return or_a_r(H);
            case 0xB5: return or_a_r(L);
            case 0xB6: return or_a_hl();
            case 0xB7: return or_a_r(A);
            case 0xB8: return cp_a_r(B);
            case 0xB9: return cp_a_r(C);
            case 0xBA: return cp_a_r(D);
            case 0xBB: return cp_a_r(E);
            case 0xBC: return cp_a_r(H);
            case 0xBD: return cp_a_r(L);
            case 0xBE: return cp_a_hl();
            case 0xBF: return cp_a_r(A);
            case 0xC0: return ret_f(FLAG_Z, false);
            case 0xC1: return pop_rr(bc);
            case 0xC2: return jp_f_nn(FLAG_Z, false);
            case 0xC3: return jp_nn();
            case 0xC4: return call_f_nn(FLAG_Z, false);
            case 0xC5: return push_rr(bc);
            case 0xC6: return add_a_n();
            case 0xC7: return rst_n(0x00);
            case 0xC8: return ret_f(FLAG_Z, true);
            case 0xC9: return ret();
            case 0xCA: return jp_f_nn(FLAG_Z, true);
            case 0xCC: return call_f_nn(FLAG_Z, true);
            case 0xCD: return call_nn();
            case 0xCE: return adc_a_n();
            case 0xCF: return rst_n(0x08);
            case 0xD0: return ret_f(FLAG_C, false);
            case 0xD1: return pop_rr(de);
            case 0xD2: return jp_f_nn(FLAG_C, false);
            case 0xD4: return call_f_nn(FLAG_C, false);
            case 0xD5: return push_rr(de);
            case 0xD6: return sub_a_n();
            case 0xD7: return rst_n(0x10);
            case 0xD8: return ret_f(FLAG_C, true);
            case 0xD9: return reti();
            case 0xDA: return jp_f_nn(FLAG_C, true);
            case 0xDC: return call_f_nn(FLAG_C, true);
            case 0xDE: return sbc_a_n();
            case 0xDF: return rst_n(0x18);
            case 0xE0: return ld_n_a();
            case 0xE1: return pop_rr(hl);
            case 0xE2: return ld_c_a();
            case 0xE5: return push_rr(hl);
            case 0xE6: return and_a_n();
            case 0xE7: return rst_n(0x20);
            case 0xE8: return add_sp_dd();
            case 0xE9: return jp_hl();
            case 0xEA: return ld_nn_a();
            case 0xEE: return xor_a_n();
            case 0xEF: return rst_n(0x28);
            case 0xF0: return ld_a_n();
            case 0xF1: return pop_rr(af);
            case 0xF2: return ld_a_c();
            case 0xF3: return di();
            case 0xF5: return push_rr(af);
            case 0xF6: return or_a_n();
            case 0xF7: return rst_n(0x30);
            case 0xF8: return ld_hl_sp_dd();
            case 0xF9: return ld_sp_hl();
            case 0xFA: return ld_a_nn();
            case 0xFB: return ei();
            case 0xFE: return cp_a_n();
            case 0xFF: return rst_n(0x38);
        }
        return nop();
    }

    CPU::ExecuteResult CPU::decode_16bit(Instruction instruction)
    {
        switch (instruction) {
            case 0x00: return rlc_r(B);
            case 0x01: return rlc_r(C);
            case 0x02: return rlc_r(D);
            case 0x03: return rlc_r(E);
            case 0x04: return rlc_r(H);
            case 0x05: return rlc_r(L);
            case 0x06: return rlc_hl();
            case 0x07: return rlc_r(A);
            case 0x08: return rrc_r(B);
            case 0x09: return rrc_r(C);
            case 0x0A: return rrc_r(D);
            case 0x0B: return rrc_r(E);
            case 0x0C: return rrc_r(H);
            case 0x0D: return rrc_r(L);
            case 0x0E: return rrc_hl();
            case 0x0F: return rrc_r(A);
            case 0x10: return rl_r(B);
            case 0x11: return rl_r(C);
            case 0x12: return rl_r(D);
            case 0x13: return rl_r(E);
            case 0x14: return rl_r(H);
            case 0x15: return rl_r(L);
            case 0x16: return rl_hl();
            case 0x17: return rl_r(A);
            case 0x18: return rr_r(B);
            case 0x19: return rr_r(C);
            case 0x1A: return rr_r(D);
            case 0x1B: return rr_r(E);
            case 0x1C: return rr_r(H);
            case 0x1D: return rr_r(L);
            case 0x1E: return rr_hl();
            case 0x1F: return rr_r(A);
            case 0x20: return sla_r(B);
            case 0x21: return sla_r(C);
            case 0x22: return sla_r(D);
            case 0x23: return sla_r(E);
            case 0x24: return sla_r(H);
            case 0x25: return sla_r(L);
            case 0x26: return sla_hl();
            case 0x27: return sla_r(A);
            case 0x28: return sra_r(B);
            case 0x29: return sra_r(C);
            case 0x2A: return sra_r(D);
            case 0x2B: return sra_r(E);
            case 0x2C: return sra_r(H);
            case 0x2D: return sra_r(L);
            case 0x2E: return sra_hl();
            case 0x2F: return sra_r(A);
            case 0x30: return swap_r(B);
            case 0x31: return swap_r(C);
            case 0x32: return swap_r(D);
            case 0x33: return swap_r(E);
            case 0x34: return swap_r(H);
            case 0x35: return swap_r(L);
            case 0x36: return swap_hl();
            case 0x37: return swap_r(A);
            case 0x38: return srl_r(B);
            case 0x39: return srl_r(C);
            case 0x3A: return srl_r(D);
            case 0x3B: return srl_r(E);
            case 0x3C: return srl_r(H);
            case 0x3D: return srl_r(L);
            case 0x3E: return srl_hl();
            case 0x3F: return srl_r(A);
            case 0x40: return bit_n_r(0, B);
            case 0x41: return bit_n_r(0, C);
            case 0x42: return bit_n_r(0, D);
            case 0x43: return bit_n_r(0, E);
            case 0x44: return bit_n_r(0, H);
            case 0x45: return bit_n_r(0, L);
            case 0x46: return bit_n_hl(0);
            case 0x47: return bit_n_r(0, A);
            case 0x48: return bit_n_r(1, B);
            case 0x49: return bit_n_r(1, C);
            case 0x4A: return bit_n_r(1, D);
            case 0x4B: return bit_n_r(1, E);
            case 0x4C: return bit_n_r(1, H);
            case 0x4D: return bit_n_r(1, L);
            case 0x4E: return bit_n_hl(1);
            case 0x4F: return bit_n_r(1, A);
            case 0x50: return bit_n_r(2, B);
            case 0x51: return bit_n_r(2, C);
            case 0x52: return bit_n_r(2, D);
            case 0x53: return bit_n_r(2, E);
            case 0x54: return bit_n_r(2, H);
            case 0x55: return bit_n_r(2, L);
            case 0x56: return bit_n_hl(2);
            case 0x57: return bit_n_r(2, A);
            case 0x58: return bit_n_r(3, B);
            case 0x59: return bit_n_r(3, C);
            case 0x5A: return bit_n_r(3, D);
            case 0x5B: return bit_n_r(3, E);
            case 0x5C: return bit_n_r(3, H);
            case 0x5D: return bit_n_r(3, L);
            case 0x5E: return bit_n_hl(3);
            case 0x5F: return bit_n_r(3, A);
            case 0x60: return bit_n_r(4, B);
            case 0x61: return bit_n_r(4, C);
            case 0x62: return bit_n_r(4, D);
            case 0x63: return bit_n_r(4, E);
            case 0x64: return bit_n_r(4, H);
            case 0x65: return bit_n_r(4, L);
            case 0x66: return bit_n_hl(4);
            case 0x67: return bit_n_r(4, A);
            case 0x68: return bit_n_r(5, B);
            case 0x69: return bit_n_r(5, C);
            case 0x6A: return bit_n_r(5, D);
            case 0x6B: return bit_n_r(5, E);
            case 0x6C: return bit_n_r(5, H);
            case 0x6D: return bit_n_r(5, L);
            case 0x6E: return bit_n_hl(5);
            case 0x6F: return bit_n_r(5, A);
            case 0x70: return bit_n_r(6, B);
            case 0x71: return bit_n_r(6, C);
            case 0x72: return bit_n_r(6, D);
            case 0x73: return bit_n_r(6, E);
            case 0x74: return bit_n_r(6, H);
            case 0x75: return bit_n_r(6, L);
            case 0x76: return bit_n_hl(6);
            case 0x77: return bit_n_r(6, A);
            case 0x78: return bit_n_r(7, B);
            case 0x79: return bit_n_r(7, C);
            case 0x7A: return bit_n_r(7, D);
            case 0x7B: return bit_n_r(7, E);
            case 0x7C: return bit_n_r(7, H);
            case 0x7D: return bit_n_r(7, L);
            case 0x7E: return bit_n_hl(7);
            case 0x7F: return bit_n_r(7, A);
            case 0x80: return res_n_r(0, B);
            case 0x81: return res_n_r(0, C);
            case 0x82: return res_n_r(0, D);
            case 0x83: return res_n_r(0, E);
            case 0x84: return res_n_r(0, H);
            case 0x85: return res_n_r(0, L);
            case 0x86: return res_n_hl(0);
            case 0x87: return res_n_r(0, A);
            case 0x88: return res_n_r(1, B);
            case 0x89: return res_n_r(1, C);
            case 0x8A: return res_n_r(1, D);
            case 0x8B: return res_n_r(1, E);
            case 0x8C: return res_n_r(1, H);
            case 0x8D: return res_n_r(1, L);
            case 0x8E: return res_n_hl(1);
            case 0x8F: return res_n_r(1, A);
            case 0x90: return res_n_r(2, B);
            case 0x91: return res_n_r(2, C);
            case 0x92: return res_n_r(2, D);
            case 0x93: return res_n_r(2, E);
            case 0x94: return res_n_r(2, H);
            case 0x95: return res_n_r(2, L);
            case 0x96: return res_n_hl(2);
            case 0x97: return res_n_r(2, A);
            case 0x98: return res_n_r(3, B);
            case 0x99: return res_n_r(3, C);
            case 0x9A: return res_n_r(3, D);
            case 0x9B: return res_n_r(3, E);
            case 0x9C: return res_n_r(3, H);
            case 0x9D: return res_n_r(3, L);
            case 0x9E: return res_n_hl(3);
            case 0x9F: return res_n_r(3, A);
            case 0xA0: return res_n_r(4, B);
            case 0xA1: return res_n_r(4, C);
            case 0xA2: return res_n_r(4, D);
            case 0xA3: return res_n_r(4, E);
            case 0xA4: return res_n_r(4, H);
            case 0xA5: return res_n_r(4, L);
            case 0xA6: return res_n_hl(4);
            case 0xA7: return res_n_r(4, A);
            case 0xA8: return res_n_r(5, B);
            case 0xA9: return res_n_r(5, C);
            case 0xAA: return res_n_r(5, D);
            case 0xAB: return res_n_r(5, E);
            case 0xAC: return res_n_r(5, H);
            case 0xAD: return res_n_r(5, L);
            case 0xAE: return res_n_hl(5);
            case 0xAF: return res_n_r(5, A);
            case 0xB0: return res_n_r(6, B);
            case 0xB1: return res_n_r(6, C);
            case 0xB2: return res_n_r(6, D);
            case 0xB3: return res_n_r(6, E);
            case 0xB4: return res_n_r(6, H);
            case 0xB5: return res_n_r(6, L);
            case 0xB6: return res_n_hl(6);
            case 0xB7: return res_n_r(6, A);
            case 0xB8: return res_n_r(7, B);
            case 0xB9: return res_n_r(7, C);
            case 0xBA: return res_n_r(7, D);
            case 0xBB: return res_n_r(7, E);
            case 0xBC: return res_n_r(7, H);
            case 0xBD: return res_n_r(7, L);
            case 0xBE: return res_n_hl(7);
            case 0xBF: return res_n_r(7, A);
            case 0xC0: return set_n_r(0, B);
            case 0xC1: return set_n_r(0, C);
            case 0xC2: return set_n_r(0, D);
            case 0xC3: return set_n_r(0, E);
            case 0xC4: return set_n_r(0, H);
            case 0xC5: return set_n_r(0, L);
            case 0xC6: return set_n_hl(0);
            case 0xC7: return set_n_r(0, A);
            case 0xC8: return set_n_r(1, B);
            case 0xC9: return set_n_r(1, C);
            case 0xCA: return set_n_r(1, D);
            case 0xCB: return set_n_r(1, E);
            case 0xCC: return set_n_r(1, H);
            case 0xCD: return set_n_r(1, L);
            case 0xCE: return set_n_hl(1);
            case 0xCF: return set_n_r(1, A);
            case 0xD0: return set_n_r(2, B);
            case 0xD1: return set_n_r(2, C);
            case 0xD2: return set_n_r(2, D);
            case 0xD3: return set_n_r(2, E);
            case 0xD4: return set_n_r(2, H);
            case 0xD5: return set_n_r(2, L);
            case 0xD6: return set_n_hl(2);
            case 0xD7: return set_n_r(2, A);
            case 0xD8: return set_n_r(3, B);
            case 0xD9: return set_n_r(3, C);
            case 0xDA: return set_n_r(3, D);
            case 0xDB: return set_n_r(3, E);
            case 0xDC: return set_n_r(3, H);
            case 0xDD: return set_n_r(3, L);
            case 0xDE: return set_n_hl(3);
            case 0xDF: return set_n_r(3, A);
            case 0xE0: return set_n_r(4, B);
            case 0xE1: return set_n_r(4, C);
            case 0xE2: return set_n_r(4, D);
            case 0xE3: return set_n_r(4, E);
            case 0xE4: return set_n_r(4, H);
            case 0xE5: return set_n_r(4, L);
            case 0xE6: return set_n_hl(4);
            case 0xE7: return set_n_r(4, A);
            case 0xE8: return set_n_r(5, B);
            case 0xE9: return set_n_r(5, C);
            case 0xEA: return set_n_r(5, D);
            case 0xEB: return set_n_r(5, E);
            case 0xEC: return set_n_r(5, H);
            case 0xED: return set_n_r(5, L);
            case 0xEE: return set_n_hl(5);
            case 0xEF: return set_n_r(5, A);
            case 0xF0: return set_n_r(6, B);
            case 0xF1: return set_n_r(6, C);
            case 0xF2: return set_n_r(6, D);
            case 0xF3: return set_n_r(6, E);
            case 0xF4: return set_n_r(6, H);
            case 0xF5: return set_n_r(6, L);
            case 0xF6: return set_n_hl(6);
            case 0xF7: return set_n_r(6, A);
            case 0xF8: return set_n_r(7, B);
            case 0xF9: return set_n_r(7, C);
            case 0xFA: return set_n_r(7, D);
            case 0xFB: return set_n_r(7, E);
            case 0xFC: return set_n_r(7, H);
            case 0xFD: return set_n_r(7, L);
            case 0xFE: return set_n_hl(7);
            case 0xFF: return set_n_r(7, A);
        }
        return nop();
    }

    std::optional<u8> CPU::check_interrupts() 
    {
        // Check interrupt master enable
        if (!ime) {
            return std::nullopt;
        }

        // Check each interrupt bit
        u8 interrupt_enable = memory->read(0xFFFF);
        u8 interrupt_flag = memory->read(0xFF0F);
        for (u8 i = 0; i <= I_JOYPAD; i++) {
            if (FLAG_SET(interrupt_enable, i) && FLAG_SET(interrupt_flag, i)) {
                return i;
            }
        }
        return std::nullopt;
    }

    unsigned int CPU::interrupt_service_routine(u8 interrupt)
    {
        // Acknowledge interupt
        u8 interrupt_flag = memory->read(0xFF0F);
        interrupt_flag &= ~(1 << interrupt);
        memory->write(0xFF0F, interrupt_flag);

        // Disable further interupt handling
        ime = false;

        // Write PC to stack
        memory->write(sp - 1, pc.hi());
        memory->write(sp - 2, pc.lo());
        sp = sp - 2;

        // Jump to interrupt handler
        Address interrupt_handler = 0x40 + 8 * interrupt;
        pc = interrupt_handler;

        return 20;
    }

    CPU::ExecuteResult CPU::ld_r_r(Register8 &dst, Register8 src)
    {
        dst = src;
        return {static_cast<Address>(pc + 1), 4};
    }

    CPU::ExecuteResult CPU::ld_r_n(Register8 &dst)
    {
        u8 value = read_next_8();
        dst = value;
        return {static_cast<Address>(pc + 2), 8};
    }

    CPU::ExecuteResult CPU::ld_r_adr(Register8 &dst, Address src)
    {
        dst = memory->read(src);
        return {static_cast<Address>(pc + 1), 8};
    }

    CPU::ExecuteResult CPU::ld_adr_r(Address dst, Register8 src)
    {
        memory->write(dst, src);
        return {static_cast<Address>(pc + 1), 8};
    }

    CPU::ExecuteResult CPU::ld_hl_n()
    {
        u8 value = read_next_8();
        memory->write(hl, value);
        return {static_cast<Address>(pc + 2), 12};
    }

    CPU::ExecuteResult CPU::ld_a_nn()
    {
        Address addr = read_next_16();
        A = memory->read(addr);
        return {static_cast<Address>(pc + 3), 16};
    }

    CPU::ExecuteResult CPU::ld_nn_a()
    {
        Address addr = read_next_16();
        memory->write(addr, A);
        return {static_cast<Address>(pc + 3), 16};
    }

    CPU::ExecuteResult CPU::ld_a_n()
    {
        u8 offset = read_next_8();
        A = memory->read_io(offset);
        return {static_cast<Address>(pc + 2), 12};
    }

    CPU::ExecuteResult CPU::ld_n_a()
    {
        u8 offset = read_next_8();
        memory->write_io(offset, A);
        return {static_cast<Address>(pc + 2), 12};
    }

    CPU::ExecuteResult CPU::ld_a_c()
    {
        A = memory->read_io(C);
        return {static_cast<Address>(pc + 1), 8};
    }

    CPU::ExecuteResult CPU::ld_c_a()
    {
        memory->write_io(C, A);
        return {static_cast<Address>(pc + 1), 8};
    }

    CPU::ExecuteResult CPU::ldi_hl_a()
    {
        memory->write(hl, A);
        hl = hl + 1;
        return {static_cast<Address>(pc + 1), 8};
    }

    CPU::ExecuteResult CPU::ldi_a_hl()
    {
        A = memory->read(hl);
        hl = hl + 1;
        return {static_cast<Address>(pc + 1), 8};
    }

    CPU::ExecuteResult CPU::ldd_hl_a()
    {
        memory->write(hl, A);
        hl = hl - 1;
        return {static_cast<Address>(pc + 1), 8};
    }

    CPU::ExecuteResult CPU::ldd_a_hl()
    {
        A = memory->read(hl);
        hl = hl - 1;
        return {static_cast<Address>(pc + 1), 8};
    }

    CPU::ExecuteResult CPU::ld_rr_nn(Register16 &dst)
    {
        u16 value = read_next_16();
        dst = value;
        Address next = static_cast<Address>(pc + 3);
        return {static_cast<Address>(pc + 3), 12};
    }

    CPU::ExecuteResult CPU::ld_nn_sp()
    {
        Address addr = read_next_16();
        memory->write(addr, sp.lo());
        memory->write(addr + 1, sp.hi());
        return {static_cast<Address>(pc + 3), 20};
    }

    CPU::ExecuteResult CPU::ld_sp_hl()
    {
        sp = hl;
        return {static_cast<Address>(pc + 1), 8};
    }

    CPU::ExecuteResult CPU::push_rr(Register16 src)
    {
        memory->write(sp - 1, src.hi());
        memory->write(sp - 2, src.lo());
        sp = sp - 2;
        return {static_cast<Address>(pc + 1), 16};
    }

    CPU::ExecuteResult CPU::pop_rr(Register16 &dst)
    {
        dst.lo() = memory->read(sp);
        dst.hi() = memory->read(sp + 1);
        sp = sp + 2;
        F &= 0xF0;
        return {static_cast<Address>(pc + 1), 12};
    }

    CPU::ExecuteResult CPU::add_a_r(Register8 op)
    {
        A = add_f(A, op);
        return {static_cast<Address>(pc + 1), 4};
    }

    CPU::ExecuteResult CPU::add_a_n()
    {
        u8 n = read_next_8();
        A = add_f(A, n);
        return {static_cast<Address>(pc + 2), 8};
    }

    CPU::ExecuteResult CPU::add_a_hl()
    {
        u8 op = memory->read(hl);
        A = add_f(A, op);
        return {static_cast<Address>(pc + 1), 8};
    }

    CPU::ExecuteResult CPU::adc_a_r(Register8 op)
    {
        A = adc_f(A, op);
        return {static_cast<Address>(pc + 1), 4};
    }

    CPU::ExecuteResult CPU::adc_a_n()
    {
        u8 n = read_next_8();
        A = adc_f(A, n);
        return {static_cast<Address>(pc + 2), 8};
    }

    CPU::ExecuteResult CPU::adc_a_hl()
    {
        u8 op = memory->read(hl);
        A = adc_f(A, op);
        return {static_cast<Address>(pc + 1), 8};
    }

    CPU::ExecuteResult CPU::sub_a_r(Register8 op)
    {
        A = sub_f(A, op);
        return {static_cast<Address>(pc + 1), 4};
    }

    CPU::ExecuteResult CPU::sub_a_n()
    {
        u8 n = read_next_8();
        A = sub_f(A, n);
        return {static_cast<Address>(pc + 2), 8};
    }

    CPU::ExecuteResult CPU::sub_a_hl()
    {
        u8 op = memory->read(hl);
        A = sub_f(A, op);
        return {static_cast<Address>(pc + 1), 8};
    }

    CPU::ExecuteResult CPU::sbc_a_r(Register8 op)
    {
        A = sbc_f(A, op);
        return {static_cast<Address>(pc + 1), 4};
    }

    CPU::ExecuteResult CPU::sbc_a_n()
    {
        u8 n = read_next_8();
        A = sbc_f(A, n);
        return {static_cast<Address>(pc + 2), 8};
    }

    CPU::ExecuteResult CPU::sbc_a_hl()
    {
        u8 op = memory->read(hl);
        A = sbc_f(A, op);
        return {static_cast<Address>(pc + 1), 8};
    }

    CPU::ExecuteResult CPU::and_a_r(Register8 op)
    {
        A = and_f(A, op);
        return {static_cast<Address>(pc + 1), 4};
    }

    CPU::ExecuteResult CPU::and_a_n()
    {
        u8 n = read_next_8();
        A = and_f(A, n);
        return {static_cast<Address>(pc + 2), 8};
    }

    CPU::ExecuteResult CPU::and_a_hl()
    {
        u8 op = memory->read(hl);
        A = and_f(A, op);
        return {static_cast<Address>(pc + 1), 8};
    }

    CPU::ExecuteResult CPU::xor_a_r(Register8 op)
    {
        A = xor_f(A, op);
        return {static_cast<Address>(pc + 1), 4};
    }

    CPU::ExecuteResult CPU::xor_a_n()
    {
        u8 n = read_next_8();
        A = xor_f(A, n);
        return {static_cast<Address>(pc + 2), 8};
    }

    CPU::ExecuteResult CPU::xor_a_hl()
    {
        u8 op = memory->read(hl);
        A = xor_f(A, op);
        return {static_cast<Address>(pc + 1), 8};
    }

    CPU::ExecuteResult CPU::or_a_r(Register8 op)
    {
        A = or_f(A, op);
        return {static_cast<Address>(pc + 1), 4};
    }

    CPU::ExecuteResult CPU::or_a_n()
    {
        u8 n = read_next_8();
        A = or_f(A, n);
        return {static_cast<Address>(pc + 2), 8};
    }

    CPU::ExecuteResult CPU::or_a_hl()
    {
        u8 op = memory->read(hl);
        A = or_f(A, op);
        return {static_cast<Address>(pc + 1), 8};
    }

    CPU::ExecuteResult CPU::cp_a_r(Register8 op)
    {
        cp_f(A, op);
        return {static_cast<Address>(pc + 1), 4};
    }

    CPU::ExecuteResult CPU::cp_a_n()
    {
        u8 n = read_next_8();
        cp_f(A, n);
        return {static_cast<Address>(pc + 2), 8};
    }

    CPU::ExecuteResult CPU::cp_a_hl()
    {
        u8 op = memory->read(hl);
        cp_f(A, op);
        return {static_cast<Address>(pc + 1), 8};
    }

    CPU::ExecuteResult CPU::inc_r(Register8 &reg)
    {
        reg = inc_f(reg);
        return {static_cast<Address>(pc + 1), 4};
    }

    CPU::ExecuteResult CPU::inc_hl()
    {
        u8 op = memory->read(hl);
        memory->write(hl, inc_f(op));
        return {static_cast<Address>(pc + 1), 12};
    }

    CPU::ExecuteResult CPU::dec_r(Register8 &reg)
    {
        reg = dec_f(reg);
        return {static_cast<Address>(pc + 1), 4};
    }

    CPU::ExecuteResult CPU::dec_hl()
    {
        u8 op = memory->read(hl);
        memory->write(hl, dec_f(op));
        return {static_cast<Address>(pc + 1), 12};
    }

    CPU::ExecuteResult CPU::daa()
    {
        if (get_flag_n()) {
            if (get_flag_c()) {
                A -= 0x60;
            }
            if (get_flag_h()) {
                A -= 0x6;
            }
        } else {
            if (get_flag_c() || A > 0x99) {
                A += 0x60;
                set_flag_c(1);
            }
            if (get_flag_h() || LO(A) > 0x09) {
                A += 0x6;
            }
        }
        set_flag_z(A == 0);
        set_flag_h(false);
        return {static_cast<Address>(pc + 1), 4};
    }

    CPU::ExecuteResult CPU::cpl()
    {
        A ^= 0xFF;
        set_flag_n(true);
        set_flag_h(true);
        return {static_cast<Address>(pc + 1), 4};
    }

    CPU::ExecuteResult CPU::add_hl_rr(Register16 rr)
    {
        hl = add_f(hl, rr);
        return {static_cast<Address>(pc + 1), 8};
    }

    CPU::ExecuteResult CPU::inc_rr(Register16 &rr)
    {
        rr = rr + 1;
        return {static_cast<Address>(pc + 1), 8};
    }

    CPU::ExecuteResult CPU::dec_rr(Register16 &rr)
    {
        rr = rr - 1;
        return {static_cast<Address>(pc + 1), 8};
    }

    CPU::ExecuteResult CPU::add_sp_dd()
    {
        u8 op = read_next_8();
        bool sign = BIT_7(op);
        sp.lo() = add_f(sp.lo(), op);
        if (get_flag_c() && !sign) {
            sp.hi() += 1;
        }
        if (!get_flag_c() && sign) {
            sp.hi() -= 1;
        }
        set_flag_z(false);
        set_flag_n(false);
        return {static_cast<Address>(pc + 2), 16};
    }

    CPU::ExecuteResult CPU::ld_hl_sp_dd()
    {
        u8 op = read_next_8();
        bool sign = op & 0x80;
        hl = sp;
        hl.lo() = add_f(hl.lo(), op);
        if (get_flag_c() && !sign) {
            hl.hi() += 1;
        }
        if (!get_flag_c() && sign) {
            hl.hi() -= 1;
        }
        set_flag_z(false);
        set_flag_n(false);
        return {static_cast<Address>(pc + 2), 12};
    }

    CPU::ExecuteResult CPU::rlca()
    {
        set_flag_z(false);
        set_flag_n(false);
        set_flag_h(false);
        set_flag_c(BIT_7(A));
        A = (A << 1) | BIT_7(A) >> 7;
        return {static_cast<Address>(pc + 1), 4};
    }

    CPU::ExecuteResult CPU::rla()
    {
        u8 carry = get_flag_c();
        set_flag_z(false);
        set_flag_n(false);
        set_flag_h(false);
        set_flag_c(BIT_7(A));
        A = (A << 1) | carry;
        return {static_cast<Address>(pc + 1), 4};
    }

    CPU::ExecuteResult CPU::rrca()
    {
        set_flag_z(false);
        set_flag_n(false);
        set_flag_h(false);
        set_flag_c(BIT_0(A));
        A = (A >> 1) | BIT_0(A) << 7;
        return {static_cast<Address>(pc + 1), 4};
    }

    CPU::ExecuteResult CPU::rra()
    {
        u8 carry = get_flag_c();
        set_flag_z(false);
        set_flag_n(false);
        set_flag_h(false);
        set_flag_c(BIT_0(A));
        A = (A >> 1) | (carry << 7);
        return {static_cast<Address>(pc + 1), 4};
    }

    CPU::ExecuteResult CPU::rlc_r(Register8 &r)
    {
        set_flag_n(false);
        set_flag_h(false);
        set_flag_c(BIT_7(r));
        r = (r << 1) | BIT_7(r) >> 7;
        set_flag_z(r == 0);
        return {static_cast<Address>(pc + 2), 8};
    }

    CPU::ExecuteResult CPU::rlc_hl()
    {
        u8 op = memory->read(hl);
        set_flag_n(false);
        set_flag_h(false);
        set_flag_c(BIT_7(op));
        op = (op << 1) | BIT_7(op) >> 7;
        set_flag_z(op == 0);
        memory->write(hl, op);
        return {static_cast<Address>(pc + 2), 16};
    }

    CPU::ExecuteResult CPU::rl_r(Register8 &r)
    {
        u8 carry = get_flag_c();
        set_flag_n(false);
        set_flag_h(false);
        set_flag_c(BIT_7(r));
        r = (r << 1) | carry;
        set_flag_z(r == 0);
        return {static_cast<Address>(pc + 2), 8};
    }

    CPU::ExecuteResult CPU::rl_hl()
    {
        u8 op = memory->read(hl);
        u8 carry = get_flag_c();
        set_flag_n(false);
        set_flag_h(false);
        set_flag_c(BIT_7(op));
        op = (op << 1) | carry;
        set_flag_z(op == 0);
        memory->write(hl, op);
        return {static_cast<Address>(pc + 2), 16};
    }

    CPU::ExecuteResult CPU::rrc_r(Register8 &r)
    {
        set_flag_n(false);
        set_flag_h(false);
        set_flag_c(BIT_0(r));
        r = (r >> 1) | BIT_0(r) << 7;
        set_flag_z(r == 0);
        return {static_cast<Address>(pc + 2), 8};
    }

    CPU::ExecuteResult CPU::rrc_hl()
    {
        u8 op = memory->read(hl);
        set_flag_n(false);
        set_flag_h(false);
        set_flag_c(BIT_0(op));
        op = (op >> 1) | BIT_0(op) << 7;
        set_flag_z(op == 0);
        memory->write(hl, op);
        return {static_cast<Address>(pc + 2), 16};
    }

    CPU::ExecuteResult CPU::rr_r(Register8 &r)
    {
        u8 carry = get_flag_c();
        set_flag_n(false);
        set_flag_h(false);
        set_flag_c(BIT_0(r));
        r = (r >> 1) | (carry << 7);
        set_flag_z(r == 0);
        return {static_cast<Address>(pc + 2), 8};
    }

    CPU::ExecuteResult CPU::rr_hl()
    {
        u8 op = memory->read(hl);
        u8 carry = get_flag_c();
        set_flag_n(false);
        set_flag_h(false);
        set_flag_c(BIT_0(op));
        op = (op >> 1) | (carry << 7);
        set_flag_z(op == 0);
        memory->write(hl, op);
        return {static_cast<Address>(pc + 2), 16};
    }

    CPU::ExecuteResult CPU::sla_r(Register8 &r)
    {
        set_flag_n(false);
        set_flag_h(false);
        set_flag_c(BIT_7(r));
        r = r << 1;
        set_flag_z(r == 0);
        return {static_cast<Address>(pc + 2), 8};
    }

    CPU::ExecuteResult CPU::sla_hl()
    {
        u8 op = memory->read(hl);
        set_flag_n(false);
        set_flag_h(false);
        set_flag_c(BIT_7(op));
        op = op << 1;
        set_flag_z(op == 0);
        memory->write(hl, op);
        return {static_cast<Address>(pc + 2), 16};
    }

    CPU::ExecuteResult CPU::sra_r(Register8 &r)
    {
        set_flag_n(false);
        set_flag_h(false);
        set_flag_c(BIT_0(r));
        r = r >> 1 | BIT_7(r);
        set_flag_z(r == 0);
        return {static_cast<Address>(pc + 2), 8};
    }

    CPU::ExecuteResult CPU::sra_hl()
    {
        u8 op = memory->read(hl);
        set_flag_n(false);
        set_flag_h(false);
        set_flag_c(BIT_0(op));
        op = op >> 1 | BIT_7(op);
        set_flag_z(op == 0);
        memory->write(hl, op);
        return {static_cast<Address>(pc + 2), 16};
    }

    CPU::ExecuteResult CPU::srl_r(Register8 &r)
    {
        set_flag_n(false);
        set_flag_h(false);
        set_flag_c(BIT_0(r));
        r = r >> 1;
        set_flag_z(r == 0);
        return {static_cast<Address>(pc + 2), 8};
    }

    CPU::ExecuteResult CPU::srl_hl()
    {
        u8 op = memory->read(hl);
        set_flag_n(false);
        set_flag_h(false);
        set_flag_c(BIT_0(op));
        op = op >> 1;
        set_flag_z(op == 0);
        memory->write(hl, op);
        return {static_cast<Address>(pc + 2), 16};
    }

    CPU::ExecuteResult CPU::swap_r(Register8 &r)
    {
        u8 hi = HI(r);
        r = r << 4 | hi >> 4;
        set_flag_z(r == 0);
        set_flag_n(false);
        set_flag_h(false);
        set_flag_c(false);
        return {static_cast<Address>(pc + 2), 8};
    }

    CPU::ExecuteResult CPU::swap_hl()
    {
        u8 op = memory->read(hl);
        u8 hi = HI(op);
        op = op << 4 | hi >> 4;
        memory->write(hl, op);
        set_flag_z(op == 0);
        set_flag_n(false);
        set_flag_h(false);
        set_flag_c(false);
        return {static_cast<Address>(pc + 2), 16};
    }

    CPU::ExecuteResult CPU::bit_n_r(u8 n, Register8 r)
    {
        bool bit = (r >> n) & 1;
        set_flag_z(!bit);
        set_flag_n(false);
        set_flag_h(true);
        return {static_cast<Address>(pc + 2), 8};
    }

    CPU::ExecuteResult CPU::bit_n_hl(u8 n)
    {
        u8 op = memory->read(hl);
        bool bit = (op >> n) & 1;
        set_flag_z(!bit);
        set_flag_n(false);
        set_flag_h(true);
        return {static_cast<Address>(pc + 2), 12};
    }

    CPU::ExecuteResult CPU::set_n_r(u8 n, Register8 &r)
    {
        r |= 1 << n;
        return {static_cast<Address>(pc + 2), 8};
    }

    CPU::ExecuteResult CPU::set_n_hl(u8 n)
    {
        u8 op = memory->read(hl);
        op |= 1 << n;
        memory->write(hl, op);
        return {static_cast<Address>(pc + 2), 16};
    }

    CPU::ExecuteResult CPU::res_n_r(u8 n, Register8 &r)
    {
        r &= ~(1 << n);
        return {static_cast<Address>(pc + 2), 8};
    }

    CPU::ExecuteResult CPU::res_n_hl(u8 n)
    {
        u8 op = memory->read(hl);
        op &= ~(1 << n);
        memory->write(hl, op);
        return {static_cast<Address>(pc + 2), 16};
    }

    CPU::ExecuteResult CPU::ccf()
    {
        set_flag_n(false);
        set_flag_h(false);
        set_flag_c(get_flag_c() ^ 1);
        return {static_cast<Address>(pc + 1), 4};
    }

    CPU::ExecuteResult CPU::scf()
    {
        set_flag_n(false);
        set_flag_h(false);
        set_flag_c(true);
        return {static_cast<Address>(pc + 1), 4};
    }

    CPU::ExecuteResult CPU::nop() { return {static_cast<Address>(pc + 1), 4}; }

    CPU::ExecuteResult CPU::halt()
    {
        halted = true;
        return {static_cast<Address>(pc + 1), 4};
    }

    CPU::ExecuteResult CPU::stop()
    {
        halted = true;
        return {static_cast<Address>(pc + 1), 4};
    }

    CPU::ExecuteResult CPU::di()
    {
        ime = false;
        return {static_cast<Address>(pc + 1), 4};
    }

    CPU::ExecuteResult CPU::ei()
    {
        ime = true;
        return {static_cast<Address>(pc + 1), 4};
    }

    CPU::ExecuteResult CPU::jp_nn()
    {
        u16 nn = read_next_16();
        return {nn, 16};
    }

    CPU::ExecuteResult CPU::jp_hl() { return {hl, 4}; }

    CPU::ExecuteResult CPU::jp_f_nn(u8 flag, bool value)
    {
        if (get_flag(flag) == value) {
            return jp_nn();
        }
        return {static_cast<Address>(pc + 3), 12};
    }

    CPU::ExecuteResult CPU::jr_dd()
    {
        i8 dd = read_next_8();
        return {static_cast<Address>(pc + 2 + dd), 12};
    }

    CPU::ExecuteResult CPU::jr_f_dd(u8 flag, bool value)
    {
        if (get_flag(flag) == value) {
            return jr_dd();
        }
        return {static_cast<Address>(pc + 2), 8};
    }

    CPU::ExecuteResult CPU::call_nn()
    {
        Register16 return_pc = pc + 3;
        memory->write(sp - 1, return_pc.hi());
        memory->write(sp - 2, return_pc.lo());
        sp = sp - 2;

        u16 nn = read_next_16();
        return {nn, 24};
    }

    CPU::ExecuteResult CPU::call_f_nn(u8 flag, bool value)
    {
        if (get_flag(flag) == value) {
            return call_nn();
        }
        return {static_cast<Address>(pc + 3), 12};
    }

    CPU::ExecuteResult CPU::ret()
    {
        Register16 return_pc;
        return_pc.lo() = memory->read(sp);
        return_pc.hi() = memory->read(sp + 1);
        sp = sp + 2;
        return {return_pc, 16};
    }

    CPU::ExecuteResult CPU::ret_f(u8 flag, bool value)
    {
        if (get_flag(flag) == value) {
            Register16 return_pc;
            return_pc.lo() = memory->read(sp);
            return_pc.hi() = memory->read(sp + 1);
            sp = sp + 2;
            return {return_pc, 20};
        }
        return {static_cast<Address>(pc + 1), 8};
    }

    CPU::ExecuteResult CPU::reti()
    {
        ime = true;
        return ret();
    }

    CPU::ExecuteResult CPU::rst_n(u8 n)
    {
        Register16 return_pc = pc + 1;
        memory->write(sp - 1, return_pc.hi());
        memory->write(sp - 2, return_pc.lo());
        sp = sp - 2;
        return {n, 24};
    }

    u8 CPU::read_next_8() const { return memory->read(pc + 1); }

    u16 CPU::read_next_16() const
    {
        return (u16)memory->read(pc + 1) | (u16)memory->read(pc + 2) << 8;
    }

    u8 CPU::add_f(u8 a, u8 b)
    {
        u16 res = (u16)a + (u16)b;
        set_flag_z((u8)res == 0);
        set_flag_n(false);
        set_flag_h(LO(a) + LO(b) > 0xF);
        set_flag_c(res > 0xFF);
        return (u8)res;
    }

    u16 CPU::add_f(u16 a, u16 b)
    {
        u32 res = (u32)a + (u32)b;
        set_flag_n(false);
        set_flag_h((((a & 0xFFF) + (b & 0xFFF)) & 0x1000) == 0x1000);
        set_flag_c(res > 0xFFFF);
        return res;
    }

    u8 CPU::adc_f(u8 a, u8 b)
    {
        u8 c = get_flag_c();
        u16 res = (u16)a + (u16)b + c;
        set_flag_z((u8)res == 0);
        set_flag_n(false);
        set_flag_h(LO(a) + LO(b) + c > 0xF);
        set_flag_c(res > 0xFF);
        return (u8)res;
    }

    u8 CPU::sub_f(u8 a, u8 b)
    {
        u8 res = a - b;
        set_flag_z(res == 0);
        set_flag_n(true);
        set_flag_h(LO(a) < LO(b));
        set_flag_c(a < b);
        return res;
    }

    u8 CPU::sbc_f(u8 a, u8 b)
    {
        u8 c = get_flag_c();
        u8 res = a - b - c;
        set_flag_z(res == 0);
        set_flag_n(true);
        set_flag_h(LO(a) < LO(b) + c);
        set_flag_c(a < b + c);
        return res;
    }

    u8 CPU::and_f(u8 a, u8 b)
    {
        u8 res = a & b;
        set_flag_z(res == 0);
        set_flag_n(false);
        set_flag_h(true);
        set_flag_c(false);
        return res;
    }

    u8 CPU::xor_f(u8 a, u8 b)
    {
        u8 res = a ^ b;
        set_flag_z(res == 0);
        set_flag_n(false);
        set_flag_h(false);
        set_flag_c(false);
        return res;
    }

    u8 CPU::or_f(u8 a, u8 b)
    {
        u8 res = a | b;
        set_flag_z(res == 0);
        set_flag_n(false);
        set_flag_h(false);
        set_flag_c(false);
        return res;
    }

    void CPU::cp_f(u8 a, u8 b)
    {
        u8 res = a - b;
        set_flag_z(res == 0);
        set_flag_n(true);
        set_flag_h(LO(a) < LO(b));
        set_flag_c(a < b);
    }

    u8 CPU::inc_f(u8 a)
    {
        u8 res = a + 1;
        set_flag_z(res == 0);
        set_flag_n(false);
        set_flag_h(LO(a) + 1 > 0xF);
        return res;
    }

    u8 CPU::dec_f(u8 a)
    {
        u8 res = a - 1;
        set_flag_z(res == 0);
        set_flag_n(true);
        set_flag_h(LO(a) < 1);
        return res;
    }
} // namespace Gameboy
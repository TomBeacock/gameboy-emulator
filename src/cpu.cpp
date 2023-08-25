#include "cpu.h"

#include "display.h"
#include "memory.h"

#define A af.hi()
#define F af.lo()
#define B bc.hi()
#define C bc.lo()
#define D de.hi()
#define E de.lo()
#define H hl.hi()
#define L hl.lo()

namespace Gameboy
{
    CPU::CPU(Memory *memory, Display *display) : memory(memory), display(display) {}

    unsigned int CPU::cycle()
    {
        Instruction instruction = fetch();
        ExecuteResult result = decode(instruction);
        pc = result.next_pc;
        return result.cycles;
    }

    CPU::Instruction CPU::fetch()
    {
        uint8_t inst = memory->read(pc);
        bool prefixed = inst == 0xCB;
        if (prefixed) {
            inst = memory->read(pc + 1);
        }
        return {inst, prefixed};
    }

    CPU::ExecuteResult CPU::decode(const Instruction &instruction)
    {
        switch (instruction.encoding) {
            case 0x01: return ld_r_nn(bc);
            case 0x02: return ld_adr_r(bc, A);

            case 0x04: return inc_r(B);
            case 0x05: return dec_r(B);
            case 0x06: return ld_r_n(B);

            case 0x08: return ld_nn_sp();

            case 0x0A: return ld_r_adr(A, bc);

            case 0x0C: return inc_r(C);
            case 0x0D: return dec_r(C);
            case 0x0E: return ld_r_n(C);

            case 0x11: return ld_r_nn(de);
            case 0x12: return ld_adr_r(de, A);

            case 0x14: return inc_r(D);
            case 0x15: return dec_r(D);
            case 0x16: return ld_r_n(D);

            case 0x1A: return ld_r_adr(A, de);

            case 0x1C: return inc_r(E);
            case 0x1D: return dec_r(E);
            case 0x1E: return ld_r_n(E);

            case 0x21: return ld_r_nn(hl);
            case 0x22: return ldi_hl_a();

            case 0x24: return inc_r(H);
            case 0x25: return dec_r(H);
            case 0x26: return ld_r_n(H);
            case 0x27: return daa();

            case 0x2A: return ldi_a_hl();

            case 0x2C: return inc_r(L);
            case 0x2D: return dec_r(L);
            case 0x2E: return ld_r_n(L);
            case 0x2F: return cpl();

            case 0x31: return ld_r_nn(sp);
            case 0x32: return ldd_hl_a();

            case 0x34: return inc_hl();
            case 0x35: return dec_hl();
            case 0x36: return ld_hl_n();

            case 0x3A: return ldd_a_hl();

            case 0x3C: return inc_r(A);
            case 0x3D: return dec_r(A);
            case 0x3E: return ld_r_n(A);

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

            case 0xC1: return pop_rr(bc);

            case 0xC5: return push_rr(bc);
            case 0xC6: return add_a_n();

            case 0xCE: return adc_a_n();

            case 0xD1: return pop_rr(de);

            case 0xD5: return push_rr(de);

            case 0xE0: return ld_n_a();
            case 0xE1: return pop_rr(hl);
            case 0xE2: return ld_c_a();

            case 0xE5: return push_rr(hl);
            case 0xE6: return and_a_n();

            case 0xEA: return ld_nn_a();
            case 0xEE: return xor_a_n();

            case 0xF0: return ld_a_n();
            case 0xF1: return pop_rr(af);
            case 0xF2: return ld_a_c();

            case 0xF5: return push_rr(af);
            case 0xF6: return or_a_n();

            case 0xF9: return ld_sp_hl();
            case 0xFA: return ld_a_nn();

            case 0xFE: return cp_a_n();

            default: break;
        }
    }

    CPU::ExecuteResult CPU::ld_r_r(Register8 &dst, Register8 src)
    {
        dst = src;
        return {pc + 1, 4};
    }

    CPU::ExecuteResult CPU::ld_r_n(Register8 &dst)
    {
        uint8_t value = read_next_8();
        dst = value;
        return {pc + 2, 8};
    }

    CPU::ExecuteResult CPU::ld_r_adr(Register8 &dst, Address src)
    {
        dst = memory->read(src);
        return {pc + 1, 8};
    }

    CPU::ExecuteResult CPU::ld_adr_r(Address dst, Register8 src)
    {
        memory->write(dst, src);
        return {pc + 1, 8};
    }

    CPU::ExecuteResult CPU::ld_hl_n()
    {
        uint8_t value = read_next_8();
        memory->write(hl, value);
        return {pc + 2, 12};
    }

    CPU::ExecuteResult CPU::ld_a_nn()
    {
        Address addr = read_next_16();
        A = memory->read(addr);
        return {pc + 3, 16};
    }

    CPU::ExecuteResult CPU::ld_nn_a()
    {
        Address addr = read_next_16();
        memory->write(addr, A);
        return {pc + 3, 16};
    }

    CPU::ExecuteResult CPU::ld_a_n()
    {
        uint8_t offset = read_next_8();
        A = memory->read_io(offset);
        return {pc + 2, 12};
    }

    CPU::ExecuteResult CPU::ld_n_a()
    {
        uint8_t offset = read_next_8();
        memory->write_io(offset, A);
        return {pc + 2, 12};
    }

    CPU::ExecuteResult CPU::ld_a_c()
    {
        A = memory->read_io(C);
        return {pc + 1, 8};
    }

    CPU::ExecuteResult CPU::ld_c_a()
    {
        memory->write_io(C, A);
        return {pc + 1, 8};
    }

    CPU::ExecuteResult CPU::ldi_hl_a()
    {
        memory->write(hl, A);
        hl = hl + 1;
        return {pc + 1, 8};
    }

    CPU::ExecuteResult CPU::ldi_a_hl()
    {
        A = memory->read(hl);
        hl = hl + 1;
        return {pc + 1, 8};
    }

    CPU::ExecuteResult CPU::ldd_hl_a()
    {
        memory->write(hl, A);
        hl = hl - 1;
        return {pc + 1, 8};
    }

    CPU::ExecuteResult CPU::ldd_a_hl()
    {
        A = memory->read(hl);
        hl = hl - 1;
        return {pc + 1, 8};
    }

    CPU::ExecuteResult CPU::ld_r_nn(Register16 &dst)
    {
        uint16_t value = read_next_16();
        dst = value;
        return {pc + 3, 12};
    }

    CPU::ExecuteResult CPU::ld_nn_sp()
    {
        Address addr = read_next_16();
        memory->write(addr, sp);
        memory->write(addr, sp >> 8);
        return {pc + 3, 20};
    }

    CPU::ExecuteResult CPU::ld_sp_hl()
    {
        sp = hl;
        return {pc + 1, 8};
    }

    CPU::ExecuteResult CPU::push_rr(Register16 src)
    {
        memory->write(sp - 1, src.hi());
        memory->write(sp - 2, src.lo());
        sp = sp - 2;
        return {pc + 1, 16};
    }

    CPU::ExecuteResult CPU::pop_rr(Register16 &dst)
    {
        dst.lo() = memory->read(sp);
        dst.hi() = memory->read(sp + 1);
        sp = sp + 2;
        return {pc + 1, 12};
    }

    CPU::ExecuteResult CPU::add_a_r(Register8 op)
    {
        A = add_f(A, op);
        return {pc + 1, 4};
    }

    CPU::ExecuteResult CPU::add_a_n()
    {
        uint8_t n = read_next_8();
        A = add_f(A, n);
        return {pc + 2, 8};
    }

    CPU::ExecuteResult CPU::add_a_hl()
    {
        uint8_t op = memory->read(hl);
        A = add_f(A, op);
        return {pc + 1, 8};
    }

    CPU::ExecuteResult CPU::adc_a_r(Register8 op)
    {
        A = adc_f(A, op);
        return {pc + 1, 4};
    }

    CPU::ExecuteResult CPU::adc_a_n()
    {
        uint8_t n = read_next_8();
        A = adc_f(A, n);
        return {pc + 2, 8};
    }

    CPU::ExecuteResult CPU::adc_a_hl()
    {
        uint8_t op = memory->read(hl);
        A = adc_f(A, op);
        return {pc + 1, 8};
    }

    CPU::ExecuteResult CPU::sub_a_r(Register8 op)
    {
        A = sub_f(A, op);
        return {pc + 1, 4};
    }

    CPU::ExecuteResult CPU::sub_a_n()
    {
        uint8_t n = read_next_8();
        A = sub_f(A, n);
        return {pc + 2, 8};
    }

    CPU::ExecuteResult CPU::sub_a_hl()
    {
        uint8_t op = memory->read(hl);
        A = sub_f(A, op);
        return {pc + 1, 8};
    }

    CPU::ExecuteResult CPU::sbc_a_r(Register8 op)
    {
        A = sbc_f(A, op);
        return {pc + 1, 4};
    }

    CPU::ExecuteResult CPU::sbc_a_n()
    {
        uint8_t n = read_next_8();
        A = sbc_f(A, n);
        return {pc + 2, 8};
    }

    CPU::ExecuteResult CPU::sbc_a_hl()
    {
        uint8_t op = memory->read(hl);
        A = sbc_f(A, op);
        return {pc + 1, 8};
    }

    CPU::ExecuteResult CPU::and_a_r(Register8 op)
    {
        A = and_f(A, op);
        return {pc + 1, 4};
    }

    CPU::ExecuteResult CPU::and_a_n()
    {
        uint8_t n = read_next_8();
        A = and_f(A, n);
        return {pc + 2, 8};
    }

    CPU::ExecuteResult CPU::and_a_hl()
    {
        uint8_t op = memory->read(hl);
        A = and_f(A, op);
        return {pc + 1, 8};
    }

    CPU::ExecuteResult CPU::xor_a_r(Register8 op)
    {
        A = xor_f(A, op);
        return {pc + 1, 4};
    }

    CPU::ExecuteResult CPU::xor_a_n()
    {
        uint8_t n = read_next_8();
        A = xor_f(A, n);
        return {pc + 2, 8};
    }

    CPU::ExecuteResult CPU::xor_a_hl()
    {
        uint8_t op = memory->read(hl);
        A = xor_f(A, op);
        return {pc + 1, 8};
    }

    CPU::ExecuteResult CPU::or_a_r(Register8 op)
    {
        A = or_f(A, op);
        return {pc + 1, 4};
    }

    CPU::ExecuteResult CPU::or_a_n()
    {
        uint8_t n = read_next_8();
        A = or_f(A, n);
        return {pc + 2, 8};
    }

    CPU::ExecuteResult CPU::or_a_hl()
    {
        uint8_t op = memory->read(hl);
        A = or_f(A, op);
        return {pc + 1, 8};
    }

    CPU::ExecuteResult CPU::cp_a_r(Register8 op)
    {
        cp_f(A, op);
        return {pc + 1, 4};
    }

    CPU::ExecuteResult CPU::cp_a_n()
    {
        uint8_t n = read_next_8();
        cp_f(A, n);
        return {pc + 2, 8};
    }

    CPU::ExecuteResult CPU::cp_a_hl()
    {
        uint8_t op = memory->read(hl);
        cp_f(A, op);
        return {pc + 1, 8};
    }

    CPU::ExecuteResult CPU::inc_r(Register8 &reg)
    {
        reg = inc_f(reg);
        return {pc + 1, 4};
    }

    CPU::ExecuteResult CPU::inc_hl()
    {
        uint8_t op = memory->read(hl);
        memory->write(hl, inc_f(op));
        return {pc + 1, 12};
    }

    CPU::ExecuteResult CPU::dec_r(Register8 &reg)
    {
        reg = dec_f(reg);
        return {pc + 1, 4};
    }

    CPU::ExecuteResult CPU::dec_hl()
    {
        uint8_t op = memory->read(hl);
        memory->write(hl, dec_f(op));
        return {pc + 1, 12};
    }

    CPU::ExecuteResult CPU::daa()
    {
        if (get_subtract_flag()) {
            if (get_carry_flag()) {
                A -= 0x60;
            }
            if (get_half_carry_flag()) {
                A -= 0x6;
            }
        } else {
            if (get_carry_flag() || A > 0x99) {
                A += 0x60;
                set_carry_flag(1);
            }
            if (get_half_carry_flag() || (A & 0xF) > 0x09) {
                A += 0x6;
            }
        }
        set_zero_flag(A == 0);
        set_half_carry_flag(false);
    }

    CPU::ExecuteResult CPU::cpl()
    {
        A ^= 0xFF;
        set_subtract_flag(true);
        set_half_carry_flag(true);
        return {pc + 1, 4};
    }

    uint8_t CPU::read_next_8() const { return memory->read(pc + 1); }

    uint16_t CPU::read_next_16() const
    {
        return memory->read(pc + 1) | (uint16_t)memory->read(pc + 2) << 8;
    }

    uint8_t CPU::add_f(uint8_t a, uint8_t b)
    {
        uint16_t res = (uint16_t)a + (uint16_t)b;
        set_zero_flag((uint8_t)res == 0);
        set_subtract_flag(false);
        set_half_carry_flag((a & 0x0F) + (b & 0x0F) > 0xF);
        set_carry_flag(res > 0xFF);
        return res;
    }

    uint8_t CPU::adc_f(uint8_t a, uint8_t b)
    {
        b += get_carry_flag();
        uint16_t res = (uint16_t)a + (uint16_t)b;
        set_zero_flag((uint8_t)res == 0);
        set_subtract_flag(false);
        set_half_carry_flag((a & 0x0F) + (b & 0x0F) > 0xF);
        set_carry_flag(res > 0xFF);
        return res;
    }

    uint8_t CPU::sub_f(uint8_t a, uint8_t b)
    {
        uint8_t res = a - b;
        set_zero_flag(res == 0);
        set_subtract_flag(true);
        set_half_carry_flag((a & 0x0F) < (b & 0x0F));
        set_carry_flag(a < b);
        return res;
    }

    uint8_t CPU::sbc_f(uint8_t a, uint8_t b)
    {
        b += get_carry_flag();
        uint8_t res = a - b;
        set_zero_flag(res == 0);
        set_subtract_flag(true);
        set_half_carry_flag((a & 0x0F) < (b & 0x0F));
        set_carry_flag(a < b);
        return res;
    }

    uint8_t CPU::and_f(uint8_t a, uint8_t b)
    {
        uint8_t res = a & b;
        set_zero_flag(res == 0);
        set_subtract_flag(false);
        set_half_carry_flag(true);
        set_carry_flag(false);
        return res;
    }

    uint8_t CPU::xor_f(uint8_t a, uint8_t b)
    {
        uint8_t res = a ^ b;
        set_zero_flag(res == 0);
        set_subtract_flag(false);
        set_half_carry_flag(false);
        set_carry_flag(false);
        return res;
    }

    uint8_t CPU::or_f(uint8_t a, uint8_t b)
    {
        uint8_t res = a | b;
        set_zero_flag(res == 0);
        set_subtract_flag(false);
        set_half_carry_flag(false);
        set_carry_flag(false);
        return res;
    }

    void CPU::cp_f(uint8_t a, uint8_t b)
    {
        uint8_t res = a - b;
        set_zero_flag(res == 0);
        set_subtract_flag(false);
        set_half_carry_flag((a & 0x0F) < (b & 0x0F));
        set_carry_flag(a < b);
    }

    uint8_t CPU::inc_f(uint8_t a)
    {
        uint8_t res = a + 1;
        set_zero_flag(res == 0);
        set_subtract_flag(false);
        set_half_carry_flag((a & 0xF) + 1 > 0xF);
        return res;
    }

    uint8_t CPU::dec_f(uint8_t a)
    {
        uint8_t res = a - 1;
        set_zero_flag(res == 0);
        set_subtract_flag(false);
        set_half_carry_flag((a & 0xF) < 1);
        return res;
    }
} // namespace Gameboy
#include "cpu.h"

#include "display.h"
#include "memory.h"

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
            case 0x01: return load_reg_nn(bc);
            case 0x02: return load_addr_reg(bc, af.hi());

            case 0x06: return load_reg_n(bc.hi());

            case 0x08: return load_nn_sp();

            case 0x0A: return load_reg_addr(af.hi(), bc);

            case 0x0E: return load_reg_n(bc.lo());

            case 0x11: return load_reg_nn(de);
            case 0x12: return load_addr_reg(de, af.hi());

            case 0x16: return load_reg_n(de.hi());

            case 0x1A: return load_reg_addr(af.hi(), de);

            case 0x1E: return load_reg_n(de.lo());

            case 0x21: return load_reg_nn(hl);
            case 0x22: return load_incr_hl_a();

            case 0x26: return load_reg_n(hl.hi());

            case 0x2A: return load_incr_a_hl();

            case 0x2E: return load_reg_n(hl.lo());

            case 0x31: return load_reg_nn(sp);
            case 0x32: return load_decr_hl_a();

            case 0x36: return load_hl_n();

            case 0x3A: return load_decr_a_hl();

            case 0x3E: return load_reg_n(af.hi());

            case 0x40: return load_reg_reg(bc.hi(), bc.hi());
            case 0x41: return load_reg_reg(bc.hi(), bc.lo());
            case 0x42: return load_reg_reg(bc.hi(), de.hi());
            case 0x43: return load_reg_reg(bc.hi(), de.lo());
            case 0x44: return load_reg_reg(bc.hi(), hl.hi());
            case 0x45: return load_reg_reg(bc.hi(), hl.lo());
            case 0x46: return load_reg_addr(bc.hi(), hl);
            case 0x47: return load_reg_reg(bc.hi(), af.hi());
            case 0x48: return load_reg_reg(bc.lo(), bc.hi());
            case 0x49: return load_reg_reg(bc.lo(), bc.lo());
            case 0x4A: return load_reg_reg(bc.lo(), de.hi());
            case 0x4B: return load_reg_reg(bc.lo(), de.lo());
            case 0x4C: return load_reg_reg(bc.lo(), hl.hi());
            case 0x4D: return load_reg_reg(bc.lo(), hl.lo());
            case 0x4E: return load_reg_addr(bc.lo(), hl);
            case 0x4F: return load_reg_reg(bc.lo(), af.hi());
            case 0x50: return load_reg_reg(de.hi(), bc.hi());
            case 0x51: return load_reg_reg(de.hi(), bc.lo());
            case 0x52: return load_reg_reg(de.hi(), de.hi());
            case 0x53: return load_reg_reg(de.hi(), de.lo());
            case 0x54: return load_reg_reg(de.hi(), hl.hi());
            case 0x55: return load_reg_reg(de.hi(), hl.lo());
            case 0x56: return load_reg_addr(de.hi(), hl);
            case 0x57: return load_reg_reg(de.hi(), af.hi());
            case 0x58: return load_reg_reg(de.lo(), bc.hi());
            case 0x59: return load_reg_reg(de.lo(), bc.lo());
            case 0x5A: return load_reg_reg(de.lo(), de.hi());
            case 0x5B: return load_reg_reg(de.lo(), de.lo());
            case 0x5C: return load_reg_reg(de.lo(), hl.hi());
            case 0x5D: return load_reg_reg(de.lo(), hl.lo());
            case 0x5E: return load_reg_addr(de.lo(), hl);
            case 0x5F: return load_reg_reg(de.lo(), af.hi());
            case 0x60: return load_reg_reg(hl.hi(), bc.hi());
            case 0x61: return load_reg_reg(hl.hi(), bc.lo());
            case 0x62: return load_reg_reg(hl.hi(), de.hi());
            case 0x63: return load_reg_reg(hl.hi(), de.lo());
            case 0x64: return load_reg_reg(hl.hi(), hl.hi());
            case 0x65: return load_reg_reg(hl.hi(), hl.lo());
            case 0x66: return load_reg_addr(hl.hi(), hl);
            case 0x67: return load_reg_reg(hl.hi(), af.hi());
            case 0x68: return load_reg_reg(hl.lo(), bc.hi());
            case 0x69: return load_reg_reg(hl.lo(), bc.lo());
            case 0x6A: return load_reg_reg(hl.lo(), de.hi());
            case 0x6B: return load_reg_reg(hl.lo(), de.lo());
            case 0x6C: return load_reg_reg(hl.lo(), hl.hi());
            case 0x6D: return load_reg_reg(hl.lo(), hl.lo());
            case 0x6E: return load_reg_addr(hl.lo(), hl);
            case 0x6F: return load_reg_reg(hl.lo(), af.hi());
            case 0x70: return load_addr_reg(hl, bc.hi());
            case 0x71: return load_addr_reg(hl, bc.lo());
            case 0x72: return load_addr_reg(hl, de.hi());
            case 0x73: return load_addr_reg(hl, de.lo());
            case 0x74: return load_addr_reg(hl, hl.hi());
            case 0x75: return load_addr_reg(hl, hl.lo());

            case 0x77: return load_addr_reg(hl, af.hi());
            case 0x78: return load_reg_reg(af.hi(), bc.hi());
            case 0x79: return load_reg_reg(af.hi(), bc.lo());
            case 0x7A: return load_reg_reg(af.hi(), de.hi());
            case 0x7B: return load_reg_reg(af.hi(), de.lo());
            case 0x7C: return load_reg_reg(af.hi(), hl.hi());
            case 0x7D: return load_reg_reg(af.hi(), hl.lo());
            case 0x7E: return load_reg_addr(af.hi(), hl);
            case 0x7F: return load_reg_reg(af.hi(), af.hi());
            case 0x80: return add_a_reg(bc.hi());
            case 0x81: return add_a_reg(bc.lo());
            case 0x82: return add_a_reg(de.hi());
            case 0x83: return add_a_reg(de.lo());
            case 0x84: return add_a_reg(hl.hi());
            case 0x85: return add_a_reg(hl.lo());
            case 0x86: return add_a_hl();
            case 0x87: return add_a_reg(af.hi());
            case 0x88: return add_a_reg_carry(bc.hi());
            case 0x89: return add_a_reg_carry(bc.lo());
            case 0x8A: return add_a_reg_carry(de.hi());
            case 0x8B: return add_a_reg_carry(de.lo());
            case 0x8C: return add_a_reg_carry(hl.hi());
            case 0x8D: return add_a_reg_carry(hl.lo());
            case 0x8E: return add_a_hl_carry();
            case 0x8F: return add_a_reg_carry(af.hi());
            case 0x90: return sub_a_reg(bc.hi());
            case 0x91: return sub_a_reg(bc.lo());
            case 0x92: return sub_a_reg(de.hi());
            case 0x93: return sub_a_reg(de.lo());
            case 0x94: return sub_a_reg(hl.hi());
            case 0x95: return sub_a_reg(hl.lo());
            case 0x96: return sub_a_hl();
            case 0x97: return sub_a_reg(af.hi());
            case 0x98: return sub_a_reg_carry(bc.hi());
            case 0x99: return sub_a_reg_carry(bc.lo());
            case 0x9A: return sub_a_reg_carry(de.hi());
            case 0x9B: return sub_a_reg_carry(de.lo());
            case 0x9C: return sub_a_reg_carry(hl.hi());
            case 0x9D: return sub_a_reg_carry(hl.lo());
            case 0x9E: return sub_a_hl_carry();
            case 0x9F: return sub_a_reg_carry(af.hi());

            case 0xC1: return pop_reg(bc);

            case 0xC5: return push_reg(bc);
            case 0xC6: return add_a_n();

            case 0xCE: return add_a_n_carry();

            case 0xD1: return pop_reg(de);

            case 0xD5: return push_reg(de);

            case 0xE0: return load_io_n_a();
            case 0xE1: return pop_reg(hl);
            case 0xE2: return load_io_c_a();

            case 0xE5: return push_reg(hl);

            case 0xEA: return load_nn_a();

            case 0xF0: return load_a_io_n();
            case 0xF1: return pop_reg(af);
            case 0xF2: return load_a_io_c();

            case 0xF5: return push_reg(af);

            case 0xF9: return load_sp_hl();
            case 0xFA: return load_a_nn();

            default: break;
        }
    }

    CPU::ExecuteResult CPU::load_reg_reg(Register8 &dst, Register8 src)
    {
        dst = src;
        return {pc + 1, 4};
    }

    CPU::ExecuteResult CPU::load_reg_n(Register8 &dst)
    {
        uint8_t value = read_next_8();
        dst = value;
        return {pc + 2, 8};
    }

    CPU::ExecuteResult CPU::load_reg_addr(Register8 &dst, Address src)
    {
        dst = memory->read(src);
        return {pc + 1, 8};
    }

    CPU::ExecuteResult CPU::load_addr_reg(Address dst, Register8 src)
    {
        memory->write(dst, src);
        return {pc + 1, 8};
    }

    CPU::ExecuteResult CPU::load_hl_n()
    {
        uint8_t value = read_next_8();
        memory->write(hl, value);
        return {pc + 2, 12};
    }

    CPU::ExecuteResult CPU::load_a_nn()
    {
        Address addr = read_next_16();
        af.hi() = memory->read(addr);
        return {pc + 3, 16};
    }

    CPU::ExecuteResult CPU::load_nn_a()
    {
        Address addr = read_next_16();
        memory->write(addr, af.hi());
        return {pc + 3, 16};
    }

    CPU::ExecuteResult CPU::load_a_io_n()
    {
        uint8_t offset = read_next_8();
        af.hi() = memory->read_io(offset);
        return {pc + 2, 12};
    }

    CPU::ExecuteResult CPU::load_io_n_a()
    {
        uint8_t offset = read_next_8();
        memory->write_io(offset, af.hi());
        return {pc + 2, 12};
    }

    CPU::ExecuteResult CPU::load_a_io_c()
    {
        af.hi() = memory->read_io(bc.lo());
        return {pc + 1, 8};
    }

    CPU::ExecuteResult CPU::load_io_c_a()
    {
        memory->write_io(bc.lo(), af.hi());
        return {pc + 1, 8};
    }

    CPU::ExecuteResult CPU::load_incr_hl_a()
    {
        memory->write(hl, af.hi());
        hl = hl + 1;
        return {pc + 1, 8};
    }

    CPU::ExecuteResult CPU::load_incr_a_hl()
    {
        af.hi() = memory->read(hl);
        hl = hl + 1;
        return {pc + 1, 8};
    }

    CPU::ExecuteResult CPU::load_decr_hl_a()
    {
        memory->write(hl, af.hi());
        hl = hl - 1;
        return {pc + 1, 8};
    }

    CPU::ExecuteResult CPU::load_decr_a_hl()
    {
        af.hi() = memory->read(hl);
        hl = hl - 1;
        return {pc + 1, 8};
    }

    CPU::ExecuteResult CPU::load_reg_nn(Register16 &dst)
    {
        uint16_t value = read_next_16();
        dst = value;
        return {pc + 3, 12};
    }

    CPU::ExecuteResult CPU::load_nn_sp()
    {
        Address addr = read_next_16();
        memory->write(addr, sp);
        memory->write(addr, sp >> 8);
        return {pc + 3, 20};
    }

    CPU::ExecuteResult CPU::load_sp_hl()
    {
        sp = hl;
        return {pc + 1, 8};
    }

    CPU::ExecuteResult CPU::push_reg(Register16 src)
    {
        memory->write(sp - 1, src.hi());
        memory->write(sp - 2, src.lo());
        sp = sp - 2;
        return {pc + 1, 16};
    }

    CPU::ExecuteResult CPU::pop_reg(Register16 &dst)
    {
        dst.lo() = memory->read(sp);
        dst.hi() = memory->read(sp + 1);
        sp = sp + 2;
        return {pc + 1, 12};
    }

    CPU::ExecuteResult CPU::add_a_reg(Register8 op)
    {
        af.hi() = add8(af.hi(), op);
        return {pc + 1, 4};
    }

    CPU::ExecuteResult CPU::add_a_n()
    {
        uint8_t n = read_next_8();
        af.hi() = add8(af.hi(), n);
        return {pc + 2, 8};
    }

    CPU::ExecuteResult CPU::add_a_hl()
    {
        uint8_t op = memory->read(hl);
        af.hi() = add8(af.hi(), op);
        return {pc + 1, 8};
    }

    CPU::ExecuteResult CPU::add_a_reg_carry(Register8 op)
    {
        af.hi() = add8_carry(af.hi(), op);
        return {pc + 1, 4};
    }

    CPU::ExecuteResult CPU::add_a_n_carry()
    {
        uint8_t n = read_next_8();
        af.hi() = add8_carry(af.hi(), n);
        return {pc + 2, 8};
    }

    CPU::ExecuteResult CPU::add_a_hl_carry()
    {
        uint8_t op = memory->read(hl);
        af.hi() = add8_carry(af.hi(), op);
        return {pc + 1, 8};
    }

    CPU::ExecuteResult CPU::sub_a_reg(Register8 op)
    {
        af.hi() = sub8(af.hi(), op);
        return {pc + 1, 4};
    }

    CPU::ExecuteResult CPU::sub_a_n()
    {
        uint8_t n = read_next_8();
        af.hi() = sub8(af.hi(), n);
        return {pc + 2, 8};
    }

    CPU::ExecuteResult CPU::sub_a_hl()
    {
        uint8_t op = memory->read(hl);
        af.hi() = sub8(af.hi(), op);
        return {pc + 1, 8};
    }

    CPU::ExecuteResult CPU::sub_a_reg_carry(Register8 op)
    {
        af.hi() = sub8_carry(af.hi(), op);
        return {pc + 1, 4};
    }

    CPU::ExecuteResult CPU::sub_a_n_carry()
    {
        uint8_t n = read_next_8();
        af.hi() = sub8_carry(af.hi(), n);
        return {pc + 2, 8};
    }

    CPU::ExecuteResult CPU::sub_a_hl_carry()
    {
        uint8_t op = memory->read(hl);
        af.hi() = sub8_carry(af.hi(), op);
        return {pc + 1, 8};
    }

    uint8_t CPU::read_next_8() const { return memory->read(pc + 1); }

    uint16_t CPU::read_next_16() const
    {
        return memory->read(pc + 1) | (uint16_t)memory->read(pc + 2) << 8;
    }

    uint8_t CPU::add8(uint8_t a, uint8_t b)
    {
        uint16_t res = (uint16_t)a + (uint16_t)b;
        set_zero_flag((uint8_t)res == 0);
        set_subtract_flag(false);
        set_half_carry_flag((a & 0x0F) + (b & 0x0F) > 0xF);
        set_carry_flag(res > 0xFF);
        return res;
    }

    uint8_t CPU::add8_carry(uint8_t a, uint8_t b)
    {
        b += get_carry_flag();
        uint16_t res = (uint16_t)a + (uint16_t)b;
        set_zero_flag((uint8_t)res == 0);
        set_subtract_flag(false);
        set_half_carry_flag((a & 0x0F) + (b & 0x0F) > 0xF);
        set_carry_flag(res > 0xFF);
        return res;
    }

    uint8_t CPU::sub8(uint8_t a, uint8_t b)
    {
        uint8_t res = a - b;
        set_zero_flag(res == 0);
        set_subtract_flag(true);
        set_half_carry_flag((a & 0x0F) < (b & 0x0F));
        set_carry_flag(a < b);
        return res;
    }

    uint8_t CPU::sub8_carry(uint8_t a, uint8_t b)
    {
        b += get_carry_flag();
        uint8_t res = a - b;
        set_zero_flag(res == 0);
        set_subtract_flag(true);
        set_half_carry_flag((a & 0x0F) < (b & 0x0F));
        set_carry_flag(a < b);
        return res;
    }
} // namespace Gameboy
#pragma once

#include "types.h"

namespace Gameboy
{
    class MMU
    {
      public:
        MMU();

        u8 read(u16 address) const { return memory[address]; }
        u8 read_io(u8 offset) const { return memory[0xFF00 + offset]; }

        void write(u16 address, u8 value) { memory[address] = value; }
        void write_io(u8 offset, u8 value) { memory[0xFF00 + offset] = value; }

      private:
        u8 *memory;
    };
} // namespace Gameboy
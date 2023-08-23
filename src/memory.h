#pragma once

#include <cstdint>

namespace Gameboy
{
    class Memory
    {
      public:
        Memory();

        uint8_t read(uint16_t address) const { return memory[address]; }
        uint8_t read_io(uint8_t offset) const { return memory[0xFF00 + offset]; }

        void write(uint16_t address, uint8_t value) { memory[address] = value; }
        void write_io(uint8_t offset, uint8_t value) { memory[0xFF00 + offset] = value; }

      private:
        uint8_t *memory;
    };
} // namespace Gameboy
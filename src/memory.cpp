#include "memory.h"

namespace Gameboy
{
    Memory::Memory() : memory(new uint8_t[0xFFFF]) {}
} // namespace Gameboy
#include "memory.h"

namespace Gameboy
{
    Memory::Memory() : memory(new u8[0xFFFF]) {}
} // namespace Gameboy
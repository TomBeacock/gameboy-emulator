#include "mmu.h"

namespace Gameboy
{
    MMU::MMU() : memory(new u8[0xFFFF]) {}
} // namespace Gameboy
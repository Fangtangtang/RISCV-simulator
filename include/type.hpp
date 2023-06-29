#ifndef CODE_TYPE_HPP
#define CODE_TYPE_HPP

#include <cstdint>

using MachineCode = uint32_t;
using MemoryUnit = uint8_t;

using MemoryAddr = uint32_t;
using RegisterUnit = int32_t;
using TwoBytes = uint16_t;

using Index = int8_t;
using Number = int32_t;
using UnsignedNumber = uint32_t;
using Byte = int8_t;

inline Number SignExtend(const Number &number, const int ini_size) {
    if (ini_size == 8)return ((number & 0xff) | ((number & 0x80) ? 0xffffff00 : 0x0));
    if (ini_size == 12)return ((number & 0xfff) | ((number & 0x800) ? 0xfffff000 : 0x0));
    if (ini_size == 13)return ((number & 0x1fff) | ((number & 0x1000) ? 0xffffe000 : 0x0));
    if (ini_size == 16)return ((number & 0xffff) | ((number & 0x8000) ? 0xffff0000 : 0x0));
    if (ini_size == 21) return ((number & 0x1fffff) | ((number & 0x100000) ? 0xfffe00000 : 0x0));
    return number;
}

#endif //CODE_TYPE_HPP

#ifndef CODE_MEMORY_HPP
#define CODE_MEMORY_HPP

#include "../include/type.hpp"
#include "decoder.hpp"
#include "../include/scanner.hpp"

class Memory {
    MemoryUnit storage[8050000];

    void WriteWordInMem(const MemoryAddr &addr, const Number &inf);

    void WriteHalfWordInMem(const MemoryAddr &addr, const Number &inf);

    void WriteByteInMem(const MemoryAddr &addr, const Number &inf);

    void ReadWordFromMem(const MemoryAddr &addr, Number &result);

    void ReadHalfWordFromMem(const MemoryAddr &addr, Number &result);

    void ReadByteFromMem(const MemoryAddr &addr, Number &result);

    void ReadUnsignedHalfWordFromMem(const MemoryAddr &addr, Number &result);

    void ReadUnsignedByteFromMem(const MemoryAddr &addr, Number &result);

    friend class Scanner;

public:
    /*
     * scan all the instructions
     * store all the machine code in memory
     */
    void Initialize();

    void InstructionFetch(const MemoryAddr &addr, MachineCode &machineCode);

    /*
     * visit memory
     * 3 clock cycle
     * return true if visit
     */
    bool VisitMemory(const Instruction &instruction, const Number &nrs1, const Number &nrs2, Number &result);

    void PrintStack() {
        std::cout << "\nSTACK:\n";
        for (int i = 4660; i < 4680; ++i) {
            std::cout << std::hex << i << ' ' << std::dec << i << '\t' << storage[i] << '\n';
        }
        std::cout << "\n";
    }
};

void Memory::Initialize() {
    Scanner scanner;
    MemoryAddr memoryAddr;
    while (scanner.HaveMoreTokens()) {
        scanner.Scan(memoryAddr, storage);
    }
}

void Memory::InstructionFetch(const MemoryAddr &addr, MachineCode &machineCode) {
    machineCode = 0;
    for (int i = 3; i >= 0; --i) {
        machineCode <<= 8;
        machineCode |= (MachineCode) storage[addr + i];
    }
}

bool Memory::VisitMemory(const Instruction &instruction, const Number &nrs1, const Number &nrs2, Number &result) {
    switch (instruction.instructionType) {
        case LB:
            ReadByteFromMem(nrs1 + instruction.immediate, result);
            return true;
        case LH:
            ReadHalfWordFromMem(nrs1 + instruction.immediate, result);
            return true;
        case LW:
            ReadWordFromMem(nrs1 + instruction.immediate, result);
            return true;
        case LBU:
            ReadUnsignedByteFromMem(nrs1 + instruction.immediate, result);
            return true;
        case LHU:
            ReadUnsignedHalfWordFromMem(nrs1 + instruction.immediate, result);
            return true;
        case SB:
            WriteByteInMem(nrs1 + instruction.immediate, nrs2);
            return true;
        case SH:
            WriteHalfWordInMem(nrs1 + instruction.immediate, nrs2);
            return true;
        case SW:
            WriteWordInMem(nrs1 + instruction.immediate, nrs2);
            return true;
        default:
            return false;
    }
}

void Memory::WriteWordInMem(const MemoryAddr &addr, const Number &inf) {
    storage[addr + 3] = MemoryUnit(inf >> 24);
    storage[addr + 2] = MemoryUnit(inf >> 16);
    storage[addr + 1] = MemoryUnit(inf >> 8);
    storage[addr] = (MemoryUnit) inf;
}

void Memory::WriteHalfWordInMem(const MemoryAddr &addr, const Number &inf) {
    storage[addr + 1] = MemoryUnit(inf >> 8);
    storage[addr] = (MemoryUnit) inf;
}

void Memory::WriteByteInMem(const MemoryAddr &addr, const Number &inf) {
    storage[addr] = (MemoryUnit) inf;
}

void Memory::ReadWordFromMem(const MemoryAddr &addr, Number &result) {
    result = (Number) storage[addr + 3];
    result <<= 8;
    result |= ((Number) storage[addr + 2]);
    result <<= 8;
    result |= ((Number) storage[addr + 1]);
    result <<= 8;
    result |= ((Number) storage[addr]);
}

void Memory::ReadHalfWordFromMem(const MemoryAddr &addr, Number &result) {
    result = (Number) storage[addr + 1];
    result <<= 8;
    result |= ((Number) storage[addr]);
    result = SignExtend(result, 16);
}

void Memory::ReadByteFromMem(const MemoryAddr &addr, Number &result) {
    result = SignExtend(storage[addr], 8);
}

void Memory::ReadUnsignedHalfWordFromMem(const MemoryAddr &addr, Number &result) {
    result = (Number) storage[addr + 1];
    result <<= 8;
    result |= ((Number) storage[addr]);
}

void Memory::ReadUnsignedByteFromMem(const MemoryAddr &addr, Number &result) {
    result = storage[addr] & 0xff;
}

#endif //CODE_MEMORY_HPP

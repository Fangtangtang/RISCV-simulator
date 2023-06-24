/*
 * Load Buffer
 * -----------------------------------------------
 * buffer for load instructions
 * execute in seq
 */
#ifndef CODE_LOADBUFFER_HPP
#define CODE_LOADBUFFER_HPP

#include "../include/queue.hpp"
#include "../include/type.hpp"
#include "../tool/decoder.hpp"
#include "../tool/memory.hpp"
#include "bus.hpp"

class LBType {
    InstructionType type;
    Number nrs1;//value of rs1
    Number imme;//immediate number
    Index Q = -1;//dependence of nrs1
    Index RoB;//entry in RoB
};

class LoadBuffer {
    Queue<LBType> LBQueue{8};
    Byte timer;//count time
public:
    /*
     * whether LBQueue have spare space for new instruction
     */
    bool Full();

    void AddInstruction(const Instruction &instruction, const Index &entry);

    /*
     * execute in one clock cycle
     * if executed, push into bus
     */
    void Execute(CDB &bus);

    /*
     * remove dependence
     * TODO traverse to modify?
     */
    void Modify(const std::pair<Index, Number> &pair);

};

#endif //CODE_LOADBUFFER_HPP

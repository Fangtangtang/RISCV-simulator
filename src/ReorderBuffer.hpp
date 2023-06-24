/*
 * Reorder Buffer
 * ---------------------------------------------------------
 * add all the instructions in seq
 * ensure correct commit order
 */
#ifndef CODE_REORDERBUFFER_HPP
#define CODE_REORDERBUFFER_HPP

#include "../include/queue.hpp"
#include "../tool/decoder.hpp"
#include "src/RegFile.hpp"
#include "src/ReservationStation.hpp"
#include "src/StoreBuffer.hpp"
#include "src/LoadBuffer.hpp"

class RoBType {
//    bool busy = false;
    bool ready = false;//ready to commit?
    InstructionType type;
    Byte dest{};//dest reg
    Number value = 0; //if instruction is conditional branch, record the other address
    bool predict = false; //predict whether to branch
};

class ReorderBuffer {
    Queue<RoBType> RoBQueue{8};
public:
    /*
     * try to add Instruction into RoB
     * if RoBQueue is full return -1
     * else return entry(index in RoBQueue.storage)
     */
    Index AddInstruction(const Instruction &instruction, const RegisterFile &registerFile,
                         ReservationStation &RS, StoreBuffer &storeBuffer, LoadBuffer &loadBuffer);

    /*
     * modify ele in RoB
     * return dest
     */
    Byte Modify(const std::pair<Index, Number> &pair);

    /*
     * commit all the ready ones
     * if commit EXIT return true
     */
    bool Commit(Registers&registers);
};

#endif //CODE_REORDERBUFFER_HPP

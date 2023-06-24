/*
 * Reservation Station
 * --------------------------------------------------------
 * buffer for ALU
 * execute if parameters are available
 */
#ifndef CODE_RESERVATIONSTATION_HPP
#define CODE_RESERVATIONSTATION_HPP

#include "../include/type.hpp"
#include "../tool/decoder.hpp"
#include "../tool/ALU.hpp"
#include "bus.hpp"

class RSType {
    bool busy = false;
    InstructionType type;
    Number nrs1, nrs2;
    Number Imme;
    Index Q1, Q2;
    Index RoB;
};

class PCReservationStation {
    bool busy = false;
    Number Imme;
    Index Q;
    Index RoB;
public:
    void AddInstruction(const Instruction &instruction, const Index &entry);

    /*
     * if not bust return false
     * if Q==-1
     * modify pc,  push into bus
     * return true
     */
    bool Execute(CDB &bus, RegisterUnit &pc);

    /*
     * remove dependence
     */
    void Modify(const std::pair<Index, Number> &pair);

};

class ReservationStation {
    RSType RS[8];
public:
    /*
     * return index of RS if is not busy
     * return -1 if full
     */
    Index GetSpace();

    /*
     * add instruction to the spare space
     */
    void AddInstruction(const Index &index, const Instruction &instruction, const Index &entry);

    /*
     * execute in one clock cycle
     * if executed, push into bus
     */
    void Execute(CDB &bus);

    /*
     * remove dependence
     * traverse RS to modify
     */
    void Modify(const std::pair<Index, Number> &pair);

};

#endif //CODE_RESERVATIONSTATION_HPP

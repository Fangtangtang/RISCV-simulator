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

class ReservationStation;

class RSType {
    bool busy = false;
    InstructionType type;
    Number nrs1{}, nrs2{};
    Number Imme{};
    Index Q1 = -1, Q2 = -1;
    Index RoB{};

    friend class ReservationStation;

    friend std::ostream &operator<<(std::ostream &os, const RSType &obj) {
        std::cout << obj.busy << " " << Convert(obj.type) << ' ' << obj.nrs1 << ' ' << obj.nrs2 << ' '
                  << obj.Imme << ' ' << (Number) obj.Q1 << ' ' << (Number) obj.Q2 << ' '
                  << (Number) obj.RoB;
        return os;
    }
};

class PCReservationStation {
    bool busy = false;
    Number nrs1{};
    Number imme{};
    Index Q = -1;
    Index RoB{};
public:
    void AddInstruction(const Instruction &instruction, const RegisterFile &registerFile, const Index &entry);

    /*
     * if not busy return false
     * if Q==-1
     * modify pc,  push into bus
     * return true
     */
    bool Execute(CDB &bus, RegisterUnit &pc);

    /*
     * remove dependence
     */
    void Modify(const std::pair<Index, Number> &pair);

    void Clear();


};

void PCReservationStation::AddInstruction(const Instruction &instruction, const RegisterFile &registerFile,
                                          const Index &entry) {
    busy = true;
    imme = instruction.immediate;
    Q = registerFile.GetValue(instruction.rs1, nrs1);
    RoB = entry;
}

bool PCReservationStation::Execute(CDB &bus, RegisterUnit &pc) {
    if (!busy) return false;
    if (Q == -1) {
        busy = false;
        Number num = pc + 4;
        bus.Add(RoB, num);
        pc = imme + nrs1;
        if (pc & 1) pc &= (pc - 1);
        return true;
    }
    return false;
}

void PCReservationStation::Modify(const std::pair<Index, Number> &pair) {
    if (Q == -1)return;
    if (Q == pair.first) {
        nrs1 = pair.second;
        Q = -1;
    }
}

void PCReservationStation::Clear() {
    busy = false;
}


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
    void AddInstruction(const Index &index, const Instruction &instruction,
                        const RegisterFile &registerFile, const Index &entry);

    /*
     * execute in one clock cycle
     * if executed, push into bus
     */
    void Execute(CDB &bus, const ALU &alu);

    /*
     * remove dependence
     * traverse RS to modify
     */
    void Modify(const std::pair<Index, Number> &pair);

    void Clear();

    void Print();
};

Index ReservationStation::GetSpace() {
    for (Index i = 0; i < 8; ++i) {
        if (!RS[i].busy) return i;
    }
    return -1;
}

void ReservationStation::AddInstruction(const Index &index, const Instruction &instruction,
                                        const RegisterFile &registerFile, const Index &entry) {
    RS[index].busy = true;
    RS[index].type = instruction.instructionType;
    RS[index].RoB = entry;
    RS[index].Q1 = registerFile.GetValue(instruction.rs1, RS[index].nrs1);
    RS[index].Q2 = registerFile.GetValue(instruction.rs2, RS[index].nrs2);
    RS[index].Imme = instruction.immediate;
}

void ReservationStation::Execute(CDB &bus, const ALU &alu) {
    Number value;
    for (auto &i: RS) {
        if (i.busy) {
            if (i.Q1 == -1 && i.Q2 == -1) {//no dependence
                i.busy = false;
                value = alu.Execute(i.type, i.nrs1, i.nrs2, i.Imme);
                bus.Add(i.RoB, value);
            }
        }
    }
}

void ReservationStation::Modify(const std::pair<Index, Number> &pair) {
    for (auto &i: RS) {
        if (i.busy) {
            if (i.Q1 == pair.first) {
                i.Q1 = -1;
                i.nrs1 = pair.second;
            }
            if (i.Q2 == pair.first) {
                i.Q2 = -1;
                i.nrs2 = pair.second;
            }
        }
    }
}

void ReservationStation::Clear() {
    for (auto &i: RS) {
        i.busy = false;
    }
}

void ReservationStation::Print() {
    std::cout << "RS:\n";
    for (int i = 0; i < 8; ++i) {
        std::cout << i << ": " << RS[i] << '\n';
    }
}


#endif //CODE_RESERVATIONSTATION_HPP

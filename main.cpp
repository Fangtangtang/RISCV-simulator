#include "src/ReorderBuffer.hpp"
#include "src/bus.hpp"
#include "tool/memory.hpp"
#include "src/predictor.hpp"
#include "src/RegFile.hpp"
#include "src/ReservationStation.hpp"
#include "src/MemoryBuffer.hpp"

enum STATUS {
    process, wait, pause
};

int main() {
    freopen("/mnt/f/repo/RISCV-simulator/test.data", "r", stdin);
    freopen("my.out", "w", stdout);
    Decoder decoder;
    Memory memory{};
    Registers registers;
    ALU alu;
    ReorderBuffer RoB;
    ReservationStation RS;
    PCReservationStation pcRS{};
    MemoryBuffer memoryBuffer;
    RegisterFile registerFile;
    Predictor predictor;
    CDB bus;
    memory.Initialize();//scan all the code
    registerFile.Reset(registers);
    RegisterUnit pc;//pc points at the address of code, used in IF
    MachineCode machineCode = 0;
    Instruction instruction;
    STATUS IP_flag = process;
    bool reset_flag = false;//instruction process flag
    Index entry;
    int size_;
    Byte dest;
    int clock = 0;
    std::pair<Index, Number> pair;
    while (true) {
        ++clock;
//        std::cout<<clock<<'\n';
        if (IP_flag == process) {
            //IF
            memory.InstructionFetch(pc, machineCode);
            //ID
            decoder.Decode(machineCode, instruction);//decode machine code to get instruction
        }
        if (IP_flag != pause) {
//            std::cout << clock << '\t' << std::hex << pc << std::dec << '\t' << Convert(instruction.instructionType)
//                      << ' '
//                      << unsigned(instruction.rs1) << ' ' << unsigned(instruction.rs2) << ' '
//                      << unsigned(instruction.rd) << ' '
//                      << instruction.immediate << '\n';
            entry = RoB.AddInstruction(instruction, registerFile, predictor, RS, memoryBuffer, pcRS, pc);
        }
        if (entry == -1)IP_flag = wait;
        else if (instruction.instructionType == JALR ||
                 instruction.instructionType == EXIT) {//fail to add
            IP_flag = pause;
        } else {
            IP_flag = process;//pc updated
        }
        //try to add into buffers
        //EX MEM
        if (IP_flag == pause && pcRS.Execute(bus, pc))
            IP_flag = process;
        RS.Execute(bus, alu);
        memoryBuffer.Execute(bus, memory);
        size_ = bus.Size();
        for (int i = 0; i < size_; ++i) {
            pair = bus.GetEle(i);
            RS.Modify(pair);
            memoryBuffer.Modify(pair);
            if (IP_flag == pause)pcRS.Modify(pair);
            dest = RoB.Modify(pair, predictor);
            registerFile.Update(dest, pair);
        }
        bus.Clear();
        //WB
        if (RoB.Commit(registers, reset_flag, pc)) break;
        if (reset_flag) {
            registerFile.Reset(registers);
            pcRS.Clear();
            RS.Clear();
            memoryBuffer.Clear();
            machineCode = 0;
            instruction.instructionType = WAIT;
            IP_flag = process;
            reset_flag = false;
        }
//        registerFile.Print();
//        RoB.Print();
//        registers.Print();
//        RS.Print();
//        memoryBuffer.Print();
//        loadBuffer.Print();
//        storeBuffer.Print();
    }
    std::cout << ((UnsignedNumber) registers.ReadRegister() & 255);
    return 0;
}
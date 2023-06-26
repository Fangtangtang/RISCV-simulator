#include "src/ReorderBuffer.hpp"
#include "src/bus.hpp"
#include "tool/memory.hpp"
#include "src/predictor.hpp"
#include "src/RegFile.hpp"
#include "src/ReservationStation.hpp"
#include "src/StoreBuffer.hpp"
#include "src/LoadBuffer.hpp"

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
    StoreBuffer storeBuffer;
    LoadBuffer loadBuffer;
    RegisterFile registerFile;
    Predictor predictor;
    CDB bus;
    memory.Initialize();//scan all the code
    registerFile.Reset(registers);
    RegisterUnit pc;//pc points at the address of code, used in IF
    MachineCode machineCode = 0;
    Instruction instruction;
    bool IP_flag = true, reset_flag = false;//instruction process flag
    Index entry;
    int size_;
    Byte dest;
    std::pair<Index, Number> pair;
    while (true) {
        if (IP_flag) {
            //IF
            memory.InstructionFetch(pc, machineCode);
            //ID
            decoder.Decode(machineCode, instruction);//decode machine code to get instruction
//            if (instruction.instructionType == JALR)
//                IP_flag = false;//pc don't update in the clock cycle
            entry = RoB.AddInstruction(instruction, registerFile, predictor, RS, storeBuffer, loadBuffer, pcRS, pc);
            if (entry == -1 || instruction.instructionType == JALR ||
                instruction.instructionType == EXIT) {//fail to add
                IP_flag = false;
            } else {
                IP_flag = true;//pc updated
            }
        }
        //try to add into buffers
        //EX MEM
        if (!IP_flag && pcRS.Execute(bus, pc))
            IP_flag = true;
        RS.Execute(bus, alu);
        loadBuffer.Execute(bus, memory);
        storeBuffer.Execute(bus, memory);
        size_ = bus.Size();
        for (int i = 0; i < size_; ++i) {
            pair = bus.GetEle(i);
            RS.Modify(pair);
            loadBuffer.Modify(pair);
            storeBuffer.Modify(pair);
            if (!IP_flag)pcRS.Modify(pair);
            dest = RoB.Modify(pair, predictor);
            registerFile.Update(dest, pair);
        }
        bus.Clear();
        //WB
        if (RoB.Commit(registers, reset_flag)) break;
        if (reset_flag) {
            registerFile.Reset(registers);
            pcRS.Clear();
            RS.Clear();
            loadBuffer.Clear();
            storeBuffer.Clear();
        }
//        registerFile.Print();
//        RoB.Print();
//        RS.Print();
        loadBuffer.Print();
//        storeBuffer.Print();
    }
    std::cout << ((UnsignedNumber) registers.ReadRegister() & 255);
    return 0;
}
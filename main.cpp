#include "src/ReorderBuffer.hpp"
#include "src/ReservationStation.hpp"
#include "src/StoreBuffer.hpp"
#include "src/LoadBuffer.hpp"
#include "src/RegFile.hpp"
#include "src/predictor.hpp"
#include "src/bus.hpp"
#include "tool/memory.hpp"

int main() {
    Memory memory{};
    Registers registers;
    ALU alu;
    ReorderBuffer RoB;
    ReservationStation RS;
    StoreBuffer storeBuffer;
    LoadBuffer loadBuffer;
    RegisterFile registerFile;
    Predictor predictor;
    CDB bus;
    memory.Initialize();//scan all the code
    RegisterUnit pc;//pc points at the address of code, used in IF
    MachineCode pre_machineCode = 0, after_machineCode = 0;
    Instruction instruction;
    bool IP_flag= true;//instruction process flag
    while (true){
        //IF






    }
    std::cout << ((UnsignedNumber)registers.ReadRegister()& 255);
    return 0;
}
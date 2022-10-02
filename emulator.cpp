#include <QDebug>

#include <algorithm>
#include <cstring>

#include "emulator.h"
#include "mos6502.h"
#include "log.h"

Emulator::Emulator(){
    // Allocate memory
    this -> memory = new uint8_t[Emulator::kMemorySize];
    memset(this -> memory, 0xff, Emulator::kMemorySize);

    // Register to the helper functions
    EmulatorHelper::registerEmulator(this);

    // Instantiate cpu
    this -> cpu = new mos6502(EmulatorHelper::busRead, EmulatorHelper::busWrite);
    #ifndef lowerMemory // We can't load the reset vector if we don't have the whole memory
    // Reset the cpu
    this -> cpu -> Reset();
    #endif
}

Emulator::~Emulator(){
    // Deregister from helper functions
    EmulatorHelper::deregisterEmulator();

    // Clean up memory
    delete[] memory;
    delete cpu;
}

mos6502 *Emulator::get6502(){
    return cpu;
}

uint8_t Emulator::getMemoryValue(uint16_t address){
    return this -> memory[address];
}

/**
 * Sets memory address `address` to `value`, then emits `memoryChanged(value)`
 * @param address
 * @param value
 */
void Emulator::setMemoryValue(uint16_t address, uint8_t value){
    this -> memory[address] = value;
    emit memoryChanged(address);
}

void Emulator::step(){
    // Notify that we're about to run an instruction
    emit instructionRan();

    // Grab the previous values of the registers
    uint8_t A_before = cpu -> GetA();
    uint8_t P_before = cpu -> GetP();
    uint16_t PC_before = cpu -> GetPC();
    uint8_t S_before = cpu -> GetS();
    uint8_t X_before = cpu -> GetX();
    uint8_t Y_before = cpu -> GetY();

    // Run the next instruction, keeping track of how many cycles we ran
    uint64_t cycleCount = 0;
    this -> cpu -> Run(1, cycleCount);

    std::vector<Register> registers_to_update;
    // See if the register values changed and notify if they have
    if(A_before != cpu -> GetA()) registers_to_update.push_back(Register::A);
    if(P_before != cpu -> GetP()) registers_to_update.push_back(Register::P);
    if(PC_before != cpu -> GetPC()) registers_to_update.push_back(Register::PC);
    if(S_before != cpu -> GetS()) registers_to_update.push_back(Register::S);
    if(X_before != cpu -> GetX()) registers_to_update.push_back(Register::X);
    if(Y_before != cpu -> GetY()) registers_to_update.push_back(Register::Y);

    emit registersChanged(registers_to_update);

    // Log that we ran the instruction and the number of cycles
    Log::Info() << "Executed one instruction, " << cycleCount << " cycles";
}

void Emulator::resetCPU(){
    this -> cpu -> Reset();
}

// The emulator instance registered
// (Needed to be declared here to avoid double declaration)
namespace EmulatorHelper{
    Emulator *emulator;
}

void EmulatorHelper::registerEmulator(Emulator *emulator){
   EmulatorHelper::emulator = emulator;
}

void EmulatorHelper::deregisterEmulator(){
    EmulatorHelper::emulator = nullptr;
}

uint8_t EmulatorHelper::busRead(uint16_t address){
    return emulator -> getMemoryValue(address);
}


void EmulatorHelper::busWrite(uint16_t address, uint8_t value){
    emulator -> setMemoryValue(address, value);
}

void EmulatorHelper::replaceMemory(uint8_t *newContents, size_t offset, size_t length){
    memcpy((emulator -> memory) + offset, newContents, length);
    for(int offsetInNewContents = 0; offsetInNewContents < length; offsetInNewContents++){
        emulator -> setMemoryValue(offset + offsetInNewContents, newContents[offsetInNewContents]);
    }
}

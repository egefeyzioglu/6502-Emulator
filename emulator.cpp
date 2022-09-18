#include <QDebug>

#include <algorithm>
#include <cstring>

#include "emulator.h"
#include "mos6502.h"
#include "log.h"

Emulator::Emulator(){
    // Allocate memory
    this -> memory = new uint8_t[Emulator::kMemorySize];
    memset(this -> memory, 0, Emulator::kMemorySize);

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


void Emulator::setMemoryValue(uint16_t address, uint8_t value){
    this -> memory[address] = value;
}

void Emulator::step(){
    uint64_t count = 0;
    this -> cpu -> Run(1, count);
    Log::Info() << "Tried to run 1 step, ran " << count << " steps";
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
    return emulator -> memory[address];
}


void EmulatorHelper::busWrite(uint16_t address, uint8_t value){
    emulator -> memory[address] = value;
}

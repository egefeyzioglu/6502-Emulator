#pragma once

#include <vector>

#include "mos6502.h"

// Forward declaration of the emulator class
class Emulator;

// Helper functions for memory access
namespace EmulatorHelper{
    // Associate emulator instance to these functions
    void registerEmulator(Emulator *emulator);
    // Remove association of emulator instance from these functions
    void deregisterEmulator();
    // The emulator instance registered
    // Emulator *emulator; // Not declaring here to avoid double declaration when this file is included

    // Memory - CPU interface, set
    void busWrite(uint16_t address, uint8_t value);
    // Memory - CPU interface, get
    uint8_t busRead(uint16_t address);

    void replaceMemory(uint8_t *newContents, size_t offset, size_t lenght);
}

// The emulator class
class Emulator{
public:
    Emulator();
    ~Emulator();

    // Get the CPU instance
    mos6502 *get6502();

    // Get value from memory address
    uint8_t getMemoryValue(uint16_t address);
    // Set value at memory address
    void setMemoryValue(uint16_t address, uint8_t value);

    // Step one instruction
    void step();

    // Reset the CPU
    void resetCPU();


    //#define lowerMemory

    #ifdef lowerMemory
    // Memory size
    constexpr static size_t kMemorySize = 0x1000;
    #else
    // Memory size
    constexpr static size_t kMemorySize = 0x10000;
    #endif
    constexpr static size_t kProgMemOffset = 0x5f00;

private:
    // The CPU instance
    mos6502 *cpu;

    // The memory array
    uint8_t *memory;

    friend void EmulatorHelper::registerEmulator(Emulator *emulator);
    friend void EmulatorHelper::deregisterEmulator();;
    friend void EmulatorHelper::busWrite(uint16_t address, uint8_t value);
    friend uint8_t EmulatorHelper::busRead(uint16_t address);
    friend void EmulatorHelper::replaceMemory(uint8_t *newContents, size_t offset, size_t length);
};

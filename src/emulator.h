#pragma once

#include <QObject>

#include <vector>
#include <map>

#include "mos6502.h"
#include "memorymappeddevice.h"

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

    void replaceMemory(uint8_t *new_contents, size_t offset, size_t lenght);
}


class ProcessorRunWorker : public QObject{
    Q_OBJECT
public:
    ProcessorRunWorker(Emulator *emulator);

    void runCPU();
    void interrupt();

    bool should_run = true;
    Emulator *emulator;
};


// The emulator class
class Emulator : public QObject{
    Q_OBJECT
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
    int step();

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

    /**
     * The CPU will attempt to run at this speed
     */
    int clock_speed = 1000000;

    /**
     * The actual speed the CPU is running at
     */
    double real_clock_speed = 0;

    /**
     * The state of the emulator can be stored in this class. It will remember the registers and the entire memory
     */
    struct EmulatorState{
        EmulatorState(uint16_t PC, uint8_t S, uint8_t P, uint8_t A, uint8_t X, uint8_t Y, uint8_t *memory, size_t kMemorySize);
        ~EmulatorState();
        uint16_t PC;
        uint8_t S;
        uint8_t P;
        uint8_t A;
        uint8_t X;
        uint8_t Y;
        uint8_t *memory;
        size_t kMemorySize;
    };

    /**
     * Run the processor at `clockSpeed`
     */
    void run();

    /**
     * Interrupt the processor
     */
    void interrupt();


    enum Register{
        A,
        P,
        PC,
        S,
        X,
        Y
    };

    struct AddressRange{
        AddressRange(uint16_t base_address, uint16_t end_address) : base_address{base_address}, end_address{end_address} {};

        uint16_t base_address;
        uint16_t end_address;

        bool operator<(const AddressRange rhs) const{
            return this -> base_address < rhs.base_address;
        }
    };

    /**
     * Slot for when memory assigned to a DMA device changes without being directly set by the CPU
     *
     * Emits memoryChanged with the same parameter
     */
    void deviceMemoryChanged(uint16_t address);

signals:
    void memoryChanged(uint16_t address);
    void instructionRan();
    void registersChanged(std::vector<Register> registers_to_update);
    void startRunWorker();
    void interruptRunWorker();

private:
    // The CPU instance
    mos6502 *cpu;

    // The memory array
    uint8_t *memory;

    /**
     * If the emulator is currently in the run state
     *
     * When true, run() can't be called (will immediately return,) and memory and register
     * changes aren't reported to the UI
     */
    bool is_running;

    /**
     * The emulator's state before run() is called
     */
    EmulatorState *previous_state;

    QThread *worker_thread;

    /**
     * A map of all the memory devices we know of
     * The key is the `AddressRange` of the device, and the value is the
     * device object itself
     */
    std::map<AddressRange, MemoryMappedDevice*> memory_devices;

    /**
     * Adds a memory mapped device to the emulator
     * @param device
     */
    void addMemoryDevice(MemoryMappedDevice *device);

    friend void EmulatorHelper::registerEmulator(Emulator *emulator);
    friend void EmulatorHelper::deregisterEmulator();;
    friend void EmulatorHelper::busWrite(uint16_t address, uint8_t value);
    friend uint8_t EmulatorHelper::busRead(uint16_t address);
    friend void EmulatorHelper::replaceMemory(uint8_t *newContents, size_t offset, size_t length);
};

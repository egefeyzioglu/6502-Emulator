#include <QDebug>
#include <QThread>

#include <algorithm>
#include <cstring>

#include "emulator.h"
#include "mos6502.h"
#include "log.h"
#include "programram.h"
#include "rom.h"

Emulator::Emulator(){
    // Allocate memory
    this -> memory = new uint8_t[Emulator::kMemorySize];
    memset(this -> memory, 0xff, Emulator::kMemorySize);

    // Register to the helper functions
    EmulatorHelper::registerEmulator(this);

    // Set up memory
    ProgramRAM *program_RAM = new ProgramRAM(0x0000, 0x3fff);
    addMemoryDevice(program_RAM);
    ROM *program_ROM = new ROM(0x91ff, 0xffff - 0x91ff, 0xff);
    addMemoryDevice(program_ROM);

    // Instantiate cpu
    this -> cpu = new mos6502(EmulatorHelper::busRead, EmulatorHelper::busWrite);
    // Reset the cpu
    this -> cpu -> Reset();
    is_running = false;
}

Emulator::~Emulator(){
    // Deregister from helper functions
    EmulatorHelper::deregisterEmulator();

    // Clean up memory mapped devices
    for(auto memoryDevice : memory_devices){
        delete memoryDevice.second;
    }

    // Clean up memory
    delete[] memory;
    delete cpu;
}

mos6502 *Emulator::get6502(){
    return cpu;
}

uint8_t Emulator::getMemoryValue(uint16_t address){
    // Find the memory device corresponding to the given address and get its value
    for(auto const &memoryDevice : this -> memory_devices){
        if(memoryDevice.first.base_address <= address && memoryDevice.first.end_address >= address){
            return memoryDevice.second->getValue(address);
        }
    }
    return 0xFF; // Return -1 if the address is invalid
}

/**
 * Sets memory address `address` to `value`, then emits `memoryChanged(value)`
 * @param address
 * @param value
 */
void Emulator::setMemoryValue(uint16_t address, uint8_t value){
    // Find the memory device corresponding to the given address and set its value
    for(auto const &memoryDevice : this -> memory_devices){
        if(memoryDevice.first.base_address <= address && memoryDevice.first.end_address >= address){
            memoryDevice.second->setValue(address, value);
            if(!is_running) emit memoryChanged(address);
        }
    }
}

int Emulator::step(){
    if(!is_running){
        // If we're not in run mode, run instruction and notify normally

        // Notify that we're about to run an instruction
        emit instructionRan();

        // Grab the previous values of the registers
        uint8_t A_before = cpu -> GetA();
        uint8_t P_before = cpu -> GetP();
        uint16_t PC_before = cpu -> GetPC();
        uint8_t S_before = cpu -> GetS();
        uint8_t X_before = cpu -> GetX();
        uint8_t Y_before = cpu -> GetY();

        uint64_t cycle_count = 0;
        this -> cpu -> Run(1, cycle_count);

        // See if the register values changed and notify if they have
        std::vector<Register> registers_to_update;
        if(A_before != cpu -> GetA()) registers_to_update.push_back(Register::A);
        if(P_before != cpu -> GetP()) registers_to_update.push_back(Register::P);
        if(PC_before != cpu -> GetPC()) registers_to_update.push_back(Register::PC);
        if(S_before != cpu -> GetS()) registers_to_update.push_back(Register::S);
        if(X_before != cpu -> GetX()) registers_to_update.push_back(Register::X);
        if(Y_before != cpu -> GetY()) registers_to_update.push_back(Register::Y);

        emit registersChanged(registers_to_update);

        // Log and return the number of cycles we ran
        Log::Info() << "Executed one instruction, " << cycle_count << " cycles";
        return cycle_count;
    } else {
        // Otherwise, just step and return the cycle count
        uint64_t cycle_count = 0;
        this -> cpu -> Run(1, cycle_count);
        return cycle_count;
    }



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

void EmulatorHelper::replaceMemory(uint8_t *new_contents, size_t offset, size_t length){
    memcpy((emulator -> memory) + offset, new_contents, length); // TODO: Remove
    // Set the memory contents in the given range from the given buffer
    for(int offset_in_new_contents = 0; offset_in_new_contents < length; offset_in_new_contents++){
        emulator -> setMemoryValue(offset + offset_in_new_contents, new_contents[offset_in_new_contents]);
    }
}

Emulator::EmulatorState::EmulatorState(uint16_t PC, uint8_t S, uint8_t P, uint8_t A, uint8_t X, uint8_t Y, uint8_t *memory, size_t kMemorySize) : PC{PC}, S{S}, P{P}, A{A}, X{X}, Y{Y}, kMemorySize{kMemorySize}{
    this -> memory = new uint8_t[kMemorySize];
    memcpy(this -> memory, memory, kMemorySize);
}

Emulator::EmulatorState::~EmulatorState(){
    delete memory;
}

void Emulator::run(){
    if(is_running) return; // Can't run if we're already running

    // Save the current state so we can diff later
    previous_state = new EmulatorState(cpu -> GetPC(), cpu -> GetS(), cpu -> GetP(), cpu -> GetA(), cpu -> GetX(), cpu -> GetY(), memory, kMemorySize);

    // Spawn new thread to run the CPU
    worker_thread = new QThread();
    // Create the worker object and set it up in the new thread
    ProcessorRunWorker *worker = new ProcessorRunWorker(this);
    worker -> moveToThread(worker_thread);
    connect(this, &Emulator::startRunWorker, worker, &ProcessorRunWorker::runCPU);
    connect(this, &Emulator::interruptRunWorker, worker, &ProcessorRunWorker::interrupt);
    // Start the worker
    worker_thread -> start(QThread::HighPriority);
    emit startRunWorker();
    is_running = true;
}

void Emulator::interrupt(){
    // Stop the worker
    emit interruptRunWorker();
    worker_thread -> terminate();
    worker_thread -> wait();
    // We're no longer running, run() can be called again and changes should be updated the regular way
    is_running = false;
    // The real clock speed is now 0
    this -> real_clock_speed = 0;

    // See if the memory has changed and notify what changed if it has
    for(int addr = 0; addr < previous_state -> kMemorySize; addr++){
        if(previous_state -> memory[addr] != memory[addr]){
            emit memoryChanged((uint16_t) addr);
        }
    }
    // See if the register values changed and notify if they have
    std::vector<Register> registers_to_update;
    if(previous_state -> A != cpu -> GetA()) registers_to_update.push_back(Register::A);
    if(previous_state -> P != cpu -> GetP()) registers_to_update.push_back(Register::P);
    if(previous_state -> PC != cpu -> GetPC()) registers_to_update.push_back(Register::PC);
    if(previous_state -> S != cpu -> GetS()) registers_to_update.push_back(Register::S);
    if(previous_state -> X != cpu -> GetX()) registers_to_update.push_back(Register::X);
    if(previous_state -> Y != cpu -> GetY()) registers_to_update.push_back(Register::Y);

    emit registersChanged(registers_to_update);
}

void ProcessorRunWorker::runCPU(){
    should_run = true;
    // If we should be running
    while(should_run){
        // This is how long one period should be
        long long period_nanos = 1e9/(emulator->clock_speed);
        // Run the processor by one instruction and measure the time and cycles
        auto beginning = std::chrono::steady_clock().now();
        int cycles = emulator -> step();
        // Busy loop until we use up all the time we have for this instruction
        while(std::chrono::duration_cast<chrono::nanoseconds>(std::chrono::steady_clock().now() - beginning).count() < (period_nanos * cycles)) asm("");
        emulator -> real_clock_speed = ((double) 1e9 * (double) cycles)/((double) std::chrono::duration_cast<chrono::nanoseconds>(std::chrono::steady_clock().now() - beginning).count());
    }
}

void ProcessorRunWorker::interrupt(){
    should_run = false;
}

ProcessorRunWorker::ProcessorRunWorker(Emulator *emulator) : emulator{emulator} {}

void Emulator::addMemoryDevice(MemoryMappedDevice *device){
    // Format correctly and add to the map
    this -> memory_devices[
            Emulator::AddressRange(device -> getBaseAddress(), // The base address
                                   device -> getBaseAddress() + device -> getAddressSpaceLength() // The end address
                                   )
            ] = device;
    connect(device, &MemoryMappedDevice::addressChanged, this, &Emulator::deviceMemoryChanged);
}


void Emulator::deviceMemoryChanged(uint16_t address){
    emit memoryChanged(address);
}

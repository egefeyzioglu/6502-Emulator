#include <QDebug>
#include <QThread>

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
    isRunning = false;
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
    if(!isRunning) emit memoryChanged(address);
}

int Emulator::step(){
    if(!isRunning){
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

        uint64_t cycleCount = 0;
        this -> cpu -> Run(1, cycleCount);

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
        Log::Info() << "Executed one instruction, " << cycleCount << " cycles";
        return cycleCount;
    } else {
        // Otherwise, just step and return the cycle count
        uint64_t cycleCount = 0;
        this -> cpu -> Run(1, cycleCount);
        return cycleCount;
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

void EmulatorHelper::replaceMemory(uint8_t *newContents, size_t offset, size_t length){
    memcpy((emulator -> memory) + offset, newContents, length);
    for(int offsetInNewContents = 0; offsetInNewContents < length; offsetInNewContents++){
        emulator -> setMemoryValue(offset + offsetInNewContents, newContents[offsetInNewContents]);
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
    if(isRunning) return; // Can't run if we're already running

    // Save the current state so we can diff later
    previousState = new EmulatorState(cpu -> GetPC(), cpu -> GetS(), cpu -> GetP(), cpu -> GetA(), cpu -> GetX(), cpu -> GetY(), memory, kMemorySize);

    // Spawn new thread to run the CPU
    workerThread = new QThread();
    // Create the worker object and set it up in the new thread
    ProcessorRunWorker *worker = new ProcessorRunWorker(this);
    worker -> moveToThread(workerThread);
    connect(this, &Emulator::startRunWorker, worker, &ProcessorRunWorker::runCPU);
    connect(this, &Emulator::interruptRunWorker, worker, &ProcessorRunWorker::interrupt);
    // Start the worker
    workerThread -> start(QThread::HighPriority);
    emit startRunWorker();
    isRunning = true;
}

void Emulator::interrupt(){
    // Stop the worker
    emit interruptRunWorker();
    workerThread -> terminate();
    workerThread -> wait();
    // We're no longer running, run() can be called again and changes should be updated the regular way
    isRunning = false;
    // The real clock speed is now 0
    this -> realClockSpeed = 0;

    // See if the memory has changed and notify what changed if it has
    for(int addr = 0; addr < previousState -> kMemorySize; addr++){
        if(previousState -> memory[addr] != memory[addr]){
            emit memoryChanged((uint16_t) addr);
        }
    }
    // See if the register values changed and notify if they have
    std::vector<Register> registers_to_update;
    if(previousState -> A != cpu -> GetA()) registers_to_update.push_back(Register::A);
    if(previousState -> P != cpu -> GetP()) registers_to_update.push_back(Register::P);
    if(previousState -> PC != cpu -> GetPC()) registers_to_update.push_back(Register::PC);
    if(previousState -> S != cpu -> GetS()) registers_to_update.push_back(Register::S);
    if(previousState -> X != cpu -> GetX()) registers_to_update.push_back(Register::X);
    if(previousState -> Y != cpu -> GetY()) registers_to_update.push_back(Register::Y);

    emit registersChanged(registers_to_update);
}

void ProcessorRunWorker::runCPU(){
    shouldRun = true;
    // If we should be running
    while(shouldRun){
        // This is how long one period should be
        long long periodNanos = 1e9/(emulator->clockSpeed);
        // Run the processor by one instruction and measure the time and cycles
        auto beginning = std::chrono::steady_clock().now();
        int cycles = emulator -> step();
        // Busy loop until we use up all the time we have for this instruction
        while(std::chrono::duration_cast<chrono::nanoseconds>(std::chrono::steady_clock().now() - beginning).count() < (periodNanos * cycles)) asm("");
        emulator -> realClockSpeed = ((double) 1e9 * (double) cycles)/((double) std::chrono::duration_cast<chrono::nanoseconds>(std::chrono::steady_clock().now() - beginning).count());
    }
}

void ProcessorRunWorker::interrupt(){
    shouldRun = false;
}

ProcessorRunWorker::ProcessorRunWorker(Emulator *emulator) : emulator{emulator} {}

#include "programram.h"

#include <cstring>

ProgramRAM::ProgramRAM(uint16_t base_address, size_t length) : MemoryMappedDevice(base_address, length){
    // Instantiate the memory array with the given size
    this -> memory = new uint8_t[length];
    memset(this -> memory, 0xff, length);
}

ProgramRAM::~ProgramRAM(){
    // Clean up the memory array
    delete[] memory;
}

bool ProgramRAM::setValue(uint16_t address, uint8_t value){
    // Calculate the relative address
    size_t relativeAddress = address - this -> baseAddress;
    // If the address is valid, set the value and return true
    if(relativeAddress < this -> addressSpaceLength){
        memory[relativeAddress] = value;
        return true;
    }
    // Otherwise return false
    return false;
}
uint8_t ProgramRAM::getValue(uint16_t address){
    // Calculate the relative address
    size_t relativeAddress = address - this -> baseAddress;
    // If the address is valid, return the value
    if(relativeAddress < this -> addressSpaceLength){
        return memory[relativeAddress];
    }
    // Return -1 for invalid address
    return 0xFF;
}

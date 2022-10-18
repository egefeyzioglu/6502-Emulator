#include "rom.h"

#include <cstring>

ROM::ROM(uint16_t base_address, size_t length, size_t kNumBankedMemories) : MemoryMappedDevice(base_address, length),
                                                                                kNumBankedMemories{kNumBankedMemories},
                                                                                memorySize{length}{
    // Create and initialise the memory arrays
    memories = new uint8_t*[kNumBankedMemories];
    for(int bankNum = 0; bankNum < kNumBankedMemories; bankNum++){
        memories[bankNum] = new uint8_t[memorySize];
        memset(memories[bankNum], 0xFF, memorySize);
    }

    // Make the garbage data here at least valid
    current_bank_number = current_bank_number % kNumBankedMemories;

}

ROM::~ROM(){
    // Delete both levels of the memrory array
    for(int bankNum = 0; bankNum < kNumBankedMemories; bankNum++){
        delete[] memories[bankNum];
    }
    delete[] memories;
}

bool ROM::setValue(uint16_t address, uint8_t value){
    if(address == base_address){
        // Relative address 0 is the bank number register
        current_bank_number = value;
        // Notify that we changed a bunch of addresses
        // TODO: This should ideally be a single call passed all the way up
        for(int i = base_address; i < base_address + address_space_length; i++)
            emit addressChanged(i);
        return true;
    }else{
        // Everything else is a VRAM address
        uint16_t relative_address = address - (base_address + 1);
        if(relative_address < memorySize){
            memories[current_bank_number][relative_address] = value;
            return true;
        }
        return false;
    }
}

uint8_t ROM::getValue(uint16_t address){
    // Relative address 0 is the bank number register
    if(address == base_address){
        return current_bank_number;
    } else {
        // Grab value from memory, check if it's valid and if so, return it
        uint16_t relative_address = address - (base_address + 1);
        if(relative_address < memorySize){
            return memories[current_bank_number][relative_address];
        }
        return 0xFF; // Return -1 if the address is invalid
    }
}

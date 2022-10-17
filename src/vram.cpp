#include "vram.h"

#include <cstring>

ROM::ROM(uint16_t base_address, size_t length, size_t kNumBankedMemories) : MemoryMappedDevice(base_address, (length - 1) * kNumBankedMemories),
                                                                                kNumBankedMemories{kNumBankedMemories},
                                                                                memorySize{length - 1}{
    memories = new uint8_t*[kNumBankedMemories];
    for(int bankNum = 0; bankNum < kNumBankedMemories; bankNum++){
        memories[bankNum] = new uint8_t[memorySize];
        memset(memories[bankNum], 0xFF, memorySize);
    }

    // Make the garbage data here at least valid
    currentBankNumber = currentBankNumber % kNumBankedMemories;

}

ROM::~ROM(){
    for(int bankNum = 0; bankNum < kNumBankedMemories; bankNum++){
        delete[] memories[bankNum];
    }
    delete[] memories;
}

bool ROM::setValue(uint16_t address, uint8_t value){
    if(address == baseAddress){
        // Relative address 0 is the bank number register
        currentBankNumber = value;
        return true;
    }else{
        // Everything else is a VRAM address
        uint16_t relative_address = address - (baseAddress + 1);
        if(relative_address < memorySize){
            memories[currentBankNumber][relative_address] = value;
            return true;
        }
        return false;
    }
}

uint8_t ROM::getValue(uint16_t address){
    // Relative address 0 is the bank number register
    if(address == baseAddress){
        return currentBankNumber;
    } else {
        // Grab value from memory, check if it's valid and if so, return it
        uint16_t relative_address = address - (baseAddress + 1);
        if(relative_address < memorySize){
            return memories[currentBankNumber][relative_address];
        }
        return 0xFF; // Return -1 if the address is invalid
    }
}

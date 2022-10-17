#ifndef VRAM_H
#define VRAM_H

#include "memorymappeddevice.h"

class ROM : public MemoryMappedDevice
{
public:
    ROM(uint16_t base_address, size_t length, size_t kNumBankedMemories);
    ~ROM();

    bool setValue(uint16_t address, uint8_t value) override;
    uint8_t getValue(uint16_t address) override;

private:
    uint8_t** memories;
    size_t currentBankNumber;
    const size_t kNumBankedMemories;
    const size_t memorySize;
};

#endif // VRAM_H

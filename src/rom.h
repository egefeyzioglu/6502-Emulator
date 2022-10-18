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
    /**
     * 2D array (not really) of all the banked memories
     */
    uint8_t** memories;
    /**
     * Currently selected bank of memory
     */
    size_t current_bank_number;
    /**
     * Number of banked memories
     */
    const size_t kNumBankedMemories;
    /**
     * How large an individual bank of memory is
     *
     * This is different from addressSpaceLength(), since the bank
     * number register also occupies space, this excludes that.
     */
    const size_t memorySize;
};

#endif // VRAM_H

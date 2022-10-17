#ifndef PROGRAMRAM_H
#define PROGRAMRAM_H

#include "memorymappeddevice.h"


class ProgramRAM : public MemoryMappedDevice {
public:
    ProgramRAM(uint16_t base_address, size_t length);
    ~ProgramRAM();

    bool setValue(uint16_t address, uint8_t value) override;
    uint8_t getValue(uint16_t address) override;

private:
    uint8_t *memory;
};

#endif // PROGRAMRAM_H

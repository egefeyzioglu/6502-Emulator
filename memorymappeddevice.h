#ifndef MEMORYMAPPEDDEVICE_H
#define MEMORYMAPPEDDEVICE_H

#include <QObject>

#include <cstdint>

class MemoryMappedDevice : public QObject{

    Q_OBJECT

public:
    /**
     * Sets the register at a given address to a given value.
     *
     * @param address the address of the register being set, absolute
     * @param value the value the register is being set to
     * @return if the assignment was valid
     */
    virtual bool setValue(uint16_t address, uint8_t value) = 0;

    /**
     * Gets the value of a register at a given value. If the address
     * is invalid, an unspecified value can be returned but the call
     * should be stable.
     *
     * @param address the address of the register being read, absolute
     * @return the value of the register
     */
    virtual uint8_t getValue(uint16_t address) = 0;

    virtual uint16_t getBaseAddress(){return baseAddress;}
    virtual size_t getAddressSpaceLength(){return addressSpaceLength;}

    virtual ~MemoryMappedDevice(){};

signals:
    /**
     * Notify that the value at an address not directly accessed was changed
     *
     * @param address
     */
    void addressChanged(uint16_t address);

protected:

    MemoryMappedDevice(uint16_t baseAddress, size_t addressSpaceLength) : baseAddress{baseAddress}, addressSpaceLength{addressSpaceLength} {}
    /**
     * The base address of the device
     */
    const uint16_t baseAddress;

    /**
     * The size of the address space occupied by the device
     */
    const size_t addressSpaceLength;
};

#endif // MEMORYMAPPEDDEVICE_H

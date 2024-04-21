#ifndef MMIO_MMIO_H
#define MMIO_MMIO_H

#include <cstdint>

namespace mmio
{
    class Mmio
    {
    public:
        uint8_t get(const uint16_t address);

        void set(const uint16_t address, const uint8_t value);
    };
}

#endif
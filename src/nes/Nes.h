#ifndef NES_NES_H
#define NES_NES_H

#include <filesystem>

#include "cpu/MOS6502.h"

namespace nes
{
    class Nes
    {
    public:
        Nes();

        bool insert_cartridge(const std::filesystem::path &filename);

        bool init();

    private:
        cpu::MOS6502 cpu;

        std::shared_ptr<mmio::Mmio> mmio;
    };
}

#endif
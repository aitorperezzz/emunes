#ifndef NES_NES_H
#define NES_NES_H

#include <filesystem>

#include "cpu/MOS6502.h"

namespace nes
{
    class Nes
    {
    public:
        Nes(const std::filesystem::path &rom_filename);

        bool init();

        bool run();

    private:
        cpu::MOS6502 cpu;
        std::filesystem::path rom_filename;
    };
}

#endif
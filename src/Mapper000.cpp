#include "Mapper000.h"


namespace nes
{
    Mapper000::Mapper000(uint8_t nPRGBanks, uint8_t nCHRBanks): Mapper(nPRGBanks, nCHRBanks)
    {

    }

    bool Mapper000::PRG_mmap(uint16_t addr, uint16_t &m_addr)
    {   
        // if PRGROM is 16KB
        //     CPU Address Bus          PRG ROM
        //     0x8000 -> 0xBFFF: Map    0x0000 -> 0x3FFF
        //     0xC000 -> 0xFFFF: Mirror 0x0000 -> 0x3FFF
        // if PRGROM is 32KB
        //     CPU Address Bus          PRG ROM
        //     0x8000 -> 0xFFFF: Map    0x0000 -> 0x7FFF	
        if (addr >= 0x8000 && addr <= 0xFFFF)
        {
            m_addr = addr & (m_nPRGBanks > 1 ? 0x7FFF : 0x3FFF);
            return true;
        }

        return false;
    }
    bool Mapper000::CHR_mmap(uint16_t addr, uint16_t &m_addr)
    {
        // There is no mapping required for PPU
        // PPU Address Bus          CHR ROM
        // 0x0000 -> 0x1FFF: Map    0x0000 -> 0x1FFF
        if (addr >= 0x0000 && addr <= 0x1FFF)
        {
            m_addr = addr;
            return true;
        }

        return false;
    }
}

#pragma once
#include "Mapper.h"
namespace nes
{
    class Mapper000 : public Mapper
    {
    private:
        /* data */
    public:
        bool PRG_mmap(uint16_t addr, uint16_t &m_addr);
        bool CHR_mmap(uint16_t addr, uint16_t &m_addr);
        Mapper000(uint8_t nPRGBanks, uint8_t nCHRBanks);
    };  
}

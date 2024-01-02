#pragma once
#include <cstdint>
#include "Cartridge.h"
namespace nes
{
    class Mapper
    {
        
        public:
            Mapper(uint8_t nPRGBanks, uint8_t nCHRBanks):m_nPRGBanks(nPRGBanks),m_nCHRBanks(nCHRBanks){};
            virtual ~Mapper() = default;

            // 地址转换, 虚函数这里提供接口定义，由后续的继承的子类做实现，注意后面=0
            virtual bool PRG_mmap(uint16_t addr, uint16_t &m_addr) = 0;
            virtual bool CHR_mmap(uint16_t addr, uint16_t &m_addr) = 0;

        protected:
            uint8_t m_nPRGBanks = 0;
	        uint8_t m_nCHRBanks = 0;
    };
}





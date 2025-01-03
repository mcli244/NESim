#pragma once
#include <cstdint>
#include "Cartridge.h"

/*
    mapper是存在在卡带上的一个硬件设备
    用于处理PRG/CHR的页面切换的
    原因是CPU6502地址线只有16bit，最大寻址空间为64KB,且只有后32KB给卡带使用。
    卡带程序大于32KB时就需要扩展容量，这里mapper诞生了，通过分时复用方式把大于32KB部分的数据动态映射到CPU的寻址空间中。
    对于CPU来说是不感知的
    如何实现切换
        1. 卡带程序写，如向0x8000-0xFFFF中特定地址写入特定数据触发卡带的mapper进行地址地址映射。
        2. 所以不同的卡带有不同的mapper
*/

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





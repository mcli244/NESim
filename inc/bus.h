#pragma once
#include <vector>
#include <cstdint>
#include "Cartridge.h"
namespace nes
{
    using BUS_ADDR = std::uint16_t;    // 针对6502CPU来说，地址线为16bit
    using BUS_DATA = std::uint8_t;     // 针对6502CPU来说，地址线为8bit
    class bus
    {
    private:
        std::vector<BUS_DATA> cpuRAM;    // 总线上挂的内存，用容器模拟    2KB

        std::vector<BUS_DATA> ResetRAM;    // 测试使用
        std::shared_ptr<class Cartridge> m_cart;

    public:
        BUS_DATA read(BUS_ADDR addr);
        void write(BUS_ADDR addr, BUS_DATA value);
        bool connectCartridge(std::shared_ptr<class Cartridge> &cart);

    public:
        bus();
        ~bus();
    };
}


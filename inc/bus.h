#pragma once
#include <vector>
#include <cstdint>

namespace nes
{
    using BUS_ADDR = std::uint16_t;    // 针对6502CPU来说，地址线为16bit
    using BUS_DATA = std::uint8_t;     // 针对6502CPU来说，地址线为8bit
    class bus
    {
    private:
        std::vector<BUS_DATA> ram;    // 总线上挂的内存，用容器模拟

    public:
        BUS_DATA read(BUS_ADDR addr);
        void write(BUS_ADDR addr, BUS_DATA value);

    public:
        bus();
        ~bus();
    };
}


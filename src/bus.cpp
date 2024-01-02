#include "bus.h"

namespace nes
{
    bus::bus():
    cpuRAM(2048, 0),
    ResetRAM(2, 0)  // 测试使用
    {

    }

    bus::~bus(){};

    bool bus::connectCartridge(std::shared_ptr<class Cartridge> &cart)
    {
        m_cart = cart;
    }

    BUS_DATA bus::read(BUS_ADDR addr)
    {
        if(addr < 0x2000) // 8KB的内存映射，但是只有前2KB有SRAM可访问
            return cpuRAM[addr & 0x7FF];  // 2KB
        else if(addr < 0x4000) // ppu
        {

        }
        else if(addr < 0x4020)    // IO Reg
        {

        }
        // 后面的寻址空间就是卡带中的空间了
        else if(addr < 0x6000)  // 0x4020 - 0x6000 Expansion ROM
        {
            
        }
        else
        {
            if(addr == 0xfffc)
                return ResetRAM[0];
            else if(addr == 0xfffd)
                return ResetRAM[1];
        }

        return 0;
    }

    void bus::write(BUS_ADDR addr, BUS_DATA value)
    {
        if(addr < 0x2000) // 8KB的内存映射，但是只有前2KB有SRAM可访问
            cpuRAM[addr & 0x7FF] = value;  // 2KB
        else if(addr < 0x4000) // ppu
        {

        }
        else if(addr < 0x4020)    // IO Reg
        {

        }
        // 后面的寻址空间就是卡带中的空间了
        else if(addr < 0x6000)  // 0x4020 - 0x6000 Expansion ROM
        {
            
        }
        else
        {
            
            if(addr == 0xfffc)
                ResetRAM[0] = value;
            else if(addr == 0xfffd)
                ResetRAM[1] = value;
        }
    }
}



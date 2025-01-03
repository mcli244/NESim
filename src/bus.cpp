#include "bus.h"
#include "log.h"

namespace nes
{
    bus::bus():
    cpuRAM(2048, 0),
    ResetRAM(2, 0)  // 测试使用
    {

    }

    bus::~bus(){};

    bool bus::connectCartridge(nes::Cartridge *cart)
    {
        m_cart = cart;
    }

    bus::BUS_DATA bus::read(BUS_ADDR addr)
    {
        //LOG_DEBUG("read addr:0x%x", addr);
        if(addr < 0x2000) // 8KB的内存映射，但是只有前2KB有SRAM可访问
            return cpuRAM[addr & 0x7FF];  // 2KB
        else if(addr < 0x4000) // ppu
        {

        }
        else if(addr < 0x4020)    // IO Reg
        {

        }
        // 后面的寻址空间就是卡带中的空间了
        else
        {
            if(m_cart)
                return m_cart->read(addr);
        }

        return 0;
    }

    void bus::write(BUS_ADDR addr, BUS_DATA value)
    {
        //LOG_DEBUG("write addr:0x%x value:0x%x", addr, value);
        if(addr < 0x2000) // 8KB的内存映射，但是只有前2KB有SRAM可访问
            cpuRAM[addr & 0x7FF] = value;  // 2KB
        else if(addr < 0x4000) // ppu
        {

        }
        else if(addr < 0x4020)    // IO Reg
        {

        }
        // 后面的寻址空间就是卡带中的空间了
        else
        {
            if(m_cart)
                m_cart->write(addr, value);
        }
    }
}



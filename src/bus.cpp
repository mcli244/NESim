#include "bus.h"
#include "log.h"

namespace nes
{
    bus::bus():
    cpuRAM(2048, 0)
    {
        m_cart = nullptr;
        m_ppu = nullptr;
    }

    bus::~bus(){
        m_cart = nullptr;
        m_ppu = nullptr;
    };

    bool bus::connect(nes::Cartridge *cart, nes::olc2c02 *ppu)
    {
        if(cart && ppu)
        {
            m_cart = cart;
            m_ppu = ppu;
            return true;
        }

        LOG_ERROR("parameter error");
        return false;
    }

    bus::BUS_DATA bus::read(BUS_ADDR addr)
    {
        if(addr < 0x2000)
        {
            return cpuRAM[addr & 0x7FF];  // 2KB
        }
        else if(addr < 0x4000) // ppu
        {
            return m_ppu->read(addr);
        }
        else if(addr < 0x4020)    // IO Reg
        {

        }
        else
        {
            return m_cart->read(addr);
        }

        return 0;
    }

    void bus::write(BUS_ADDR addr, BUS_DATA value)
    {
        if(addr < 0x2000)
        {
            cpuRAM[addr & 0x7FF] = value;
        }
        else if(addr < 0x4000) // ppu
        {
            m_ppu->write(addr, value);
        }
        else if(addr < 0x4020)    // IO Reg
        {
            if(addr == 0x4014)  // OAMDMA
            {
                // DMA功能，这里做模拟就直接在这个时钟周期内把数据搬移到PPU内部
                // cpuRAM --> PPUOAM (256B)
                // TODO:这里有个潜在的问题，就是运行模拟器的设备如果拷贝这256字节用时太长，则可能会影响时序
                uint8_t *p = m_ppu->getOAMAddr();
                int cnt = 0;
                uint16_t r_addr = value;
                while(1)
                {
                    p[cnt++] = read(r_addr++);
                    if(cnt >= 256) break;
                }
            }
        }
        else
        {
            m_cart->write(addr, value);
        }
    }
}



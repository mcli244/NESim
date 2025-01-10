#pragma once
#include <vector>
#include <cstdint>
#include "Cartridge.h"
#include "olc2c02.h"

/***********************************************************************************************
 * 总线，用于连接CPU、PPU、Cartridge, 为CPU访问其他设备提供统一接口
 *  Address range	Size	Device
 *  $0000–$07FF	    $0800	2 KB internal RAM
 *  $0800–$0FFF	    $0800	|'''''''''''''''''''''''|
 *  $1000–$17FF	    $0800   |Mirrors of $0000–$07FF |
 *  $1800–$1FFF	    $0800   |.......................|
 *  $2000–$2007	    $0008	NES PPU registers
 *  $2008–$3FFF	    $1FF8	Mirrors of $2000–$2007 (repeats every 8 bytes)
 *  $4000–$4017	    $0018	NES APU and I/O registers
 *  $4018–$401F	    $0008	APU and I/O functionality that is normally disabled. See CPU Test Mode.
 *  $4020–$FFFF     $BFE0   Unmapped. Available for cartridge use.
************************************************************************************************/

namespace nes
{
    class bus
    {
    public:
        using BUS_ADDR = std::uint16_t;    
        using BUS_DATA = std::uint8_t;   
        static const BUS_ADDR NMIVector = 0xfffa;
        static const BUS_ADDR ResetVector = 0xfffc;
        static const BUS_ADDR IRQVector = 0xfffe;
    
    public:
        bus();
        ~bus();
        BUS_DATA read(BUS_ADDR addr);
        void write(BUS_ADDR addr, BUS_DATA value);
        bool connectCartridge(nes::Cartridge *cart);
        bool connectPPU(nes::olc2c02 *ppu);
        bool connect(nes::Cartridge *cart, nes::olc2c02 *ppu);

    private:
        std::vector<BUS_DATA> cpuRAM; 
        nes::olc2c02 *m_ppu = nullptr;
        nes::Cartridge *m_cart = nullptr;
    };
}


#pragma once
#include <vector>
#include <cstdint>
#include "Cartridge.h"
#include "olc2c02.h"
/*
Address range	Size	Device
$0000–$07FF	    $0800	2 KB internal RAM
$0800–$0FFF	    $0800	|'''''''''''''''''''''''|
$1000–$17FF	    $0800   |Mirrors of $0000–$07FF |
$1800–$1FFF	    $0800   |.......................|
$2000–$2007	    $0008	NES PPU registers
$2008–$3FFF	    $1FF8	Mirrors of $2000–$2007 (repeats every 8 bytes)
$4000–$4017	    $0018	NES APU and I/O registers
$4018–$401F	    $0008	APU and I/O functionality that is normally disabled. See CPU Test Mode.
$4020–$FFFF     $BFE0   Unmapped. Available for cartridge use.
    • $4020–$6000   $1FE0   Expansion ROM
    • $6000–$7FFF   $2000   Usually cartridge RAM, when present.
    • $8000–$FFF9	$7FF9   Usually cartridge ROM and mapper registers.
        • $8000-$BFFF     $4000    LPRG-ROM
        • $C000-$FFF9     $3FF9    UPRG-ROM
    • $FFFA-$FFFF	$0006   Reset/IRQ/NMI
        • $FFFA-$FFFB     $0002    NMIVector
        • $FFFC-$FFFD     $0002    ResetVector
        • $FFFE-$FFFF     $0002    IRQVector
*/

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

    private:
        std::vector<BUS_DATA> cpuRAM;    // 总线上挂的内存，用容器模拟    2KB
        nes::Cartridge *m_cart = nullptr;
        std::vector<BUS_DATA> ResetRAM;    // 测试使用
        nes::olc2c02 *m_ppu = nullptr;
    };
}


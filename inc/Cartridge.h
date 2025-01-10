#pragma once
#include <cstdint>
#include <string>
#include <fstream>
#include <vector>
#include <memory>
#include "Mapper000.h"

/*
    卡带：程序+mapper
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

    mapper
        把卡带的ROM/RAM映射到CPU能寻址到的范围$4020–$FFFF
        卡带数据可能大于$4020–$FFFF 这个范围，所以卡带提供了动态映射的能力。
        例如：向$8001 地址写入特定数据，则将卡带中的某块PRG映射到$4020–$FFFF空间
        卡带中由硬件完成，这里做模拟器只需要把地址映射完就行。
*/

namespace nes
{
    enum CartridgeMirrorMode{Vertical, Horizontal};
    class Cartridge
    {
        public:
            Cartridge(){};
            ~Cartridge(){};

            bool loadNesFile(std::string nesFile);
            uint8_t read(uint16_t addr);                // 由CPU调用，地址是CPU视角的地址，内部进由卡带处理做映射到相应的内存地址
            bool write(uint16_t addr, uint8_t value);   // 由CPU调用，地址是CPU视角的地址，内部进由卡带处理做映射到相应的内存地址
            enum CartridgeMirrorMode getMirrorMode(void);

        private:
            uint8_t readCHR(uint16_t addr);
            bool writeCHR(uint16_t addr, uint8_t value);
            uint8_t readPRG(uint16_t addr);
            bool writePRG(uint16_t addr, uint8_t value);
            void printHeader(void);

        private:
            std::vector<uint8_t> vCHRMemory;    
            std::vector<uint8_t> vPRGMemory;    
            std::shared_ptr<class Mapper> pMapper;
            uint8_t nMapperID = 0;
            uint8_t nPRGBanks = 0;
            uint8_t nCHRBanks = 0;
            enum CartridgeMirrorMode mirrorMode;
    };
}
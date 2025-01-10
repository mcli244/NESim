#pragma once
#include <cstdint>
#include "Cartridge.h"
#include "map.h"

/*
    // PPU Memory Map PPU的地址线是14bit的，最大寻址空间为0x3FFF
    $0000-$0FFF	$1000	Pattern table 0	Cartridge
    $1000-$1FFF	$1000	Pattern table 1	Cartridge
    $2000-$23FF	$0400	Nametable 0	Cartridge
    $2400-$27FF	$0400	Nametable 1	Cartridge
    $2800-$2BFF	$0400	Nametable 2	Cartridge
    $2C00-$2FFF	$0400	Nametable 3	Cartridge
    $3000-$3EFF	$0F00	Unused	Cartridge
    $3F00-$3F1F	$0020	Palette RAM indexes	Internal to PPU
    $3F20-$3FFF	$00E0	Mirrors of $3F00-$3F1F	Internal to PPU
*/
namespace nes
{
    class olc2c02
    {
        public:
            /* ppu对外提供的操作接口，地址以CPU视角定义，从0x2000开始, 用于操作PPU的寄存器 */
            uint8_t read(uint16_t cpu_addr);
            void write(uint16_t cpu_addr, uint8_t value);
            bool connectCartridge(nes::Cartridge *cart);
            void reset(void);
            void clock(void);
            olc2c02();
            ~olc2c02();

        public:// TODO: 测试需要外部访问寄存器，正式版本需要改为private
            
            /* ppu内部总线操作，地址以PPU视角定义，从0x0000开始 */
            uint8_t readVRAM(uint16_t ppu_addr);
            uint8_t writeVRAM(uint16_t ppu_addr, uint8_t dat);
        
        public: // TODO: 测试需要外部访问寄存器，正式版本需要改为private
            /* VARM */
            uint8_t PatternTable[2][1024];
            uint8_t NameTable[2][1024];
            uint8_t Palette[32];

            nes::Cartridge *m_cart = nullptr;
            uint16_t CurrentVRAMAddress;
            uint16_t TemporaryVRAMAddress;

            struct{
                uint8_t x;
                uint8_t y;
            }ScrollPosition;
            
            int32_t ScanLineCnt, PPUClockCnt;
            uint16_t BgTileIndex;
            uint16_t NameTableIndex, AttributeTableIndex, TileIndexLsb, TileIndexMsb;
            enum PPUREG{
                PPUCTRL	    = 0x2000,
                PPUMASK	    = 0x2001,
                PPUSTATUS	= 0x2002,
                OAMADDR	    = 0x2003,
                OAMDATA	    = 0x2004,
                PPUSCROLL	= 0x2005,
                PPUADDR	    = 0x2006,
                PPUDATA	    = 0x2007,
                OAMDMA      = 0x4014
            };

            // display
            Map map = Map(128, 128);

            // register
            union
            {
                struct{
                    uint8_t NameTableIndex : 2;
                    uint8_t IncrementMode : 1;
                    uint8_t SpritePattrenTableIndex : 1;
                    uint8_t BackgroundPattrenTableIndex : 1;
                    uint8_t SpriteSize : 1;
                    uint8_t SlaveMode : 1;
                    uint8_t EnableNMI : 1;
                };
                uint8_t val;
            }reg_ctrl;

            union
            {
                struct{
                    uint8_t unused : 5;
                    uint8_t SpriteOverflow : 1;
                    uint8_t SpriteZeroHit : 1;
                    uint8_t VerticalBlank : 1;
                };
                uint8_t val;
            }reg_status;

            union
            {
                struct{
                    uint8_t GrayScale : 1;
                    uint8_t RenderBackgroundLeft : 1;
                    uint8_t RenderSpritesLeft : 1;
                    uint8_t RenderBackground : 1;
                    uint8_t RenderSprites : 1;
                    uint8_t EnhanceRed : 1;
                    uint8_t EnhanceGreen : 1;
                    uint8_t EnhanceBlue : 1;
                };
                uint8_t val;
            }reg_mask;  

            union
            {
                struct{
                    uint16_t coarse_x : 5;
                    uint16_t coarse_y : 5;
                    uint16_t nametable_x : 1;
                    uint16_t nametable_y : 1;
                    uint16_t fine_y : 3;
                    uint16_t unused : 1;
                };
                uint16_t val;
            }reg_addr;  

            union
            {
                struct{
                    uint8_t Toggle : 1;
                    uint8_t UnUsed : 7;
                };
                uint8_t val;
            }reg_w;
    };
}
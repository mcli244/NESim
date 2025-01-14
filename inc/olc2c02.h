#pragma once
#include <cstdint>
// #include "olc6502.h"
#include "Cartridge.h"
#include "map.h"
#include <functional>


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
            bool setNMICb(std::function<void(void)> cb);
            
            void reset(void);
            void clock(void);
            olc2c02();
            ~olc2c02();

        public:// TODO: 测试需要外部访问寄存器，正式版本需要改为private
            
            /* ppu内部总线操作，地址以PPU视角定义，从0x0000开始 */
            uint8_t busRead(uint16_t ppu_addr);
            uint8_t busWrite(uint16_t ppu_addr, uint8_t dat);
            void DrawTile(uint8_t PatternTableIndex, uint8_t TileIndex);
            void DrawAllTile(uint8_t PatternTableIndex);
        
        public: // TODO: 测试需要外部访问寄存器，正式版本需要改为private
            /* VARM */
            uint8_t VRAM[2048];
            uint8_t PatternTable[2][4096];
            uint8_t NameTable[2][1024];
            uint8_t Palette[32];

            nes::Cartridge *m_cart = nullptr;
            std::function<void(void)> cpuNMICb;
            uint8_t PPUDataTmp;

            struct{
                uint8_t x;
                uint8_t y;
            }ScrollPosition;
            
            int32_t ScanLineCnt, PPUClockCnt;
            uint16_t BgTileIndex;
            uint16_t NameTableIndex, AttributeTableIndex, TileIndexLsb, TileIndexMsb;
            uint16_t TileIndexLsbLast, TileIndexMsbLast;
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
            Map map = Map(256, 256);

            // register
            union
            {
                struct{
                    uint8_t NameTableIndex : 2;                 // 名称表索引，(0 = $2000; 1 = $2400; 2 = $2800; 3 = $2C00)
                    uint8_t IncrementMode : 1;                  // 当CPU读取或者写入PPUDATA时，VRAMaddr会自动增加。规则是：0：增加1, 1：增加32
                    uint8_t SpritePattrenTableIndex : 1;        // Sprite pattern table address for 8x8 sprites (0: $0000; 1: $1000; ignored in 8x16 mode)
                    uint8_t BackgroundPattrenTableIndex : 1;    // 背景使用的Pattren (0: $0000; 1: $1000)
                    uint8_t SpriteSize : 1;                     // 精灵大小，0:8x8  1:8x16
                    uint8_t SlaveMode : 1;                      // PPU master/slave select (0: read backdrop from EXT pins; 1: output color on EXT pins) 模拟中没使用
                    uint8_t EnableNMI : 1;                      // Vblank NMI 使能 (0: off, 1: on)
                };
                uint8_t val;
            }reg_ctrl;

            union
            {
                struct{
                    uint8_t unused : 5;
                    uint8_t SpriteOverflow : 1; // Sprite 溢出标志
                    uint8_t SpriteZeroHit : 1;  // Sprite 0 命中标志
                    uint8_t VerticalBlank : 1;  //  Vblank 标志，读取时清除。
                };
                uint8_t val;
            }reg_status;

            union
            {
                struct{
                    uint8_t GrayScale : 1;                      // 灰度（0：正常颜色，1：灰度） 
                    uint8_t RenderBackgroundLeft : 1;           // 1：在屏幕最左边的 8 个像素显示背景，0：隐藏
                    uint8_t RenderSpritesLeft : 1;              // 1：在屏幕最左边的 8 个像素显示精灵，0：隐藏
                    uint8_t RenderBackground : 1;               // 1：启用背景渲染
                    uint8_t RenderSprites : 1;                  // 1：启用精灵渲染
                    uint8_t EnhanceRed : 1;                     // 强调红色（PAL/Dendy 上为绿色）
                    uint8_t EnhanceGreen : 1;                   // 强调绿色（PAL/Dendy 上为红色）
                    uint8_t EnhanceBlue : 1;                    // 强调蓝色
                };      
                uint8_t val;
            }reg_mask;  

            union
            {
                struct{
                    uint8_t Toggle : 1;
                    uint8_t UnUsed : 7;
                };
                uint8_t val;
            }reg_w; // 每次写入PPUSCROLL或PPUADDR时切换，指示这是第一次还是第二次写入。读取PPUSTATUS时清除。有时称为“写入锁存器”或“写入切换”。

            
            union
            {
                // 渲染阶段
                struct{
                    uint16_t coarse_x : 5;
                    uint16_t coarse_y : 5;
                    uint16_t nametable_x : 1;
                    uint16_t nametable_y : 1;
                    uint16_t fine_y : 3;
                    uint16_t unused : 1;
                };

                // 非渲染阶段
                uint16_t val;   // 临时VRAM地址（15位）；也可以看作是屏幕左上角的地址。
            }reg_v; //渲染期间，用于滚动位置。渲染之外，用作当前 VRAM 地址。
            

            union
            {
                // 渲染阶段
                struct{
                    uint16_t coarse_x : 5;
                    uint16_t coarse_y : 5;
                    uint16_t nametable_x : 1;
                    uint16_t nametable_y : 1;
                    uint16_t fine_y : 3;
                    uint16_t unused : 1;
                };

                // 非渲染阶段
                uint16_t val;   // 临时VRAM地址（15位）；也可以看作是屏幕左上角的地址。
            }reg_t; // 在渲染期间，指定下一个扫描线的起始粗 x 滚动和屏幕的起始 y 滚动。在渲染之外，在将滚动或 VRAM 地址传输到 v 之前保存它。

            
            union
            {
                struct{
                    uint8_t FineX : 3;   
                    uint8_t UnUsed : 5;
                };
                uint8_t val;
            }reg_x; // 当前滚动的精细 x 位置，与 v 一起渲染时使用。
            
    };
}
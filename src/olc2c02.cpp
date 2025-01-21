#include "olc2c02.h"
#include "log.h"


/*
    $2000–$2007	    $0008	NES PPU registers
    $2008–$3FFF	    $1FF8	Mirrors of $2000–$2007 (repeats every 8 bytes)
    3.1	$2000 (PPUCTRL) write
        bit[0:1]:   选择NameTable
                    00 0x2000   
                    01 0x2400
                    10 0x2800
                    11 0x2C00
        bit2:   
        bit3：jl PatternTable选择 ， 0:0x0000      1:0x10000
        bit4：bg PatternTable选择 ， 0:0x0000      1:0x10000
        bit5: jl size 0: 8x8    1:  8x16
        bit7: V_Blank NMI
    3.2	$2002 (PPUSTATUS) read
    3.3	$2005 (PPUSCROLL) first write (w is 0)
    3.4	$2005 (PPUSCROLL) second write (w is 1)
    3.5	$2006 (PPUADDR) first write (w is 0)
    3.6	$2006 (PPUADDR) second write (w is 1)
*/

/*
    $2000–$2007	    $0008	NES PPU registers
    $2008–$3FFF	    $1FF8	Mirrors of $2000–$2007 (repeats every 8 bytes)
    3.1	$2000 (PPUCTRL) write
        bit7: 是能NMI中断
        bit5: 精灵大小 0: 8x8    1:  8x16
        bit4：背景 PatternTable选择 ， 0:0x0000      1:0x10000
        bit3：精灵 PatternTable选择 ， 0:0x0000      1:0x10000
        bit2: 增长模式  0: 增加1（水平滚动）    1： 增加32 （垂直滚动）  
        bit[1:0]:   选择NameTable
                    00 0x2000   
                    01 0x2400
                    10 0x2800
                    11 0x2C00
    3.2	$2002 (PPUSTATUS) read
        bit[0:4]:   unused   
        bit5: 精灵是否溢出，0: 未超过8个    1：超过了8个 
        bit6：
        bit7：
    3.3	$2005 (PPUSCROLL) first write (w is 0)
    3.4	$2005 (PPUSCROLL) second write (w is 1)
    3.5	$2006 (PPUADDR) first write (w is 0)
    3.6	$2006 (PPUADDR) second write (w is 1)

PPUCTRL	    $2000	VPHB SINN	W	NMI enable (V), PPU master/slave (P), sprite height (H), background tile select (B), sprite tile select (S), increment mode (I), nametable select / X and Y scroll bit 8 (NN)
PPUMASK	    $2001	BGRs bMmG	W	color emphasis (BGR), sprite enable (s), background enable (b), sprite left column enable (M), background left column enable (m), greyscale (G)
PPUSTATUS	$2002	VSO- ----	R	vblank (V), sprite 0 hit (S), sprite overflow (O); read resets write pair for $2005/$2006
OAMADDR	    $2003	AAAA AAAA	W	OAM read/write address
OAMDATA	    $2004	DDDD DDDD	RW	OAM data read/write
PPUSCROLL	$2005	XXXX XXXX YYYY YYYY	Wx2	X and Y scroll bits 7-0 (two writes: X scroll, then Y scroll)
PPUADDR	    $2006	..AA AAAA AAAA AAAA	Wx2	VRAM address (two writes: most significant byte, then least significant byte)
PPUDATA	    $2007	DDDD DDDD	RW	VRAM data read/write
OAMDMA	    $4014	AAAA AAAA	W	OAM DMA high address



*/
namespace nes
{

    void olc2c02::pixelColorInit(void)
    {
        auto ColorMux = [&](uint8_t r, uint8_t g, uint8_t b) 
        {
            uint32_t color = 0;
            color = (r << 16) | (g << 8) | (b);
            return color;
        };

        PixelColor[0x00] = ColorMux(84, 84, 84);
        PixelColor[0x01] = ColorMux(0, 30, 116);
        PixelColor[0x02] = ColorMux(8, 16, 144);
        PixelColor[0x03] = ColorMux(48, 0, 136);
        PixelColor[0x04] = ColorMux(68, 0, 100);
        PixelColor[0x05] = ColorMux(92, 0, 48);
        PixelColor[0x06] = ColorMux(84, 4, 0);
        PixelColor[0x07] = ColorMux(60, 24, 0);
        PixelColor[0x08] = ColorMux(32, 42, 0);
        PixelColor[0x09] = ColorMux(8, 58, 0);
        PixelColor[0x0A] = ColorMux(0, 64, 0);
        PixelColor[0x0B] = ColorMux(0, 60, 0);
        PixelColor[0x0C] = ColorMux(0, 50, 60);
        PixelColor[0x0D] = ColorMux(0, 0, 0);
        PixelColor[0x0E] = ColorMux(0, 0, 0);
        PixelColor[0x0F] = ColorMux(0, 0, 0);

        PixelColor[0x10] = ColorMux(152, 150, 152);
        PixelColor[0x11] = ColorMux(8, 76, 196);
        PixelColor[0x12] = ColorMux(48, 50, 236);
        PixelColor[0x13] = ColorMux(92, 30, 228);
        PixelColor[0x14] = ColorMux(136, 20, 176);
        PixelColor[0x15] = ColorMux(160, 20, 100);
        PixelColor[0x16] = ColorMux(152, 34, 32);
        PixelColor[0x17] = ColorMux(120, 60, 0);
        PixelColor[0x18] = ColorMux(84, 90, 0);
        PixelColor[0x19] = ColorMux(40, 114, 0);
        PixelColor[0x1A] = ColorMux(8, 124, 0);
        PixelColor[0x1B] = ColorMux(0, 118, 40);
        PixelColor[0x1C] = ColorMux(0, 102, 120);
        PixelColor[0x1D] = ColorMux(0, 0, 0);
        PixelColor[0x1E] = ColorMux(0, 0, 0);
        PixelColor[0x1F] = ColorMux(0, 0, 0);

        PixelColor[0x20] = ColorMux(236, 238, 236);
        PixelColor[0x21] = ColorMux(76, 154, 236);
        PixelColor[0x22] = ColorMux(120, 124, 236);
        PixelColor[0x23] = ColorMux(176, 98, 236);
        PixelColor[0x24] = ColorMux(228, 84, 236);
        PixelColor[0x25] = ColorMux(236, 88, 180);
        PixelColor[0x26] = ColorMux(236, 106, 100);
        PixelColor[0x27] = ColorMux(212, 136, 32);
        PixelColor[0x28] = ColorMux(160, 170, 0);
        PixelColor[0x29] = ColorMux(116, 196, 0);
        PixelColor[0x2A] = ColorMux(76, 208, 32);
        PixelColor[0x2B] = ColorMux(56, 204, 108);
        PixelColor[0x2C] = ColorMux(56, 180, 204);
        PixelColor[0x2D] = ColorMux(60, 60, 60);
        PixelColor[0x2E] = ColorMux(0, 0, 0);
        PixelColor[0x2F] = ColorMux(0, 0, 0);

        PixelColor[0x30] = ColorMux(236, 238, 236);
        PixelColor[0x31] = ColorMux(168, 204, 236);
        PixelColor[0x32] = ColorMux(188, 188, 236);
        PixelColor[0x33] = ColorMux(212, 178, 236);
        PixelColor[0x34] = ColorMux(236, 174, 236);
        PixelColor[0x35] = ColorMux(236, 174, 212);
        PixelColor[0x36] = ColorMux(236, 180, 176);
        PixelColor[0x37] = ColorMux(228, 196, 144);
        PixelColor[0x38] = ColorMux(204, 210, 120);
        PixelColor[0x39] = ColorMux(180, 222, 120);
        PixelColor[0x3A] = ColorMux(168, 226, 144);
        PixelColor[0x3B] = ColorMux(152, 226, 180);
        PixelColor[0x3C] = ColorMux(160, 214, 228);
        PixelColor[0x3D] = ColorMux(160, 162, 160);
        PixelColor[0x3E] = ColorMux(0, 0, 0);
        PixelColor[0x3F] = ColorMux(0, 0, 0);
    }

    olc2c02::olc2c02()
    {
        map.Clear();
        memset(&PatternTable[0], 0, 1024);
        memset(&PatternTable[1], 0, 1024);

        memset(&NameTable[0], 0, 1024);
        memset(&NameTable[1], 0, 1024);

        memset(&Palette, 0, 32);
        pixelColorInit();
    };

    olc2c02::~olc2c02()
    {
        m_cart = nullptr;
    };

    
    uint32_t olc2c02::getColor(uint8_t index)
    {
        return PixelColor[index&0x3F];
    }

    uint8_t olc2c02::read(uint16_t cpu_addr)
    {   
        uint8_t data = 0;
        switch (cpu_addr)
        {
        case PPUCTRL:    break;
        case PPUMASK:    break;
        case PPUSTATUS:  
            data = (reg_status.val & 0xE0); // 状态寄存器只有高三位有效
            // 读取状态寄存器会清除w寄存器和VerticalBlank标志
            reg_status.VerticalBlank = 0;
            reg_w.Toggle = 0;
            break;
        case OAMADDR:    break;
        case OAMDATA:    
            data = pOAM[oamAddr];
            break;
        case PPUSCROLL:  break;
        case PPUADDR:    break;
        case PPUDATA:  
            /*
                从 PPUDATA 读取不会直接返回当前 VRAM 地址的值，而是返回内部读取缓冲区的内容。
                此读取缓冲区在每次读取 PPUDATA 时都会更新，但仅在先前的内容返回到 CPU 之后才会更新，
                从而有效地将 PPUDATA 读取延迟了一次。这是因为 PPU 总线读取太慢，无法及时完成以服务 
                CPU 读取。由于此读取缓冲区，在通过 PPUADDR 设置 VRAM 地址后，应先读取 PPUDATA 以
                准备读取缓冲区（忽略结果），然后再从中读取所需的数据。请注意，读取缓冲区仅在 PPUDATA 
                读取时更新。它不受写入或其他 PPU 进程（如渲染）的影响，并且会无限期地保持其值直到下一次读取。
            */  
            {
                uint8_t val = busRead(reg_v.val);    // 用内部寄存器的VRAM的当前地址读取
                if(cpu_addr < 0x3f00)  
                {
                    data = PPUDataTmp;
                    PPUDataTmp = val;
                }
                else    // 读取调色板数据时，不用延迟，直接返回
                {
                    data = val;
                }
                reg_v.val += (reg_ctrl.IncrementMode ? 32 : 1);
            }
            break;
        default:
            // LOG_ERROR("addr:0x%x not supported!", addr);
            break;
        }

        return data;
    } 

    void olc2c02::write(uint16_t cpu_addr, uint8_t dat)
    {
        switch (cpu_addr)
        {
        case PPUCTRL:    
            reg_ctrl.val = dat;
            reg_t.nametable = reg_ctrl.NameTableIndex;
            break;
        case PPUMASK:    
            reg_mask.val = dat;  
            break;
        case PPUSTATUS:    break;
        case OAMADDR:
            oamAddr = dat;
            break;
        case OAMDATA:    
            pOAM[oamAddr] = dat;
            break;
        case PPUSCROLL:    
            // 此寄存器用于改变滚动位置，告诉 PPU 通过PPUCTRL选择的名称表中的哪个像素应该位于渲染屏幕的左上角。 
            // PPUSCROLL 需要两次写入：第一次是 X 滚动，第二次是 Y 滚动。
            if(reg_w.Toggle == 0)
            {
                fine_x = dat & 0x07;
                reg_t.coarse_x = dat >> 3;
                reg_w.Toggle = 1;
            }
            else
            {
                reg_t.fine_y = dat & 0x07;
                reg_t.coarse_y = dat >> 3;
                reg_w.Toggle = 0;
                // LOG_INFO("reg_t %02d %02d %02d %02d", reg_t.coarse_x, reg_t.coarse_y, fine_x, reg_t.fine_y);
            }
            break;
        case PPUADDR:    
            /* 数据总线时8bit, 地址是14bit， 所以这里要写两次。第一次写入高6位，第二次写入地8位 */
            if(reg_w.Toggle == 0)
            {
                reg_t.val &= ~0xFF00;
                reg_t.val |= ((dat & 0x3F) << 8);
                reg_w.Toggle = 1;
            }
            else
            {
                reg_t.val &= ~0x00FF;
                reg_t.val |= (dat & 0xFF);
                reg_v.val = reg_t.val;
                reg_w.Toggle = 0;
            }
            break;
        case PPUDATA:    
            busWrite(reg_v.val, dat);
            reg_v.val += (reg_ctrl.IncrementMode ? 32 : 1);  // 自动增长
            break;
        default:
            //LOG_ERROR("addr:0x%x not supported!", cpu_addr);
            break;
        }
    }
    
    /*
        $0000-1FFF 通常由卡带映射到CHR-ROM 或 CHR-RAM，通常带有银行切换机制。
        $2000-2FFF 通常映射到 2kB NES 内部 VRAM，提供 2 个名称表，其镜像配置由卡带控制，
                    但它可以部分或全部重新映射到卡带上的 ROM 或 RAM，允许最多同时使用 4 个名称表。
        $3000-3EFF 通常是 $2000-2EFF 的 2kB 区域的镜像。PPU 不会从这个地址范围进行渲染，因此这个空间的实用性可以忽略不计。
        $3F00-3FFF 不可配置，始终映射到内部调色板控件。
    */
    uint8_t olc2c02::busRead(uint16_t ppu_addr)
    {
        uint8_t data = 0x00;

        if(ppu_addr <= 0x1FFF)  // --> Cartridge CHR-RAM/ROM
        {
            data = m_cart->read(0x6000 | ppu_addr); // 卡带提供的接口是以CPU视角传入地址的
        }
        else if(ppu_addr <= 0x3EFF) // NameTable
        {
            /* 0x2000 - 0x2FFF      4KB                 // 总共4KB，但是实际内存只有2KB，其余2KB是前面2KB的镜像
                - 0x2000 - 0x23FF   1KB NameTable 0
                - 0x2400 - 0x27FF   1KB NameTable 1
                - 0x2800 - 0x2BFF	1KB NameTable 2     // 这部分内存实际不存在，对1KB的访问会被映射到NameTable 0/1 具体看卡带中的MirrorMode
                - 0x2C00 - 0x2FFF	1KB NameTable 3     // 这部分内存实际不存在，对1KB的访问会被映射到NameTable 0/1 具体看卡带中的MirrorMode
               0x3000 - 0x3EFF      3KB                 // 这部分内存PPU是不会访问的，通常为0x2000 - 0x2FFF的映射
            */
            nes::CartridgeMirrorMode minorMode = m_cart->getMirrorMode();
            if(minorMode == nes::CartridgeMirrorMode::Horizontal)
            {
                //
				// +--------+--------+	
				// | 2000 A | 2400 A |	
				// +--------+--------+
				// | 2800 B | 2C00 B |	
				// +--------+--------+
				//
                if (ppu_addr >= 0x2000 && ppu_addr <= 0x23FF)
                    data = NameTable[0][ppu_addr & 0x03FF];
                if (ppu_addr >= 0x2400 && ppu_addr <= 0x27FF)
                    data = NameTable[0][ppu_addr & 0x03FF];
                if (ppu_addr >= 0x2800 && ppu_addr <= 0x2BFF)
                    data = NameTable[1][ppu_addr & 0x03FF];
                if (ppu_addr >= 0x2C00 && ppu_addr <= 0x2FFF)
                    data = NameTable[1][ppu_addr & 0x03FF];
            }
            else
            {
                //
				// +--------+--------+	
				// | 2000 A | 2400 B |	
				// +--------+--------+
				// | 2800 A | 2C00 B |	
				// +--------+--------+
				//
                if (ppu_addr >= 0x2000 && ppu_addr <= 0x23FF)
                    data = NameTable[0][ppu_addr & 0x03FF];
                if (ppu_addr >= 0x2400 && ppu_addr <= 0x27FF)
                    data = NameTable[1][ppu_addr & 0x03FF];
                if (ppu_addr >= 0x2800 && ppu_addr <= 0x2BFF)
                    data = NameTable[0][ppu_addr & 0x03FF];
                if (ppu_addr >= 0x2C00 && ppu_addr <= 0x2FFF)
                    data = NameTable[1][ppu_addr & 0x03FF];
            }
        }
        else if(ppu_addr <= 0x3FFF)  // Palette
        {
            /* 0x3F00 - 0x3FFF           256B                 
                - 0x3F00 - 0x3F1F        32B   // 真实内存，分成了背景和精灵的调色盘
                    - 0x3F00 - 0x3F03    4B    // Background Palette 0
                    - 0x3F04 - 0x3F07    4B    // Background Palette 1
                    - 0x3F08 - 0x3F0B    4B    // Background Palette 2
                    - 0x3F0C - 0x3F0F    4B    // Background Palette 3
                    - 0x3F10 - 0x3F13    4B    // Spirit Palette 0
                    - 0x3F14 - 0x3F17    4B    // Spirit Palette 1
                    - 0x3F18 - 0x3F1B    4B    // Spirit Palette 2
                    - 0x3F1C - 0x3F1F    4B    // Spirit Palette 3
                - 0x3F20 - 0x3FFF   223B    // 0x3F00 - 0x3F1F的镜像
                1. 8个palette的第0个字节，通常情况下颜色值相同，也可以存不同颜色，但是PPU只用3F00处的作为背景色，
                    所以其余7个palette的第0字节实际上是没有用上的，只用了后面3个字节
                2. 用Background Palette 0的第0个字节作为通用背景色，且PPU只用3F00处的作为背景色
                3. Palette的第0个字节，对于背景来说，就是一种背景颜色；对与精灵来说，就是透明的不用渲染
                4. PPU最多支持32中颜色，从0-31进行编号；调色板中的值实际就是这个编号。 
                5. 调色板中每个字节中的组成如下，这个构成了一个索引，凭此可从32字节的颜色中选择一个颜色。
                    4bit0
                    -----
                    SAAPP
                    |||||
                    |||++- tile pattern的像素值
                    |++--- attributes中的调色板编号 0-4
                    +----- 背景0/精灵1的选择
                6. 颜色数据放哪里？ 
                    - 颜色由NES硬件固化的编码，总共64中颜色。
                    - 这些颜色由6bit(RGB:222)组成
            */
            // ppu_addr &= 0x001F; // 32B的Palette空间
            // // if (ppu_addr == 0x0010) ppu_addr = 0x0000;
            // // if (ppu_addr == 0x0014) ppu_addr = 0x0004;
            // // if (ppu_addr == 0x0018) ppu_addr = 0x0008;
            // // if (ppu_addr == 0x001C) ppu_addr = 0x000C;
            // // data = Palette[ppu_addr] & (reg_mask.GrayScale ? 0x30 : 0x3F);  
            // data = Palette[ppu_addr] & 0x1F;    // 取低5位有效  

            ppu_addr &= 0x001F;
            if (ppu_addr == 0x0010) ppu_addr = 0x0000;
            if (ppu_addr == 0x0014) ppu_addr = 0x0004;
            if (ppu_addr == 0x0018) ppu_addr = 0x0008;
            if (ppu_addr == 0x001C) ppu_addr = 0x000C;
            data = Palette[ppu_addr] & (reg_mask.GrayScale ? 0x30 : 0x3F);
        }
        // else
        //     LOG_ERROR("addr:0x%x not supported!", ppu_addr);

        return data;
    }

    uint8_t olc2c02::busWrite(uint16_t ppu_addr, uint8_t dat)
    {   
        if(ppu_addr <= 0x1FFF)   // --> Cartridge CHR-RAM/ROM
        {
            m_cart->write(0x6000 | ppu_addr, dat);
        }
        else if(ppu_addr <= 0x3EFF) // NameTable
        {
            nes::CartridgeMirrorMode minorMode = m_cart->getMirrorMode();;
            if(minorMode == nes::CartridgeMirrorMode::Horizontal)
            {
                if (ppu_addr >= 0x2000 && ppu_addr <= 0x23FF)
                    NameTable[0][ppu_addr & 0x03FF] = dat;
                if (ppu_addr >= 0x2400 && ppu_addr <= 0x27FF)
                    NameTable[0][ppu_addr & 0x03FF] = dat;
                if (ppu_addr >= 0x2800 && ppu_addr <= 0x2BFF)
                    NameTable[1][ppu_addr & 0x03FF] = dat;
                if (ppu_addr >= 0x2C00 && ppu_addr <= 0x2FFF)
                    NameTable[1][ppu_addr & 0x03FF] = dat;
            }
            else
            {
                if (ppu_addr >= 0x2000 && ppu_addr <= 0x23FF)
                    NameTable[0][ppu_addr & 0x03FF] = dat;
                if (ppu_addr >= 0x2400 && ppu_addr <= 0x27FF)
                    NameTable[1][ppu_addr & 0x03FF] = dat;
                if (ppu_addr >= 0x2800 && ppu_addr <= 0x2BFF)
                    NameTable[0][ppu_addr & 0x03FF] = dat;
                if (ppu_addr >= 0x2C00 && ppu_addr <= 0x2FFF)
                    NameTable[1][ppu_addr & 0x03FF] = dat;
            }
            
        }
        else if(ppu_addr <= 0x3FFF)  // Palette
        {
            // ppu_addr &= 0x001F;
            // Palette[ppu_addr] = dat;
            ppu_addr &= 0x001F;
            if (ppu_addr == 0x0010) ppu_addr = 0x0000;
            if (ppu_addr == 0x0014) ppu_addr = 0x0004;
            if (ppu_addr == 0x0018) ppu_addr = 0x0008;
            if (ppu_addr == 0x001C) ppu_addr = 0x000C;
            Palette[ppu_addr] = dat;
        }
        // else
        //     LOG_ERROR("addr:0x%x not supported!", ppu_addr);
    }

    void olc2c02::reset(void)
    {
        reg_ctrl.val = 0;
        reg_mask.val = 0;
        reg_status.val = 0;
        reg_w.val = 0;
        reg_t.val = 0;
        ScanLineCnt = -1;
        PPUClockCnt = 0;
        PPUDataTmp = 0;
        oddFrame = false;
        fine_x = 0x00;
        spriteScanlineCnt = 0;
    }

    void olc2c02::DrawTile(uint8_t PatternTableIndex, uint8_t TileIndex)
    {
        map.Clear();
        uint32_t data_offset = TileIndex * 16;
        for(uint8_t row=0; row<8; row++)
        {
            uint8_t tile_lsb = busRead(PatternTableIndex * 0x1000 + row + data_offset);
            uint8_t tile_msb = busRead(PatternTableIndex * 0x1000 + row + 0x0008 + data_offset);
            for(uint8_t col=0; col<8; col++)
            {
                uint8_t pixel = ((tile_msb & 0x01) << 1) | (tile_lsb & 0x01);
                tile_lsb >>= 1;
                tile_msb >>= 1;
                uint8_t pixel_x = (7 - col);
                uint8_t pixel_y = row;
                switch (pixel)
                {
                case 0: map.DrawPoint(pixel_x, pixel_y, 0); break;
                case 1: map.DrawPoint(pixel_x, pixel_y, 63); break;
                case 2: map.DrawPoint(pixel_x, pixel_y, 127); break;
                case 3: map.DrawPoint(pixel_x, pixel_y, 255); break;
                default:
                    LOG_ERROR("tmp:0x%x", pixel);
                    break;
                }
            }
        }
        map.Refresh();
    }
    void olc2c02::DrawAllTile(uint8_t PatternTableIndex)
    {
        map.Clear();
        // draw tile 
        for(uint8_t TileY=0; TileY<16; TileY++)
        {
            for(uint8_t TileX=0; TileX<16; TileX++)
            {
                uint32_t data_offset = TileX * 16 + TileY * 16 * 16;
                for(uint8_t row=0; row<8; row++)
                {
                    uint8_t tile_lsb = busRead(PatternTableIndex * 0x1000 + row + data_offset);
                    uint8_t tile_msb = busRead(PatternTableIndex * 0x1000 + row + 0x0008 + data_offset);
                    for(uint8_t col=0; col<8; col++)
                    {
                        uint8_t pixel = ((tile_msb & 0x01) << 1) | (tile_lsb & 0x01);
                        tile_lsb >>= 1;
                        tile_msb >>= 1;
                        uint8_t pixel_x = TileX * 8 + (7 - col);
                        uint8_t pixel_y = TileY * 8 + row;
                        switch (pixel)
                        {
                        case 0: map.DrawPoint(pixel_x, pixel_y, 0); break;
                        case 1: map.DrawPoint(pixel_x, pixel_y, 63); break;
                        case 2: map.DrawPoint(pixel_x, pixel_y, 127); break;
                        case 3: map.DrawPoint(pixel_x, pixel_y, 255); break;
                        default:
                            LOG_ERROR("tmp:0x%x", pixel);
                            break;
                        }
                    }
                }
            }
        }
        map.Refresh();
    }

    bool olc2c02::connectCartridge(nes::Cartridge *cart)
    {
        m_cart = cart;
    }   
    
    bool olc2c02::setNMICb(std::function<void(void)> cb)
    {
        cpuNMICb = cb;
    }

    void olc2c02::clock(void)
    {
        
        auto IncrementScrollX = [&]()
        {
            if (reg_mask.RenderBackground || reg_mask.RenderSprites)
            {
                if (reg_v.coarse_x == 31)
                {
                    reg_v.coarse_x = 0;
                    reg_v.nametable = reg_v.nametable ^ 0x01;
                }
                else
                {
                    reg_v.coarse_x++;
                }
            }
        };

        auto IncrementScrollY = [&]()
        {
            if (reg_mask.RenderBackground || reg_mask.RenderSprites)
            {
                // If possible, just increment the fine y offset
                if (reg_v.fine_y < 7)
                {
                    reg_v.fine_y++;
                }
                else
                {
                    reg_v.fine_y = 0;
                    if (reg_v.coarse_y == 29)
                    {
                        reg_v.coarse_y = 0;
                        reg_v.nametable = reg_v.nametable ^ 0x02;
                    }
                    else if (reg_v.coarse_y == 31)
                    {
                        reg_v.coarse_y = 0;
                    }
                    else
                    {
                        reg_v.coarse_y++;
                    }
                }
            }
        };
        auto TransferAddressX = [&]()
        {
            if (reg_mask.RenderBackground || reg_mask.RenderSprites)
            {
                reg_v.nametable = reg_t.nametable; 
                reg_v.coarse_x    = reg_t.coarse_x;
            }
        };

        auto TransferAddressY = [&]()
        {
            if (reg_mask.RenderBackground || reg_mask.RenderSprites)
            {
                reg_v.fine_y      = reg_t.fine_y;
                reg_v.nametable = reg_t.nametable; 
                reg_v.coarse_y    = reg_t.coarse_y;
            }
        };

        #if 1
        // ==============================================================================
        // Prime the "in-effect" background tile shifters ready for outputting next
        // 8 pixels in scanline.
        auto LoadBackgroundShifters = [&]()
        {	
            // Each PPU update we calculate one pixel. These shifters shift 1 bit along
            // feeding the pixel compositor with the binary information it needs. Its
            // 16 bits wide, because the top 8 bits are the current 8 pixels being drawn
            // and the bottom 8 bits are the next 8 pixels to be drawn. Naturally this means
            // the required bit is always the MSB of the shifter. However, "fine x" scrolling
            // plays a part in this too, whcih is seen later, so in fact we can choose
            // any one of the top 8 bits.
            bg_shifter_pattern_lo = (bg_shifter_pattern_lo & 0xFF00) | bg_next_tile_lsb;
            bg_shifter_pattern_hi = (bg_shifter_pattern_hi & 0xFF00) | bg_next_tile_msb;

            // Attribute bits do not change per pixel, rather they change every 8 pixels
            // but are synchronised with the pattern shifters for convenience, so here
            // we take the bottom 2 bits of the attribute word which represent which 
            // palette is being used for the current 8 pixels and the next 8 pixels, and 
            // "inflate" them to 8 bit words.
            bg_shifter_attrib_lo  = (bg_shifter_attrib_lo & 0xFF00) | ((bg_next_tile_attrib & 0b01) ? 0xFF : 0x00);
            bg_shifter_attrib_hi  = (bg_shifter_attrib_hi & 0xFF00) | ((bg_next_tile_attrib & 0b10) ? 0xFF : 0x00);
        };


        // ==============================================================================
        // Every cycle the shifters storing pattern and attribute information shift
        // their contents by 1 bit. This is because every cycle, the output progresses
        // by 1 pixel. This means relatively, the state of the shifter is in sync
        // with the pixels being drawn for that 8 pixel section of the scanline.
        auto UpdateShifters = [&]()
        {
            if (reg_mask.RenderBackground)
            {
                // Shifting background tile pattern row
                bg_shifter_pattern_lo <<= 1;
                bg_shifter_pattern_hi <<= 1;

                // Shifting palette attributes by 1
                bg_shifter_attrib_lo <<= 1;
                bg_shifter_attrib_hi <<= 1;
            }

            if (reg_mask.RenderSprites && cycle >= 1 && cycle < 258)
            {
                for (int i = 0; i < sprite_count; i++)
                {
                    if (spriteScanline[i].x > 0)
                    {
                        spriteScanline[i].x--;
                    }
                    else
                    {
                        sprite_shifter_pattern_lo[i] <<= 1;
                        sprite_shifter_pattern_hi[i] <<= 1;
                    }
                }
            }
        };

        

        // All but 1 of the secanlines is visible to the user. The pre-render scanline
        // at -1, is used to configure the "shifters" for the first visible scanline, 0.
        if (scanline >= -1 && scanline < 240)
        {		
            // Background Rendering ======================================================

            if (scanline == 0 && cycle == 0 && odd_frame && (reg_mask.RenderBackground || reg_mask.RenderSprites))
            {
                // "Odd Frame" cycle skip
                cycle = 1;
            }

            if (scanline == -1 && cycle == 1)
            {
                // Effectively start of new frame, so clear vertical blank flag
                reg_status.VerticalBlank = 0;

                // Clear sprite overflow flag
                reg_status.SpriteOverflow = 0;
                
                // Clear the sprite zero hit flag
                reg_status.SpriteZeroHit = 0;

                // Clear Shifters
                for (int i = 0; i < 8; i++)
                {
                    sprite_shifter_pattern_lo[i] = 0;
                    sprite_shifter_pattern_hi[i] = 0;
                }
            }


            if ((cycle >= 2 && cycle < 258) || (cycle >= 321 && cycle < 338))
            {
                UpdateShifters();
                
                
                // In these cycles we are collecting and working with visible data
                // The "shifters" have been preloaded by the end of the previous
                // scanline with the data for the start of this scanline. Once we
                // leave the visible region, we go dormant until the shifters are
                // preloaded for the next scanline.

                // Fortunately, for background rendering, we go through a fairly
                // repeatable sequence of events, every 2 clock cycles.
                switch ((cycle - 1) % 8)
                {
                case 0:
                    // Load the current background tile pattern and attributes into the "shifter"
                    LoadBackgroundShifters();

                    // Fetch the next background tile ID
                    // "(reg_v.val & 0x0FFF)" : Mask to 12 bits that are relevant
                    // "| 0x2000"                 : Offset into nametable space on PPU address bus
                    bg_next_tile_id = busRead(0x2000 | (reg_v.val & 0x0FFF));

                    // Explanation:
                    // The bottom 12 bits of the loopy register provide an index into
                    // the 4 nametables, regardless of nametable mirroring configuration.
                    // nametable_y(1) nametable_x(1) coarse_y(5) coarse_x(5)
                    //
                    // Consider a single nametable is a 32x32 array, and we have four of them
                    //   0                1
                    // 0 +----------------+----------------+
                    //   |                |                |
                    //   |                |                |
                    //   |    (32x32)     |    (32x32)     |
                    //   |                |                |
                    //   |                |                |
                    // 1 +----------------+----------------+
                    //   |                |                |
                    //   |                |                |
                    //   |    (32x32)     |    (32x32)     |
                    //   |                |                |
                    //   |                |                |
                    //   +----------------+----------------+
                    //
                    // This means there are 4096 potential locations in this array, which 
                    // just so happens to be 2^12!
                    break;
                case 2:
                    // Fetch the next background tile attribute. OK, so this one is a bit
                    // more involved :P

                    // Recall that each nametable has two rows of cells that are not tile 
                    // information, instead they represent the attribute information that
                    // indicates which palettes are applied to which area on the screen.
                    // Importantly (and frustratingly) there is not a 1 to 1 correspondance
                    // between background tile and palette. Two rows of tile data holds
                    // 64 attributes. Therfore we can assume that the attributes affect
                    // 8x8 zones on the screen for that nametable. Given a working resolution
                    // of 256x240, we can further assume that each zone is 32x32 pixels
                    // in screen space, or 4x4 tiles. Four system palettes are allocated
                    // to background rendering, so a palette can be specified using just
                    // 2 bits. The attribute byte therefore can specify 4 distinct palettes.
                    // Therefore we can even further assume that a single palette is
                    // applied to a 2x2 tile combination of the 4x4 tile zone. The very fact
                    // that background tiles "share" a palette locally is the reason why
                    // in some games you see distortion in the colours at screen edges.

                    // As before when choosing the tile ID, we can use the bottom 12 bits of
                    // the loopy register, but we need to make the implementation "coarser"
                    // because instead of a specific tile, we want the attribute byte for a 
                    // group of 4x4 tiles, or in other words, we divide our 32x32 address
                    // by 4 to give us an equivalent 8x8 address, and we offset this address
                    // into the attribute section of the target nametable.

                    // Reconstruct the 12 bit loopy address into an offset into the
                    // attribute memory

                    // "(reg_v.coarse_x >> 2)"        : integer divide coarse x by 4, 
                    //                                      from 5 bits to 3 bits
                    // "((reg_v.coarse_y >> 2) << 3)" : integer divide coarse y by 4, 
                    //                                      from 5 bits to 3 bits,
                    //                                      shift to make room for coarse x

                    // Result so far: YX00 00yy yxxx

                    // All attribute memory begins at 0x03C0 within a nametable, so OR with
                    // result to select target nametable, and attribute byte offset. Finally
                    // OR with 0x2000 to offset into nametable address space on PPU bus.				
                    bg_next_tile_attrib = busRead(0x23C0 | (reg_v.nametable << 11) 
                                                        | ((reg_v.coarse_y >> 2) << 3) 
                                                        | (reg_v.coarse_x >> 2));
                    
                    // Right we've read the correct attribute byte for a specified address,
                    // but the byte itself is broken down further into the 2x2 tile groups
                    // in the 4x4 attribute zone.

                    // The attribute byte is assembled thus: BR(76) BL(54) TR(32) TL(10)
                    //
                    // +----+----+			    +----+----+
                    // | TL | TR |			    | ID | ID |
                    // +----+----+ where TL =   +----+----+
                    // | BL | BR |			    | ID | ID |
                    // +----+----+			    +----+----+
                    //
                    // Since we know we can access a tile directly from the 12 bit address, we
                    // can analyse the bottom bits of the coarse coordinates to provide us with
                    // the correct offset into the 8-bit word, to yield the 2 bits we are
                    // actually interested in which specifies the palette for the 2x2 group of
                    // tiles. We know if "coarse y % 4" < 2 we are in the top half else bottom half.
                    // Likewise if "coarse x % 4" < 2 we are in the left half else right half.
                    // Ultimately we want the bottom two bits of our attribute word to be the
                    // palette selected. So shift as required...				
                    if (reg_v.coarse_y & 0x02) bg_next_tile_attrib >>= 4;
                    if (reg_v.coarse_x & 0x02) bg_next_tile_attrib >>= 2;
                    bg_next_tile_attrib &= 0x03;
                    break;

                    // Compared to the last two, the next two are the easy ones... :P

                case 4: 
                    // Fetch the next background tile LSB bit plane from the pattern memory
                    // The Tile ID has been read from the nametable. We will use this id to 
                    // index into the pattern memory to find the correct sprite (assuming
                    // the sprites lie on 8x8 pixel boundaries in that memory, which they do
                    // even though 8x16 sprites exist, as background tiles are always 8x8).
                    //
                    // Since the sprites are effectively 1 bit deep, but 8 pixels wide, we 
                    // can represent a whole sprite row as a single byte, so offsetting
                    // into the pattern memory is easy. In total there is 8KB so we need a 
                    // 13 bit address.

                    // "(reg_ctrl.BackgroundPattrenTableIndex << 12)"  : the pattern memory selector 
                    //                                         from control register, either 0K
                    //                                         or 4K offset
                    // "((uint16_t)bg_next_tile_id << 4)"    : the tile id multiplied by 16, as
                    //                                         2 lots of 8 rows of 8 bit pixels
                    // "(reg_v.fine_y)"                  : Offset into which row based on
                    //                                         vertical scroll offset
                    // "+ 0"                                 : Mental clarity for plane offset
                    // Note: No PPU address bus offset required as it starts at 0x0000
                    bg_next_tile_lsb = busRead((reg_ctrl.BackgroundPattrenTableIndex << 12) 
                                            + ((uint16_t)bg_next_tile_id << 4) 
                                            + (reg_v.fine_y) + 0);
                    break;
                case 6:
                    // Fetch the next background tile MSB bit plane from the pattern memory
                    // This is the same as above, but has a +8 offset to select the next bit plane
                    bg_next_tile_msb = busRead((reg_ctrl.BackgroundPattrenTableIndex << 12)
                                            + ((uint16_t)bg_next_tile_id << 4)
                                            + (reg_v.fine_y) + 8);
                    break;
                case 7:
                    // Increment the background tile "pointer" to the next tile horizontally
                    // in the nametable memory. Note this may cross nametable boundaries which
                    // is a little complex, but essential to implement scrolling
                    IncrementScrollX();
                    break;
                }
            }

            // End of a visible scanline, so increment downwards...
            if (cycle == 256)
            {
                IncrementScrollY();
            }

            //...and reset the x position
            if (cycle == 257)
            {
                LoadBackgroundShifters();
                TransferAddressX();
            }

            // Superfluous reads of tile id at end of scanline
            if (cycle == 338 || cycle == 340)
            {
                bg_next_tile_id = busRead(0x2000 | (reg_v.val & 0x0FFF));
            }

            if (scanline == -1 && cycle >= 280 && cycle < 305)
            {
                // End of vertical blank period so reset the Y address ready for rendering
                TransferAddressY();
            }

            // Foreground Rendering ========================================================
            // I'm gonna cheat a bit here, which may reduce compatibility, but greatly
            // simplifies delivering an intuitive understanding of what exactly is going
            // on. The PPU loads sprite information successively during the region that
            // background tiles are not being drawn. Instead, I'm going to perform
            // all sprite evaluation in one hit. THE NES DOES NOT DO IT LIKE THIS! This makes
            // it easier to see the process of sprite evaluation.
            if (cycle == 257 && scanline >= 0)
            {
                // We've reached the end of a visible scanline. It is now time to determine
                // which sprites are visible on the next scanline, and preload this info
                // into buffers that we can work with while the scanline scans the row.

                // Firstly, clear out the sprite memory. This memory is used to store the
                // sprites to be rendered. It is not the OAM.
                memset(spriteScanline, 0xFF, 8 * sizeof(sObjectAttributeEntry));

                // The NES supports a maximum number of sprites per scanline. Nominally
                // this is 8 or fewer sprites. This is why in some games you see sprites
                // flicker or disappear when the scene gets busy.
                sprite_count = 0;

                // Secondly, clear out any residual information in sprite pattern shifters
                for (uint8_t i = 0; i < 8; i++)
                {
                    sprite_shifter_pattern_lo[i] = 0;
                    sprite_shifter_pattern_hi[i] = 0;
                }

                // Thirdly, Evaluate which sprites are visible in the next scanline. We need
                // to iterate through the OAM until we have found 8 sprites that have Y-positions
                // and heights that are within vertical range of the next scanline. Once we have
                // found 8 or exhausted the OAM we stop. Now, notice I count to 9 sprites. This
                // is so I can set the sprite overflow flag in the event of there being > 8 sprites.
                uint8_t nOAMEntry = 0;

                // New set of sprites. Sprite zero may not exist in the new set, so clear this
                // flag.
                bSpriteZeroHitPossible = false;

                while (nOAMEntry < 64 && sprite_count < 9)
                {
                    // Note the conversion to signed numbers here
                    int16_t diff = ((int16_t)scanline - (int16_t)OAM[nOAMEntry].y);

                    // If the difference is positive then the scanline is at least at the
                    // same height as the sprite, so check if it resides in the sprite vertically
                    // depending on the current "sprite height mode"
                    // FLAGGED
                    
                    if (diff >= 0 && diff < (reg_ctrl.SpriteSize ? 16 : 8) && sprite_count < 8)
                    {
                        // Sprite is visible, so copy the attribute entry over to our
                        // scanline sprite cache. Ive added < 8 here to guard the array
                        // being written to.
                        if (sprite_count < 8)
                        {
                            // Is this sprite sprite zero?
                            if (nOAMEntry == 0)
                            {
                                // It is, so its possible it may trigger a 
                                // sprite zero hit when drawn
                                bSpriteZeroHitPossible = true;
                            }

                            memcpy(&spriteScanline[sprite_count], &OAM[nOAMEntry], sizeof(sObjectAttributeEntry));						
                        }			
                        sprite_count++;
                    }
                    nOAMEntry++;
                } // End of sprite evaluation for next scanline

                // Set sprite overflow flag
                reg_status.SpriteOverflow = (sprite_count >= 8);

                // Now we have an array of the 8 visible sprites for the next scanline. By 
                // the nature of this search, they are also ranked in priority, because
                // those lower down in the OAM have the higher priority.

                // We also guarantee that "Sprite Zero" will exist in spriteScanline[0] if
                // it is evaluated to be visible. 
            }

            if (cycle == 340)
            {
                // Now we're at the very end of the scanline, I'm going to prepare the 
                // sprite shifters with the 8 or less selected sprites.

                for (uint8_t i = 0; i < sprite_count; i++)
                {
                    // We need to extract the 8-bit row patterns of the sprite with the
                    // correct vertical offset. The "Sprite Mode" also affects this as
                    // the sprites may be 8 or 16 rows high. Additionally, the sprite
                    // can be flipped both vertically and horizontally. So there's a lot
                    // going on here :P

                    uint8_t sprite_pattern_bits_lo, sprite_pattern_bits_hi;
                    uint16_t sprite_pattern_addr_lo, sprite_pattern_addr_hi;

                    // Determine the memory addresses that contain the byte of pattern data. We
                    // only need the lo pattern address, because the hi pattern address is always
                    // offset by 8 from the lo address.
                    if (!reg_ctrl.SpriteSize)
                    {
                        // 8x8 Sprite Mode - The control register determines the pattern table
                        if (!(spriteScanline[i].attribute & 0x80))
                        {
                            // Sprite is NOT flipped vertically, i.e. normal    
                            sprite_pattern_addr_lo = 
                            (reg_ctrl.SpritePattrenTableIndex << 12  )  // Which Pattern Table? 0KB or 4KB offset
                            | (spriteScanline[i].id   << 4   )  // Which Cell? Tile ID * 16 (16 bytes per tile)
                            | (scanline - spriteScanline[i].y); // Which Row in cell? (0->7)
                                                    
                        }
                        else
                        {
                            // Sprite is flipped vertically, i.e. upside down
                            sprite_pattern_addr_lo = 
                            (reg_ctrl.SpritePattrenTableIndex << 12  )  // Which Pattern Table? 0KB or 4KB offset
                            | (spriteScanline[i].id   << 4   )  // Which Cell? Tile ID * 16 (16 bytes per tile)
                            | (7 - (scanline - spriteScanline[i].y)); // Which Row in cell? (7->0)
                        }

                    }
                    else
                    {
                        // 8x16 Sprite Mode - The sprite attribute determines the pattern table
                        if (!(spriteScanline[i].attribute & 0x80))
                        {
                            // Sprite is NOT flipped vertically, i.e. normal
                            if (scanline - spriteScanline[i].y < 8)
                            {
                                // Reading Top half Tile
                                sprite_pattern_addr_lo = 
                                ((spriteScanline[i].id & 0x01)      << 12)  // Which Pattern Table? 0KB or 4KB offset
                                | ((spriteScanline[i].id & 0xFE)      << 4 )  // Which Cell? Tile ID * 16 (16 bytes per tile)
                                | ((scanline - spriteScanline[i].y) & 0x07 ); // Which Row in cell? (0->7)
                            }
                            else
                            {
                                // Reading Bottom Half Tile
                                sprite_pattern_addr_lo = 
                                ( (spriteScanline[i].id & 0x01)      << 12)  // Which Pattern Table? 0KB or 4KB offset
                                | (((spriteScanline[i].id & 0xFE) + 1) << 4 )  // Which Cell? Tile ID * 16 (16 bytes per tile)
                                | ((scanline - spriteScanline[i].y) & 0x07  ); // Which Row in cell? (0->7)
                            }
                        }
                        else
                        {
                            // Sprite is flipped vertically, i.e. upside down
                            if (scanline - spriteScanline[i].y < 8)
                            {
                                // Reading Top half Tile
                                sprite_pattern_addr_lo = 
                                ( (spriteScanline[i].id & 0x01)      << 12)    // Which Pattern Table? 0KB or 4KB offset
                                | (((spriteScanline[i].id & 0xFE) + 1) << 4 )    // Which Cell? Tile ID * 16 (16 bytes per tile)
                                | (7 - (scanline - spriteScanline[i].y) & 0x07); // Which Row in cell? (0->7)
                            }
                            else
                            {
                                // Reading Bottom Half Tile
                                sprite_pattern_addr_lo = 
                                ((spriteScanline[i].id & 0x01)       << 12)    // Which Pattern Table? 0KB or 4KB offset
                                | ((spriteScanline[i].id & 0xFE)       << 4 )    // Which Cell? Tile ID * 16 (16 bytes per tile)
                                | (7 - (scanline - spriteScanline[i].y) & 0x07); // Which Row in cell? (0->7)
                            }
                        }
                    }

                    // Phew... XD I'm absolutely certain you can use some fantastic bit 
                    // manipulation to reduce all of that to a few one liners, but in this
                    // form it's easy to see the processes required for the different
                    // sizes and vertical orientations

                    // Hi bit plane equivalent is always offset by 8 bytes from lo bit plane
                    sprite_pattern_addr_hi = sprite_pattern_addr_lo + 8;

                    // Now we have the address of the sprite patterns, we can read them
                    sprite_pattern_bits_lo = busRead(sprite_pattern_addr_lo);
                    sprite_pattern_bits_hi = busRead(sprite_pattern_addr_hi);

                    // If the sprite is flipped horizontally, we need to flip the 
                    // pattern bytes. 
                    if (spriteScanline[i].attribute & 0x40)
                    {
                        // This little lambda function "flips" a byte
                        // so 0b11100000 becomes 0b00000111. It's very
                        // clever, and stolen completely from here:
                        // https://stackoverflow.com/a/2602885
                        auto flipbyte = [](uint8_t b)
                        {
                            b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
                            b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
                            b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
                            return b;
                        };

                        // Flip Patterns Horizontally
                        sprite_pattern_bits_lo = flipbyte(sprite_pattern_bits_lo);
                        sprite_pattern_bits_hi = flipbyte(sprite_pattern_bits_hi);
                    }

                    // Finally! We can load the pattern into our sprite shift registers
                    // ready for rendering on the next scanline
                    sprite_shifter_pattern_lo[i] = sprite_pattern_bits_lo;
                    sprite_shifter_pattern_hi[i] = sprite_pattern_bits_hi;
                }
            }
        }

        if (scanline == 240)
        {
            // Post Render Scanline - Do Nothing!
        }

        if (scanline >= 241 && scanline < 261)
        {
            if (scanline == 241 && cycle == 1)
            {
                // Effectively end of frame, so set vertical blank flag
                reg_status.VerticalBlank = 1;

                // If the control register tells us to emit a NMI when
                // entering vertical blanking period, do it! The CPU
                // will be informed that rendering is complete so it can
                // perform operations with the PPU knowing it wont
                // produce visible artefacts
                if (reg_ctrl.EnableNMI) 
                    nmi = true;
            }
        }



        // Composition - We now have background & foreground pixel information for this cycle

        // Background =============================================================
        uint8_t bg_pixel = 0x00;   // The 2-bit pixel to be rendered
        uint8_t bg_palette = 0x00; // The 3-bit index of the palette the pixel indexes

        // We only render backgrounds if the PPU is enabled to do so. Note if 
        // background rendering is disabled, the pixel and palette combine
        // to form 0x00. This will fall through the colour tables to yield
        // the current background colour in effect
        if (reg_mask.RenderBackground)
        {
            if (reg_mask.RenderBackgroundLeft || (cycle >= 9))
            {
                // Handle Pixel Selection by selecting the relevant bit
                // depending upon fine x scolling. This has the effect of
                // offsetting ALL background rendering by a set number
                // of pixels, permitting smooth scrolling
                uint16_t bit_mux = 0x8000 >> fine_x;

                // Select Plane pixels by extracting from the shifter 
                // at the required location. 
                uint8_t p0_pixel = (bg_shifter_pattern_lo & bit_mux) > 0;
                uint8_t p1_pixel = (bg_shifter_pattern_hi & bit_mux) > 0;

                // Combine to form pixel index
                bg_pixel = (p1_pixel << 1) | p0_pixel;

                // Get palette
                uint8_t bg_pal0 = (bg_shifter_attrib_lo & bit_mux) > 0;
                uint8_t bg_pal1 = (bg_shifter_attrib_hi & bit_mux) > 0;
                bg_palette = (bg_pal1 << 1) | bg_pal0;
            }
        }

        // Foreground =============================================================
        uint8_t fg_pixel = 0x00;   // The 2-bit pixel to be rendered
        uint8_t fg_palette = 0x00; // The 3-bit index of the palette the pixel indexes
        uint8_t fg_priority = 0x00;// A bit of the sprite attribute indicates if its
                                // more important than the background
        if (reg_mask.RenderSprites)
        {
            // Iterate through all sprites for this scanline. This is to maintain
            // sprite priority. As soon as we find a non transparent pixel of
            // a sprite we can abort
            if (reg_mask.RenderBackgroundLeft || (cycle >= 9))
            {

                bSpriteZeroBeingRendered = false;

                for (uint8_t i = 0; i < sprite_count; i++)
                {
                    // Scanline cycle has "collided" with sprite, shifters taking over
                    if (spriteScanline[i].x == 0)
                    {
                        // Note Fine X scrolling does not apply to sprites, the game
                        // should maintain their relationship with the background. So
                        // we'll just use the MSB of the shifter

                        // Determine the pixel value...
                        uint8_t fg_pixel_lo = (sprite_shifter_pattern_lo[i] & 0x80) > 0;
                        uint8_t fg_pixel_hi = (sprite_shifter_pattern_hi[i] & 0x80) > 0;
                        fg_pixel = (fg_pixel_hi << 1) | fg_pixel_lo;

                        // Extract the palette from the bottom two bits. Recall
                        // that foreground palettes are the latter 4 in the 
                        // palette memory.
                        fg_palette = (spriteScanline[i].attribute & 0x03) + 0x04;
                        fg_priority = (spriteScanline[i].attribute & 0x20) == 0;

                        // If pixel is not transparent, we render it, and dont
                        // bother checking the rest because the earlier sprites
                        // in the list are higher priority
                        if (fg_pixel != 0)
                        {
                            if (i == 0) // Is this sprite zero?
                            {
                                bSpriteZeroBeingRendered = true;
                            }

                            break;
                        }
                    }
                }
            }		
        }

        // Now we have a background pixel and a foreground pixel. They need
        // to be combined. It is possible for sprites to go behind background
        // tiles that are not "transparent", yet another neat trick of the PPU
        // that adds complexity for us poor emulator developers...

        uint8_t pixel = 0x00;   // The FINAL Pixel...
        uint8_t palette = 0x00; // The FINAL Palette...

        if (bg_pixel == 0 && fg_pixel == 0)
        {
            // The background pixel is transparent
            // The foreground pixel is transparent
            // No winner, draw "background" colour
            pixel = 0x00;
            palette = 0x00;
        }
        else if (bg_pixel == 0 && fg_pixel > 0)
        {
            // The background pixel is transparent
            // The foreground pixel is visible
            // Foreground wins!
            pixel = fg_pixel;
            palette = fg_palette;
        }
        else if (bg_pixel > 0 && fg_pixel == 0)
        {
            // The background pixel is visible
            // The foreground pixel is transparent
            // Background wins!
            pixel = bg_pixel;
            palette = bg_palette;
        }
        else if (bg_pixel > 0 && fg_pixel > 0)
        {
            // The background pixel is visible
            // The foreground pixel is visible
            // Hmmm...
            if (fg_priority)
            {
                // Foreground cheats its way to victory!
                pixel = fg_pixel;
                palette = fg_palette;
            }
            else
            {
                // Background is considered more important!
                pixel = bg_pixel;
                palette = bg_palette;
            }

            // Sprite Zero Hit detection
            if (bSpriteZeroHitPossible && bSpriteZeroBeingRendered)
            {
                // Sprite zero is a collision between foreground and background
                // so they must both be enabled
                if (reg_mask.RenderBackground & reg_mask.RenderSprites)
                {
                    // The left edge of the screen has specific switches to control
                    // its appearance. This is used to smooth inconsistencies when
                    // scrolling (since sprites x coord must be >= 0)
                    if (!(reg_mask.RenderBackgroundLeft | reg_mask.RenderBackgroundLeft))
                    {
                        if (cycle >= 9 && cycle < 258)
                        {
                            reg_status.SpriteZeroHit = 1;
                        }
                    }
                    else
                    {
                        if (cycle >= 1 && cycle < 258)
                        {
                            reg_status.SpriteZeroHit = 1;
                        }
                    }
                }
            }
        }

        // Now we have a final pixel colour, and a palette for this cycle
        // of the current scanline. Let's at long last, draw that ^&%*er :P
        // sprScreen->SetPixel(cycle - 1, scanline, GetColourFromPaletteRam(palette, pixel));
        uint8_t ColorIndex =  busRead(0x3F00 + (palette << 2) + pixel) & 0x3F;
        map.DrawPoint(cycle - 1, scanline, getColor(ColorIndex));

        // Advance renderer - it never stops, it's relentless
        cycle++;
        // if(reg_mask.RenderBackground || reg_mask.RenderSprites)
        //     if (cycle == 260 && scanline < 240)
        //     {
        //         cart->GetMapper()->scanline();
        //     }

        if (cycle >= 341)
        {
            cycle = 0;
            scanline++;
            if (scanline >= 261)
            {
                map.Refresh();      // 刷新一帧
                scanline = -1;
                frame_complete = true;
                odd_frame = !odd_frame;
            }
        }

        #else

        auto SpriteEvaluation = [&]()
        {
            /* 跟背景提取动作相同，(实际上PPU硬件电路共用的一套)
                1. 垃圾名称表字节   (2个PPUClockCnt)
                2. 垃圾名称表字节   (2个PPUClockCnt)
                3. 图案表图块低位   (2个PPUClockCnt)
                4. 图案表图块高位（图案表图块低位 +8 个字节）   (2个PPUClockCnt)
                这里模拟只读取图案表数据就行了
                数据来源：
                CPU(cpuRAM或者cart) --> DMA --> PPU OAM
                虽然PPU提供了OAMADDR和OAMDATA两个寄存器，但是基本上不使用，因为单字节太慢了(硬件)，所以选择DMA方式传输
                PPU到这个时钟周期就认为OAM的数据被更新好了，这里直接使用即可。
                X
                |
                v
            Y-->x  x  x  x  x  x  x 
                x  x  x  x  x  x  x 
                x  x  x  x  x  x  x 
            ----x  x  x  x  x  x  x---- ScanLineCnt
                x  x  x  x  x  x  x 
                x  x  x  x  x  x  x 
                x  x  x  x  x  x  x 
                x  x  x  x  x  x  x
            */
           
            // memset(spriteScanline, 0xFF, 8 * sizeof(sObjectAttributeEntry));
			// spriteScanlineCnt = 0;

            uint8_t oamCnt = 0;
            while(oamCnt < 64 && spriteScanlineCnt < 9)
            {
                int32_t diff_y = ScanLineCnt - OAM[oamCnt].y;
                //LOG_INFO("ScanLineCnt:%d oamCnt:%d OAM[oamCnt].y:%d diff_y:%d", ScanLineCnt, oamCnt, OAM[oamCnt].y, diff_y);
                if(diff_y >= 0 && diff_y < (reg_ctrl.SpriteSize ? 16 : 8))  // 命中
                {
                    if(oamCnt == 0 && spriteScanlineCnt < 8)
                    {
                        // 标记0号精灵被命中 TODO:跟后面渲染时背景和精灵的叠加有关
                    }
                    memcpy(&spriteScanline[spriteScanlineCnt], &OAM[oamCnt], sizeof(sObjectAttributeEntry)); 
                    spriteScanlineCnt ++;
                    //LOG_INFO("ScanLineCnt:%d spriteScanlineCnt:%d", ScanLineCnt, spriteScanlineCnt);
                }
                oamCnt ++;
            }
            
        };

        // 根据当前x值，从spriteScanline(行命中的精灵列表)中去选择一个精灵
        auto SpriteScanMatch = [&](int x)
        {
            /*
                X          PPUClockCnt           
                |              |
                v              v
            Y-->x  x  x  x  x  x  x  x 
                x  x  x  x  x  x  x  x
                x  x  x  x  x  x  x  x
            ----x  x  x  x  x  x  x  x----ScanLineCnt
                x  x  x  x  x  x  x  x
                x  x  x  x  x  x  x  x
                x  x  x  x  x  x  x  x
                x  x  x  x  x  x  x  x
            */
            int i = 0;
            uint8_t SpPiexl = 0;
            for(i=0; i<spriteScanlineCnt; i++)
            {
                int diff = x - spriteScanline[i].x;
                if ( diff >= 0  && diff <= 7)
                    return i;
            }

            return -1;
        };

        auto getSpriteTile = [&](int index)
        {
            uint16_t SpriteTileLsbAddr;

            // 分8x8和8x16两种情况
            if(0 == reg_ctrl.SpriteSize)    // 8x8
            {
                if(spriteScanline[index].attribute & 0x40)  // 水平翻转
                {
                    SpriteTileLsbAddr = 
                                    (reg_ctrl.SpritePattrenTableIndex ? 0x1000:0)
                                    | (spriteScanline[index].id << 4)    // 每个tile 16Bytes
                                    | (ScanLineCnt - spriteScanline[index].y);
                }
                else
                {
                    SpriteTileLsbAddr = 
                                    (reg_ctrl.SpritePattrenTableIndex ? 0x1000:0)
                                    | (spriteScanline[index].id << 4)    // 每个tile 16Bytes
                                    | (7 -(ScanLineCnt - spriteScanline[index].y));
                }
            }
            else    // 8X16
            {
            
            }

            SpritesTileIndexLsb = busRead(SpriteTileLsbAddr); 
            SpritesTileIndexMsb = busRead(SpriteTileLsbAddr + 8); 
        };

        auto getSpriteColorIndex = [&](int index)
        {
            return spriteScanline[index].attribute & 0x03;
        };

        auto getSpritePriority= [&](int index)
        {
            return spriteScanline[index].attribute & 0x20;
        };

        
        // 262scanline、341clock 这里完全按照2c02的硬件行为做处理
        if(ScanLineCnt >= -1 && ScanLineCnt <= 239)   // 正常图片显示周期， PPU不停在读取内存数据，所以CPU不要访问PPU内存。
        {
            if(PPUClockCnt == 0)
            {
                
            }

            if(ScanLineCnt == -1 && PPUClockCnt == 1)
            {
                reg_status.VerticalBlank = 0;
			    reg_status.SpriteOverflow = 0;
			    reg_status.SpriteZeroHit = 0;
            }
        
            if ((PPUClockCnt >= 1 && PPUClockCnt <=256) || (PPUClockCnt >= 321 && PPUClockCnt <=336))
            {
                if(PPUClockCnt <= 256 && ScanLineCnt >= 0)  // 321-336也会提取，但是不渲染
                {
                    uint8_t pixel_x = PPUClockCnt - 1;
                    uint8_t pixel_y = ScanLineCnt; 

                    // 精灵
                    // int SpriteIndex = SpriteScanMatch(pixel_x);
                    // if(SpriteIndex >= 0)
                    // {
                    //     getSpriteTile(SpriteIndex);
                    // }
                    // uint8_t SpritePixel = (((SpritesTileIndexMsb & muxBit) ? 1:0) << 1) | ((SpritesTileIndexLsb & muxBit) ? 1:0);
                    // uint8_t SpriteColorIndex = busRead(0x3F10 + (getSpriteColorIndex(SpriteIndex) << 2) + SpritePixel) & 0x3F;

                    // 背景
                    uint8_t pixel = (((BgTileIndexMsbLast & muxBit) ? 1:0) << 1) | ((BgTileIndexLsbLast & muxBit) ? 1:0);
                    uint8_t ColorIndex =  busRead(0x3F00 + (BgPaletteIndexLast << 2) + pixel) & 0x3F;
                    muxBit >>= 1;

                    // 判断精灵和背景
                    // if(SpriteIndex >= 0)
                    // {
                    //     if(getSpritePriority(SpriteIndex) == 0) // 精灵在背景前面
                    //     {
                    //         ColorIndex = SpriteColorIndex;
                    //     }

                    // }
                    
                    map.DrawPoint(pixel_x, pixel_y, getColor(ColorIndex));   
                }

                uint8_t offset = (PPUClockCnt-1) % 8;
                switch (offset)
                {
                case 0: // read NameTable, 用于决定使用那一块Pattern
                    /*
                    $0000-$0FFF	$1000	Pattern table 0	Cartridge
                    $1000-$1FFF	$1000	Pattern table 1	Cartridge
                    */
                    BgTileIndex = busRead(0x2000 | reg_v.val & 0x0FFF);  // reg_addr是CPU设置的
                    break;
                case 2: // read AttributeTable
                    /*
                        $2000-$23FF	$0400	1024B   Nametable 0	Cartridge
                            $2000 - $23BF   960B    NameTable 0
                            $23C0 - $23FF   64B     AttributeTable 0
                        $2400-$27FF	$0400	1024B   Nametable 1	Cartridge
                            $2400 - $27BF   960B    NameTable 1
                            $27C0 - $27FF   64B     AttributeTable 1
                        $2800-$2BFF	$0400	1024B   Nametable 2	Cartridge
                            $2800 - $2BBF   960B    NameTable 2
                            $28C0 - $2BFF   64B     AttributeTable 2
                        $2C00-$2FFF	$0400	1024B   Nametable 3	Cartridge
                            $2C00 - $2FBF   960B    NameTable 3
                            $2FC0 - $2FFF   64B     AttributeTable 3

                        AttributeTable用64B去表示图块的调色盘
                        一个字节可以表示4x4个tile图块的调色盘，每个tile由8x8像素组成，故AttributeTable的一个字节表示16x16像素区域的调色盘。
                        Byte: 76543210
                              ||||||||
                              ||||||++---- 左上（TL）
                              ||||++------ 右上（TR）
                              ||++-------- 左下（BL）
                              ++---------- 右下（BR）
                        +----+----+----+----+
                        | TL | TL | TR | TR |
                        +----+----+----+----+
                        | TL | TL | TR | TR |       +-----+
                        +----+----+----+----+   ==> | ATB |
                        | BL | BL | BR | BR |       +-----+
                        +----+----+----+----+
                        | BL | BL | BR | BR |
                        +----+----+----+----+
                        例：AttributeTable Byte:0b11100100
                                                  ||||||||
                                                  ||||||++---- 左上（TL）: 00 调色盘0
                                                  ||||++------ 右上（TR）: 01 调色盘1
                                                  ||++-------- 左下（BL）：10 调色盘2
                                                  ++---------- 右下（BR）：11 调色盘3
                        一帧图片组成 8x8个ATB, 每个ATB用一个Byte表示，共计64Bytes
                        +-----+-----+-----+-----+-----+-----+-----+-----+
                        | ATB | ATB | ATB | ATB | ATB | ATB | ATB | ATB |
                        +-----+-----+-----+-----+-----+-----+-----+-----+
                        | ATB | ATB | ATB | ATB | ATB | ATB | ATB | ATB |
                        +-----+-----+-----+-----+-----+-----+-----+-----+
                        | ATB | ATB | ATB | ATB | ATB | ATB | ATB | ATB |
                        +-----+-----+-----+-----+-----+-----+-----+-----+
                        | ATB | ATB | ATB | ATB | ATB | ATB | ATB | ATB |
                        +-----+-----+-----+-----+-----+-----+-----+-----+
                        | ATB | ATB | ATB | ATB | ATB | ATB | ATB | ATB |
                        +-----+-----+-----+-----+-----+-----+-----+-----+
                        | ATB | ATB | ATB | ATB | ATB | ATB | ATB | ATB |
                        +-----+-----+-----+-----+-----+-----+-----+-----+
                        | ATB | ATB | ATB | ATB | ATB | ATB | ATB | ATB |
                        +-----+-----+-----+-----+-----+-----+-----+-----+
                        | ATB | ATB | ATB | ATB | ATB | ATB | ATB | ATB |
                        +-----+-----+-----+-----+-----+-----+-----+-----+
                    */
                    {
                        // 提取当前像素所在的属性表中的位置
                        // uint8_t BgAttributeTableIndex = busRead(0x23C0 
                        //                                     | reg_v.nametable * 0x400 
                        //                                     | reg_v.coarse_y * 8
                        //                                     | reg_v.coarse_x);
                        // 提取当前像素的调色盘
                        // uint8_t tx = (ScanLineCnt % 16);
                        // uint8_t ty = ((PPUClockCnt-1) % 16);
                        // uint8_t offset_t = 0;
                        // if(tx < 8) {
                        //     if(ty < 8)  offset_t = 0;   // TL
                        //     else        offset_t = 4;   // BL
                        // } else {
                        //     if(ty < 8)  offset_t = 2;   // TR
                        //     else        offset_t = 6;   // BR
                        // }
                        // PaletteIndex =  (AttributeTableByte >> offset_t) & 0x03;
                        BgPaletteIndex = busRead(0x23C0 | ( ((reg_v.nametable&0x01) ? 1:0) << 11) 
                                                        | ( ((reg_v.nametable&0x02) ? 1:0)  << 10) 
                                                        | ((reg_v.coarse_y >> 2) << 3) 
                                                        | (reg_v.coarse_x >> 2));
                        if (reg_v.coarse_y & 0x02) BgPaletteIndex >>= 4;
                        if (reg_v.coarse_x & 0x02) BgPaletteIndex >>= 2;
                        BgPaletteIndex &= 0x03;
                    }
                    break;
                    /*
                    -------------------------------------------------------------------------------------------
                    单个Tile的组成，Tile是一个8x8的像素点阵，每个像素由2bit组成，也就是4种灰度， 00， 01， 10， 11。
                    Byte00    0 0 0 0 0 0 0 0         Byte08  0 0 0 0 0 0 0 0      00 00 00 00 00 00 00 00
                    Byte01    0 0 0 0 0 0 0 0         Byte09  0 0 0 0 0 0 0 0      00 00 00 00 00 00 00 00
                    Byte02    0 0 0 0 0 0 0 0         Byte10  0 0 0 0 0 0 0 0      00 00 00 00 00 00 00 00
                    Byte03    0 0 1 0 0 0 0 0    +    Byte11  0 0 0 0 0 0 0 0  =>  00 00 10 00 00 00 00 00
                    Byte04    0 0 0 0 0 0 0 0         Byte12  0 0 0 0 0 0 0 0      00 00 00 00 00 00 00 00
                    Byte05    0 0 0 0 0 0 0 0         Byte13  0 0 0 0 0 0 0 0      00 00 00 00 00 00 00 00
                    Byte06    0 0 0 0 0 0 0 0         Byte14  0 0 0 0 0 0 1 0      00 00 00 00 00 00 01 00
                    Byte07    0 0 0 0 0 0 1 0         Byte15  0 0 0 0 0 0 1 0      00 00 00 00 00 00 11 00
                                LSB                     MSB                           RESULT
                    -------------------------------------------------------------------------------------------
                    PattrenTable中的数据排列
                    Byte0       Byte1      Byte2      ...   Byte7       Byte8        Byte9 ...     Byte14       Byte15
                    Tile0Lsb0   Tile0Lsb1  Tile0Lsb2  ...   Tile0Lsb7   Tile0Msb0    Tile0Msb1 ... Tile0Msb6    Tile0Msb7

                    Byte16      Byte17     Byte18     ...   Byte23       Byte24      Byte25 ...     Byte30       Byte31
                    Tile1Lsb0   Tile1Lsb1  Tile1Lsb2  ...   Tile1Lsb7   Tile1Msb0    Tile1Msb1 ... Tile1Msb6    Tile1Msb7
                    read PatternTable low
                    reg_ctrl寄存器中的值是有CPU设置的
                    需要得到本次像素，在VRAM中的具体的地址
                    1. reg_ctrl.BackgroundPattrenTableIndex ? 0x1000 : 0x0000 :先得到是哪一块PattrenTable
                    2. NameTableIndex * 8 :得到本次像素所在的NameTableIndex, *16是因为一个Tile用16Byte保存，NameTableIndex是索引
                    3. reg_v.fine_y :是本次像素在Tile中所在的行，Tile每行8bit构成
                    */
                case 4:
                    BgTileIndexLsb = busRead((reg_ctrl.BackgroundPattrenTableIndex ? 0x1000 : 0x0000 )  // reg_ctrl 由于CPU设置
                                            | BgTileIndex * 16    
                                            | reg_v.fine_y);     // reg_addr由于CPU设置
                    break;
                case 6:
                    BgTileIndexMsb = busRead((reg_ctrl.BackgroundPattrenTableIndex ? 0x1000 : 0x0000)  // reg_ctrl 由于CPU设置
                                            | BgTileIndex * 16    
                                            | reg_v.fine_y + 8); // reg_addr由于CPU设置
                    break;   
                case 7:
                    if (PPUClockCnt == 256)
                    {
                        IncrementScrollY();
                    }
                    // inc hori 水平增加,由PPU自己维护，这个水平坐标是属性相关的坐标，跟调色盘相关
                    IncrementScrollX(); 

                    BgTileIndexLast = BgTileIndex;
                    BgTileIndexLsbLast = BgTileIndexLsb;
                    BgTileIndexMsbLast = BgTileIndexMsb;
                    BgPaletteIndexLast = BgPaletteIndex;
                    muxBit = 0x80;
                    break;
                default:
                    break;
                };

                
            }

            if(PPUClockCnt == 257)  // 从t寄存器更新到v寄存器，更新水平坐标
            {
                TransferAddressX();
            }

            if(ScanLineCnt == -1 && (PPUClockCnt >= 280 && PPUClockCnt <= 304))
            {
                // End of vertical blank period so reset the Y address ready for rendering
                TransferAddressY();
            }

            if(PPUClockCnt == 320)    // 读取下一条扫描线的精灵数据, PPU精灵评估阶段
            {
                //SpriteEvaluation();
            }

            if (PPUClockCnt == 337 || PPUClockCnt == 339)   // 一行的最后四个PPUClockCnt，共提取两个NT，作用不详
            {
                BgTileIndex = busRead(0x2000 | (reg_v.val & 0x0FFF));
            }
            
        }
        else if(ScanLineCnt == 240)
        {
        
        }
        else if(ScanLineCnt >= 241 && ScanLineCnt <= 260) // 垂直消隐线, PPU不会访问内部内存，此时允许CPU对PPU内存进行访问
        {
            if(PPUClockCnt == 1)
            {
                reg_status.VerticalBlank = 1;
                if(reg_ctrl.EnableNMI)
                {
                    // 通知CPU NMI中断
                    // cpuNMICb();
                    nmi = true;
                }
            }
        }
        else
        {

        }

        PPUClockCnt ++;
        if(PPUClockCnt >= 341)
        {
            PPUClockCnt = 0;
            if(ScanLineCnt >= 261)
            {
                ScanLineCnt = -1;   // 一帧完成
                map.Refresh();      // 刷新一帧
                oddFrame = !oddFrame;
            }
            else
                ScanLineCnt ++;
        }
        #endif
    }
}





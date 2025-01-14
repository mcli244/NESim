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
    olc2c02::olc2c02()
    {
        map.Clear();
        memset(&PatternTable[0], 0, 1024);
        memset(&PatternTable[1], 0, 1024);

        memset(&NameTable[0], 0, 1024);
        memset(&NameTable[1], 0, 1024);

        memset(&Palette, 0, 32);

        auto ColorMux = [&](uint8_t r, uint8_t g, uint8_t b) 
        {
            uint32_t color = 0;
            color = (r << 16) | (g << 8) | (b); // 使用按位或
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
        case OAMADDR:    LOG_DEBUG("OAMADDR!!!!!!!!!!! cpu_addr:0x%x dat:0x%x", cpu_addr, dat); break;
        case OAMDATA:    LOG_DEBUG("OAMDATA!!!!!!!!!!! cpu_addr:0x%x dat:0x%x", cpu_addr, dat); break;
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
            LOG_ERROR("addr:0x%x not supported!", cpu_addr);
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
            ppu_addr &= 0x001F; // 32B的Palette空间
            // if (ppu_addr == 0x0010) ppu_addr = 0x0000;
            // if (ppu_addr == 0x0014) ppu_addr = 0x0004;
            // if (ppu_addr == 0x0018) ppu_addr = 0x0008;
            // if (ppu_addr == 0x001C) ppu_addr = 0x000C;
            // data = Palette[ppu_addr] & (reg_mask.GrayScale ? 0x30 : 0x3F);  
            data = Palette[ppu_addr] & 0x1F;    // 取低5位有效  
        }
        else
            LOG_ERROR("addr:0x%x not supported!", ppu_addr);

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
            ppu_addr &= 0x001F;
            Palette[ppu_addr] = dat;
        }
        else
            LOG_ERROR("addr:0x%x not supported!", ppu_addr);
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
        m_state = PreRender;
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
        // 262scanline、341clock 这里完全按照2c02的硬件行为做处理
        
        #if 0
        switch (m_state)
        {
        case PreRender:
            if(PPUClockCnt == 1)
            {
                reg_status.SpriteOverflow = 0;
                reg_status.SpriteZeroHit = 0;
                reg_status.VerticalBlank = 0;
            }
            else if(PPUClockCnt == 258 && reg_mask.RenderBackground && reg_mask.RenderSprites)
            {
                reg_v.val &= ~0x41f;
                reg_v.val |= reg_t.val & 0x41f;
            }
            else if(PPUClockCnt > 280 && PPUClockCnt <=304 && reg_mask.RenderBackground && reg_mask.RenderSprites)
            {
                reg_v.val &= ~0x7be0;
                reg_v.val |= reg_t.val & 0x7be0;
            }

            if(PPUClockCnt >= 339 - (!m_evenFrame && reg_mask.RenderBackground && reg_mask.RenderSprites))
            {
                PPUClockCnt = 0;
                ScanLineCnt = 0;
                m_state = Visible;
            }

            // add IRQ support for MMC3
            // if(PPUClockCnt==260 && m_showBackground && m_showSprites){
            //     m_bus.scanlineIRQ();
            // }

            break;
        case Visible:
            if(PPUClockCnt >0 && PPUClockCnt <=256)
            {
                int x = PPUClockCnt - 1;
                int y = ScanLineCnt;

                if(reg_mask.RenderBackground)   // 背景
                {
                    auto x_fine = (fine_x + x) % 8;
                    // 每8个时钟读取一个点的数据
                    if(x%9 == 8)  
                    {
                        BgTileIndex = busRead(0x2000 | reg_v.val & 0x0FFF);
                        // 提取当前像素所在的属性表中的位置
                        uint8_t AttributeTableByte = busRead(0x23C0 
                                                            | reg_v.nametable * 0x400 
                                                            | reg_v.coarse_y * 8
                                                            | reg_v.coarse_x);
                        // 提取当前像素的调色盘
                        uint8_t tx = (ScanLineCnt % 16);
                        uint8_t ty = ((PPUClockCnt-1) % 16);
                        uint8_t offset_t = 0;
                        if(tx < 8) {
                            if(ty < 8)  offset_t = 0;   // TL
                            else        offset_t = 4;   // BL
                        } else {
                            if(ty < 8)  offset_t = 2;   // TR
                            else        offset_t = 6;   // BR
                        }
                        PaletteIndex =  (AttributeTableByte >> offset_t) & 0x03;

                        TileIndexLsb = busRead(reg_ctrl.BackgroundPattrenTableIndex ? 0x1000 : 0x0000  // reg_ctrl 由于CPU设置
                                            | BgTileIndex * 16    
                                            | reg_v.fine_y);     // reg_addr由于CPU设置
                        TileIndexMsb = busRead(reg_ctrl.BackgroundPattrenTableIndex ? 0x1000 : 0x0000  // reg_ctrl 由于CPU设置
                                            | BgTileIndex * 16    
                                            | reg_v.fine_y + 8); // reg_addr由于CPU设置
                        
                        // 渲染完8个像素要切换一下tile
                        if (reg_v.coarse_x == 31)
                        {
                            reg_v.coarse_x = 0; // 下一行的第0个tile
                            // reg_v.nametable_x = ~reg_v.nametable_x; // TODO: 切换名称表
                            reg_v.nametable = reg_v.nametable ^ 0x01;

                        }
                        else
                        {
                            reg_v.coarse_x++;
                        }
                    }

                    // 在这里的1-256的clock中，每一个PPUClock渲染一个像素点，既一行数据
                    uint8_t tile_pattern_val;

                    // TileIndexMsb     |    TileIndexLsb
                    // 0 1 2 3 4 5 6 7       0 1 2 3 4 5 6 7   
                    // 需要一个在title中的偏移
                    tile_pattern_val = (((TileIndexMsb >> (7 - offset)) & 0x01) << 1) | ((TileIndexMsb >> (7 - offset)) & 0x01);
                    /*
                        4bit0
                        -----
                        SAAPP
                        |||||
                        |||++- tile pattern的像素值
                        |++--- attributes中的调色板编号 0-4
                        +----- 背景0/精灵1的选择
                    */
                    uint8_t ColorIndex =  (1 << 4) | ((PaletteIndex & 0x03) <<2) | tile_pattern_val; 
                    
                    // TODO:pixel需要判断背景和精灵的像素值和层级关系
                    map.DrawPoint(x, y, getColor(ColorIndex));
                }
            }
            else if(PPUClockCnt == 257 && reg_mask.RenderBackground)
            {
            
            }
            else if(PPUClockCnt == 258 && reg_mask.RenderBackground && reg_mask.RenderSprites)
            {
                reg_v.val &= ~0x41f;
                reg_v.val |= reg_t.val & 0x41f;
            }
            else if(PPUClockCnt == 260 && reg_mask.RenderBackground && reg_mask.RenderSprites)
            {
                // add IRQ support for MMC3
                // m_bus.scanlineIRQ();
            }
            else if(PPUClockCnt >= 340)
            {
                ScanLineCnt ++;
                PPUClockCnt = 0;
                if(ScanLineCnt >= 240)
                    m_state == PostRender;
            }
            break;
        case PostRender:
            if(PPUClockCnt >= 340)
            {
                ScanLineCnt ++;
                PPUClockCnt = 0;
                m_state = VerticalBlanking;
                map.Refresh();
            }
            break;
        case VerticalBlanking:
            if(PPUClockCnt == 1 && ScanLineCnt == 241)
            {
                reg_status.VerticalBlank = 1;
                cpuNMICb();
            }

            if(PPUClockCnt >= 340)
            {
                ScanLineCnt ++;
                PPUClockCnt = 0;
            }

            if(ScanLineCnt >= 261)
            {
                m_state = PreRender;
                ScanLineCnt = 0;
                m_evenFrame = !m_evenFrame;
            }
            break;
        default:
            LOG_ERROR("Unsupported state! state:%d", m_state);
            break;
        }

        PPUClockCnt++;

        #else
        if(ScanLineCnt == -1)
        {
            /*
                这是一条虚拟扫描线，其唯一目的是用下一条扫描线的前两个图块的数据填充移位寄存器。
                尽管这条扫描线没有渲染任何像素，但 PPU仍会像常规扫描线一样进行内存访问，使用 
                PPU 的V 寄存器的当前值，对于精灵提取，使用当前处于次级 OAM 中的任何数据（例如，
                来自上一帧的 扫描线 239 的精灵评估结果）。此扫描线的长度会有所不同，具体取决于
                渲染的是偶数帧还是奇数帧。对于奇数帧，扫描线末尾的循环会被跳过（这是通过从 (339,261) 
                直接跳转到 (0,0) 内部完成的，用最后一个虚拟名称表提取的最后一个标记替换第一个
                可见扫描线开头的空闲标记）。对于偶数帧，最后一个循环会正常发生。这样做是为了弥补
                PPU 物理输出视频信号方式的一些缺点，最终结果是当屏幕不滚动时图像更清晰。但是，
                可以通过保持渲染禁用直到此扫描线通过后再绕过此行为，这会导致图像具有“点爬行”效果，
                类似于但不完全像隔行视频中看到的那样。在此扫描线的 280 到 304 像素期间，如果启用了
                渲染，则重新加载垂直滚动位。
            */

            if(PPUClockCnt == 1)
            {
                reg_status.VerticalBlank = 0;
			    reg_status.SpriteOverflow = 0;
			    reg_status.SpriteZeroHit = 0;
            }
            else if(PPUClockCnt>=321 && PPUClockCnt <= 336)   // 321-336
            {
                /*  在这里，获取下一个扫描线的前两个图块，并将其加载到移位寄存器中。同样，每次内存访问需要 2 个 PPU 周期才能完成，而两个图块需要执行 4 个周期：
                    名称表字节
                    属性表字节
                    图案桌瓷砖低
                    图案表图块高位（图案表图块低位 +8 个字节）
                */

            }
        }
        else if(ScanLineCnt >= 0 && ScanLineCnt <= 239)   // 正常图片显示周期， PPU不停在读取内存数据，所以CPU不要访问PPU内存。
        {
            if(PPUClockCnt == 0)
            {
                
            }
            else if(PPUClockCnt <= 256 || (PPUClockCnt >= 321 && PPUClockCnt <= 336))
            {
                uint8_t offset = (fine_x + PPUClockCnt-1) % 8;
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
                        uint8_t AttributeTableByte = busRead(0x23C0 
                                                            | reg_v.nametable * 0x400 
                                                            | reg_v.coarse_y * 8
                                                            | reg_v.coarse_x);
                        // 提取当前像素的调色盘
                        uint8_t tx = (ScanLineCnt % 16);
                        uint8_t ty = ((PPUClockCnt-1) % 16);
                        uint8_t offset_t = 0;
                        if(tx < 8) {
                            if(ty < 8)  offset_t = 0;   // TL
                            else        offset_t = 4;   // BL
                        } else {
                            if(ty < 8)  offset_t = 2;   // TR
                            else        offset_t = 6;   // BR
                        }
                        PaletteIndex =  (AttributeTableByte >> offset_t) & 0x03;
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
                    TileIndexLsb = busRead(reg_ctrl.BackgroundPattrenTableIndex ? 0x1000 : 0x0000  // reg_ctrl 由于CPU设置
                                            | BgTileIndex * 16    
                                            | reg_v.fine_y);     // reg_addr由于CPU设置
                    break;
                case 6:
                    TileIndexMsb = busRead(reg_ctrl.BackgroundPattrenTableIndex ? 0x1000 : 0x0000  // reg_ctrl 由于CPU设置
                                            | BgTileIndex * 16    
                                            | reg_v.fine_y + 8); // reg_addr由于CPU设置
                    break;   
                case 7:
                    // 渲染完8个像素要切换一下tile
                    if (reg_v.coarse_x == 31)
                    {
                        reg_v.coarse_x = 0; // 下一行的第0个tile
                        // reg_v.nametable_x = ~reg_v.nametable_x; // TODO: 切换名称表
                        reg_v.nametable = reg_v.nametable ^ 0x01;

                    }
                    else
                    {
                        reg_v.coarse_x++;
                    }
                    break;
                default:
                    break;
                };

                // 在这里的1-256的clock中，每一个PPUClock渲染一个像素点，既一行数据
                int x, y;
                x = PPUClockCnt - 1;    // 屏幕:0-255 . PPUClockCnt: 1-256
                y = ScanLineCnt;        // 屏幕:0-239 . ScanLineCnt: 0-239

                uint8_t tile_pattern_val;

                // TileIndexMsb     |    TileIndexLsb
                // 0 1 2 3 4 5 6 7       0 1 2 3 4 5 6 7   
                // 需要一个在title中的偏移
                tile_pattern_val = (((TileIndexMsb >> (7 - offset)) & 0x01) << 1) | ((TileIndexLsb >> (7 - offset)) & 0x01);
                /*
                    4bit0
                    -----
                    SAAPP
                    |||||
                    |||++- tile pattern的像素值
                    |++--- attributes中的调色板编号 0-4
                    +----- 背景0/精灵1的选择
                */
                uint8_t ColorIndex =  (1 << 4) | ((PaletteIndex & 0x03) <<2) | tile_pattern_val; 
                
                // TODO:pixel需要判断背景和精灵的像素值和层级关系
                map.DrawPoint(x, y, getColor(ColorIndex));
                //map.Refresh();

                if(PPUClockCnt == 256)  // 渲染完这一行的有效数据
                {
                    if(reg_v.fine_y < 7)   
                    {
                        reg_v.fine_y ++;
                    }
                    else    
                    {
                        reg_v.fine_y = 0;
                        if(reg_v.coarse_y == 29)
                        {
                            reg_v.coarse_y = 0;
                            // reg_v.nametable_x = ~reg_v.nametable_x; // TODO: 切换名称表
                            reg_v.nametable = reg_v.nametable ^ 0x02;
                        }
                        else
                        {
                            reg_v.coarse_y ++;
                        }
                    }
                }
            }
            else if(PPUClockCnt <= 320)   // 257-320
            {
                /*
                    下一个扫描线上的精灵的图块数据在此处获取。同样，每次内存访问需要 2 个 PPU 周期才能完成，并且对 8 个精灵中的每一个执行 4 个周期：
                    垃圾名称表字节
                    垃圾名称表字节
                    图案桌瓷砖低
                    图案表图块高位（图案表图块低位 +8 个字节）
                    发生垃圾提取，以便执行 BG 图块提取的相同电路可以重新用于精灵图块提取。
                    如果下一个扫描线上的精灵少于 8 个，则由于次级 OAM 中有虚拟精灵数据（请参阅精灵评估），剩余的精灵将对图块 $FF 进行虚拟提取。然后丢弃此数据，并用一组透明的值加载精灵。
                    除此之外，每个精灵的 X 位置和属性都会从辅助 OAM 加载到各自的计数器/锁存器中。这发生在第二次垃圾名称表提取期间，属性字节在第一个刻度期间加载，X 坐标在第二个刻度期间加载。
                */
            }
            // else if(PPUClockCnt <= 336)   // 321-336
            // {
            //     /*  在这里，获取下一个扫描线的前两个图块，并将其加载到移位寄存器中。同样，每次内存访问需要 2 个 PPU 周期才能完成，而两个图块需要执行 4 个周期：
            //         名称表字节
            //         属性表字节
            //         图案桌瓷砖低
            //         图案表图块高位（图案表图块低位 +8 个字节）
            //     */
            // }
            else if(PPUClockCnt <= 340)// 337-340
            {
                /*  提取了两个字节，但目的未知。每次提取都需要 2 个 PPU 周期。
                    名称表字节
                    名称表字节
                    这里获取的两个字节都是将在下一个扫描线（即图块 3）开头获取的同一个名称表字节。已知至少一个映射器（MMC5）使用这串连续的三个名称表获取来计时扫描线计数器。
                */
            }
            {
                
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
                    cpuNMICb();
                }
            }
        }
        else
        {

        }

        // uint8_t offset = (PPUClockCnt - 1) % 8;
        // uint8_t pixel = (((TileIndexMsb >> offset) & 0x01) << 1) | ((TileIndexLsb >> offset) & 0x01);
        // // LOG_INFO("offset:0x%x pixel:0x%x PPUClockCnt:%d ScanLineCnt:%d BgTileIndex:%d TileIndexLsb:0x%x TileIndexMsb:0x%x ScrollPosition.x:%d ScrollPosition.y:%d", 
        // //     offset, pixel, PPUClockCnt, ScanLineCnt, BgTileIndex, TileIndexLsb, TileIndexMsb, ScrollPosition.x, ScrollPosition.y);
        // switch (pixel)
        // {
        // case 0: map.DrawPoint(PPUClockCnt - 1, ScanLineCnt, 0); break;
        // case 1: map.DrawPoint(PPUClockCnt - 1, ScanLineCnt, 63); break;
        // case 2: map.DrawPoint(PPUClockCnt - 1, ScanLineCnt, 127); break;
        // case 3: map.DrawPoint(PPUClockCnt - 1, ScanLineCnt, 255); break;
        // default:
        //     LOG_ERROR("tmp:0x%x", pixel);
        //     break;
        // }
        // // map.Refresh();

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





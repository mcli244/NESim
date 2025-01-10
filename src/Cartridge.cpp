#include "Cartridge.h"
#include "log.h"
#include <fstream>

namespace nes
{
    namespace{
        struct sHeader
        {
            char name[4];               // 魔数标识 "NES\x1A" (ASCII 值: 0x4E 0x45 0x53 0x1A)
            uint8_t prg_rom_chunks;     // PRG-ROM 页数（每页 16 KB）
            uint8_t chr_rom_chunks;     // CHR-ROM 页数（每页 8 KB，如果为 0，则游戏使用 CHR-RAM）
            uint8_t mapper1;            // 标志位 6（控制镜像、存在电池支持、Trainer 等）
                                        //    用来控制镜像、电池支持等：
                                        //    Bit 0: 镜像方式，0 = 水平镜像，1 = 垂直镜像。
                                        //    Bit 1: 是否存在电池支持的 RAM。
                                        //    Bit 2: 是否有 Trainer（512 字节，位于文件头之后）。
                                        //    Bit 3: 四屏镜像。
                                        //    Bit 4–7: Mapper 号低 4 位。
            uint8_t mapper2;            // 标志位 7（控制 TV 系统、Mapper 号的高位等）
                                        //    控制 TV 系统和 Mapper 号的高位：
                                        //    Bit 0–1: Console 类型，0 = NES/FC，1 = Vs. System。
                                        //    Bit 2: PlayChoice-10 标志。
                                        //    Bit 3–7: Mapper 号高 4 位
            uint8_t prg_ram_size;       // PRG-RAM 大小（每页 8 KB，如果为 0，默认值为 1 页）
            uint8_t tv_system1;         // 标志位 9（NTSC/PAL Bit 0: TV 系统标志，0 = NTSC，1 = PAL。）
            uint8_t tv_system2;         // 标志位 10（扩展区位信息，通常为 0 用于扩展用途）
            char unused[5];
        } header;
        enum MIRROR
        {
            HORIZONTAL,
            VERTICAL,
            ONESCREEN_LO,
            ONESCREEN_HI,
        } mirror = HORIZONTAL;
    }
    
    void Cartridge::printHeader(void)
    {
        uint8_t mapper = 0;

        LOG_INFO("################ NES INFO ##################");
        LOG_INFO("PRG-ROM:%d - %d KB", header.prg_rom_chunks, header.prg_rom_chunks * 16);
        if(0 == header.chr_rom_chunks){
            LOG_INFO("CHR-RAM:%d - %d KB", header.prg_ram_size, header.prg_ram_size * 8);
        } else {
            LOG_INFO("CHR-ROM:%d - %d KB", header.chr_rom_chunks, header.chr_rom_chunks * 8);
        }
            
        if(header.mapper1 & (1 << 0)){
            LOG_INFO("MirrorMode: Vertical mirror image");
        } else {
            LOG_INFO("MirrorMode: Horizontal mirror image");
        }
        if(header.mapper1 & (1 << 1)){
            LOG_INFO("Battery-supported RAM: existing");
        } else {    
            LOG_INFO("Battery-supported RAM: does not exist");
        }
        if(header.mapper1 & (1 << 2)){
            LOG_INFO("Trainer: existing");
        } else {    
            LOG_INFO("Trainer: does not exist");
        }
        if(header.mapper1 & (1 << 3)){   
            LOG_INFO("Four-screen mirroring: existing");
        } else {    
            LOG_INFO("Four-screen mirroring: does not exist");
        }
        mapper = ((header.mapper2 >> 4) << 4) | (header.mapper1 >> 4);
        LOG_INFO("Mapper: %d", mapper);
        LOG_INFO("################ NES END ##################");

    }

    bool Cartridge::loadNesFile(std::string nesFile)
    {
        bool ret = false;
        std::ifstream ifs;
        ifs.open(nesFile, std::ifstream::binary);
        if (!ifs.is_open())
        {
            LOG_ERROR("File open fialed:%s", nesFile.c_str());
            return false;
        }

        // Read file header
        ifs.read((char*)&header, sizeof(sHeader));
        if(std::string(header.name, 4) != "NES\x1A")
        {
            LOG_ERROR("Not is NES File:%s", nesFile.c_str());
            ifs.close();
            return false;
        }

        // If a "trainer" exists we just need to read past
        // it before we get to the good stuff
        if (header.mapper1 & 0x04)
            ifs.seekg(512, std::ios_base::cur);

        // Determine Mapper ID
        nMapperID = ((header.mapper2 >> 4) << 4) | (header.mapper1 >> 4);
        nPRGBanks = header.prg_rom_chunks;
        vPRGMemory.resize(nPRGBanks * 16 *1024);
        ifs.read((char*)vPRGMemory.data(), vPRGMemory.size());

        if(0 == header.chr_rom_chunks)
        {
            nCHRBanks = header.prg_ram_size;
            LOG_INFO("PRG RAM!");
        }
        else
            nCHRBanks = header.chr_rom_chunks;
        vCHRMemory.resize(nCHRBanks * 8 *1024);
        ifs.read((char*)vCHRMemory.data(), vCHRMemory.size());
 
        // Load appropriate mapper
        switch (nMapperID)
        {
            case 0: 
                pMapper = std::make_shared<nes::Mapper000>(nPRGBanks, nCHRBanks); 
                LOG_INFO("supported nMapperID:%d", nMapperID);
                ret = true;
            break;
            default:
                LOG_ERROR("Not yet supported nMapperID:%d", nMapperID);
                ret = false;
                break;
        }

        printHeader();
        ifs.close();
        return ret;
    }

    uint8_t Cartridge::readCHR(uint16_t addr)
    {
        uint16_t m_addr = 0;
        addr -= 0x6000; // TODO:test
        if(pMapper && pMapper->CHR_mmap(addr, m_addr))
        {
            //LOG_INFO("readCHR addr:0x%x m_addr:0x%x dat:0x%x", addr, m_addr, vCHRMemory[m_addr]);
            return vCHRMemory[m_addr];
        }
        return 0xff;
    }

    bool Cartridge::writeCHR(uint16_t addr, uint8_t value)
    {
        uint16_t m_addr = 0;
        if(pMapper && pMapper->CHR_mmap(addr, m_addr)){
            vCHRMemory[m_addr] = value;
            return true;
        }
        
        return false;
    }

    uint8_t Cartridge::readPRG(uint16_t addr)
    {
        uint16_t m_addr = 0;
        if(pMapper && pMapper->PRG_mmap(addr, m_addr)){
            return vPRGMemory[m_addr];
        }
        return 0xff;
    }

    bool Cartridge::writePRG(uint16_t addr, uint8_t value)
    {
        uint16_t m_addr = 0;
        if(pMapper->PRG_mmap(addr, m_addr)){
            vPRGMemory[m_addr] = value;
            return true;
        }

        return false;
    }

    uint8_t Cartridge::read(uint16_t addr)
    {
        /*
            • $4020–$5FFF   $1FE0   Expansion ROM
            • $6000–$7FFF   $2000   Usually cartridge RAM, when present.
            • $8000–$FFF9	$7FF9   Usually cartridge ROM and mapper registers.
                • $8000-$BFFF     $4000    LPRG-ROM
                • $C000-$FFF9     $3FF9    UPRG-ROM
        */
        // TODO
        if(addr >= 0x4020 && addr < 0x5FFF)
        {
            //LOG_ERROR("Expansion ROM! TODO!!");
        }
        else if(addr >= 0x6000 && addr <= 0x7FFF)
            return readCHR(addr);
        else if(addr >= 0x8000 && addr <= 0xFFFF)
            return readPRG(addr);
        else
        {
            // LOG_ERROR("addr failed addr:0x%x", addr);
        }
        return 0;
    }

    bool Cartridge::write(uint16_t addr, uint8_t value)
    {
        if(addr >= 0x4020 && addr <= 0x6000)
        {
            //LOG_ERROR("Expansion ROM! TODO!!");
        }
        else if(addr >= 0x6000 && addr <= 0x7FFF)
            return writeCHR(addr, value);
        else if(addr >= 0x8000 && addr <= 0xFFFF)
            return writePRG(addr, value);
        else
        {
            // LOG_ERROR("addr failed addr:0x%x", addr);
        }
        return false;
    }
    CartridgeMirrorMode Cartridge::getMirrorMode(void)
    {
        return mirrorMode;
    }
}

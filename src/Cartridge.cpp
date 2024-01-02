#include "Cartridge.h"
#include "log.h"
#include <fstream>

namespace nes
{
    Cartridge::Cartridge(std::string nesFile)
    {
        // iNES Format Header
        struct sHeader
        {
            char name[4];
            uint8_t prg_rom_chunks;
            uint8_t chr_rom_chunks;
            uint8_t mapper1;
            uint8_t mapper2;
            uint8_t prg_ram_size;
            uint8_t tv_system1;
            uint8_t tv_system2;
            char unused[5];
        } header;

        LOG_INFO("NES File:%s", nesFile.c_str());
        std::ifstream ifs;
        ifs.open(nesFile, std::ifstream::binary);
        if (ifs.is_open())
        {
            // Read file header
            ifs.read((char*)&header, sizeof(sHeader));

            // If a "trainer" exists we just need to read past
            // it before we get to the good stuff
            if (header.mapper1 & 0x04)
                ifs.seekg(512, std::ios_base::cur);

            // Determine Mapper ID
            nMapperID = ((header.mapper2 >> 4) << 4) | (header.mapper1 >> 4);
            mirror = (header.mapper1 & 0x01) ? VERTICAL : HORIZONTAL;

            // "Discover" File Format
            uint8_t nFileType = 1;

            if (nFileType == 0)
            {

            }

            if (nFileType == 1)
            {
                nPRGBanks = header.prg_rom_chunks;
                vPRGMemory.resize(nPRGBanks * 16384);
                ifs.read((char*)vPRGMemory.data(), vPRGMemory.size());

                nCHRBanks = header.chr_rom_chunks;
                vCHRMemory.resize(nCHRBanks * 8192);
                ifs.read((char*)vCHRMemory.data(), vCHRMemory.size());
            }

            if (nFileType == 2)
            {

            }

            // Load appropriate mapper
            switch (nMapperID)
            {
            case 0: pMapper = std::make_shared<nes::Mapper000>(nPRGBanks, nCHRBanks); break;
            default:
                LOG_ERROR("Not yet supported nMapperID:%d", nMapperID);
                break;
            }

            LOG_INFO("nMapperID:%d", nMapperID);
            LOG_INFO("mirror:%d", mirror);
            LOG_INFO("nPRGBanks:%d", nPRGBanks);
            LOG_INFO("nCHRBanks:%d", nCHRBanks);

            ifs.close();
        }
    }

    uint8_t Cartridge::readCHR(uint16_t addr)
    {
        uint16_t m_addr = 0;
        if(pMapper->CHR_mmap(addr, m_addr))
            return vCHRMemory[m_addr];
        else
            LOG_ERROR("readCHR failed addr:0x%x", addr);
        return 0;
    }

    bool Cartridge::writeCHR(uint16_t addr, uint8_t value)
    {
        uint16_t m_addr = 0;
        if(pMapper->CHR_mmap(addr, m_addr))
        {
            vCHRMemory[m_addr] = value;
            return true;
        }
        else
            LOG_ERROR("Read only! vPRGMemory");
        return 0;
    }

    uint8_t Cartridge::readPRG(uint16_t addr)
    {
        uint16_t m_addr = 0;
        if(pMapper && pMapper->PRG_mmap(addr, m_addr))
            return vPRGMemory[m_addr];
        else
            LOG_ERROR("readCHR failed addr:0x%x", addr);
        return 0;
    }

    bool Cartridge::writePRG(uint16_t addr, uint8_t value)
    {
        uint16_t m_addr = 0;
        if(pMapper->PRG_mmap(addr, m_addr))
        {
            vPRGMemory[m_addr] = value;
            return true;
        }
        else
            LOG_ERROR("Read only! vPRGMemory");
        return 0;
    }

}

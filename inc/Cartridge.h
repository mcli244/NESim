#pragma once
#include <cstdint>
#include <string>
#include <fstream>
#include <vector>
#include <memory>
#include "Mapper000.h"
namespace nes
{
    class Cartridge
    {
        public:
            enum MIRROR
            {
                HORIZONTAL,
                VERTICAL,
                ONESCREEN_LO,
                ONESCREEN_HI,
            } mirror = HORIZONTAL;


        private:
            std::vector<uint8_t> vCHRMemory;
            std::vector<uint8_t> vPRGMemory;
            uint8_t nMapperID = 0;
            uint8_t nPRGBanks = 0;
            uint8_t nCHRBanks = 0;

            std::shared_ptr<class Mapper> pMapper;
            

        public:
            // nes::Mapper map;
            uint8_t readCHR(uint16_t addr);
            bool writeCHR(uint16_t addr, uint8_t value);
            uint8_t readPRG(uint16_t addr);
            bool writePRG(uint16_t addr, uint8_t value);
            Cartridge(std::string nesFile);
            ~Cartridge(){};

    };
}
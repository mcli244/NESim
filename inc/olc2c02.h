#pragma once
#include "bus.h"

namespace nes
{
    class olc2c02
    {
        private:
            nes::bus *ppuBus;
        
        public:
            void connectBus(nes::bus *bus);
            uint8_t read(uint16_t addr);
            void write(uint16_t addr, uint8_t value);

            olc2c02();
            olc2c02(nes::bus *bus);
            ~olc2c02();
    };
}
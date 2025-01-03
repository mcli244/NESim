#pragma once
#include <cstdint>

namespace nes
{
    class olc2c02
    {
        public:
            uint8_t read(uint16_t addr);
            void write(uint16_t addr, uint8_t value);

            olc2c02(){};
            ~olc2c02(){};
    };
}
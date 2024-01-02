#include "olc2c02.h"

namespace nes
{
    olc2c02::olc2c02(){};
    olc2c02::olc2c02(nes::bus *bus):ppuBus(bus){};

    olc2c02::~olc2c02(){};

    void olc2c02::connectBus(nes::bus *bus){
        ppuBus = bus;
    };

    uint8_t olc2c02::read(uint16_t addr)
    {   
        if(ppuBus) return ppuBus->read(addr);
        else        return 0;
    }

    void olc2c02::write(uint16_t addr, uint8_t dat)
    {
        if(ppuBus) 
            ppuBus->write(addr, dat);
    }
}





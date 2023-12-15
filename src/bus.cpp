#include "bus.h"

namespace nes
{
    bus::bus():
    ram(64*1024, 0)
    {

    }

    bus::~bus(){};

    BUS_DATA bus::read(BUS_ADDR addr)
    {
        if(addr >= 0 && addr <= 0xFFFF)
            return ram[addr];
        return 0;
    }

    void bus::write(BUS_ADDR addr, BUS_DATA value)
    {
        if(addr >= 0 && addr <= 0xFFFF)
            ram[addr] = value;
    }
}



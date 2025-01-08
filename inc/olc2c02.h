#pragma once
#include <cstdint>
#include "Cartridge.h"

namespace nes
{
    class olc2c02
    {
        public:
            uint8_t read(uint16_t addr);
            void write(uint16_t addr, uint8_t value);
            bool connectCartridge(nes::Cartridge *cart){m_cart = cart;};

            olc2c02(){};
            ~olc2c02(){m_cart = nullptr;};
        
        private:
            nes::Cartridge *m_cart = nullptr;
            union
            {
                struct{
                    uint8_t NameTableIndex : 2;
                    uint8_t IncrementMode : 1;
                    uint8_t SpritePattrenTableIndex : 1;
                    uint8_t BackgroundPattrenTableIndex : 1;
                    uint8_t SpriteSize : 1;
                    uint8_t SlaveMode : 1;
                    uint8_t EnableNMI : 1;
                };
                uint8_t val;
            }reg_ctrl;

            union
            {
                struct{
                    uint8_t unused : 5;
                    uint8_t SpriteOverflow : 1;
                    uint8_t SpriteZeroHit : 1;
                    uint8_t VerticalBlank : 1;
                };
                uint8_t val;
            }reg_status;

            union
            {
                struct{
                    uint8_t GrayScale : 1;
                    uint8_t RenderBackgroundLeft : 1;
                    uint8_t RenderSpritesLeft : 1;
                    uint8_t RenderBackground : 1;
                    uint8_t RenderSprites : 1;
                    uint8_t EnhanceRed : 1;
                    uint8_t EnhanceGreen : 1;
                    uint8_t EnhanceBlue : 1;
                };
                uint8_t val;
            }reg_mask;

            uint16_t CurrentVRAMAddress;
            uint16_t TemporaryVRAMAddress;
            uint8_t FineXScroll;
            uint8_t Toggle; 
    };
}
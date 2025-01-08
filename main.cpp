#include <iostream>
#include <curses.h>
#include <sstream>
#include <unistd.h>
#include "olc6502.h"
#include "olc2c02.h"
#include "Cartridge.h"
#include "log.h"
#include "map.h"

int main(int argc, char **argv)
{
    Log::get_instance()->init("./nesim.log", 0, 2000, 800000, 0);
    LOG_INFO("build:%s %s", __DATE__, __TIME__);

    nes::bus     mainBus;
    nes::olc6502 cpu(&mainBus);
    nes::olc2c02 ppu;

    nes::Cartridge cartridge;
    if(argc < 2)
    {
        LOG_ERROR("Usage: %s <NesFile>", argv[0]);
        return 0;
    }
    
    if(!cartridge.loadNesFile(argv[1]))
    {
        LOG_ERROR("load file failed file:%s", argv[1]);
        return 0;
    }
    ppu.connectCartridge(&cartridge);
    mainBus.connectCartridge(&cartridge);
    mainBus.connectPPU(&ppu);
    
    cpu.reset();
    std::map<uint16_t, std::string> mapAsm;
    mapAsm = cpu.disassemble(0x0000, 0xFFFF);

    Map map(128, 128);
    map.Clear();
    map.DrawPoint(0, 0, 0xff0000);
 
    // draw tile 
    for(uint8_t TileY=0; TileY<16; TileY++)
    {
        for(uint8_t TileX=0; TileX<16; TileX++)
        {
            uint32_t data_offset = TileX * 16 + TileY * 16 * 16;
            for(uint8_t row=0; row<8; row++)
            {
                uint8_t tile_lsb = cpu.read(0x6000 + row + data_offset);
                uint8_t tile_msb = cpu.read(0x6000 + row + 0x0008 + data_offset);
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

	initscr(); 
    if(!has_colors()){
        endwin();
        fprintf(stderr,"Error - no color support on this terminal \n");
        exit(1);
    } 
    if(start_color() != OK){
        endwin();
        fprintf(stderr,"Error -could not initialize colors\n");
        exit(2);
    }

    init_pair(1,COLOR_RED,COLOR_BLACK);
    init_pair(2,COLOR_BLUE,COLOR_BLACK);

    keypad(stdscr, 1);
    cbreak();
    int cnt = 0;
    bool runing = true;
    while(runing)
    {
        clear();
        // box(stdscr,ACS_VLINE,ACS_HLINE);//画一个框
        int addr = 0x2000;
        for(int i=0; i<16; i++)
        {
            mvprintw(i, 0, "$%04X: ", addr);
            for(int y=0; y<16; y++)
            {
                printw("%02X ", cpu.read(addr++));
            }
        }

        addr = cpu.pc;
        for(int i=0; i<10; i++)
        {
            mvprintw(i + 17, 0, "$%04X: ", addr);
            for(int y=0; y<16; y++)
            {
                printw("%02X ", cpu.read(addr++));
            }
        }

        mvprintw(0, 60, "STATUS: N V - B D I Z C");
        mvprintw(1, 60, "STATUS: %d %d %d %d %d %d %d %d", 
            cpu.GetFlag(nes::olc6502::FLAGS6502::N), cpu.GetFlag(nes::olc6502::FLAGS6502::V), 
            cpu.GetFlag(nes::olc6502::FLAGS6502::U), cpu.GetFlag(nes::olc6502::FLAGS6502::B), 
            cpu.GetFlag(nes::olc6502::FLAGS6502::D), cpu.GetFlag(nes::olc6502::FLAGS6502::I), 
            cpu.GetFlag(nes::olc6502::FLAGS6502::Z), cpu.GetFlag(nes::olc6502::FLAGS6502::C));

        mvprintw(2, 60, "STACK: $%04X [%04d]", cpu.stkp, cpu.stkp);
        mvprintw(3, 60, "PC: $%04X [%04d]", cpu.pc, cpu.pc);
        mvprintw(4, 60, "A:  $%02X [%04d]", cpu.a , cpu.a );
        mvprintw(5, 60, "X:  $%02X [%04d]", cpu.x , cpu.x );
        mvprintw(6, 60, "Y:  $%02X [%04d]", cpu.y , cpu.y );

        // 显示返回边代码, 显示PC附近的反汇编
        std::map<uint16_t, std::string>::iterator mapIter;
        
        mapIter = mapAsm.find(cpu.pc);
        if (mapIter != mapAsm.end())
        {
            for(int i=0; i<10; i++)
            {
                mapIter --;
                if(mapIter == mapAsm.begin())
                    break;
            }
            for(int i=0; i<20; i++)
            {
                if(mapIter->first == cpu.pc)
                {
                    attrset(A_NORMAL); /* 先将属性设定为正常模式 */
                    attrset(COLOR_PAIR(2));
                    mvprintw(8+i, 60, "%s", mapIter->second.c_str());
                    attroff(COLOR_PAIR(2));
                }
                else
                    mvprintw(8+i, 60, "%s", mapIter->second.c_str());
                mapIter ++;
            }
        }
        

        // move(LINES/2,COLS/2);//光标移到中心
        // waddstr(stdscr,"hello,world!");//输出
        mvprintw(28, 0, " SPACE = step      R = Rest      Q = Quit");
        refresh();//逻辑屏幕的改动在物理屏幕（显示器）上显示
        // cpu.clock();
        // sleep(1);
        
        char input = getch();
        switch(input)
        {
            case ' ': cpu.clock(); cpu.pass(); break;
            case 'r': 
            case 'R': cpu.reset(); break;
            case 'q': 
            case 'Q': runing = false; break;
        }
    }
    
    endwin();//结束curses
    return 0;
}

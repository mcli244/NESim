#include <iostream>
#include <curses.h>
#include <sstream>
#include <unistd.h>
#include "olc6502.h"
#include "olc2c02.h"
// #include "Cartridge.h"
#include "log.h"
#include "map.h"

int main(int argc, char **argv)
{
    Log::get_instance()->init("./nesim.log", 0, 2000, 800000, 0);
    LOG_INFO("build:%s %s", __DATE__, __TIME__);

    if(argc < 2)
    {
        LOG_ERROR("Usage: %s <NesFile>", argv[0]);
        return 0;
    }

    nes::Cartridge cartridge;
    if(!cartridge.loadNesFile(argv[1]))
    {
        LOG_ERROR("load file failed file:%s", argv[1]);
        return 0;
    }

    nes::bus     mainBus;
    mainBus.connectCartridge(&cartridge);
    
    nes::olc2c02 ppu;
    ppu.connectCartridge(&cartridge);
    mainBus.connectPPU(&ppu);
    
    nes::olc6502 cpu(&mainBus);
    cpu.reset();
    std::map<uint16_t, std::string> mapAsm;
    mapAsm = cpu.disassemble(0x0000, 0xFFFF);

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
    int index = 0;
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
        for(int i=0; i<16; i++)
        {
            mvprintw(i + 17, 0, "$%04X: ", addr);
            for(int y=0; y<16; y++)
            {
                printw("%02X ", cpu.read(addr++));
            }
        }

        addr = 0;
        for(int i=0; i<16; i++)
        {
            mvprintw(i + 17 + 17, 0, "$%04X: ", addr);
            for(int y=0; y<16; y++)
            {
                printw("%02X ", ppu.readVRAM(addr++));
            }
        }

        mvprintw(17 + 17 + 17, 0, " SPACE = step      R = Rest      Q = Quit");
        
        index = 0;
        mvprintw(index++, 60, "STATUS: N V - B D I Z C");
        mvprintw(index++, 60, "STATUS: %d %d %d %d %d %d %d %d", 
            cpu.GetFlag(nes::olc6502::FLAGS6502::N), cpu.GetFlag(nes::olc6502::FLAGS6502::V), 
            cpu.GetFlag(nes::olc6502::FLAGS6502::U), cpu.GetFlag(nes::olc6502::FLAGS6502::B), 
            cpu.GetFlag(nes::olc6502::FLAGS6502::D), cpu.GetFlag(nes::olc6502::FLAGS6502::I), 
            cpu.GetFlag(nes::olc6502::FLAGS6502::Z), cpu.GetFlag(nes::olc6502::FLAGS6502::C));

        mvprintw(index++, 60, "STACK: $%04X [%04d]", cpu.stkp, cpu.stkp);
        mvprintw(index++, 60, "PC: $%04X [%04d]", cpu.pc, cpu.pc);
        mvprintw(index++, 60, "A:  $%02X [%04d]", cpu.a , cpu.a );
        mvprintw(index++, 60, "X:  $%02X [%04d]", cpu.x , cpu.x );
        mvprintw(index++, 60, "Y:  $%02X [%04d]", cpu.y , cpu.y );

        index++;
        mvprintw(index++, 60, "PPUCTRL   [%04d]", ppu.reg_ctrl.val);
        mvprintw(index++, 60, "PPUMASK   [%04d]", ppu.reg_mask.val);
        mvprintw(index++, 60, "PPUSTATUS [%04d]", ppu.reg_status.val);
        mvprintw(index++, 60, "ScanLineCnt: %04d", ppu.ScanLineCnt);
        mvprintw(index++, 60, "PPUClockCnt: %04d", ppu.PPUClockCnt);
        index++;

        // 显示返回边代码, 显示PC附近的反汇编
        std::map<uint16_t, std::string>::iterator mapIter;
        
        mapIter = mapAsm.find(cpu.pc);
        if (mapIter != mapAsm.end())
        {
            for(int i=0; i<5; i++)
            {
                mapIter --;
                if(mapIter == mapAsm.begin())
                    break;
            }
            for(int i=0; i<10; i++)
            {
                if(mapIter->first == cpu.pc)
                {
                    attrset(A_NORMAL); /* 先将属性设定为正常模式 */
                    attrset(COLOR_PAIR(2));
                    mvprintw(index+i, 60, "%s", mapIter->second.c_str());
                    attroff(COLOR_PAIR(2));
                }
                else
                    mvprintw(index+i, 60, "%s", mapIter->second.c_str());
                mapIter ++;
            }
        }
        
        // move(LINES/2,COLS/2);//光标移到中心
        // waddstr(stdscr,"hello,world!");//输出
        
        refresh();//逻辑屏幕的改动在物理屏幕（显示器）上显示

        char input = getch();
        switch(input)
        {
            case ' ': 
                cpu.clock();
                cpu.pass();

                ppu.clock();
                ppu.clock();
                ppu.clock();
                break;
            case 'r': 
            case 'R': 
                cpu.reset();
                ppu.reset();
                break;
            case 'q': 
            case 'Q': runing = false; break;
        }
    }
    
    endwin();//结束curses
    return 0;
}

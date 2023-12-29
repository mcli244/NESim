#include <iostream>
#include <curses.h>
#include <sstream>
#include <unistd.h>
#include "olc6502.h"

int main(int argc, char **argv)
{
    nes::bus     mainBus;
    nes::olc6502 cpu(&mainBus);
    

    // cpu.connectBus(&mainBus);
    

    // Convert hex string into bytes for RAM
    std::stringstream ss;
    ss << "A2 03 8E 00 00 A2 64 8E 01 00 AC 00 00 A9 00 18 6D 01 00 88 D0 FA 8D 02 00 EA EA EA";
    uint16_t nOffset = 0x8000;
    while (!ss.eof())
    {
        std::string b;
        ss >> b;

        cpu.write(nOffset++, (uint8_t)std::stoul(b, nullptr, 16));
    }

    // Set Reset Vector
    cpu.write(0xFFFC, 0x00);
    cpu.write(0xFFFD, 0x80);

    cpu.reset();

    // Extract dissassembly
    std::map<uint16_t, std::string> mapAsm;
    mapAsm = cpu.disassemble(0x0000, 0xFFFF);

    // std::map<uint16_t, std::string>::iterator m1_Iter;
    // for ( m1_Iter = mapAsm.begin( ); m1_Iter != mapAsm.end( ); m1_Iter++ )
    //     std::cout << m1_Iter->first<<" "<<m1_Iter->second<<std::endl;

    // return 0;

	initscr(); //初始化屏幕进入curses图形化工作方式
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
        int addr = 0;
        for(int i=0; i<16; i++)
        {
            mvprintw(i, 0, "$%04X: ", addr);
            for(int y=0; y<16; y++)
            {
                printw("%02X ", cpu.read(addr++));
            }
        }

        addr = 0x8000;
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
            cpu.GetFlag(nes::olc6502::FLAGS6502::N), cpu.GetFlag(nes::olc6502::FLAGS6502::V), cpu.GetFlag(nes::olc6502::FLAGS6502::U), cpu.GetFlag(nes::olc6502::FLAGS6502::B), 
            cpu.GetFlag(nes::olc6502::FLAGS6502::D), cpu.GetFlag(nes::olc6502::FLAGS6502::I), cpu.GetFlag(nes::olc6502::FLAGS6502::Z), cpu.GetFlag(nes::olc6502::FLAGS6502::C));

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

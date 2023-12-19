#include <iostream>
#include <curses.h>
#include "olc6502.h"

int main(int argc, char **argv)
{
    nes::olc6502 cpu;
    cpu.reset();

	initscr(); //初始化屏幕进入curses图形化工作方式
    // box(stdscr,ACS_VLINE,ACS_HLINE);//画一个框
    int addr = 0;
    for(int i=0; i<16; i++)
    {
        mvprintw(i, 0, "$%04X: ", addr);
        for(int y=0; y<16; y++)
        {
            addr ++;
            printw("%02X ", cpu.read(addr));
        }
    }

    addr = 0x800;
    for(int i=0; i<16; i++)
    {
        mvprintw(i + 18, 0, "$%04X: ", addr);
        for(int y=0; y<16; y++)
        {
            addr ++;
            printw("%02X ", cpu.read(addr));
        }
    }

    mvprintw(0, 60, "STATUS:");
    mvprintw(1, 60, "STACK: $%04X [%04d]", cpu.stkp, cpu.stkp);
    mvprintw(2, 60, "PC: $%04X [%04d]", cpu.pc, cpu.pc);
    mvprintw(3, 60, "A:  $%02X [%04d]", cpu.a , cpu.a );
    mvprintw(4, 60, "X:  $%02X [%04d]", cpu.x , cpu.x );
    mvprintw(5, 60, "Y:  $%02X [%04d]", cpu.y , cpu.y );

    // move(LINES/2,COLS/2);//光标移到中心
    // waddstr(stdscr,"hello,world!");//输出
    refresh();//逻辑屏幕的改动在物理屏幕（显示器）上显示
    getch();//屏幕暂停
    endwin();//结束curses
    return 0;
}

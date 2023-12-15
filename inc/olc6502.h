#pragma once
#include <string>
#include <map>
#include "bus.h"

namespace nes
{
    class olc6502
    {
        
    public:
        const uint16_t NMIVector = 0xfffa;
        const uint16_t ResetVector = 0xfffc;
        const uint16_t IRQVector = 0xfffe;

        enum FLAGS6502
        {
            C = (1 << 0),	// Carry Flag(C)       ：进位标志(一般对于无符号数来说)，如果最近一条指令有溢出——上溢：超出了 255，下溢：低于 0，则设置该 bit 为 1，比如说执行 255 + 1 会上溢，将 Carry Flag 置 1。有了 Carry Flag，使得可以进行长度超过 8 位的运算。
            Z = (1 << 1),	// Zero                ：最近一条指令的结果是否为 0，如果是，则置 1，否则清零
            I = (1 << 2),	// Disable Interrupts  ：置 1 会使得系统忽略 IRQ 中断，清零则响应，SEI(Set Interrupt Disable) 指令将该位置 1，CLI(Clear Interrupt Disable)将该位清 0
            D = (1 << 3),	// Decimal Mode        ：该位用来将 6502 切换到 BCD 模式，但 NES 里面不用
            B = (1 << 4),	// Break               ：该位用来表示一个 BRK(Break) 指令在执行，该指令就是发出一个 IRQ 中断，类似 x86 下的 INT
            U = (1 << 5),	// Unused              
            V = (1 << 6),	// Overflow            ：溢出标志(一般对于有符号数来说)，如果运算有溢出，则置 1
            N = (1 << 7),	// Negative            ：该位就是运算结果的最高位
        };

        uint8_t a;      // Accumulator(A) 8-bit，累加器，用来存放运算结果或者从内存取回来的数据
        uint8_t x;      // Index Register X(X) 8-bit，索引寄存器X，用来作为循环的计数器或者特定寻址下的偏移量，也可以存放从内存取出来的数据，还能用来设置或者获取栈指针
        uint8_t y;      // Index Register Y(Y) 8-bit，索引寄存器Y，基本同 X，但是 Y 不能影响栈指针，就是不能用 Y 的值来设置栈指针
        uint8_t stkp;     // Stack Pointer(SP): 堆栈指针 
        uint16_t pc;    // Program Counter(PC): 程序计数器
        uint8_t status;      // Processor Status(P) 状态寄存器

    private: 
        // Assisstive variables to facilitate emulation
        uint8_t  fetched     = 0x00;   // Represents the working input value to the ALU
        uint16_t temp        = 0x0000; // A convenience variable used everywhere
        uint16_t addr_abs    = 0x0000; // All used memory addresses end up in here
        uint16_t addr_rel    = 0x00;   // Represents absolute address following a branch
        uint8_t  opcode      = 0x00;   // Is the instruction byte
        uint8_t  cycles      = 0;	   // Counts how many cycles the instruction has remaining
        uint32_t clock_count = 0;	   // A global accumulation of the number of clocks

        // The read location of data can come from two sources, a memory address, or
        // its immediately available as part of the instruction. This function decides
        // depending on address mode of instruction byte
        uint8_t fetch();

        //该结构体和下面的vector用于编译和存储
        //操作码转换表。6502可以有效地有256个
        //不同的指令。每一个都以数字形式存储在一个表中
        //顺序，以便可以轻松查找，不需要解码。
        //每个表项保存:
        // pneumonic:指令的文本表示(用于拆卸)
        // opcode Function:指向操作码实现的函数指针
        //操作码地址模式:函数指针的实现
        //指令使用的寻址机制
        //周期数:一个整数，表示时钟周期的基数
        // cpu需要执行该指令
        struct INSTRUCTION
        {
            std::string name;		
            uint8_t     (olc6502::*operate )(void) = nullptr;
            uint8_t     (olc6502::*addrmode)(void) = nullptr;
            uint8_t     cycles = 0;
        };

        std::vector<INSTRUCTION> lookup;
    private: 
        //寻址模式  =============================================
        // 6502具有多种寻址模式来访问数据
        //内存，有些是直接的，有些是间接的
        // c++中的指针)。每个操作码包含关于哪个操作码的信息
        //应该使用寻址模式来方便
        //指令，关于它读取/写入数据的位置
        //使用。地址模式改变报文的字节数
        //构成完整指令，因此我们实现寻址
        //在执行指令之前，确保程序
        //计数器在正确的位置，指令为
        //输入所需的地址和时钟数
        //计算指令所需的周期。这些函数
        //可以根据位置调整所需的周期数
        //以及如何访问内存，因此它们返回所需的
        //调整。

        uint8_t IMP();	uint8_t IMM();	
        uint8_t ZP0();	uint8_t ZPX();	
        uint8_t ZPY();	uint8_t REL();
        uint8_t ABS();	uint8_t ABX();	
        uint8_t ABY();	uint8_t IND();	
        uint8_t IZX();	uint8_t IZY();

    private: 
        //操作码  ======================================================
        // 6502 CPU提供56个“合法”操作码。我
        //没有模拟“非官方”操作码。就像每个操作码一样
        //由一个字节定义，可能有256种可能的代码。
        //代码不是在处理器上以“switch case”样式使用的，
        //相反，它们负责切换的单个部分
        // CPU电路的开关。这里列出的操作码是官方的，
        //表示芯片所提供的功能
        //这些代码是开发者想要的。非官方的
        //代码当然也会影响CPU电路
        //有趣的方式，并且可以被利用来获得额外的
        //功能!
        //

        //这些函数通常返回0，但有些函数可以
        //在某些情况下执行时需要更多的时钟周期
        //与特定寻址模式相结合的条件。如果是这样的话
        //在这种情况下，它们返回1。

        //我已经包含了每个函数的详细解释
        //类实现文件。请注意，它们列在
        //为方便查找，按字母顺序排列。

        uint8_t ADC();	uint8_t AND();	uint8_t ASL();	uint8_t BCC();
        uint8_t BCS();	uint8_t BEQ();	uint8_t BIT();	uint8_t BMI();
        uint8_t BNE();	uint8_t BPL();	uint8_t BRK();	uint8_t BVC();
        uint8_t BVS();	uint8_t CLC();	uint8_t CLD();	uint8_t CLI();
        uint8_t CLV();	uint8_t CMP();	uint8_t CPX();	uint8_t CPY();
        uint8_t DEC();	uint8_t DEX();	uint8_t DEY();	uint8_t EOR();
        uint8_t INC();	uint8_t INX();	uint8_t INY();	uint8_t JMP();
        uint8_t JSR();	uint8_t LDA();	uint8_t LDX();	uint8_t LDY();
        uint8_t LSR();	uint8_t NOP();	uint8_t ORA();	uint8_t PHA();
        uint8_t PHP();	uint8_t PLA();	uint8_t PLP();	uint8_t ROL();
        uint8_t ROR();	uint8_t RTI();	uint8_t RTS();	uint8_t SBC();
        uint8_t SEC();	uint8_t SED();	uint8_t SEI();	uint8_t STA();
        uint8_t STX();	uint8_t STY();	uint8_t TAX();	uint8_t TAY();
        uint8_t TSX();	uint8_t TXA();	uint8_t TXS();	uint8_t TYA();

        //我用这个函数捕获所有“非官方”操作码。它是
        //在功能上与NOP相同
        uint8_t XXX();

    private:
        class bus cpuBus;

    public:
        uint8_t GetFlag(FLAGS6502 f);
        void    SetFlag(FLAGS6502 f, bool v);

        uint8_t read(uint16_t addr);
        void write(uint16_t addr, uint8_t value);
        void pushStack(uint8_t value);
        uint8_t pullStack();
        
        // 外部触发的函数模拟，在实际硬件上为外部的一些引脚或者CPU内部的引脚来触发这些动作
        void reset();	// 复位中断，强制CPU恢复到已知状态
        void irq();		// 中断请求，停下当前的动作，从中断向量表中读取中断服务程序运行，运行完后再接续之前的动作。可被屏蔽
        void nmi();		// 不可屏蔽中断，像一些致命错误发生时，必须对此进行处理的中断动作，同中断请求类似，但是不可被屏蔽。
        void clock();	// 模拟一个时钟节拍，在硬件上由晶振提供一个稳定的时钟节拍，CPU据此进行指令的动作。

        

    public:

        olc6502();
        ~olc6502();
    };
}




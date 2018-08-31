/*
TW8836 MCU --DP80390XP compatible--

Cache 256 Bytes
XDATA   2KByte

                                CFFF+-----------+
                                    |  Chip     |
                                    |  Register |
    FF+---------------------+   0C00+-----------+
      | Internal |    SFR   |       |           |
      |   RAM    |          |       |  unused   |
    7F+----------+----------+   0800+-----------+
      |     Internal        |       |  Data     |
      |       RAM           |       |  Memory   |
    00+---------------------+   0000+-----------+
        IDATA        DATA


    00~07  Reg. Bank0
    08~0F  Reg. Bank1
    10~17  Reg. Bank2
    18~1F  Reg. Bank3
    20~27  Bit 00~3F
    28~2F  Bit 40~7F
    30~7F  General User RAM & Stack Space

Register Bank
    By default, register bank0 us used.
    Interrupt use register bank1,2,3.
    Tip.If Interrupt does not use, code can use these as data memory.
Bit memory
    Use SETB, CLR commands

SFR memory

TW8836B SFR  MAP------------
   -0-   -1-   -2-   -3-   -4-   -5-   -6-   -7-   
80:P0    SP    DPL0  DPH0  DPL1  DPH1  DPS   PCON  
88:TCON  TMOD  TL0   TL1   TH0   TH1   CKCON ..... 
90:P1    EXIF  _WTST _DPX0 ..... _DPX1 ..... ..... 
98:SCON0 SBUF0 BANK  ..... CACHE ACON  FIFO0 FIFO1 
A0:P2    ..... ..... ..... ..... ..... ..... ..... 
A8:IE    ..... ..... ..... ..... ..... ..... ..... 
B0:P3    ..... ..... ..... ..... ..... ..... ..... 
B8:IP    ..... ..... ..... ..... ..... ..... ..... 
C0:SCON1 SBUF1 CCL1  CCH1  CCL2  CCH2  CCL3  CCH3  
C8:T2CON T2IF  RLDL  RLDH  TL2   TH2   CCEN  ..... 
D0:PSW   ..... ..... ..... ..... ..... ..... ..... 
D8:WDCON ..... ..... ..... ..... ..... ..... ..... 
E0:ACC   ..... CAMC  ..... ..... ..... ..... ..... 
E8:EIE   STAT  _MXAX TA    ..... ..... ..... ..... 
F0:B     ..... ..... ..... ..... ..... ..... ..... 
F8:EIP   ..... E2IF  E2IE  E2IP  E2IM  E2IT  .....

StackPointer:
    It starts from 07h in startup.
    On start up code, it use below.
        IF PBPSTACK <> 0
        EXTRN DATA (?C_PBP)
                MOV     ?C_PBP,#LOW PBPSTACKTOP
        ENDIF
                MOV     SP,#?STACK-1
                LJMP    ?C_START
    After reset, the stack pointer is initialized to 07h. The stack will 
    start growing up from address 8h.
    The Keil C compiler uses internal DATA memory for your variables and also 
    allows you to use register banks 1, 2, and 3. If the stack pointer were not 
    adjusted, calls to functions and interrupts would overwrite your variables.
    Therefore, the last thing the startup code does is to set the stack pointer 
    to the end of all your internal DATA variables.

Interrupt
# bank name  enable prio   flag    Port func    description
-- -  ----   ----   ----   ----    ---- -----   -----------
0  1  EX0    IE.0   IP.0   IE0     P2.0 ext0_   chip interrupt. ->video ISR.
1  1  ET0    IE.1   IP.1   TF0          timer0_ time tick
2  1  EX1	 IE.2   IP.2   IE1     P2.1 ext1_   DE(data enable) end.
3  1  ET1	 IE.3   IP.3   TF1          timer1_ ->touch.
4  2  ES0    IE.4   IP.4   RI/TI        uart0_  uart0.
5  1  ET2	 IE.5   IP.5   T2IF         timer2_ ->remo sampling
6  1  ES1    IE.6   IP.6   RI1/TI1      uart1_
7  1  EINT2  EIE.0	EIP.0  EIF.0   P2.2	ext2_   DMA done.
8  1  EINT3  EIE.1	EIP.1  EIF.1   P2.3	ext3_   Touch_Ready interrupt.
9  1  EINT4  EIE.2	EIP.2  EIF.2  	    ext4_   xram 7FF access by I2C.
10 1  EINT5  EIE.3  EIP.3  EIF.3        ext5_   
11 1  EINT6  EIE.4	EIP.4  EIF.4   P2.4 ext6_   Touch_Pen interrupt.
12 3  EWDI	 EIE.5	EIP.5     	   WDIF watchdog_
13 1  EINT7  E2IE.0	E2IP.0 E2IF.0  P1.0 ext7_   Programable. P1.0 Pin60
14 1  EINT8  E2IE.1	E2IP.1 E2IF.1  P1.1 ext8_   Programable. P1.1 Pin61
15 1  EINT9  E2IE.2	E2IP.2 E2IF.2  P1.2 ext9_   Programable. P1.2 Pin62
16 1  EINT10 E2IE.3	E2IP.3 E2IF.3  P1.3 ext10_  Programable. P1.3 Pin63
17 1  EINT11 E2IE.4	E2IP.4 E2IF.4  P1.4 ext11_  Programable. P1.4 Pin87  ->remo activate
18 1  EINT12 E2IE.5	E2IP.5 E2IF.5  P1.5 ext12_  Programable. P1.5 Pin86
19 1  EINT13 E2IE.6	E2IP.6 E2IF.6  P1.6 ext13_  Programable. P1.6 Pin114
20 1  EINT14 E2IE.7	E2IP.7 E2IF.7  P1.7 ext14_  Programable. P1.7 Pin115

Interrupt Polling Sequence
EX0 ET0 EX1 ET1 ES ...
The "using" function attribute is used to select a register bank 

*/
#include "HWconf.h"

volatile BYTE	XDATA *DATA regTW88 = REG_START_ADDRESS;


//*****************************************************************************
//      UART 0 Interrupt                                                   
//*****************************************************************************

DATA 	BYTE	RS_buf[RS_BUF_MAX];
DATA 	BYTE	RS_in=0;
DATA	BYTE	RS_out=0;
bit		RS_Xbusy=0;


//=============================================================================
//														   
//=============================================================================
/**
* 1ms delay
*
* tic_pc will be increased every 1 timer0 interrupt that based 1ms.
* tic_pc: 0~0xffff and will not increased after 0xffff.
* @param cnt_1ms have to less then 65536. max 65sec delay
*
* tic_pc uses WORD size. It is not an automic. It needs SFRB_EA.
*/
DATA 	WORD   	tic_pc=0;
void delay1ms(WORD cnt_1ms)
{
	volatile WORD temp_tic_pc;
	SFRB_ET0 = 0;
	tic_pc = 0;								//clear
	SFRB_ET0 = 1;
	do {
		SFRB_ET0=0;
		temp_tic_pc = tic_pc;				//read
		SFRB_ET0=1;
	} while(temp_tic_pc < cnt_1ms);			//compare
}

//=============================================================================
//		Serial RX														   
//=============================================================================
/**
* get Rx data
*/
BYTE RS_rx(void)
{
	BYTE	ret;

	SFRB_ES = 0;

	ret = RS_buf[RS_out];
	RS_out++;
	if(RS_out >= RS_BUF_MAX) 
		RS_out = 0;

	SFRB_ES = 1;

	return ret;
}

/**
* check Serial RX
*
* @return
*	1:something to read.  0:empty.
*	
*/
BYTE RS_ready(void)
{
	if( RS_in == RS_out ) return 0;
	else return 1;
}
/**
* send Tx
*/
void RS_tx(BYTE tx_buf)
{
	while(RS_Xbusy);
	SFRB_ES = 0;
	SFR_SBUF = tx_buf;
	RS_Xbusy=1;
	SFRB_ES = 1;
}
BYTE InitCpu()
{
/*----------HW-start-----------*/
	//初始化UART-115200-8-1-non
	
	//初始化定时器 时心跳时钟
	
	//初始化IO口
	
/*----------HW-end-------------*/
	
/*----------Drv-start----------*/
	
/*----------Drv-end------------*/
	
/*----------App-start----------*/
/*----------App-end------------*/
	//	
	//	
	//	
	//	
	//	
	//	
	TWBASE = 0x00;					// Base address of TW88xx
	SFR_CAMC = 1;					// Chip Access Mode Control. E2[0]=1b:16bit mode
	
    WriteTW88(REG4E2, 0x69);		// Timer0 Divider : for systick
    WriteTW88(REG4E3, 0x78);		// 27M/27000 = 1ms	
		
    WriteTW88(REG4E4, 0x01);		// Timer1 Divider : 
    WriteTW88(REG4E5, 0x0E);		// 27M/270 = 10us	

		WriteTW88(REG084,ReadTW88(REG084)|0x02);	//GPIO 4x Enable     	- GPIO41 enable
		WriteTW88(REG08C,ReadTW88(REG09C)| 0x02);	//GPIO 4x direction		- GPIO41 output
		WriteTW88(REG094,ReadTW88(REG094)| 0x02);	//GPIO 4x output data	- GPIO41 outdata as 1.
	//---------- Initialize interrupt -------------
		SFR_CKCON = 0x00;		// Clock control register			
												// 0000 0000
												// |||| |||+-- MD0 \.
												// |||| ||+--- MD1 	> MD[2:0] Stretch RD/WR timing
												// |||| |+---- MD2 /
												// |||| +----- T0M:  Timer0 Pre-Divider 0=div by 12,  1=div by 4
												// |||+------- T1M:  Timer1 Pre-Divider 0=div by 12,  1=div by 4
												// ||+-------- ---
												// |+--------- WD0 \ Watchdong Timeout Period
												// +---------- WD1 / 00=2^17,  01=2^20,  10=2^23,  11=2^26

    SFR_TMOD = 0x66;		// 0110 0110
												// |||| ||||   << Timer 0 >>
												// |||| |||+-- M0 \  00= 8bit timer,counter /32  01= 16bit timer,counter
												// |||| ||+--- M1 /  10= 8bit auto reload        11= 8bit timer,counter
												// |||| |+---- CT:   0=Timer Mode,    1=Counter Mode
												// |||| +----- GATE: 0=GATE not used, 1=GATE used
												// ||||        << Timer 1 >>
												// |||+------- M0 \  00= 8bit timer,counter /32  01= 16bit timer,counter
												// ||+-------- M1 /  10= 8bit auto reload        11= 8bit timer,counter
												// |+--------- CT:   0=Timer Mode,    1=Counter Mode
												// +---------- GATE: 0=GATE not used, 1=GATE used

    SFR_TCON = 0x50;		// 0101 0101
												// |||| |||+-- IT0:  INT0 Trigger 0=level, 1=edge
												// |||| ||+--- IE0:  INT0 Interrupt Flag
												// |||| |+---- IT1:  INT1 Trigger 0=level, 1=edge
												// |||| +----- IE1:  INT1 Interrupt Flag
												// |||+------- TR0:  Timer0 Run 
												// ||+-------- TF0:  Timer0 Flag
												// |+--------- TR1:  Timer1 Run
												// +---------- TF0:  Timer1 Flag

 	SFR_TH0 = 0xF6;			// 10 * 1ms = 10ms once interrupt time0
	SFR_TL0 = 0xF6;			//
	
	SFR_TH1 = 0x9C;			// 100cycle * 10uS/cycle = 1ms once interrupt
	SFR_TL1 = 0x9C;

//	SFR_T2CON = 0x12;		// 0001 0010 
//							// |||| |||+-- T2I0 \ Timer2 Input Selection 
//							// ||||	||+--- T2I1 / 00=No,  01=Timer,  10=Counter, 11=Gate
//							// ||||	|+---- T2CM:  Compare mode
//							// ||||	+----- T2R0 \ Timer2 Reload Mode 
//							// |||+------- T2R1	/ 00=No,  01=No,     10=Auto,    11=pin T2EX
//							// ||+-------- ---
//							// |+--------- I3FR: Timer2 Compare0 Interrupt Edge...
//							// +---------- T2PS: Timer2 Prescaler
//    		
//	SFR_TH2 = 0xff;	        // 0xFF2E = 0x10000-0xD2 = 0x10000-210. 
//	SFR_TL2 = 0x2e;	        // it means 210 usec interval. 

	SFR_PCON = 0xc0;		// 1100 0000
											// |||| |||+-- PMM:  Power Management Mode 0=Disable,  1=Enable
											// |||| ||+--- STOP: Stop Mode             0=Disable,  1=Enable
											// |||| |+---- SWB:  Switch Back from STOP 0=Disable,  1=Enable
											// |||| +----- ---
											// |||+------- PWE:	 (Program write Enable)
											// ||+-------- ---
											// |+--------- SMOD1:UART1 Double baudrate bit
											// +---------- SMOD0:UART0 Double baudrate bit

	SFR_SCON = 0x50;		// 0101 0000
											// |||| |||+-- RI:   Receive Interrupt Flag
											// |||| ||+--- TI:   Transmit Interrupt Flag
											// |||| |+---- RB08: 9th RX data
											// |||| +----- TB08: 9th TX data
											// |||+------- REN:	 Enable Serial Reception
											// ||+-------- SMO2: Enable Multiprocessor communication
											// |+--------- SM01 \   Baudrate Mode
											// +---------- SM00 / 00=f/12,  01=8bit var,  10=9bit,f/32,f/64,  11=9bit var

	SFR_IE	 = 0x1A;		// 0001 1010 interrupt enable
											// |||| |||+ EX0    : Chip Interrupt. I will enable it later.
											// |||| ||+- ET0    : Timer0    System Tic
											// |||| |+-- EX1    : DE End
											// |||| +--- ET1    : timer1 - touch
											// |||+----- ES     : UART0
											// ||+------ ET2    : timer2 - remocon
											// |+------- ES1    : UART1
											// +-------- EA     : Global Interrupt. I will enable it below.
														
	SFR_UART0FIFO = 0x80;	//          : UART0 FIFO


//	InitCore();
	return OK;
}
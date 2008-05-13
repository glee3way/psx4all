/*  cpuctrl.c for GP2X (CPU/LCD/RAM-Tuner Version 2.0)
    Copyright (C) 2006 god_at_hell 
    original CPU-Overclocker (c) by Hermes/PS2Reality 
	the gamma-routine was provided by theoddbot
	parts (c) Rlyehs Work

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*/

#include "minimal.h"
#include <sys/mman.h>
#include "cpuctrl.h"


/* system registers */
static struct 
{
	unsigned short SYSCLKENREG,SYSCSETREG,FPLLVSETREG,DUALINT920,DUALINT940,DUALCTRL940;
}
system_reg;

static unsigned short dispclockdiv;

static volatile unsigned short *MEM_REG;

#define SYS_CLK_FREQ 7372800


void cpuctrl_init(void)
{
	extern volatile unsigned short *gp2x_memregs; /* from minimal library rlyeh */
	MEM_REG=&gp2x_memregs[0];
	system_reg.SYSCSETREG=MEM_REG[0x91c>>1];
	system_reg.FPLLVSETREG=MEM_REG[0x912>>1];
	system_reg.SYSCLKENREG=MEM_REG[0x904>>1];
	system_reg.DUALINT920=MEM_REG[0x3B40>>1];
	system_reg.DUALINT940=MEM_REG[0x3B42>>1];
	system_reg.DUALCTRL940=MEM_REG[0x3B48>>1];
	dispclockdiv=MEM_REG[0x924>>1];
}


void cpuctrl_deinit(void)
{
	MEM_REG[0x91c>>1]=system_reg.SYSCSETREG;
	MEM_REG[0x910>>1]=system_reg.FPLLVSETREG;
	MEM_REG[0x3B40>>1]=system_reg.DUALINT920;
	MEM_REG[0x3B42>>1]=system_reg.DUALINT940;
	MEM_REG[0x3B48>>1]=system_reg.DUALCTRL940;
	MEM_REG[0x904>>1]=system_reg.SYSCLKENREG;
	MEM_REG[0x924>>1]=dispclockdiv;
}


void set_display_clock_div(unsigned div)
{
	div=((div & 63) | 64)<<8;
	MEM_REG[0x924>>1]=(MEM_REG[0x924>>1] & ~(255<<8)) | div;
}


void set_FCLK(unsigned MHZ)
{
	unsigned v;
	unsigned mdiv,pdiv=3,scale=0;
	MHZ*=1000000;
	mdiv=(MHZ*pdiv)/SYS_CLK_FREQ;
	mdiv=((mdiv-8)<<8) & 0xff00;
	pdiv=((pdiv-2)<<2) & 0xfc;
	scale&=3;
	v=mdiv | pdiv | scale;
	MEM_REG[0x910>>1]=v;
}

unsigned short get_920_Div()
{
	return (MEM_REG[0x91c>>1] & 0x7); 
}

void set_920_Div(unsigned short div)
{
	unsigned short v;
	v = MEM_REG[0x91c>>1] & (~0x3);
	MEM_REG[0x91c>>1] = (div & 0x7) | v; 
}


void set_DCLK_Div( unsigned short div )
{
	unsigned short v;
	v = (unsigned short)( MEM_REG[0x91c>>1] & (~(0x7 << 6)) );
	MEM_REG[0x91c>>1] = ((div & 0x7) << 6) | v; 
}

void set_940_Div(unsigned short div)
{	
	unsigned short v;
	v = (unsigned short)( MEM_REG[0x91c>>1] & (~(0x7 << 3)));
	MEM_REG[0x91c>>1] = ((div & 0x7) << 3) | v; 
}

void Disable_940(void)
{
	MEM_REG[0x3B42>>1];
	MEM_REG[0x3B42>>1]=0;
	MEM_REG[0x3B46>>1]=0xffff;	
	MEM_REG[0x3B48>>1]|= (1 << 7);
	MEM_REG[0x904>>1]&=0xfffe;
}

void gp2x_video_wait_vsync(void)
{
	/*
	MEM_REG[0x2846>>1]=(MEM_REG[0x2846>>1] | 0x20) & ~2;
	while(!(MEM_REG[0x2846>>1] & 2));
	*/
	while(MEM_REG[0x1182>>1]&(1<<4));
	while(!(MEM_REG[0x1182>>1]&(1<<4)));
}

void set_tRC(unsigned short timing)
{
	//printf ("set tRC = %u\r\n",timing+1);
	unsigned short v;
	v = (unsigned short)(MEM_REG[0x3804>>1] & (~(0xF << 8)));
	MEM_REG[0x3804>>1] = ((timing & 0xF) << 8) | v;	
}

void set_tRAS(unsigned short timing)
{
	//printf ("set tRAS = %u\r\n",timing+1);
	unsigned short v;
	v = (unsigned short)(MEM_REG[0x3804>>1] & (~(0xF << 4)));
	MEM_REG[0x3804>>1] = ((timing & 0xF) << 4) | v;	
}

void set_tWR(unsigned short timing)
{	
	//printf ("set tWR = %u\r\n",timing+1);
	unsigned short v;
	v = (unsigned short)(MEM_REG[0x3804>>1] & (~(0xF)));
	MEM_REG[0x3804>>1] = (timing & 0xF) | v;	
}

void set_tMRD(unsigned short timing)
{
	//printf ("set tMRD = %u\r\n",timing+1);
	unsigned short v;
	v = (unsigned short)(MEM_REG[0x3802>>1] & (~(0xF << 12)));
	MEM_REG[0x3802>>1] = ((timing & 0xF) << 12) | v;	
}

void set_tRFC(unsigned short timing)
{
	//printf ("set tRFC = %u\r\n",timing+1);
	unsigned short v;
	v = (unsigned short)(MEM_REG[0x3802>>1] & (~(0xF << 8)));
	MEM_REG[0x3802>>1] = ((timing & 0xF) << 8) | v;	
}

void set_tRP(unsigned short timing)
{
	//printf ("set tRP = %u\r\n",timing+1);
	unsigned short v;
	v = (unsigned short)(MEM_REG[0x3802>>1] & (~(0xF << 4)));
	MEM_REG[0x3802>>1] = ((timing & 0xF) << 4) | v;	
}

void set_tRCD(unsigned short timing)
{
	//printf ("set tRCD = %u\r\n",timing+1);
	unsigned short v;
	v = (unsigned short)(MEM_REG[0x3802>>1] & (~(0xF)));
	MEM_REG[0x3802>>1] = (timing & 0xF) | v;	
}

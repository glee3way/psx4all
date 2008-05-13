/*  Pcsx - Pc Psx Emulator
 *  Copyright (C) 1999-2003  Pcsx Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

void psxBiosException();
void psxBiosInit();
void psxBiosShutdown();

R3000Acpu *psxCpu;

#ifndef GP2X_MAPREGS
  u32 reg_code;
  u32 reg_pc;
#endif

int psxInit() {
	psxCpu = &psxInt;
#ifdef DYNAREC
	if (!Config.Cpu) psxCpu = &psxRec;
#endif
	Log=0;

	if (psxMemInit() == -1) return -1;

	return psxCpu->Init();
}

void psxReset() {

	psxCpu->Reset();

	psxMemReset();

	memset(psxRegs, 0, sizeof(psxRegisters));

	psxRegs->pc = 0xbfc00000; // Start in bootstrap

	psxRegs->CP0.r[12] = 0x10900000; // COP0 enabled | BEV = 1 | TS = 1
	psxRegs->CP0.r[15] = 0x00000002; // PRevID = Revision ID, same as R3000A

	psxHwReset();
	psxBiosInit();
	if (!Config.HLE) psxExecuteBios();

#ifdef EMU_LOG
//	EMU_LOG("*BIOS END*\n");
#endif
	Log=0;
}

void psxShutdown() {
	psxMemShutdown();
	psxBiosShutdown();
	psxCpu->Shutdown();
}

u32 psxException(u32 code, u32 bd) {
	// Set the Cause
	psxRegs->CP0.n.Cause = code;

	// Set the EPC & PC
	if (bd) {
#ifdef PSXCPU_LOG
		PSXCPU_LOG("bd set!!!\n");
#endif
		SysPrintf("bd set!!!\n");
		psxRegs->CP0.n.Cause|= 0x80000000;
		psxRegs->CP0.n.EPC = (psxRegs->pc - 4);

	} else {
		psxRegs->CP0.n.EPC = (psxRegs->pc);
	}

	if (psxRegs->CP0.n.Status & 0x400000)
		psxRegs->pc = 0xbfc00180;
	else
		psxRegs->pc = 0x80000080;

	// Set the Status
	psxRegs->CP0.n.Status = (psxRegs->CP0.n.Status &~0x3f) |
						  ((psxRegs->CP0.n.Status & 0xf) << 2);

	if ( !Config.HLE && (((PSXMu32(psxRegs->CP0.n.EPC) >> 24) & 0xfe) == 0x4a)) {
		// "hokuto no ken" / "Crash Bandicot 2" ... fix
		PSXMu32ref(psxRegs->CP0.n.EPC)&= SWAPu32(~0x02000000);
	}

	if (Config.HLE) psxBiosException();

	return psxRegs->pc;
}

//#define ENABLE_INTERRUPT_LOG_SUPPORT
bool  activeINTLog = false;

#ifdef  ENABLE_INTERRUPT_LOG_SUPPORT
  extern bool  activeINTLog;
  #define DO_INT_LOG(expr)   do { if(activeINTLog) SysPrintf expr; } while(false)
#else
//  #define DO_INT_LOG(expr)   do { /*if(activeINTLog)*/ SysPrintf expr; } while(false)

    #define DO_INT_LOG(expr)   do {} while(false)
#endif

INLINE  void psxTestHWInts() {
 	if (psxHu32(0x1070) & psxHu32(0x1074)) {
		if ((psxRegs->CP0.n.Status & 0x401) == 0x401) {

#ifdef PSXCPU_LOG
//			PSXCPU_LOG("Interrupt: %x %x\n", psxHu32(0x1070), psxHu32(0x1074));
#endif
//			SysPrintf("Interrupt (%x): %x %x\n", psxRegs.cycle, psxHu32(0x1070), psxHu32(0x1074));
			psxException(0x400, 0);
		}
 	}
}

void psxExecuteBios() 
{
	while (psxRegs->pc != 0x80030000)
		psxCpu->ExecuteBlock();
}
#define	spu_reg0	(psxHu16(0x1daa))
#define	t0_count	(psxHu16(0x1100))
#define	t0_mode		(psxHu32(0x1104))
#define	t0_target	(psxHu16(0x1108))
#define	t1_count	(psxHu16(0x1110))
#define	t1_mode		(psxHu32(0x1114))
#define	t1_target	(psxHu16(0x1118))
#define	t2_count	(psxHu16(0x1120))
#define	t2_mode		(psxHu32(0x1124))
#define	t2_target	(psxHu16(0x1128))
#define	int_reg		(psxHu16(0x1070))
#define	int_mask	(psxHu16(0x1074))
#define	dma_pcr		(psxHu32(0x10f0))
#define	dma_icr		(psxHu32(0x10f4))

#define UPPER   0xFFFF

#define	PER	cycles

void update_hw(u32 cycles)
{
	int tmp;

	psxCounters.base_count+=PER;

	if (psxCounters.base_count>=RCNT_VSYNC) 
	{
		psxCounters.base_count=0;

		GPU_vSinc();
#ifndef IPHONE
		SPU_async(Config.PsxType);
#endif
		psxHu32ref(0x1070)|= SWAPu32(1);
		psxRegs->interrupt|= 0x80000000;
	}

	tmp = t0_count + ((t0_mode & 0x100)?PER:PER/8);
	if (tmp>=psxCounters.t0_limit && t0_count<psxCounters.t0_limit)
	{
		tmp = 0;
		// printf("t0 == limit\n");
		if (t0_mode & 0x50) 
		{
			psxHu32ref(0x1070)|= SWAPu32(0x10);
			psxRegs->interrupt|= 0x80000000;
  	}
  }
  t0_count=tmp;

	if ((t1_mode & 0x100)==0) 
	{
		tmp = t1_count + PER;

    if (tmp>=psxCounters.t1_limit && t1_count<psxCounters.t1_limit) 
    {
		  tmp = 0;
			// printf("t1 == limit\n");
      if (t1_mode & 0x50) 
      {
				psxHu32ref(0x1070)|= SWAPu32(0x20);
				psxRegs->interrupt|= 0x80000000;
      }
    }
    t1_count=tmp;
  } 
  else
  {
    if ((psxCounters.dec1_count-=PER) <= 0) 
    {
      if (++t1_count == psxCounters.t1_limit) 
      {
				t1_count = 0;
				// printf("t1 == limit\n");
				if (t1_mode & 0x50)
				{
					psxHu32ref(0x1070)|= SWAPu32(0x20);
					psxRegs->interrupt|= 0x80000000;
				}
      }
      psxCounters.dec1_count = RCNT_HSYNC;
  	}
	}
	
  if ((t2_mode & 1) == 0)
  {
    tmp = t2_count + ((t2_mode & 0x200)?PER/8:PER);
    if (tmp>=psxCounters.t2_limit && t2_count<psxCounters.t2_limit) 
    {
      tmp = 0;
			// printf("t2 == limit\n");
      if (t2_mode & 0x50) 
      {
				psxHu32ref(0x1070)|= SWAPu32(0x40);
				psxRegs->interrupt|= 0x80000000;
      }
    }
    t2_count=tmp;
  }

	//if(psxRegs->interrupt)
	{
		/* Make sure interrupts  always when mcd is active */
		if( mcdst != 0 )
		{
			psxRegs->interrupt&=~0x80;
			sioInterrupt();
		}
	
		// cdr
		if (psxRegs->interrupt & 0x04)
		{
			if ((psxRegs->cycle - psxRegs->intCycle[2]) >= psxRegs->intCycle[2+1])
		  {
				psxRegs->interrupt&=~0x04;
				cdrInterrupt();
		    DO_INT_LOG(("  CDRInt\n"));
			}
		}

		// cdr read
		if (psxRegs->interrupt & 0x040000)
		{
			if ((psxRegs->cycle - psxRegs->intCycle[2+16]) >= psxRegs->intCycle[2+16+1])
			{
				psxRegs->interrupt&=~0x040000;
				cdrReadInterrupt();
				      DO_INT_LOG(("  CDRReadInt\n"));
			}
		}

		// sio
		if ((psxRegs->interrupt & 0x80) && (!Config.Sio)) 
		{
			if ((psxRegs->cycle - psxRegs->intCycle[7]) >= psxRegs->intCycle[7+1])
			{
				psxRegs->interrupt&=~0x80;
				sioInterrupt();
				DO_INT_LOG(("  SIOInt\n"));
			}
		}

		// gpu dma ???
		if (psxRegs->interrupt & 0x01000000)
		{
			if ((psxRegs->cycle - psxRegs->intCycle[3+24]) >= psxRegs->intCycle[3+24+1])
			{
				psxRegs->interrupt&=~0x01000000;
				gpuDMAInterrupt();
				DO_INT_LOG(("  GPUInt\n"));
			}		}
		// mdec out dma
		if (psxRegs->interrupt & 0x02000000)
		{
			if ((psxRegs->cycle - psxRegs->intCycle[5+24]) >= psxRegs->intCycle[5+24+1])
		  {
				psxRegs->interrupt&=~0x02000000;
				mdec1DMAInterrupt();
				DO_INT_LOG(("  MDEC1Int\n"));
			}
		}

		// HWINTS
		if (psxRegs->interrupt & 0x80000000)
		{
			psxRegs->interrupt&=~0x80000000;
			psxTestHWInts();
			DO_INT_LOG(("  HWInt     -> %d\n",psxRegs->cycle));
		}
	}

#ifdef IPHONE
	if( !__emulation_run ) 
	{
		char buffer[260];
		char filename[260];
		time_t curtime;
		struct tm *loctime;

		curtime = time (NULL);
		loctime = localtime (&curtime);
		strftime (buffer, 260, "%y%m%d-%I%M%p", loctime);
		sprintf(filename, "%s-%s.svs", iphonefile, buffer);

		GPU_freeze(2, NULL);
		SaveState(filename);
		app_SetSvsFile(filename);

		psx4all_prof_show();
		psxShutdown();
		CloseComponents();
		
		gp2x_deinit();
		pthread_exit(NULL);
	}
#endif
}


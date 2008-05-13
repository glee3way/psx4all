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
#include <string.h>
#include "common.h"

#ifdef _MSC_VER_
#pragma warning(disable:4244)
#endif

void psxHwReset() {
	memset(psxH, 0, 0x10000);
	if (Config.Sio) psxHu32(0x1070) |= 0x80;
	if (Config.SpuIrq) psxHu32(0x1070) |= 0x200;
	mdecInit();
	cdrReset();
	psxRcntInit();
}

u8 psxHwRead8(u32 add) {
	switch (add) {
		case 0x1f801040: return sioRead8();
		case 0x1f801800: return cdrRead0();
		case 0x1f801801: return cdrRead1();
		case 0x1f801802: return cdrRead2();
		case 0x1f801803: return cdrRead3();
		default: return psxHu8(add); 
	}
}

u16 psxHwRead16(u32 add) {
	switch (add) {
		case 0x1f801040: return ( ((u16)sioRead8()) | (((u16)sioRead8())<<8) );
		case 0x1f801044: return StatReg;
		case 0x1f801048: return ModeReg;
		case 0x1f80104a: return CtrlReg;
		case 0x1f80104e: return BaudReg;
		default:
			if (add>=0x1f801c00 && add<0x1f801e00)
            			return SPU_readRegister(add);
			else
				return psxHu16(add); 
	}
}

u32 psxHwRead32(u32 add) {
	switch (add) {
		case 0x1f801040: return ( ((u32)sioRead8()) | (((u32)sioRead8())<<8) | (((u32)sioRead8())<<16) | (((u32)sioRead8())<<24) );
		case 0x1f801810: return GPU_readData();
		case 0x1f801814: return GPU_readStatus();
		case 0x1f801820: return mdecRead0();
		case 0x1f801824: return mdecRead1();
		default: return psxHu32(add); 
	}
}

void psxHwWrite8(u32 add, u8 value) {
	switch (add) {
		case 0x1f801040: sioWrite8(value); break;
		case 0x1f801800: cdrWrite0(value); break;
		case 0x1f801801: cdrWrite1(value); break;
		case 0x1f801802: cdrWrite2(value); break;
		case 0x1f801803: cdrWrite3(value); break;
	}
	psxHu8(add) = value;
}

void psxHwWrite16(u32 add, u16 value) {
	switch (add) {
		case 0x1f801040: sioWrite8((unsigned char)value); sioWrite8((unsigned char)(value>>8)); return;
		case 0x1f801044: return;
		case 0x1f801048: ModeReg = value; return;
		case 0x1f80104a: sioWriteCtrl16(value); return;
		case 0x1f80104e: BaudReg = value; return;
		case 0x1f801070: 
			if (Config.Sio) psxHu16(0x1070) |= SWAPu16(0x80);
			if (Config.SpuIrq) psxHu16(0x1070) |= SWAPu16(0x200);
			psxHu16(0x1070) &= SWAPu16((psxHu16(0x1074) & value));
			return;
		case 0x1f801074:
			psxHu16(0x1074) = value;
			psxRegs->interrupt|= 0x80000000;
			return;
		case 0x1f801104:
			psxHu16ref(add) = SWAPu16(value);
      if ((value & 0x08)==0 || !t0_target) psxCounters.t0_limit = 0xFFFF;
      else psxCounters.t0_limit = t0_target;
      return;
		case 0x1f801108:
      psxHu16ref(add) = SWAPu16(value);
      if ((t0_mode & 0x08)==0 || !t0_target) psxCounters.t0_limit = 0xFFFF;
      else psxCounters.t0_limit = t0_target;
			return;
		case 0x1f801114:
			psxHu16ref(add) = SWAPu16(value);
      if ((value & 0x08)==0 || !t1_target) psxCounters.t1_limit = 0xFFFF;
      else psxCounters.t1_limit = t1_target;
      return;
		case 0x1f801118:
      psxHu16ref(add) = SWAPu16(value);
      if ((t1_mode & 0x08)==0 || !t1_target) psxCounters.t1_limit = 0xFFFF;
      else psxCounters.t1_limit = t1_target;
			return;
		case 0x1f801124:
			psxHu16ref(add) = SWAPu16(value);
      if ((value & 0x08)==0 || !t2_target) psxCounters.t2_limit = 0xFFFF;
      else psxCounters.t2_limit = t2_target;
      return;
		case 0x1f801128:
      psxHu16ref(add) = SWAPu16(value);
      if ((t2_mode & 0x08)==0 || !t2_target) psxCounters.t2_limit = 0xFFFF;
      else psxCounters.t2_limit = t2_target;
			return;
		default:
			if (add>=0x1f801c00 && add<0x1f801e00) {
            			SPU_writeRegister(add, value);
				return;
			} else {
				psxHu16ref(add) = SWAPu16(value);
				return;
			}
	}
}

#define DmaExec(n) {																		\
	if (SWAPu32(HW_DMA##n##_CHCR) & 0x01000000) return;										\
	HW_DMA##n##_CHCR = SWAPu32(value);														\
																							\
	if (SWAPu32(HW_DMA##n##_CHCR) & 0x01000000 && SWAPu32(HW_DMA_PCR) & (8 << (n * 4))) {	\
		psxDma##n(HW_DMA##n##_MADR, HW_DMA##n##_BCR, HW_DMA##n##_CHCR);						\
	}																						\
}

void psxHwWrite32(u32 add, u32 value) {
	switch (add) {
		case 0x1f8010a8: DmaExec(2); return; // DMA2 chcr (GPU DMA)
	    case 0x1f801040:
			sioWrite8((unsigned char)value);
			sioWrite8((unsigned char)((value&0xff) >>  8));
			sioWrite8((unsigned char)((value&0xff) >> 16));
			sioWrite8((unsigned char)((value&0xff) >> 24));
			return;
		case 0x1f801070: 
			if (Config.Sio) psxHu32ref(0x1070) |= SWAPu32(0x80);
			if (Config.SpuIrq) psxHu32ref(0x1070) |= SWAPu32(0x200);
			psxHu32ref(0x1070) &= SWAPu32((psxHu32(0x1074) & value));
			return;
		case 0x1f801074:
			psxHu32ref(0x1074) = SWAPu32(value);
			psxRegs->interrupt|= 0x80000000;
			return;
		case 0x1f801088: DmaExec(0); return; // DMA0 chcr (MDEC in DMA)
		case 0x1f801098: DmaExec(1); return; // DMA1 chcr (MDEC out DMA)
		case 0x1f8010b8: DmaExec(3); return; // DMA3 chcr (CDROM DMA)
		case 0x1f8010c8: DmaExec(4); return; // DMA4 chcr (SPU DMA)
		case 0x1f8010e8: DmaExec(6); return; // DMA6 chcr (OT clear)
		case 0x1f8010f4: add = (~value) & SWAPu32(HW_DMA_ICR); HW_DMA_ICR = SWAPu32(((add ^ value) & 0xffffff) ^ add); return;
		case 0x1f801810: GPU_writeData(value); return;
		case 0x1f801814: GPU_writeStatus(value); return;
		case 0x1f801820: mdecWrite0(value); break;
		case 0x1f801824: mdecWrite1(value); break;
		case 0x1f801100: break;
		case 0x1f801104: 
			psxHu32ref(add) = SWAPu32(value);
			if (value & 0x08) psxCounters.t0_limit = t0_target;
    	else psxCounters.t0_limit = 0xFFFF;
    	return;
		case 0x1f801108: 
			psxHu32ref(add) = SWAPu32(value);
			if (t0_mode & 0x08) psxCounters.t0_limit = t0_target;
    	else psxCounters.t0_limit = 0xFFFF;
    	return;
		case 0x1f801110: break;
		case 0x1f801114: 
			psxHu32ref(add) = SWAPu32(value);
			if (value & 0x08) psxCounters.t1_limit = t1_target;
    	else psxCounters.t1_limit = 0xFFFF;
    	return;
		case 0x1f801118:
			psxHu32ref(add) = SWAPu32(value);
			if (t1_mode & 0x08) psxCounters.t1_limit = t1_target;
    	else psxCounters.t1_limit = 0xFFFF;
    	return;
		case 0x1f801120: break;
		case 0x1f801124: 
			psxHu32ref(add) = SWAPu32(value);
			if (value & 0x08) psxCounters.t2_limit = t2_target;
    	else psxCounters.t2_limit = 0xFFFF;
    	return;
		case 0x1f801128: 
			psxHu32ref(add) = SWAPu32(value);
			if (t2_mode & 0x08) psxCounters.t2_limit = t2_target;
    	else psxCounters.t2_limit = 0xFFFF;
    	return;
	}
	psxHu32ref(add) = SWAPu32(value);
}

int psxHwFreeze(gzFile f, int Mode) {
	char Unused[4096];
	gzfreezel(Unused);
	return 0;
}

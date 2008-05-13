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

#include <string.h>
#include <stdlib.h>

#include "common.h"

u32 *psxMemWLUT = NULL;
u32 *psxMemRLUT = NULL;
s8 *psxM = NULL;
s8 *psxP = NULL;
s8 *psxR = NULL;
s8 *psxH = NULL;
s8 *psxNULLread=NULL;

int psxMemInit() {
	int i;

	psxMemRLUT = (unsigned long*)malloc(0x10000 * 4);
	psxMemWLUT = (unsigned long*)malloc(0x10000 * 4);
	memset(psxMemRLUT, 0, 0x10000 * 4);
	memset(psxMemWLUT, 0, 0x10000 * 4);

	psxM = (char*)malloc(0x00200000);
	psxP = (char*)malloc(0x00010000);
	psxH = (char*)malloc(0x00010000);
	psxR = (char*)malloc(0x00080000);
	
	psxNULLread=(char*)malloc(0x00010000);
	memset(psxNULLread, 0, 0x10000);
	
	if (psxMemRLUT == NULL || psxMemWLUT == NULL || 
		psxM == NULL || psxP == NULL || psxH == NULL) {
		SysPrintf("Error allocating memory"); return -1;
	}

// MemR
	for (i=0; i<0x10000; i++) psxMemRLUT[i]=(u32)&psxNULLread;
	for (i=0; i<0x80; i++) psxMemRLUT[i + 0x0000] = (u32)&psxM[(i & 0x1f) << 16];
	memcpy(psxMemRLUT + 0x8000, psxMemRLUT, 0x80 * 4);
	memcpy(psxMemRLUT + 0xa000, psxMemRLUT, 0x80 * 4);

	psxMemRLUT[0x1f00] = (u32)psxP;
	psxMemRLUT[0x1f80] = (u32)psxH;

	for (i=0; i<0x08; i++) psxMemRLUT[i + 0xbfc0] = (u32)&psxR[i << 16];

// MemW
	for (i=0; i<0x80; i++) psxMemWLUT[i + 0x0000] = (u32)&psxM[(i & 0x1f) << 16];
	memcpy(psxMemWLUT + 0x8000, psxMemWLUT, 0x80 * 4);
	memcpy(psxMemWLUT + 0xa000, psxMemWLUT, 0x80 * 4);

	psxMemWLUT[0x1f00] = (u32)psxP;
	psxMemWLUT[0x1f80] = (u32)psxH;

	return 0;
}

void psxMemReset() {
	FILE *f = NULL;
	char Bios[256];

	memset(psxM, 0, 0x00200000);
	memset(psxP, 0, 0x00010000);

	if(!Config.HLE)
	{
		sprintf(Bios, "%s%s", Config.BiosDir, Config.Bios);
		f = fopen(Bios, "rb");

		if (f == NULL) {
			SysPrintf ("BIOS NOT FOUND: %s", Bios);
			gp2x_timer_delay(2000);
			memset(psxR, 0, 0x80000);
			Config.HLE = 1;
		}
		else {
			fread(psxR, 1, 0x80000, f);
			fclose(f);
		}
	}
	else
	{
		memset(psxR, 0, 0x80000);
	}
}

void psxMemShutdown() {
	if( psxNULLread != NULL ) free(psxNULLread);
	if( psxM != NULL ) free(psxM);
	if( psxP != NULL ) free(psxP);
	if( psxH != NULL ) free(psxH);
	if( psxR != NULL ) free(psxR);
	if( psxMemRLUT != NULL ) free(psxMemRLUT);
	if( psxMemWLUT != NULL ) free(psxMemWLUT);
}

void psxMemWrite32_error(u32 mem, u32 value)
{
	static int mem_writeok=1;
	if (mem==0xfffe0130)
	{
		switch (value) {
			case 0x800:
			case 0x804:
				if (mem_writeok == 0) break;
				mem_writeok = 0;
				memset(psxMemWLUT + 0x0000, 0, 0x80 * 4);
				memset(psxMemWLUT + 0x8000, 0, 0x80 * 4);
				memset(psxMemWLUT + 0xa000, 0, 0x80 * 4);
				break;
			case 0x1e988:
				if (mem_writeok == 1) break;
				mem_writeok = 1;
				{ int i; for (i=0; i<0x80; i++) psxMemWLUT[i + 0x0000] = (u32)&psxM[(i & 0x1f) << 16]; }
				memcpy(psxMemWLUT + 0x8000, psxMemWLUT, 0x80 * 4);
				memcpy(psxMemWLUT + 0xa000, psxMemWLUT, 0x80 * 4);
				break;
		}
	}
#ifdef DYNAREC
	else if (!mem_writeok) REC_CLEARM(mem);
#endif	
}


s8 psxMemReadS8(u32 mem) {
	u32 t=mem>>16;
	u32 m=mem&0xFFFF;
	if (t!=0x1f80)
		return (*(((s8*)psxMemRLUT[t])+m));
	else if (m<0x1000)
		return (*(((s8*)psxH)+m));
	else
		return (s8)psxHwRead8(mem);
}

u8 psxMemRead8(u32 mem) {
	u32 t=mem>>16;
	u32 m=mem&0xFFFF;
	if (t!=0x1f80)
		return (*(((u8*)psxMemRLUT[t])+m));
	else if (m<0x1000)
		return (*(((u8*)psxH)+m));
	else
		return (u8)psxHwRead8(mem);
}

s16 psxMemReadS16(u32 mem) {
	u32 t=mem>>16;
	u32 m=mem&0xFFFF;
	if (t!=0x1f80)
		return SWAPu16(*((s16*)(((s8*)psxMemRLUT[t])+m)));
	else if (m<0x1000)
		return (s16)(SWAP16(*((u16*)(((u8*)psxH)+m))));
	else
		return (s16)psxHwRead16(mem);
}

u16 psxMemRead16(u32 mem) {
	u32 t=mem>>16;
	u32 m=mem&0xFFFF;
	if (t!=0x1f80)
		return SWAPu16(*((u16*)(((u8*)psxMemRLUT[t])+m)));
	else if (m<0x1000)
		return SWAP16(*((u16*)(((u8*)psxH)+m)));
	else
		return (u16)psxHwRead16(mem);
}

u32 psxMemRead32(u32 mem) {
	u32 t=mem>>16;
	u32 m=mem&0xFFFF;
	if (t!=0x1f80)
		return SWAPu32(*((u32*)(((u8*)psxMemRLUT[t])+m)));
	else if (m<0x1000)
		return SWAP32(*((u32*)(((u8*)psxH)+m)));
	else
		return (u32)psxHwRead32(mem);
}

void psxMemWrite8(u32 mem, u32 value) {
	u32 t=mem>>16;
	u32 m=mem&0xFFFF;
	if (t!=0x1f80)
	{
		u8 *p=(u8 *)(psxMemWLUT[t]);
		if (p) {
			*(p+m)=(u8)value;
#ifdef DYNAREC
			REC_CLEARM(mem&(~3));
#endif
		}
	}
	else if (m<0x1000)
		*(((u8*)psxH)+m)=(u8)value;
	else
		psxHwWrite8(mem, value);
}


void psxMemWrite16(u32 mem, u32 value) {
	u32 t=mem>>16;
	u32 m=mem&0xFFFF;
	if (t!=0x1f80)
	{
		u8 *p=(u8 *)(psxMemWLUT[t]);
		if (p) {
			*((u16*)(p+m))=SWAPu16(value);
#ifdef DYNAREC
			REC_CLEARM(mem&(~3));
#endif
		}
	}
	else if (m<0x1000)
		*((u16*)(((u8*)psxH)+m))=(u16)SWAPu16(value);
	else
		psxHwWrite16(mem, value);
}

void psxMemWrite32(u32 mem, u32 value) {
	u32 t=mem>>16;
	u32 m=mem&0xFFFF;
	if (t!=0x1f80)
	{
		u8 *p=(u8 *)(psxMemWLUT[t]);
		if (p) {
			*((u32*)(p+m))=SWAPu32(value);
#ifdef DYNAREC
			REC_CLEARM(mem&(~3));
#endif
		} else
			psxMemWrite32_error(mem,value);
	}
	else if (m<0x1000)
		*((u32*)(((u8*)psxH)+m))=(u32)SWAPu32(value);
	else
		psxHwWrite32(mem, value);
}

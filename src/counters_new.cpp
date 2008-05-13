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

#include "common.h"

u32 	PsxClockMult 	= uDefaultPsxMult;               //  Por defecto uDefaultPsxMult;
u32 	PsxClock     	= uPsxClockBase*uDefaultPsxMult; //  Por defecto uPsxClockBase*uDefaultPsxMult;
u32 	PsxPALClock  	= PsxClock/50;                   //  Por defecto PsxClock/50;
u32 	PsxNTSCClock 	= PsxClock/60;                   //  Por defecto PsxClock/60;
u32 	PsxCDClock   	= PsxClock/75;                   //  Por defecto PsxClock/75;
float 	PsxCycleMult 	= 2.0;


psxCounter psxCounters;

void psxRcntInit() {

	memset(&psxCounters, 0, sizeof(psxCounter));

	psxCounters.t0_limit=0xFFFF;
	psxCounters.t1_limit=0xFFFF;
	psxCounters.t2_limit=0xFFFF;
	psxCounters.dec1_count=RCNT_HSYNC;
}

int psxRcntFreeze(gzFile f, int Mode) {
	char Unused[4096 - sizeof(psxCounter)];

	gzfreezel(&psxCounters);
	gzfreezel(Unused);

	return 0;
}


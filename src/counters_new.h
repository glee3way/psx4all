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

#ifndef __PSXCOUNTERS_H__
#define __PSXCOUNTERS_H__

typedef struct {
	u32 t0_limit;
	u32 t1_limit;
	u32 t2_limit;
	u32 base_count;
	int dec1_count;
} psxCounter;

extern psxCounter psxCounters;

extern float PsxCycleMult;

void psxRcntInit();
int psxRcntFreeze(gzFile f, int Mode);

#define RCNT_HSYNC   (PsxClock/15734)
#define RCNT_VSYNC   (Config.PsxType == 0 ? PsxNTSCClock : PsxPALClock)

#endif /* __PSXCOUNTERS_H__ */


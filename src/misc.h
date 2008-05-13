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

#ifndef __MISC_H__
#define __MISC_H__

#undef s_addr

typedef struct {
	unsigned char id[8];
    unsigned long text;                   
    unsigned long data;                    
    unsigned long pc0;
    unsigned long gp0;                     
    unsigned long t_addr;
    unsigned long t_size;
    unsigned long d_addr;                  
    unsigned long d_size;                  
    unsigned long b_addr;                  
    unsigned long b_size;                  
    unsigned long s_addr;
    unsigned long s_size;
    unsigned long SavedSP;
    unsigned long SavedFP;
    unsigned long SavedGP;
    unsigned long SavedRA;
    unsigned long SavedS0;
} EXE_HEADER;

extern s8 CdromId[9];
extern s8 CdromLabel[11];

s32 LoadCdrom();
s32 LoadCdromFile(char *filename, EXE_HEADER *head);
s32 CheckCdrom();
s32 Load(s8 *ExePath);

s32 SaveState(s8 *file);
s32 LoadState(s8 *file);
s32 CheckState(s8 *file);

#endif /* __MISC_H__ */

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

#ifndef __R3000A_NEW_H__
#define __R3000A_NEW_H__

#include <stdio.h>

#include "common.h"

typedef struct {
	int  (*Init)();
	void (*Reset)();
	void (*Execute)();		/* executes up to a break */
	void (*ExecuteBlock)();	/* executes up to a jump */
	void (*Clear)(u32 Addr, u32 Size);
	void (*Shutdown)();
} R3000Acpu;

extern R3000Acpu *psxCpu;
extern R3000Acpu psxInt;
#ifdef DYNAREC
extern R3000Acpu psxRec;
#endif

typedef union {
	struct {
		unsigned long   r0, at, v0, v1, a0, a1, a2, a3,
						t0, t1, t2, t3, t4, t5, t6, t7,
						s0, s1, s2, s3, s4, s5, s6, s7,
						t8, t9, k0, k1, gp, sp, s8, ra, lo, hi;
	} n;
	unsigned long r[34]; /* Lo, Hi in r[33] and r[34] */
} psxGPRRegs;

typedef union {
	struct {
		unsigned long	Index,     Random,    EntryLo0,  EntryLo1,
						Context,   PageMask,  Wired,     Reserved0,
						BadVAddr,  Count,     EntryHi,   Compare,
						Status,    Cause,     EPC,       PRid,
						Config,    LLAddr,    WatchLO,   WatchHI,
						XContext,  Reserved1, Reserved2, Reserved3,
						Reserved4, Reserved5, ECC,       CacheErr,
						TagLo,     TagHi,     ErrorEPC,  Reserved6;
	} n;
	unsigned long r[32];
} psxCP0Regs;

// EDIT added:
typedef struct {
	unsigned long r[32];
} psxCC0Regs;

typedef struct {
	short x, y;
} SVector2D;

typedef struct {
	short z, pad;
} SVector2Dz;

typedef struct {
	short x, y, z, pad;
} SVector3D;

typedef struct {
	short x, y, z, pad;
} LVector3D;

typedef struct {
	unsigned char r, g, b, c;
} CBGR;

typedef struct {
	short m11, m12, m13, m21, m22, m23, m31, m32, m33, pad;
} SMatrix3D;

typedef union
{
	struct { u8 l,h,h2,h3; } b;
	struct { u16 l,h; } w;
	u32 d;	
} PAIR;

typedef union {
#if 0
	struct {
		SVector3D     v0, v1, v2;
		CBGR          rgb;
		long          otz;
		long          ir0, ir1, ir2, ir3;
		SVector2D     sxy0, sxy1, sxy2, sxyp;
		SVector2Dz    sz0, sz1, sz2, sz3;
		CBGR          rgb0, rgb1, rgb2;
		long          reserved;
		long          mac0, mac1, mac2, mac3;
		unsigned long irgb, orgb;
		long          lzcs, lzcr;
	} n;
#endif
	PAIR r[32];
} psxCP2Data;

typedef union {
#if 0
	struct {
		SMatrix3D rMatrix;
		long      trX, trY, trZ;
		SMatrix3D lMatrix;
		long      rbk, gbk, bbk;
		SMatrix3D cMatrix;
		long      rfc, gfc, bfc;
		long      ofx, ofy;
		long      h;
		long      dqa, dqb;
		long      zsf3, zsf4;
		long      flag;
	} n;
#endif
	PAIR r[32];
} psxCP2Ctrl;

typedef struct {
	psxGPRRegs GPR;		/* General Purpose Registers */
	psxCP0Regs CP0;		/* Coprocessor0 Registers */
	psxCC0Regs CC0;		/* Coprocessor0 control Registers */
	psxCP2Data CP2D; 	/* Cop2 data registers */
	psxCP2Ctrl CP2C; 	/* Cop2 control registers */
	u32 pc;				    /* Program counter */
	u32 code;			    /* The instruction */
	u32 cycle;
	u32 interrupt;
	u32 intCycle[32];
	u32 iRegs[32];	/* used for imm caching and back up of regs in dynarec */
	u32 psx_next_io_base;
	u32 psx_next_io_count;
	u32 psx_next_int_base;
	u32 psx_next_int_count;
} psxRegisters;

extern psxRegisters* psxRegs;

#define _i32W(x)(*((signed long*)&x))
#define _i32(x) ((signed long)x)
#define _u32W(x) x
#define _u32(x) ((unsigned long)x)

#define _i16(x) ((short)x)
#define _u16(x) ((unsigned short)x)

#define _i8(x) ((char)x
#define _u8(x) ((unsigned char)x)

/**** R3000A Instruction Macros ****/
#define _fOp_(code)		((code >> 26)       )  // The opcode part of the instruction register 
#define _fFunct_(code)		((code      ) & 0x3F)  // The funct part of the instruction register 
#define _fRd_(code)		((code >> 11) & 0x1F)  // The rd part of the instruction register 
#define _fRt_(code)		((code >> 16) & 0x1F)  // The rt part of the instruction register 
#define _fRs_(code)		((code >> 21) & 0x1F)  // The rs part of the instruction register 
#define _fSa_(code)		((code >>  6) & 0x1F)  // The sa part of the instruction register
#define _fIm_(code)		((u16)code)            // The immediate part of the instruction register
#define _fTarget_(code)	(code & 0x03ffffff)    // The target part of the instruction register

#define _fImm_(code)	((s16)code)            // sign-extended immediate
#define _fImmU_(code)	(code&0xffff)          // zero-extended immediate

#ifdef GP2X_MAPREGS
  register u32 reg_code asm("%7");
  register u32 reg_pc   asm("%8");
#else
  extern u32 reg_code;
  extern u32 reg_pc;
#endif

  #define _PC_       psxRegs->pc       // The next PC to be executed
  #define _Op_     _fOp_(psxRegs->code)
  #define _Funct_  _fFunct_(psxRegs->code)
  #define _Rd_     _fRd_(psxRegs->code)
  #define _Rt_     _fRt_(psxRegs->code)
  #define _Rs_     _fRs_(psxRegs->code)
  #define _Sa_     _fSa_(psxRegs->code)
  #define _Im_     _fIm_(psxRegs->code)
  #define _Target_ _fTarget_(psxRegs->code)
  #define _Imm_	   _fImm_(psxRegs->code)
  #define _ImmU_	 _fImmU_(psxRegs->code)


#define _rRs_   psxRegs->GPR.r[_Rs_]   // Rs register
#define _rRt_   psxRegs->GPR.r[_Rt_]   // Rt register
#define _rRd_   psxRegs->GPR.r[_Rd_]   // Rd register
#define _rSa_   psxRegs->GPR.r[_Sa_]   // Sa register
#define _rFs_   psxRegs->CP0.r[_Rd_]   // Fs register

#define _c2dRs_ psxRegs->CP2D.r[_Rs_]  // Rs cop2 data register
#define _c2dRt_ psxRegs->CP2D.r[_Rt_]  // Rt cop2 data register
#define _c2dRd_ psxRegs->CP2D.r[_Rd_]  // Rd cop2 data register
#define _c2dSa_ psxRegs->CP2D.r[_Sa_]  // Sa cop2 data register

#define _rHi_   psxRegs->GPR.n.hi   // The HI register
#define _rLo_   psxRegs->GPR.n.lo   // The LO register
#define _SetLink(x)     psxRegs->GPR.r[x] = _PC_ + 4;       // Sets the return address in the link register

#define _JumpTarget_    ((_Target_ * 4) + (_PC_ & 0xf0000000))   // Calculates the target during a jump instruction
#define _BranchTarget_  ((s16)_Im_ * 4 + _PC_)                 // Calculates the target during a branch instruction

#define	t0_count	(psxHu16(0x1100))
#define	t0_mode		(psxHu32(0x1104))
#define	t0_target	(psxHu16(0x1108))
#define	t1_count	(psxHu16(0x1110))
#define	t1_mode		(psxHu32(0x1114))
#define	t1_target	(psxHu16(0x1118))
#define	t2_count	(psxHu16(0x1120))
#define	t2_mode		(psxHu32(0x1124))
#define	t2_target	(psxHu16(0x1128))

int  psxInit();
void psxReset();
void psxShutdown();
u32 psxException(u32 code, u32 bd);
void psxExecuteBios();
int  psxTestLoadDelay(int reg, u32 tmp);
void psxDelayTest(int reg, u32 bpc);
void psxTestSWInts();
void psxTestHWInts();
void update_hw(u32 cycles);

#endif /* __R3000A_NEW_H__ */

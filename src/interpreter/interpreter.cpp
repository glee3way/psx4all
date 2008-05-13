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

#include <stdlib.h>

#include "common.h"

static int pc = 0;
static int oldpc = 0;
static int branch = 0;
static int branch2 = 0;
static int end_block_pc = 0xFFFFFFFF;
static u32 branchPC;
#ifdef INTERP
psxRegisters intRegs;
psxRegisters* psxRegs;
#endif

// These macros are used to assemble the repassembler functions
#define debugI()

#define execI() { 						\
	u32* code = (u32*) PSXM32(psxRegs->pc); 			\
	psxRegs->code = code == NULL ? 0 : *code; 		\
								\
	debugI(); 						\
 								\
	psxRegs->pc+= 4; psxRegs->cycle += 1; 			\
	psxBSC[psxRegs->code >> 26](); 				\
}

// Subsets
extern void (*psxBSC[64])();
extern void (*psxSPC[64])();
extern void (*psxREG[32])();
extern void (*psxCP0[32])();
/*
extern void (*psxCP2[64])();
extern void (*psxCP2BSC[32])();
*/

static void psxBranchTest()
{
	static u32 last_cycles = 0;
			
  update_hw(psxRegs->cycle - last_cycles);
  
  last_cycles = psxRegs->cycle;
}


static void delayRead(int reg, u32 bpc) {
	u32 rold, rnew;

//	SysPrintf("delayRead at %x!\n", psxRegs->pc);

	rold = psxRegs->GPR.r[reg];
	psxBSC[psxRegs->code >> 26](); // branch delay load
	rnew = psxRegs->GPR.r[reg];

	psxRegs->cycle += 1; //(int)((float)((psxRegs->pc-oldpc)/4) * BIAS_CYCLE_INC);
	psxRegs->pc = bpc;
	pc = oldpc = psxRegs->pc;

	psxRegs->GPR.r[reg] = rold;
	execI(); // first branch opcode
	psxRegs->GPR.r[reg] = rnew;

	psxBranchTest();

	branch = 0;
}

static void delayWrite(int reg, u32 bpc) {

/*	SysPrintf("delayWrite at %x!\n", psxRegs->pc);

	SysPrintf("%s\n", disR3000AF(psxRegs->code, psxRegs->pc-4));
	SysPrintf("%s\n", disR3000AF(PSXMu32(bpc), bpc));*/

	// no changes from normal behavior

	psxBSC[psxRegs->code >> 26]();

	branch = 0;



	psxRegs->cycle += 1; //(int)((float)((psxRegs->pc-oldpc)/4) * BIAS_CYCLE_INC);
	psxRegs->pc = bpc;
	pc = oldpc = psxRegs->pc;

	psxBranchTest();
}

static void delayReadWrite(int reg, u32 bpc) {

//	SysPrintf("delayReadWrite at %x!\n", psxRegs->pc);

	// the branch delay load is skipped

	branch = 0;



	psxRegs->cycle += 1; //(int)((float)((psxRegs->pc-oldpc)/4) * BIAS_CYCLE_INC);
	psxRegs->pc = bpc;
	pc = oldpc = psxRegs->pc;

	psxBranchTest();
}

// this defines shall be used with the tmp 
// of the next func (instead of _Funct_...)
#define _tFunct_  ((tmp      ) & 0x3F)  // The funct part of the instruction register 
#define _tRd_     ((tmp >> 11) & 0x1F)  // The rd part of the instruction register 
#define _tRt_     ((tmp >> 16) & 0x1F)  // The rt part of the instruction register 
#define _tRs_     ((tmp >> 21) & 0x1F)  // The rs part of the instruction register 
#define _tSa_     ((tmp >>  6) & 0x1F)  // The sa part of the instruction register

int psxTestLoadDelay(int reg, u32 tmp) {
	if (tmp == 0) return 0; // NOP
	switch (tmp >> 26) {
		case 0x00: // SPECIAL
			switch (_tFunct_) {
				case 0x00: // SLL
				case 0x02: case 0x03: // SRL/SRA
					if (_tRd_ == reg && _tRt_ == reg) return 1; else
					if (_tRt_ == reg) return 2; else
					if (_tRd_ == reg) return 3;
					break;

				case 0x08: // JR
					if (_tRs_ == reg) return 2;
					break;
				case 0x09: // JALR
					if (_tRd_ == reg && _tRs_ == reg) return 1; else
					if (_tRs_ == reg) return 2; else
					if (_tRd_ == reg) return 3;
					break;

				// SYSCALL/BREAK just a break;

				case 0x20: case 0x21: case 0x22: case 0x23:
				case 0x24: case 0x25: case 0x26: case 0x27: 
				case 0x2a: case 0x2b: // ADD/ADDU...
				case 0x04: case 0x06: case 0x07: // SLLV...
					if (_tRd_ == reg && (_tRt_ == reg || _tRs_ == reg)) return 1; else
					if (_tRt_ == reg || _tRs_ == reg) return 2; else
					if (_tRd_ == reg) return 3;
					break;

				case 0x10: case 0x12: // MFHI/MFLO
					if (_tRd_ == reg) return 3;
					break;
				case 0x11: case 0x13: // MTHI/MTLO
					if (_tRs_ == reg) return 2;
					break;

				case 0x18: case 0x19:
				case 0x1a: case 0x1b: // MULT/DIV...
					if (_tRt_ == reg || _tRs_ == reg) return 2;
					break;
			}
			break;

		case 0x01: // REGIMM
			switch (_tRt_) {
				case 0x00: case 0x02:
				case 0x10: case 0x12: // BLTZ/BGEZ...
					if (_tRs_ == reg) return 2;
					break;
			}
			break;

		// J would be just a break;
		case 0x03: // JAL
			if (31 == reg) return 3;
			break;

		case 0x04: case 0x05: // BEQ/BNE
			if (_tRs_ == reg || _tRt_ == reg) return 2;
			break;

		case 0x06: case 0x07: // BLEZ/BGTZ
			if (_tRs_ == reg) return 2;
			break;

		case 0x08: case 0x09: case 0x0a: case 0x0b:
		case 0x0c: case 0x0d: case 0x0e: // ADDI/ADDIU...
			if (_tRt_ == reg && _tRs_ == reg) return 1; else
			if (_tRs_ == reg) return 2; else
			if (_tRt_ == reg) return 3;
			break;

		case 0x0f: // LUI
			if (_tRt_ == reg) return 3;
			break;

		case 0x10: // COP0
			switch (_tFunct_) {
				case 0x00: // MFC0
					if (_tRt_ == reg) return 3;
					break;
				case 0x02: // CFC0
					if (_tRt_ == reg) return 3;
					break;
				case 0x04: // MTC0
					if (_tRt_ == reg) return 2;
					break;
				case 0x06: // CTC0
					if (_tRt_ == reg) return 2;
					break;
				// RFE just a break;
			}
			break;

		case 0x12: // COP2
			switch (_tFunct_) {
				case 0x00: 
					switch (_tRs_) {
						case 0x00: // MFC2
							if (_tRt_ == reg) return 3;
							break;
						case 0x02: // CFC2
							if (_tRt_ == reg) return 3;
							break;
						case 0x04: // MTC2
							if (_tRt_ == reg) return 2;
							break;
						case 0x06: // CTC2
							if (_tRt_ == reg) return 2;
							break;
					}
					break;
				// RTPS... break;
			}
			break;

		case 0x22: case 0x26: // LWL/LWR
			if (_tRt_ == reg) return 3; else
			if (_tRs_ == reg) return 2;
			break;

		case 0x20: case 0x21: case 0x23:
		case 0x24: case 0x25: // LB/LH/LW/LBU/LHU
			if (_tRt_ == reg && _tRs_ == reg) return 1; else
			if (_tRs_ == reg) return 2; else
			if (_tRt_ == reg) return 3;
			break;

		case 0x28: case 0x29: case 0x2a:
		case 0x2b: case 0x2e: // SB/SH/SWL/SW/SWR
			if (_tRt_ == reg || _tRs_ == reg) return 2;
			break;

		case 0x32: case 0x3a: // LWC2/SWC2
			if (_tRs_ == reg) return 2;
			break;
	}

	return 0;
}

#ifdef DYNAREC
u32 recIntExecuteBlock(u32 newpc)
{
	pc = oldpc = psxRegs->pc = newpc - 4;
	branch2 = 0;

#if 0
	u32 end_block = 0;
	end_block_pc = 0xFFFFFFFF;
	for(;end_block == 0;)
	{
		u32 tmp;
		u32* code = (u32*) PSXM(pc);
		tmp = code == NULL ? 0 : *code;
		pc += 4;
		switch (tmp >> 26) {
			case 0x00: // SPECIAL
				switch (_tFunct_) {
					case 0x00: // SLL
						if (!tmp) break; // NOP
					case 0x02: case 0x03: // SRL/SRA
						break;
					case 0x08: // JR
						end_block = 1;
						break;
					case 0x09: // JALR
						end_block = 1;
						break;

					// SYSCALL/BREAK just a break;

					case 0x20: case 0x21: case 0x22: case 0x23:
					case 0x24: case 0x25: case 0x26: case 0x27:
					case 0x2a: case 0x2b: // ADD/ADDU...
					case 0x04: case 0x06: case 0x07: // SLLV...
						break;

					case 0x10: case 0x12: // MFHI/MFLO
						break;
					case 0x11: case 0x13: // MTHI/MTLO
						break;

					case 0x18: case 0x19:
					case 0x1a: case 0x1b: // MULT/DIV...
						break;
				}
				break;

			case 0x01: // REGIMM
				switch (_tRt_) {
					case 0x00: case 0x02:
					case 0x10: case 0x12: // BLTZ/BGEZ...
						end_block = 1;
						break;
				}
				break;

			case 0x02: // J
			case 0x03: // JAL
				end_block = 1;
				break;

			case 0x04: case 0x05: // BEQ/BNE
				end_block = 1;
				break;

			case 0x06: case 0x07: // BLEZ/BGTZ
				end_block = 1;
				break;

			case 0x08: case 0x09: case 0x0a: case 0x0b:
			case 0x0c: case 0x0d: case 0x0e: // ADDI/ADDIU...
				break;

			case 0x0f: // LUI
				break;

			case 0x10: // COP0
				switch (_tFunct_) {
					case 0x00: // MFC0
						break;
					case 0x02: // CFC0
						break;
					case 0x04: // MTC0
						break;
					case 0x06: // CTC0
						break;
					// RFE just a break;
				}
				break;

			case 0x12: // COP2
				switch(_tFunct_)
				{
					case 0:
						switch(_tRs_) {
						case 0: end_block_pc = pc;	break;
						case 2: end_block_pc = pc;	break;
						case 4: end_block_pc = pc; 	break;
						case 6: end_block_pc = pc;	break;
						}
						break;
					case 1: end_block_pc = pc; break;
					case 6: end_block_pc = pc; break;
					case 12: end_block_pc = pc; break;
					case 16: end_block_pc = pc; break;
					case 17: end_block_pc = pc; break;
					case 18: end_block_pc = pc; break;
					case 19: end_block_pc = pc; break;
					case 20: end_block_pc = pc; break;
					case 22: end_block_pc = pc; break;
					case 27: end_block_pc = pc; break;
					case 28: end_block_pc = pc; break;
					case 30: end_block_pc = pc; break;
					case 32: end_block_pc = pc; break;
					case 40: end_block_pc = pc; break;
					case 41: end_block_pc = pc; break;
					case 42: end_block_pc = pc; break;
					case 45: end_block_pc = pc; break;
					case 46: end_block_pc = pc; break;
					case 48: end_block_pc = pc; break;
					case 61: end_block_pc = pc; break;
					case 62: end_block_pc = pc; break;
					case 63: end_block_pc = pc; break;
				}
				break;

			case 0x22: case 0x26: // LWL/LWR
				break;

			case 0x20: case 0x21: case 0x23:
			case 0x24: case 0x25: // LB/LH/LW/LBU/LHU
				break;

			case 0x28: case 0x29: case 0x2a:
			case 0x2b: case 0x2e: // SB/SH/SWL/SW/SWR
				break;

			case 0x32: case 0x3a: // LWC2/SWC2
				break;
		}
	}

	pc = oldpc;

	while (branch2 == 0 && end_block_pc != psxRegs->pc) execI();

	if( end_block_pc == psxRegs->pc )
	{
		psxRegs->cycle += (int)((float)((psxRegs->pc-oldpc)/4) * BIAS_CYCLE_INC);
		pc = oldpc = psxRegs->pc;
	}
#else

	while (branch2 == 0) execI();

#endif

	return (u32)PC_REC32(psxRegs->pc);
}
#endif

void psxDelayTest(int reg, u32 bpc)
{
	u32* code;
	u32 tmp;

	code = (u32*) PSXM32(bpc);
	tmp = code == NULL ? 0 : *code;
	branch = 1;

	switch (tmp >> 26) {
		case 0x00: // SPECIAL
			switch (_tFunct_) {
				case 0x00: // SLL
					if (!tmp) break; // NOP
				case 0x02: case 0x03: // SRL/SRA
					if (_tRd_ == reg && _tRt_ == reg) {
						delayReadWrite(reg, bpc); return;
					} else if (_tRt_ == reg) {
						delayRead(reg, bpc); return;
					} else if (_tRd_ == reg) {
						delayWrite(reg, bpc); return;
					}
					break;

				case 0x08: // JR
					if (_tRs_ == reg) {
						delayRead(reg, bpc); return;
					}
					break;
				case 0x09: // JALR
					if (_tRd_ == reg && _tRs_ == reg) {
						delayReadWrite(reg, bpc); return;
					} else if (_tRs_ == reg) {
						delayRead(reg, bpc); return;
					} else if (_tRd_ == reg) {
						delayWrite(reg, bpc); return;
					}
					break;

				// SYSCALL/BREAK just a break;

				case 0x20: case 0x21: case 0x22: case 0x23:
				case 0x24: case 0x25: case 0x26: case 0x27:
				case 0x2a: case 0x2b: // ADD/ADDU...
				case 0x04: case 0x06: case 0x07: // SLLV...
					if (_tRd_ == reg && (_tRt_ == reg || _tRs_ == reg)) {
						delayReadWrite(reg, bpc); return;
					} else if (_tRt_ == reg || _tRs_ == reg) {
						delayRead(reg, bpc); return;
					} else if (_tRd_ == reg) {
						delayWrite(reg, bpc); return;
					}
					break;

				case 0x10: case 0x12: // MFHI/MFLO
					if (_tRd_ == reg) {
						delayWrite(reg, bpc); return;
					}
					break;
				case 0x11: case 0x13: // MTHI/MTLO
					if (_tRs_ == reg) {
						delayRead(reg, bpc); return;
					}
					break;

				case 0x18: case 0x19:
				case 0x1a: case 0x1b: // MULT/DIV...
					if (_tRt_ == reg || _tRs_ == reg) {
						delayRead(reg, bpc); return;
					}
					break;
			}
			break;

		case 0x01: // REGIMM
			switch (_tRt_) {
				case 0x00: case 0x02:
				case 0x10: case 0x12: // BLTZ/BGEZ...
					if (_tRs_ == reg) {
						delayRead(reg, bpc); return;
					}
					break;
			}
			break;

		// J would be just a break;
		case 0x03: // JAL
			if (31 == reg) {
				delayWrite(reg, bpc); return;
			}
			break;

		case 0x04: case 0x05: // BEQ/BNE
			if (_tRs_ == reg || _tRt_ == reg) {
				delayRead(reg, bpc); return;
			}
			break;

		case 0x06: case 0x07: // BLEZ/BGTZ
			if (_tRs_ == reg) {
				delayRead(reg, bpc); return;
			}
			break;

		case 0x08: case 0x09: case 0x0a: case 0x0b:
		case 0x0c: case 0x0d: case 0x0e: // ADDI/ADDIU...
			if (_tRt_ == reg && _tRs_ == reg) {
				delayReadWrite(reg, bpc); return;
			} else if (_tRs_ == reg) {
				delayRead(reg, bpc); return;
			} else if (_tRt_ == reg) {
				delayWrite(reg, bpc); return;
			}
			break;

		case 0x0f: // LUI
			if (_tRt_ == reg) {
				delayWrite(reg, bpc); return;
			}
			break;

		case 0x10: // COP0
			switch (_tFunct_) {
				case 0x00: // MFC0
					if (_tRt_ == reg) { delayWrite(reg, bpc); return; }
					break;
				case 0x02: // CFC0
					if (_tRt_ == reg) { delayWrite(reg, bpc); return; }
					break;
				case 0x04: // MTC0
					if (_tRt_ == reg) { delayRead(reg, bpc); return; }
					break;
				case 0x06: // CTC0
					if (_tRt_ == reg) { delayRead(reg, bpc); return; }
					break;
				// RFE just a break;
			}
			break;

		case 0x12: // COP2
			switch (_tFunct_) {
				case 0x00:
					switch (_tRs_) {
						case 0x00: // MFC2
							if (_tRt_ == reg) { delayRead(reg, bpc); return; }
							break;
						case 0x02: // CFC2
							if (_tRt_ == reg) { delayRead(reg, bpc); return; }
							break;
						case 0x04: // MTC2
							if (_tRt_ == reg) { delayRead(reg, bpc); return; }
							break;
						case 0x06: // CTC2
							if (_tRt_ == reg) { delayRead(reg, bpc); return; }
							break;
					}
					break;
				// RTPS... break;
			}
			break;

		case 0x22: case 0x26: // LWL/LWR
			if (_tRt_ == reg) {
				delayReadWrite(reg, bpc); return;
			} else if (_tRs_ == reg) {
				delayRead(reg, bpc); return;
			}
			break;

		case 0x20: case 0x21: case 0x23:
		case 0x24: case 0x25: // LB/LH/LW/LBU/LHU
			if (_tRt_ == reg && _tRs_ == reg) {
				delayReadWrite(reg, bpc); return;
			} else if (_tRs_ == reg) {
				delayRead(reg, bpc); return;
			} else if (_tRt_ == reg) {
				delayWrite(reg, bpc); return;
			}
			break;

		case 0x28: case 0x29: case 0x2a:
		case 0x2b: case 0x2e: // SB/SH/SWL/SW/SWR
			if (_tRt_ == reg || _tRs_ == reg) {
				delayRead(reg, bpc); return;
			}
			break;

		case 0x32: case 0x3a: // LWC2/SWC2
			if (_tRs_ == reg) {
				delayRead(reg, bpc); return;
			}
			break;
	}
	psxBSC[psxRegs->code >> 26]();

	branch = 0;


	psxRegs->cycle += 1;
	psxRegs->pc = bpc;
	pc = oldpc = psxRegs->pc;

	psxBranchTest();
}

__inline void doBranch(u32 tar) {
	u32* code;
	u32 tmp;

	branch2 = branch = 1;
	branchPC = tar;

	code = (u32*) PSXM32(psxRegs->pc);
	psxRegs->code = code == NULL ? 0 : *code;

	debugI();

	psxRegs->pc+= 4;
	/*psxRegs->cycle += BIAS_CYCLE_INC;*/

	// check for load delay
	tmp = psxRegs->code >> 26;
	switch (tmp) {
		case 0x10: // COP0
			switch (_Rs_) {
				case 0x00: // MFC0
				case 0x02: // CFC0
					psxDelayTest(_Rt_, branchPC);
					return;
			}
			break;
		case 0x12: // COP2
			switch (_Funct_) {
				case 0x00:
					switch (_Rs_) {
						case 0x00: // MFC2
						case 0x02: // CFC2
							psxDelayTest(_Rt_, branchPC);
							return;
					}
					break;
			}
			break;
		case 0x32: // LWC2
			psxDelayTest(_Rt_, branchPC);
			return;
		default:
			if (tmp >= 0x20 && tmp <= 0x26) { // LB/LH/LWL/LW/LBU/LHU/LWR
				psxDelayTest(_Rt_, branchPC);
				return;
			}
			break;
	}

	psxBSC[psxRegs->code >> 26]();

	branch = 0;


	psxRegs->cycle += 1;
	psxRegs->pc = branchPC;
	pc = oldpc = psxRegs->pc;

	psxBranchTest();
}

/*********************************************************
* Arithmetic with immediate operand                      *
* Format:  OP rt, rs, immediate                          *
*********************************************************/
void psxADDI() 	{ if (!_Rt_) return; _rRt_ = _u32(_rRs_) + _Imm_ ; }		// Rt = Rs + Im 	(Exception on Integer Overflow)
void psxADDIU() { if (!_Rt_) return; _rRt_ = _u32(_rRs_) + _Imm_ ; }		// Rt = Rs + Im
void psxANDI() 	{ if (!_Rt_) return; _rRt_ = _u32(_rRs_) & _ImmU_; }		// Rt = Rs And Im
void psxORI() 	{ if (!_Rt_) return; _rRt_ = _u32(_rRs_) | _ImmU_; }		// Rt = Rs Or  Im
void psxXORI() 	{ if (!_Rt_) return; _rRt_ = _u32(_rRs_) ^ _ImmU_; }		// Rt = Rs Xor Im
void psxSLTI() 	{ if (!_Rt_) return; _rRt_ = _i32(_rRs_) < _Imm_ ; }		// Rt = Rs < Im		(Signed)
void psxSLTIU() { if (!_Rt_) return; _rRt_ = _u32(_rRs_) < ((u32)_Imm_); }		// Rt = Rs < Im		(Unsigned)

/*********************************************************
* Register arithmetic                                    *
* Format:  OP rd, rs, rt                                 *
*********************************************************/
void psxADD()	{ if (!_Rd_) return; _rRd_ = _u32(_rRs_) + _u32(_rRt_); }	// Rd = Rs + Rt		(Exception on Integer Overflow)
void psxADDU() 	{ if (!_Rd_) return; _rRd_ = _u32(_rRs_) + _u32(_rRt_); }	// Rd = Rs + Rt
void psxSUB() 	{ if (!_Rd_) return; _rRd_ = _u32(_rRs_) - _u32(_rRt_); }	// Rd = Rs - Rt		(Exception on Integer Overflow)
void psxSUBU() 	{ if (!_Rd_) return; _rRd_ = _u32(_rRs_) - _u32(_rRt_); }	// Rd = Rs - Rt
void psxAND() 	{ if (!_Rd_) return; _rRd_ = _u32(_rRs_) & _u32(_rRt_); }	// Rd = Rs And Rt
void psxOR() 	{ if (!_Rd_) return; _rRd_ = _u32(_rRs_) | _u32(_rRt_); }	// Rd = Rs Or  Rt
void psxXOR() 	{ if (!_Rd_) return; _rRd_ = _u32(_rRs_) ^ _u32(_rRt_); }	// Rd = Rs Xor Rt
void psxNOR() 	{ if (!_Rd_) return; _rRd_ =~(_u32(_rRs_) | _u32(_rRt_)); }// Rd = Rs Nor Rt
void psxSLT() 	{ if (!_Rd_) return; _rRd_ = _i32(_rRs_) < _i32(_rRt_); }	// Rd = Rs < Rt		(Signed)
void psxSLTU() 	{ if (!_Rd_) return; _rRd_ = _u32(_rRs_) < _u32(_rRt_); }	// Rd = Rs < Rt		(Unsigned)

/*********************************************************
* Register mult/div & Register trap logic                *
* Format:  OP rs, rt                                     *
*********************************************************/
void psxDIV() {
	if (_i32(_rRt_) != 0) {
		_rLo_ = _i32(_rRs_) / _i32(_rRt_);
		_rHi_ = _i32(_rRs_) % _i32(_rRt_);
	}
}

void psxDIVU() {
	if (_rRt_ != 0) {
		_rLo_ = _rRs_ / _rRt_;
		_rHi_ = _rRs_ % _rRt_;
	}
}

void psxMULT() {
	u64 res = (s64)((s64)_i32(_rRs_) * (s64)_i32(_rRt_));

	psxRegs->GPR.n.lo = (unsigned long)(res & 0xffffffff);
	psxRegs->GPR.n.hi = (unsigned long)((res >> 32) & 0xffffffff);
}

void psxMULTU() {
	u64 res = (u64)((u64)_u32(_rRs_) * (u64)_u32(_rRt_));

	psxRegs->GPR.n.lo = (unsigned long)(res & 0xffffffff);
	psxRegs->GPR.n.hi = (unsigned long)((res >> 32) & 0xffffffff);
}

/*********************************************************
* Register branch logic                                  *
* Format:  OP rs, offset                                 *
*********************************************************/
#define RepZBranchi32(op)      if(_i32(_rRs_) op 0) doBranch(_BranchTarget_);
#define RepZBranchLinki32(op)  if(_i32(_rRs_) op 0) { _SetLink(31); doBranch(_BranchTarget_); }

void psxBGEZ()   { RepZBranchi32(>=) }      // Branch if Rs >= 0
void psxBGEZAL() { RepZBranchLinki32(>=) }  // Branch if Rs >= 0 and link
void psxBGTZ()   { RepZBranchi32(>) }       // Branch if Rs >  0
void psxBLEZ()   { RepZBranchi32(<=) }      // Branch if Rs <= 0
void psxBLTZ()   { RepZBranchi32(<) }       // Branch if Rs <  0
void psxBLTZAL() { RepZBranchLinki32(<) }   // Branch if Rs <  0 and link

/*********************************************************
* Shift arithmetic with constant shift                   *
* Format:  OP rd, rt, sa                                 *
*********************************************************/
void psxSLL() { if (!_Rd_) return; _rRd_ = _u32(_rRt_) << _Sa_; } // Rd = Rt << sa
void psxSRA() { if (!_Rd_) return; _rRd_ = _i32(_rRt_) >> _Sa_; } // Rd = Rt >> sa (arithmetic)
void psxSRL() { if (!_Rd_) return; _rRd_ = _u32(_rRt_) >> _Sa_; } // Rd = Rt >> sa (logical)

//void psxSRA() { if (!_Rd_) return; _rRd_ = (((u32)(_rRt_))>>(u32)31) ? ((u32)0xFFFFFFFF << (u32)_Sa_) | ((u32)(_rRt_) >> (u32)_Sa_) : ((u32)(_rRt_) >> (u32)_Sa_); } // Rd = Rt >> sa (arithmetic)
//void psxSRAV() { if (!_Rd_) return; _rRd_ = (((u32)(_rRt_))>>31) ? ((u32)0xFFFFFFFF << (u32)(_rRs_)) | ((u32)(_rRt_) >> (u32)(_rRs_)) : ((u32)(_rRt_) >> (u32)(_rRs_)); } // Rd = Rt >> rs (arithmetic)

/*********************************************************
* Shift arithmetic with variant register shift           *
* Format:  OP rd, rt, rs                                 *
*********************************************************/
void psxSLLV() { if (!_Rd_) return; _rRd_ = _u32(_rRt_) << _u32(_rRs_); } // Rd = Rt << rs
void psxSRAV() { if (!_Rd_) return; _rRd_ = _i32(_rRt_) >> _u32(_rRs_); } // Rd = Rt >> rs (arithmetic)
void psxSRLV() { if (!_Rd_) return; _rRd_ = _u32(_rRt_) >> _u32(_rRs_); } // Rd = Rt >> rs (logical)

/*********************************************************
* Load higher 16 bits of the first word in GPR with imm  *
* Format:  OP rt, immediate                              *
*********************************************************/
void psxLUI() { if (!_Rt_) return; _rRt_ = psxRegs->code << 16; } // Upper halfword of Rt = Im

/*********************************************************
* Move from HI/LO to GPR                                 *
* Format:  OP rd                                         *
*********************************************************/
void psxMFHI() { if (!_Rd_) return; _rRd_ = _rHi_; } // Rd = Hi
void psxMFLO() { if (!_Rd_) return; _rRd_ = _rLo_; } // Rd = Lo

/*********************************************************
* Move to GPR to HI/LO & Register jump                   *
* Format:  OP rs                                         *
*********************************************************/
void psxMTHI() { _rHi_ = _rRs_; } // Hi = Rs
void psxMTLO() { _rLo_ = _rRs_; } // Lo = Rs

/*********************************************************
* Special purpose instructions                           *
* Format:  OP                                            *
*********************************************************/
void psxBREAK() {
	// Break exception - psx rom doens't handles this
}

void psxSYSCALL() {
	psxRegs->pc -= 4;
	psxException(0x20, branch);
}

void psxRFE() {
//	SysPrintf("psxRFE\n");
	psxRegs->CP0.n.Status = (psxRegs->CP0.n.Status & 0xfffffff0) |
						  ((psxRegs->CP0.n.Status & 0x3c) >> 2);
}

/*********************************************************
* Register branch logic                                  *
* Format:  OP rs, rt, offset                             *
*********************************************************/
#define RepBranchi32(op)      if(_i32(_rRs_) op _i32(_rRt_)) doBranch(_BranchTarget_);

void psxBEQ() {	RepBranchi32(==) }  // Branch if Rs == Rt
void psxBNE() {	RepBranchi32(!=) }  // Branch if Rs != Rt

/*********************************************************
* Jump to target                                         *
* Format:  OP target                                     *
*********************************************************/
void psxJ()   {               doBranch(_JumpTarget_); }
void psxJAL() {	_SetLink(31); doBranch(_JumpTarget_); }

/*********************************************************
* Register jump                                          *
* Format:  OP rs, rd                                     *
*********************************************************/
void psxJR()   {                 doBranch(_u32(_rRs_)); }
void psxJALR() {
	u32 temp = _u32(_rRs_);
	if (_Rd_) { _SetLink(_Rd_); }
	doBranch(temp);
}

/*********************************************************
* Load and store for GPR                                 *
* Format:  OP rt, offset(base)                           *
*********************************************************/

#define _oB_ (_u32(_rRs_) + _Imm_)

void psxLB() {
	if (_Rt_) {
		_rRt_ = psxMemReadS8(_oB_);
	} else {
		psxMemReadS8(_oB_);
	}
}

void psxLBU() {
	if (_Rt_) {
		_rRt_ = psxMemRead8(_oB_);
	} else {
		psxMemRead8(_oB_);
	}
}

void psxLH() {
	if (_Rt_) {
		_rRt_ = psxMemReadS16(_oB_);
	} else {
		psxMemReadS16(_oB_);
	}
}

void psxLHU() {
	if (_Rt_) {
		_rRt_ = psxMemRead16(_oB_);
	} else {
		psxMemRead16(_oB_);
	}
}

void psxLW() {
	if (_Rt_) {
		_rRt_ = psxMemRead32(_oB_);
	} else {
		psxMemRead32(_oB_);
	}
}

u32 int_LWL_MASK[4] = { 0xffffff, 0xffff, 0xff, 0 };
u32 int_LWL_SHIFT[4] = { 24, 16, 8, 0 };

void psxLWL() {
	u32 addr = _oB_;
	u32 shift = addr & 3;
	u32 mem = psxMemRead32(addr & ~3);

	if (!_Rt_) return;
	_rRt_ =	( _u32(_rRt_) & int_LWL_MASK[shift]) |
					( mem << int_LWL_SHIFT[shift]);

	/*
	Mem = 1234.  Reg = abcd

	0   4bcd   (mem << 24) | (reg & 0x00ffffff)
	1   34cd   (mem << 16) | (reg & 0x0000ffff)
	2   234d   (mem <<  8) | (reg & 0x000000ff)
	3   1234   (mem      ) | (reg & 0x00000000)
	*/
}

u32 int_LWR_MASK[4] = { 0, 0xff000000, 0xffff0000, 0xffffff00 };
u32 int_LWR_SHIFT[4] = { 0, 8, 16, 24 };

void psxLWR() {
	u32 addr = _oB_;
	u32 shift = addr & 3;
	u32 mem = psxMemRead32(addr & ~3);

	if (!_Rt_) return;
	_rRt_ =	( _u32(_rRt_) & int_LWR_MASK[shift]) |
					( mem >> int_LWR_SHIFT[shift]);

	/*
	Mem = 1234.  Reg = abcd

	0   1234   (mem      ) | (reg & 0x00000000)
	1   a123   (mem >>  8) | (reg & 0xff000000)
	2   ab12   (mem >> 16) | (reg & 0xffff0000)
	3   abc1   (mem >> 24) | (reg & 0xffffff00)
	*/
}

void psxSB() { psxMemWrite8 (_oB_, _u8 (_rRt_)); }
void psxSH() { psxMemWrite16(_oB_, _u16(_rRt_)); }
void psxSW() { psxMemWrite32(_oB_, _u32(_rRt_)); }

u32 int_SWL_MASK[4] = { 0xffffff00, 0xffff0000, 0xff000000, 0 };
u32 int_SWL_SHIFT[4] = { 24, 16, 8, 0 };

void psxSWL() {
	u32 addr = _oB_;
	u32 shift = addr & 3;
	u32 mem = psxMemRead32(addr & ~3);

	psxMemWrite32(addr & ~3,  (_u32(_rRt_) >> int_SWL_SHIFT[shift]) |
			     (  mem & int_SWL_MASK[shift]) );
	/*
	Mem = 1234.  Reg = abcd

	0   123a   (reg >> 24) | (mem & 0xffffff00)
	1   12ab   (reg >> 16) | (mem & 0xffff0000)
	2   1abc   (reg >>  8) | (mem & 0xff000000)
	3   abcd   (reg      ) | (mem & 0x00000000)
	*/
}

u32 int_SWR_MASK[4] = { 0, 0xff, 0xffff, 0xffffff };
u32 int_SWR_SHIFT[4] = { 0, 8, 16, 24 };

void psxSWR() {
	u32 addr = _oB_;
	u32 shift = addr & 3;
	u32 mem = psxMemRead32(addr & ~3);

	psxMemWrite32(addr & ~3,  (_u32(_rRt_) << int_SWR_SHIFT[shift]) |
			     (  mem & int_SWR_MASK[shift]) );

	/*
	Mem = 1234.  Reg = abcd

	0   abcd   (reg      ) | (mem & 0x00000000)
	1   bcd4   (reg <<  8) | (mem & 0x000000ff)
	2   cd34   (reg << 16) | (mem & 0x0000ffff)
	3   d234   (reg << 24) | (mem & 0x00ffffff)
	*/
}

/*********************************************************
* Moves between GPR and COPx                             *
* Format:  OP rt, fs                                     *
*********************************************************/
void psxMFC0() { if (!_Rt_) return; _rRt_ = (int)_rFs_; }
void psxCFC0() { if (!_Rt_) return; _rRt_ = (int)_rFs_; }

void psxTestSWInts() {
	// the next code is untested, if u know please
	// tell me if it works ok or not (linuzappz)
	if (psxRegs->CP0.n.Cause & psxRegs->CP0.n.Status & 0x0300 &&
		psxRegs->CP0.n.Status & 0x1) {
		psxException(psxRegs->CP0.n.Cause, branch);
	}
}

__inline void MTC0(int reg, u32 val) {
//	SysPrintf("MTC0 %d: %x\n", reg, val);
	switch (reg) {
		case 12: // Status
			psxRegs->CP0.r[12] = val;
			psxTestSWInts();
			break;

		case 13: // Cause
			psxRegs->CP0.n.Cause = val & ~(0xfc00);
			psxTestSWInts();
			break;

		default:
			psxRegs->CP0.r[reg] = val;
			break;
	}
}

void psxMTC0() { MTC0(_Rd_, _u32(_rRt_)); }
void psxCTC0() { MTC0(_Rd_, _u32(_rRt_)); }

/*********************************************************
* Unknow instruction (would generate an exception)       *
* Format:  ?                                             *
*********************************************************/
void psxNULL() {
#ifdef PSXCPU_LOG
	PSXCPU_LOG("psx: Unimplemented op %x\n", psxRegs->code);
#endif
}

void psxSPECIAL() {
	psxSPC[_Funct_]();
}

void psxREGIMM() {
	psxREG[_Rt_]();
}

void psxCOP0() {
	psxCP0[_Rs_]();
}

#define CP2_FUNC(f,n)								\
void gte##f(u32 code, u32 pc);							\
INLINE void int##f() {								\
	gte##f(psxRegs->code, psxRegs->pc);					\
 }										\

#define CP2_FUNC2(f,n)								\
void gte##f(u32 code);								\
INLINE void int##f() {								\
	gte##f(psxRegs->code);							\
 }										\

#define CP2_FUNC3(f,n)								\
void gte##f();									\
INLINE void int##f() {								\
	gte##f();								\
 }										\

CP2_FUNC2(MFC2,0);
CP2_FUNC2(MTC2,0);
CP2_FUNC2(CFC2,0);
CP2_FUNC2(CTC2,0);
CP2_FUNC(LWC2,0);
CP2_FUNC(SWC2,0);
CP2_FUNC3(DCPL,2);
CP2_FUNC3(RTPS,2);
CP2_FUNC2(OP,2);
CP2_FUNC3(NCLIP,2);
CP2_FUNC3(DPCS,2);
CP2_FUNC3(INTPL,2);
CP2_FUNC2(MVMVA,2);
CP2_FUNC3(NCDS,2);
CP2_FUNC3(NCDT,2);
CP2_FUNC3(CDP,2);
CP2_FUNC3(NCCS,2);
CP2_FUNC3(CC,2);
CP2_FUNC3(NCS,2);
CP2_FUNC3(NCT,2);
CP2_FUNC2(SQR,2);
CP2_FUNC3(DPCT,2);
CP2_FUNC3(AVSZ3,2);
CP2_FUNC3(AVSZ4,2);
CP2_FUNC3(RTPT,2);
CP2_FUNC2(GPF,2);
CP2_FUNC2(GPL,2);
CP2_FUNC3(NCCT,2);

#define intCOP2()									\
		switch(_Funct_)								\
		{									\
			case 0:								\
				switch(_Rs_) {						\
				case 0: intMFC2();	break;				\
				case 2: intCFC2();	break;				\
				case 4: intMTC2(); 	break;				\
				case 6: intCTC2();	break;				\
				}							\
				break;							\
			case 1: intRTPS(); break;					\
			case 6: intNCLIP(); break;					\
			case 12: intOP(); break;					\
			case 16: intDPCS(); break;					\
			case 17: intINTPL(); break;					\
			case 18: intMVMVA(); break;					\
			case 19: intNCDS(); break;					\
			case 20: intCDP(); break;					\
			case 22: intNCDT(); break;					\
			case 27: intNCCS(); break;					\
			case 28: intCC(); break;					\
			case 30: intNCS(); break;					\
			case 32: intNCT(); break;					\
			case 40: intSQR(); break;					\
			case 41: intDCPL(); break;					\
			case 42: intDPCT(); break;					\
			case 45: intAVSZ3(); break;					\
			case 46: intAVSZ4(); break;					\
			case 48: intRTPT(); break;					\
			case 61: intGPF(); break;					\
			case 62: intGPL(); break;					\
			case 63: intNCCT(); break;					\
		}									\

void psxCOP2() {
	intCOP2()
}

void gteLWC2(u32 code, u32 pc);
void psxLWC2()
{
	gteLWC2(psxRegs->code, psxRegs->pc);
}
void gteSWC2(u32 code, u32 pc);
void psxSWC2()
{
	gteSWC2(psxRegs->code, psxRegs->pc);
}

void (*psxBSC[64])() = {
	psxSPECIAL, psxREGIMM, psxJ   , psxJAL  , psxBEQ , psxBNE , psxBLEZ, psxBGTZ,
	psxADDI   , psxADDIU , psxSLTI, psxSLTIU, psxANDI, psxORI , psxXORI, psxLUI ,
	psxCOP0   , psxNULL  , psxCOP2, psxNULL , psxNULL, psxNULL, psxNULL, psxNULL,
	psxNULL   , psxNULL  , psxNULL, psxNULL , psxNULL, psxNULL, psxNULL, psxNULL,
	psxLB     , psxLH    , psxLWL , psxLW   , psxLBU , psxLHU , psxLWR , psxNULL,
	psxSB     , psxSH    , psxSWL , psxSW   , psxNULL, psxNULL, psxSWR , psxNULL,
	psxNULL   , psxNULL  , psxLWC2, psxNULL , psxNULL, psxNULL, psxNULL, psxNULL,
	psxNULL   , psxNULL  , psxSWC2, psxNULL , psxNULL, psxNULL, psxNULL, psxNULL
};


void (*psxSPC[64])() = {
	psxSLL , psxNULL , psxSRL , psxSRA , psxSLLV   , psxNULL , psxSRLV, psxSRAV,
	psxJR  , psxJALR , psxNULL, psxNULL, psxSYSCALL, psxBREAK, psxNULL, psxNULL,
	psxMFHI, psxMTHI , psxMFLO, psxMTLO, psxNULL   , psxNULL , psxNULL, psxNULL,
	psxMULT, psxMULTU, psxDIV , psxDIVU, psxNULL   , psxNULL , psxNULL, psxNULL,
	psxADD , psxADDU , psxSUB , psxSUBU, psxAND    , psxOR   , psxXOR , psxNOR ,
	psxNULL, psxNULL , psxSLT , psxSLTU, psxNULL   , psxNULL , psxNULL, psxNULL,
	psxNULL, psxNULL , psxNULL, psxNULL, psxNULL   , psxNULL , psxNULL, psxNULL,
	psxNULL, psxNULL , psxNULL, psxNULL, psxNULL   , psxNULL , psxNULL, psxNULL
};

void (*psxREG[32])() = {
	psxBLTZ  , psxBGEZ  , psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL,
	psxNULL  , psxNULL  , psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL,
	psxBLTZAL, psxBGEZAL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL,
	psxNULL  , psxNULL  , psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL
};

void (*psxCP0[32])() = {
	psxMFC0, psxNULL, psxCFC0, psxNULL, psxMTC0, psxNULL, psxCTC0, psxNULL,
	psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL,
	psxRFE , psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL,
	psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL
};

/*
void (*psxCP2[64])() = {
	psxBASIC, gteRTPS , psxNULL , psxNULL, psxNULL, psxNULL , gteNCLIP, psxNULL, // 00
	psxNULL , psxNULL , psxNULL , psxNULL, gteOP  , psxNULL , psxNULL , psxNULL, // 08
	gteDPCS , gteINTPL, gteMVMVA, gteNCDS, gteCDP , psxNULL , gteNCDT , psxNULL, // 10
	psxNULL , psxNULL , psxNULL , gteNCCS, gteCC  , psxNULL , gteNCS  , psxNULL, // 18
	gteNCT  , psxNULL , psxNULL , psxNULL, psxNULL, psxNULL , psxNULL , psxNULL, // 20
	gteSQR  , gteDCPL , gteDPCT , psxNULL, psxNULL, gteAVSZ3, gteAVSZ4, psxNULL, // 28
	gteRTPT , psxNULL , psxNULL , psxNULL, psxNULL, psxNULL , psxNULL , psxNULL, // 30
	psxNULL , psxNULL , psxNULL , psxNULL, psxNULL, gteGPF  , gteGPL  , gteNCCT  // 38
};

void (*psxCP2BSC[32])() = {
	gteMFC2, psxNULL, gteCFC2, psxNULL, gteMTC2, psxNULL, gteCTC2, psxNULL,
	psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL,
	psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL,
	psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL
};
*/

///////////////////////////////////////////

static int intInit() {
#ifdef INTERP
	psxRegs = &intRegs;
#endif
	return 0;
}

static void intReset() {
}

static void intExecute() {
	pc = oldpc = psxRegs->pc;
	for (;;) execI();
}

static void intExecuteBlock() {
	pc = oldpc = psxRegs->pc;
	branch2 = 0;
	while (branch2 == 0) execI();
}

void intExecuteHLE()
{
	while(psxRegs->pc != 0x80001000)
	{
		pc = oldpc = psxRegs->pc;
		branch2 = 0;
		while (branch2 == 0 && psxRegs->pc != 0x80001000) execI();
	}
}

static void intClear(u32 Addr, u32 Size) {
}

static void intShutdown() {
}

R3000Acpu psxInt = {
	intInit,
	intReset,
	intExecute,
	intExecuteBlock,
	intClear,
	intShutdown
};

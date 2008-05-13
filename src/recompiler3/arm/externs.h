#ifndef __ARM_EXTERNS_H__
#define __ARM_EXTERNS_H__

typedef struct {
	u32 	mappedto;
	u32		arm_age;
	u32		arm_use;
	u32		arm_type;
	bool	ismapped;
	bool	arm_islocked;
} ARM_RecRegister;


typedef struct {
	u32 		mappedto;
	bool		ismapped;
	bool		mips_ischanged;
} MIPS_RecRegister;


typedef struct {
	MIPS_RecRegister 	mips[32];
	ARM_RecRegister		arm[16];
	u32								reglist[16];
	u32								reglist_cnt;
} RecRegisters;


extern psxRegisters 	recRegs;
extern psxRegisters* 	psxRegs;
extern RecRegisters 	regcache;

extern u32 psxRecLUT[0x010000];
extern u8 recMemBase[RECMEM_SIZE];
extern s8 recRAM[0x200000];				/* and the ptr to the blocks here */
extern s8 recROM[0x080000];				/* and here */
extern u32 pc;						/* recompiler pc */
extern u32 oldpc;
extern u32 branch;
extern u32 rec_count;

extern u32 stores[4];                                                         
extern u32 rotations[4];                                                      

#ifdef WITH_REG_STATS
extern u32 reg_count[32];
extern u32 reg_mapped_count[32];
#endif

#ifdef WITH_DISASM
extern FILE *translation_log_fp;
extern char disasm_buffer[512];
#endif

/* Functions */
extern void rec_test_pc();
extern void rec_flush_cache();
#ifdef GIZMONDO
#define clear_insn_cache(BEG,END,FLAG) CLEAR_INSN_CACHE(BEG,END,FLAG)
#else
extern "C" void clear_insn_cache(u32 BEG, u32 END, u32 FLAG);
#endif

extern u32 recIntExecuteBlock(u32 newpc);
extern u32 psxBranchTest_rec(u32 cycles, u32 pc);
//extern u32 arm_disect_imm_32bit(u32 imm, u32 *stores, u32 *rotations);

//extern void regReset();
//extern void regClearJump();
//extern void ClearReg(u32 ra, u32 clearthis);

extern u32 *recMem;
static u32 recRecompile();

#endif

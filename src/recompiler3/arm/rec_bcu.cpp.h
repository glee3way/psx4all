static void recSYSCALL()
{
	regClearJump();

	LoadImmediate32(pc - 4, TEMP_1);

	ARM_STR_IMM(ARM_POINTER, TEMP_1, PERM_REG_1, 648);
	//LoadImmediate32(pc - 4, ARMREG_R2);

	ARM_MOV_REG_IMM(ARM_POINTER, ARMREG_R1, (branch == 1 ? 1 : 0), 0);
	ARM_MOV_REG_IMM(ARM_POINTER, ARMREG_R0, 0x20, 0);
	CALLFunc((u32)psxException);
	ARM_MOV_REG_REG(ARM_POINTER, ARMREG_R1, ARMREG_R0);

	LoadImmediate32((blockcycles+((pc-oldpc)/4)), ARMREG_R0);

	CALLFunc((u32)psxBranchTest_rec);

	end_block = 1;
}

#define rec_recompile_end(cond)													\
{																												\
		ARM_BX(ARM_POINTER, ARMREG_R0);											\
}																												\

/* Set a pending branch */
static INLINE void SetBranch()
{
	branch = 1;
	psxRegs->code = *(u32*)(psxMemRLUT[pc>>16] + (pc&0xffff));
	DISASM_MIPS
	pc+=4;

	recBSC[psxRegs->code>>26]();
	branch = 0;
}

static INLINE void iJumpNormal(u32 branchPC)
{
	branch = 1;
	psxRegs->code = *(u32*)(psxMemRLUT[pc>>16] + (pc&0xffff));
	DISASM_MIPS
	pc+=4;

	recBSC[psxRegs->code>>26]();

	branch = 0;

	regClearJump();
	LoadImmediate32(branchPC, ARMREG_R1);
	LoadImmediate32((blockcycles+((pc-oldpc)/4)), ARMREG_R0);

	CALLFunc((u32)psxBranchTest_rec);

	end_block = 1;
}

static INLINE void iJumpAL(u32 branchPC, u32 linkpc)
{
	branch = 1;
	psxRegs->code = *(u32*)(psxMemRLUT[pc>>16] + (pc&0xffff));
	DISASM_MIPS
	pc+=4;

	recBSC[psxRegs->code>>26]();

	branch = 0;

	regClearJump();
	LoadImmediate32(linkpc, TEMP_1);
	ARM_STR_IMM(ARM_POINTER, TEMP_1, PERM_REG_1, CalcDisp(31));

	LoadImmediate32(branchPC, ARMREG_R1);
	LoadImmediate32((blockcycles+((pc-oldpc)/4)), ARMREG_R0);

	CALLFunc((u32)psxBranchTest_rec);

	end_block = 1;
}

static INLINE void iJump(u32 branchPC)
{
	int i;
	branch = 1;
	psxRegs->code = *(u32*)(psxMemRLUT[pc>>16] + (pc&0xffff));
	DISASM_MIPS
	pc+=4;

	recBSC[psxRegs->code>>26]();

	branch = 0;
	
	if(ibranch > 0)
	{
		regClearJump();
		LoadImmediate32(branchPC, ARMREG_R1);
		LoadImmediate32((blockcycles+((pc-oldpc)/4)), ARMREG_R0);
		CALLFunc((u32)psxBranchTest_rec);
		end_block = 1;
		return;
	}

	ibranch++;
	blockcycles += ((pc-oldpc)/4);
	oldpc = pc = branchPC;
	recClear(branchPC, 1);

#if 0
	LoadImmediate32((u32)&psxRecLUT[0], TEMP_2);
	LoadImmediate32((u32)branchPC, TEMP_3);
	ARM_MOV_REG_IMMSHIFT(ARM_POINTER, TEMP_1, TEMP_3, ARMSHIFT_LSR, 16);
	ARM_LDR_REG_REG_SHIFT(ARM_POINTER, TEMP_2, TEMP_2, TEMP_1, ARMSHIFT_LSL, 2);
	ARM_MOV_REG_IMM(ARM_POINTER, TEMP_1, 0xFC , 0);
	ARM_ORR_REG_IMM(ARM_POINTER, TEMP_1, TEMP_1, 0xFF, 24);
	ARM_AND_REG_REG(ARM_POINTER, TEMP_1, TEMP_3, TEMP_1);
	ARM_LDR_REG_REG(ARM_POINTER, TEMP_1, TEMP_2, TEMP_1);
	ARM_TEQ_REG_IMM(ARM_POINTER, TEMP_1, 0, 0);
	u32* backpatch = (u32*)recMem;
	ARM_B_COND(ARM_POINTER, ARMCOND_EQ, 0);
	regClearBranch();
	/* BX to TEMP_1 (recMem) address if NE */
	ARM_BX(ARM_POINTER, TEMP_1);
	*backpatch |= arm_relative_offset(backpatch, (u32)recMem, 8);
#endif
}

#if 1
static void recBLTZ()
{
// Branch if Rs < 0
	u32 bpc = _Imm_ * 4 + pc;
	u32 nbpc = pc + 4;
//	iFlushRegs();

	if (bpc == nbpc && psxTestLoadDelay(_Rs_, PSXMu32(bpc)) == 0) {
		return;
	}
	
	if(!(_Rs_))
	{
		SetBranch(); return;
	}

	u32 br1 = regMipsToArm(_Rs_, REG_LOADBRANCH, REG_REGISTERBRANCH);
	SetBranch();
	ARM_CMP_REG_IMM(ARM_POINTER, br1, 0, 0);
	u32* backpatch = (u32*)recMem;
	ARM_B_COND(ARM_POINTER, ARMCOND_GE, 0);

	regClearBranch();
	LoadImmediate32(bpc, ARMREG_R1);
	LoadImmediate32((blockcycles+((pc-oldpc)/4)), ARMREG_R0);
	CALLFunc_Branch((u32)psxBranchTest_rec);

	*backpatch |= arm_relative_offset(backpatch, (u32)recMem, 8);
	regBranchUnlock(br1);
}

static void recBGTZ()
{
// Branch if Rs > 0
	u32 bpc = _Imm_ * 4 + pc;
	u32 nbpc = pc + 4;
//	iFlushRegs();

	if (bpc == nbpc && psxTestLoadDelay(_Rs_, PSXMu32(bpc)) == 0) {
		return;
	}
	
	if(!(_Rs_))
	{
		SetBranch(); return;
	}

	u32 br1 = regMipsToArm(_Rs_, REG_LOADBRANCH, REG_REGISTERBRANCH);
	SetBranch();
	ARM_CMP_REG_IMM(ARM_POINTER, br1, 0, 0);
	u32* backpatch = (u32*)recMem;
	ARM_B_COND(ARM_POINTER, ARMCOND_LE, 0);

	regClearBranch();
	LoadImmediate32(bpc, ARMREG_R1);
	LoadImmediate32((blockcycles+((pc-oldpc)/4)), ARMREG_R0);
	CALLFunc_Branch((u32)psxBranchTest_rec);

	*backpatch |= arm_relative_offset(backpatch, (u32)recMem, 8);
	regBranchUnlock(br1);
}

static void recBLTZAL()
{
// Branch if Rs < 0
	u32 bpc = _Imm_ * 4 + pc;
	u32 nbpc = pc + 4;
//	iFlushRegs();

	if (bpc == nbpc && psxTestLoadDelay(_Rs_, PSXMu32(bpc)) == 0) {
		return;
	}
	
	if(!(_Rs_))
	{
		SetBranch(); return;
	}

	u32 br1 = regMipsToArm(_Rs_, REG_LOADBRANCH, REG_REGISTERBRANCH);
	SetBranch();
	ARM_CMP_REG_IMM(ARM_POINTER, br1, 0, 0);
	u32* backpatch = (u32*)recMem;
	ARM_B_COND(ARM_POINTER, ARMCOND_GE, 0);

	regClearBranch();
	LoadImmediate32(nbpc, TEMP_1);
	ARM_STR_IMM(ARM_POINTER, TEMP_1, PERM_REG_1, CalcDisp(31));

	LoadImmediate32(bpc, ARMREG_R1);
	LoadImmediate32((blockcycles+((pc-oldpc)/4)), ARMREG_R0);
	CALLFunc_Branch((u32)psxBranchTest_rec);

	*backpatch |= arm_relative_offset(backpatch, (u32)recMem, 8);
	regBranchUnlock(br1);
}

static void recBGEZAL()
{
// Branch if Rs >= 0
	u32 bpc = _Imm_ * 4 + pc;
	u32 nbpc = pc + 4;
//	iFlushRegs();

	if (bpc == nbpc && psxTestLoadDelay(_Rs_, PSXMu32(bpc)) == 0) {
		return;
	}
	
	if(!(_Rs_))
	{
		iJumpAL(bpc, (pc + 4)); return;
	}

	u32 br1 = regMipsToArm(_Rs_, REG_LOADBRANCH, REG_REGISTERBRANCH);
	SetBranch();
	ARM_CMP_REG_IMM(ARM_POINTER, br1, 0, 0);
	u32* backpatch = (u32*)recMem;
	ARM_B_COND(ARM_POINTER, ARMCOND_LT, 0);

	regClearBranch();
	LoadImmediate32(nbpc, TEMP_1);
	ARM_STR_IMM(ARM_POINTER, TEMP_1, PERM_REG_1, CalcDisp(31));

	LoadImmediate32(bpc, ARMREG_R1);
	LoadImmediate32((blockcycles+((pc-oldpc)/4)), ARMREG_R0);
	CALLFunc_Branch((u32)psxBranchTest_rec);

	*backpatch |= arm_relative_offset(backpatch, (u32)recMem, 8);
	regBranchUnlock(br1);
}
#endif

static void recJ()
{
// j target

	iJumpNormal(_Target_ * 4 + (pc & 0xf0000000));
}

static void recJAL()
{
// jal target

	iJumpAL(_Target_ * 4 + (pc & 0xf0000000), (pc + 4));
}


static void recJR()
{
// jr Rs
	u32 br1 = regMipsToArm(_Rs_, REG_LOADBRANCH, REG_REGISTERBRANCH);
	SetBranch();

	ARM_MOV_REG_REG(ARM_POINTER, ARMREG_R1, br1);
	regBranchUnlock(br1);
	regClearJump();
	LoadImmediate32((blockcycles+((pc-oldpc)/4)), ARMREG_R0);
	CALLFunc((u32)psxBranchTest_rec);

	end_block = 1;
}

static void recJALR()
{
// jalr Rs
	u32 br1 = regMipsToArm(_Rs_, REG_LOADBRANCH, REG_REGISTERBRANCH);
	u32 rd = regMipsToArm(_Rd_, REG_FIND, REG_REGISTER);
	LoadImmediate32(pc + 4, rd);
	regMipsChanged(_Rd_);

	SetBranch();
	ARM_MOV_REG_REG(ARM_POINTER, ARMREG_R1, br1);	
	regBranchUnlock(br1);
	regClearJump();
	LoadImmediate32((blockcycles+((pc-oldpc)/4)), ARMREG_R0);
	CALLFunc((u32)psxBranchTest_rec);

	end_block = 1;
}

static void recBEQ()
{
// Branch if Rs == Rt
	u32 bpc = _Imm_ * 4 + pc;
	u32 nbpc = pc + 4;
//	iFlushRegs();

	if (bpc == nbpc && psxTestLoadDelay(_Rs_, PSXMu32(bpc)) == 0) {
		return;
	}
	
	if (_Rs_ == _Rt_)
	{
		iJumpNormal(bpc); return;
	}

	u32 br1 = regMipsToArm(_Rs_, REG_LOADBRANCH, REG_REGISTERBRANCH);
	u32 br2 = regMipsToArm(_Rt_, REG_LOADBRANCH, REG_REGISTERBRANCH);
	SetBranch();
	ARM_CMP_REG_REG(ARM_POINTER, br1, br2);

	u32* backpatch = (u32*)recMem;
	ARM_B_COND(ARM_POINTER, ARMCOND_NE, 0);

	regClearBranch();
	LoadImmediate32(bpc, ARMREG_R1);
	LoadImmediate32((blockcycles+((pc-oldpc)/4)), ARMREG_R0);
	CALLFunc_Branch((u32)psxBranchTest_rec);

	*backpatch |= arm_relative_offset(backpatch, (u32)recMem, 8);
	regBranchUnlock(br1);
	regBranchUnlock(br2);
}

static void recBNE()
{
// Branch if Rs != Rt
	u32 bpc = _Imm_ * 4 + pc;
	u32 nbpc = pc + 4;
//	iFlushRegs();

	if (bpc == nbpc && psxTestLoadDelay(_Rs_, PSXMu32(bpc)) == 0) {
		return;
	}
	
	if(!(_Rs_) && !(_Rt_))
	{
		SetBranch(); return;
	}

	u32 br1 = regMipsToArm(_Rs_, REG_LOADBRANCH, REG_REGISTERBRANCH);
	u32 br2 = regMipsToArm(_Rt_, REG_LOADBRANCH, REG_REGISTERBRANCH);
	SetBranch();
	ARM_CMP_REG_REG(ARM_POINTER, br1, br2);

	u32* backpatch = (u32*)recMem;
	ARM_B_COND(ARM_POINTER, ARMCOND_EQ, 0);

	regClearBranch();
	LoadImmediate32(bpc, ARMREG_R1);
	LoadImmediate32((blockcycles+((pc-oldpc)/4)), ARMREG_R0);
	CALLFunc_Branch((u32)psxBranchTest_rec);

	*backpatch |= arm_relative_offset(backpatch, (u32)recMem, 8);
	regBranchUnlock(br1);
	regBranchUnlock(br2);
}

#if 1
static void recBLEZ()
{
// Branch if Rs <= 0
	u32 bpc = _Imm_ * 4 + pc;
	u32 nbpc = pc + 4;
//	iFlushRegs();

	if (bpc == nbpc && psxTestLoadDelay(_Rs_, PSXMu32(bpc)) == 0) {
		return;
	}
	
	if(!(_Rs_))
	{
		iJumpNormal(bpc); return;
	}

	u32 br1 = regMipsToArm(_Rs_, REG_LOADBRANCH, REG_REGISTERBRANCH);
	SetBranch();
	ARM_CMP_REG_IMM(ARM_POINTER, br1, 0, 0);

	u32* backpatch = (u32*)recMem;
	ARM_B_COND(ARM_POINTER, ARMCOND_GT, 0);

	regClearBranch();
	LoadImmediate32(bpc, ARMREG_R1);
	LoadImmediate32((blockcycles+((pc-oldpc)/4)), ARMREG_R0);

	CALLFunc_Branch((u32)psxBranchTest_rec);

	*backpatch |= arm_relative_offset(backpatch, (u32)recMem, 8);
	regBranchUnlock(br1);
}

static void recBGEZ()
{
// Branch if Rs >= 0
	u32 bpc = _Imm_ * 4 + pc;
	u32 nbpc = pc + 4;
//	iFlushRegs();

	if (bpc == nbpc && psxTestLoadDelay(_Rs_, PSXMu32(bpc)) == 0) {
		return;
	}
	
	if(!(_Rs_))
	{
		iJumpNormal(bpc); return;
	}

	u32 br1 = regMipsToArm(_Rs_, REG_LOADBRANCH, REG_REGISTERBRANCH);
	SetBranch();
	ARM_CMP_REG_IMM(ARM_POINTER, br1, 0, 0);

	u32* backpatch = (u32*)recMem;
	ARM_B_COND(ARM_POINTER, ARMCOND_LT, 0);

	regClearBranch();
	LoadImmediate32(bpc, ARMREG_R1);
	LoadImmediate32((blockcycles+((pc-oldpc)/4)), ARMREG_R0);

	CALLFunc_Branch((u32)psxBranchTest_rec);

	*backpatch |= arm_relative_offset(backpatch, (u32)recMem, 8);
	regBranchUnlock(br1);
}
#endif

static void recRet()
{
	rec_recompile_end(ARMCOND_AL);
}

static void recBREAK() { }

static void recHLE() 
{
	regClearJump();
	
	/* Needed? */
	LoadImmediate32(pc, TEMP_1);
	ARM_STR_IMM(ARM_POINTER, TEMP_1, PERM_REG_1, 648);

	LoadImmediate32((blockcycles+((pc-oldpc)/4)), ARMREG_R0);
	CALLFunc((u32)psxHLEt[psxRegs->code & 0xffff]);

	end_block = 1;
}

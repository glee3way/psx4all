#if 1
static INLINE void iPushOfB()
{
	s32 imm16 = (s32)(s16)_Imm_;
	u32 rs = _Rs_;
	u32 r1;
	if (rs)
	{
		r1 = regMipsToArm(rs, REG_LOAD, REG_REGISTER);
		if (imm16)
		{
			gen(ADDI, ARMREG_R0, r1, imm16);
		}
		else
		{
			gen(MOV, ARMREG_R0, r1);
		}
	}
	else
	{
		gen(MOVI16, ARMREG_R0, imm16);
	}
}

#if 0
static void recLB()
{
// Rt = mem[Rs + Im] (signed)
	u32 rt = _Rt_;

	iPushOfB();
	ARM_MOV_REG_IMM(ARM_POINTER, T1, 16, 32-4);
	ARM_ADD_REG_IMMSHIFT(ARM_POINTER, T1, T1, T0, ARMSHIFT_LSR, 28);
	ARM_LDR_REG_REG_SHIFT(ARM_POINTER, T1, 11, T1, ARMSHIFT_LSL, 2);
	ARM_CMP_REG_IMM8(ARM_POINTER, T1, 0);
	ARM_LDRSB_REG_REG_COND(ARM_POINTER, T0, T0, T1, ARMCOND_NE);
	ARM_BL_COND(ARM_POINTER, ARMCOND_EQ, arm_relative_offset(recMem, (u32)psxMemReadS8, 8));
	if (rt)
	{		
		u32 r1 = regMipsToArm(rt, REG_FIND, REG_REGISTER);
		ARM_MOV_REG_REG(ARM_POINTER, r1, T0);
		regMipsChanged(rt);
	}
}
#else
static void recLB()
{
// Rt = mem[Rs + Im] (signed)
	u32 rt = _Rt_;

	iPushOfB();
	CALLFunc_NoFlush((u32)psxMemReadS8);
	if (rt)
	{
		u32 r1 = regMipsToArm(rt, REG_FIND, REG_REGISTER);
		ARM_MOV_REG_REG(ARM_POINTER, r1, ARMREG_R0);
		regMipsChanged(rt);
	}
}
#endif

#if 0
static void recLBU()
{
	u32 rt = _Rt_;

	iPushOfB();
	ARM_MOV_REG_IMM(ARM_POINTER, T1, 16, 32-4);
	ARM_ADD_REG_IMMSHIFT(ARM_POINTER, T1, T1, T0, ARMSHIFT_LSR, 28);
	ARM_LDR_REG_REG_SHIFT(ARM_POINTER, T1, 11, T1, ARMSHIFT_LSL, 2);
	ARM_CMP_REG_IMM8(ARM_POINTER, T1, 0);
	ARM_LDRB_REG_REG_SHIFT_COND(ARM_POINTER, T0, T0, T1, ARMSHIFT_LSL, 0, ARMCOND_NE);
	ARM_BL_COND(ARM_POINTER, ARMCOND_EQ, arm_relative_offset(recMem, (u32)psxMemRead8, 8));
	if (rt)
	{		
		u32 r1 = regMipsToArm(rt, REG_FIND, REG_REGISTER);
		ARM_MOV_REG_REG(ARM_POINTER, r1, T0);
		regMipsChanged(rt);
	}
}
#else
static void recLBU()
{
// Rt = mem[Rs + Im] (unsigned)
	u32 rt = _Rt_;

	iPushOfB();

	CALLFunc_NoFlush((u32)psxMemRead8);
	if (rt)
	{
		u32 r1 = regMipsToArm(rt, REG_FIND, REG_REGISTER);
		ARM_MOV_REG_REG(ARM_POINTER, r1, ARMREG_R0);
		regMipsChanged(rt);
	}
}
#endif

#if 0
static void recLH()
{
	u32 rt = _Rt_;

	iPushOfB();
	ARM_MOV_REG_IMM(ARM_POINTER, T1, 16, 32-4);
	ARM_ADD_REG_IMMSHIFT(ARM_POINTER, T1, T1, T0, ARMSHIFT_LSR, 28);
	ARM_LDR_REG_REG_SHIFT(ARM_POINTER, T1, 11, T1, ARMSHIFT_LSL, 2);
	ARM_CMP_REG_IMM8(ARM_POINTER, T1, 0);
	ARM_LDRSH_REG_REG_COND(ARM_POINTER, T0, T0, T1, ARMCOND_NE);
	ARM_BL_COND(ARM_POINTER, ARMCOND_EQ, arm_relative_offset(recMem, (u32)psxMemReadS16, 8));
	if (rt)
	{		
		u32 r1 = regMipsToArm(rt, REG_FIND, REG_REGISTER);
		ARM_MOV_REG_REG(ARM_POINTER, r1, T0);
		regMipsChanged(rt);
	}
}
#else
static void recLH()
{
// Rt = mem[Rs + Im] (signed)
	u32 rt = _Rt_;

	iPushOfB();
	CALLFunc_NoFlush((u32)psxMemReadS16);
	if (rt)
	{
		u32 r1 = regMipsToArm(rt, REG_FIND, REG_REGISTER);
		ARM_MOV_REG_REG(ARM_POINTER, r1, ARMREG_R0);
		regMipsChanged(rt);
	}
}
#endif

#if 0
static void recLHU()
{
	u32 rt = _Rt_;

	iPushOfB();
	ARM_MOV_REG_IMM(ARM_POINTER, T1, 16, 32-4);
	ARM_ADD_REG_IMMSHIFT(ARM_POINTER, T1, T1, T0, ARMSHIFT_LSR, 28);
	ARM_LDR_REG_REG_SHIFT(ARM_POINTER, T1, 11, T1, ARMSHIFT_LSL, 2);
	ARM_CMP_REG_IMM8(ARM_POINTER, T1, 0);
	ARM_LDRH_REG_REG_COND(ARM_POINTER, T0, T0, T1, ARMCOND_NE);
	ARM_BL_COND(ARM_POINTER, ARMCOND_EQ, arm_relative_offset(recMem, (u32)psxMemRead16, 8));
	if (rt)
	{		
		u32 r1 = regMipsToArm(rt, REG_FIND, REG_REGISTER);
		ARM_MOV_REG_REG(ARM_POINTER, r1, ARMREG_R0);
		regMipsChanged(rt);
	}
}
#else
static void recLHU()
{
// Rt = mem[Rs + Im] (unsigned)
	u32 rt = _Rt_;

	iPushOfB();
	CALLFunc_NoFlush((u32)psxMemRead16);
	if (rt)
	{
		u32 r1 = regMipsToArm(rt, REG_FIND, REG_REGISTER);
		ARM_MOV_REG_REG(ARM_POINTER, r1, ARMREG_R0);
		regMipsChanged(rt);
	}
}
#endif

#if 0
static void recLW()
{
	u32 rt = _Rt_;

	iPushOfB();
	ARM_MOV_REG_IMM(ARM_POINTER, T1, 16, 32-4);
	ARM_ADD_REG_IMMSHIFT(ARM_POINTER, T1, T1, T0, ARMSHIFT_LSR, 28);
	ARM_LDR_REG_REG_SHIFT(ARM_POINTER, T1, 11, T1, ARMSHIFT_LSL, 2);
	ARM_CMP_REG_IMM8(ARM_POINTER, T1, 0);
	ARM_LDR_REG_REG_SHIFT_COND(ARM_POINTER, T0, T0, T1, ARMSHIFT_LSL, 0, ARMCOND_NE);
	ARM_BL_COND(ARM_POINTER, ARMCOND_EQ, arm_relative_offset(recMem, (u32)psxMemRead32, 8));
	if (rt)
	{		
		u32 r1 = regMipsToArm(rt, REG_FIND, REG_REGISTER);
		ARM_MOV_REG_REG(ARM_POINTER, r1, T0);
		regMipsChanged(rt);
	}
}
#else
static void recLW()
{
// Rt = mem[Rs + Im] (unsigned)
	u32 rt = _Rt_;

	iPushOfB();
	CALLFunc_NoFlush((u32)psxMemRead32);
	if (rt)
	{
		u32 r1 = regMipsToArm(rt, REG_FIND, REG_REGISTER);
		ARM_MOV_REG_REG(ARM_POINTER, r1, ARMREG_R0);
		regMipsChanged(rt);
	}
}
#endif

#if 1
REC_FUNC_TEST(LWL);
#else
static void recLWL()
{
	/*
	mem = 1234 / reg = abcd

	LWL (MIPS) :
	------------
	0   4bcd   (mem << 24) | (reg & 0x00ffffff)
	1   34cd   (mem << 16) | (reg & 0x0000ffff)
	2   234d   (mem <<  8) | (reg & 0x000000ff)
	3   1234   (mem      ) | (reg & 0x00000000)
	
	AND		Ri, R0, #3         // ri = r0 & 0x00000003;
	BIC		R0, R0, #3         // r0 = r0 & 0xFFFFFFFC;
	RSB		Ri, Ri, #3         // ri = 3 - ri;
	MOV		Ri, Ri, LSL #3     // ri *= 8;
	LDR		R0, [R0]           // r0 = *((u32 *)r0);
	MVN		R1, #0             // r1 = 0xFFFFFFFF;
	BIC		Rt, Rt, R1, LSL Ri // rt = reg & ~(0xFFFFFFFF << ri);
	ORR		Rt, Rt, R0, LSL Ri // reg = (mem << ri) | (reg & ~(0xFFFFFFFF << ri));
	*/

	u32 rt = _Rt_;
	iPushOfB();
	u32 r1 = LoadRegister(rt);
	u32 r2 = AllocateTempRegister();
	ARM_MOV_REG_IMM(ARM_POINTER, T1, 16, 32-4);
	ARM_AND_REG_IMM8(ARM_POINTER, r2, T0, 3);
	ARM_BIC_REG_IMM8(ARM_POINTER, T0, T0, 3);
	ARM_RSB_REG_IMM8(ARM_POINTER, r2, r2, 3);
	ARM_ADD_REG_IMMSHIFT(ARM_POINTER, T1, T1, T0, ARMSHIFT_LSR, 28);
	ARM_MOV_REG_IMMSHIFT(ARM_POINTER, r2, r2, ARMSHIFT_LSL, 3);
	ARM_LDR_REG_REG_SHIFT(ARM_POINTER, T1, 11, T1, ARMSHIFT_LSL, 2);
	ARM_CMP_REG_IMM8(ARM_POINTER, T1, 0);
	ARM_LDR_REG_REG_SHIFT_COND(ARM_POINTER, T0, T0, T1, ARMSHIFT_LSL, 0, ARMCOND_NE);
	ARM_BL_COND(ARM_POINTER, ARMCOND_EQ, arm_relative_offset(recMem, (u32)psxMemRead32, 8));
	if (rt)
	{	
		ARM_MVN_REG_IMM8(ARM_POINTER, T1, 0);
		ARM_BIC_REG_REGSHIFT(ARM_POINTER, r1, r1, T1, ARMSHIFT_LSL, r2);
		ARM_ORR_REG_REGSHIFT(ARM_POINTER, r1, r1, T0, ARMSHIFT_LSL, r2);
		armRegs[r1].changed = true;
	}
}
#endif

#if 1
REC_FUNC_TEST(LWR);
#else
static void recLWR()
{
	/*
	mem = 1234 / reg = abcd

	LDR (ARM) :
	-----------
	0   1234 (mem      )
	1   4123 (mem @>  8)
	2   3412 (mem @> 16)
	3   2341 (mem @> 24)
	
	LWR (MIPS) :
	------------
	0   1234 (mem      ) | (reg & 0x00000000)
	1   a123 (mem >>  8) | (reg & 0xff000000)
	2   ab12 (mem >> 16) | (reg & 0xffff0000)
	3   abc1 (mem >> 24) | (reg & 0xffffff00)
	
	AND		Ri, R0, #3         // ri = r0 & 0x00000003;
	BIC		R0, R0, #3         // r0 = r0 & 0xFFFFFFFC;
	MOV		Ri, Ri, LSL #3     // ri *= 8;
	LDR		R0, [R0]           // r0 = *((u32 *)r0);
	MVN		R1, #0             // r1 = 0xFFFFFFFF;
	BIC		Rt, Rt, R1, LSR Ri // rt = reg & ~(0xFFFFFFFF >> ri);
	ORR		Rt, Rt, R0, LSR Ri // reg = (mem >> ri) | (reg & ~(0xFFFFFFFF >> ri));
	*/

	u32 rt = _Rt_;
	iPushOfB();
	u32 r1 = LoadRegister(rt);
	u32 r2 = AllocateTempRegister();
	ARM_MOV_REG_IMM(ARM_POINTER, T1, 16, 32-4);
	ARM_AND_REG_IMM8(ARM_POINTER, r2, T0, 3);
	ARM_BIC_REG_IMM8(ARM_POINTER, T0, T0, 3);
	ARM_ADD_REG_IMMSHIFT(ARM_POINTER, T1, T1, T0, ARMSHIFT_LSR, 28);
	ARM_MOV_REG_IMMSHIFT(ARM_POINTER, r2, r2, ARMSHIFT_LSL, 3);
	ARM_LDR_REG_REG_SHIFT(ARM_POINTER, T1, 11, T1, ARMSHIFT_LSL, 2);
	ARM_CMP_REG_IMM8(ARM_POINTER, T1, 0);
	ARM_LDR_REG_REG_SHIFT_COND(ARM_POINTER, T0, T0, T1, ARMSHIFT_LSL, 0, ARMCOND_NE);
	ARM_BL_COND(ARM_POINTER, ARMCOND_EQ, arm_relative_offset(recMem, (u32)psxMemRead32, 8));
	if (rt)
	{	
		ARM_MVN_REG_IMM8(ARM_POINTER, T1, 0);
		ARM_BIC_REG_REGSHIFT(ARM_POINTER, r1, r1, T1, ARMSHIFT_LSR, r2);
		ARM_ORR_REG_REGSHIFT(ARM_POINTER, r1, r1, T0, ARMSHIFT_LSR, r2);
		armRegs[r1].changed = true;
	}
}
#endif

#if 0
static void recSB()
{
	u32 rt = _Rt_;

	iPushOfB();
	if (rt)
	{
		u32 r1 = regMipsToArm(rt, REG_LOAD, REG_REGISTER);
		ARM_MOV_REG_REG(ARM_POINTER, ARMREG_R1, r1);
	}
	else
	{
		ARM_MOV_REG_IMM8(ARM_POINTER, ARMREG_R1, 0);
	}
	ARM_MOV_REG_IMM(ARM_POINTER, T2, 17, 32-4);
	ARM_ADD_REG_IMMSHIFT(ARM_POINTER, T2, T2, T0, ARMSHIFT_LSR, 28);
	ARM_LDR_REG_REG_SHIFT(ARM_POINTER, T2, 11, T2, ARMSHIFT_LSL, 2);
	ARM_CMP_REG_IMM8(ARM_POINTER, T2, 0);
	ARM_STRB_REG_REG_SHIFT_COND(ARM_POINTER, T1, T0, T2, ARMSHIFT_LSL, 0, ARMCOND_NE);
	ARM_BL_COND(ARM_POINTER, ARMCOND_EQ, arm_relative_offset(recMem, (u32)psxMemWrite8, 8));
}
#else

static void recSB()
{
// mem[Rs + Im] = Rt
	u32 rt = _Rt_;

	iPushOfB();	
	if (rt)
	{
		u32 r1 = regMipsToArm(rt, REG_LOAD, REG_REGISTER);
		ARM_MOV_REG_REG(ARM_POINTER, ARMREG_R1, r1);
	}
	else
	{
		ARM_MOV_REG_IMM8(ARM_POINTER, ARMREG_R1, 0);
	}
	CALLFunc_NoFlush((u32)psxMemWrite8);
}
#endif

#if 0
static void recSH()
{
	u32 rt = _Rt_;

	iPushOfB();
	if (rt)
	{
		u32 r1 = regMipsToArm(rt, REG_LOAD, REG_REGISTER);
		ARM_MOV_REG_REG(ARM_POINTER, T1, r1);
	}
	else
	{
		ARM_MOV_REG_IMM8(ARM_POINTER, T1, 0);
	}
	ARM_MOV_REG_IMM(ARM_POINTER, T2, 17, 32-4);
	ARM_ADD_REG_IMMSHIFT(ARM_POINTER, T2, T2, T0, ARMSHIFT_LSR, 28);
	ARM_LDR_REG_REG_SHIFT(ARM_POINTER, T2, 11, T2, ARMSHIFT_LSL, 2);
	ARM_CMP_REG_IMM8(ARM_POINTER, T2, 0);
	ARM_STRH_REG_REG_COND(ARM_POINTER, T1, T0, T2, ARMCOND_NE);
	ARM_BL_COND(ARM_POINTER, ARMCOND_EQ, arm_relative_offset(recMem, (u32)psxMemWrite16, 8));
}
#else
static void recSH()
{
// mem[Rs + Im] = Rt
	u32 rt = _Rt_;

	iPushOfB();	
	if (rt)
	{
		u32 r1 = regMipsToArm(rt, REG_LOAD, REG_REGISTER);
		ARM_MOV_REG_REG(ARM_POINTER, ARMREG_R1, r1);
	}
	else
	{
		ARM_MOV_REG_IMM8(ARM_POINTER, ARMREG_R1, 0);
	}
	CALLFunc_NoFlush((u32)psxMemWrite16);
}
#endif

#if 0
static void recSW()
{
	u32 rt = _Rt_;

	iPushOfB();
	if (rt)
	{
		u32 r1 = regMipsToArm(rt, REG_LOAD, REG_REGISTER);
		ARM_MOV_REG_REG(ARM_POINTER, T1, r1);
	}
	else
	{
		ARM_MOV_REG_IMM8(ARM_POINTER, T1, 0);
	}
	ARM_MOV_REG_IMM(ARM_POINTER, T2, 17, 32-4);
	ARM_ADD_REG_IMMSHIFT(ARM_POINTER, T2, T2, T0, ARMSHIFT_LSR, 28);
	ARM_LDR_REG_REG_SHIFT(ARM_POINTER, T2, 11, T2, ARMSHIFT_LSL, 2);
	ARM_CMP_REG_IMM8(ARM_POINTER, T2, 0);
	ARM_STR_REG_REG_SHIFT_COND(ARM_POINTER, T1, T0, T2, ARMSHIFT_LSL, 0, ARMCOND_NE);
	ARM_BL_COND(ARM_POINTER, ARMCOND_EQ, arm_relative_offset(recMem, (u32)psxMemWrite32, 8));
}
#else
static void recSW()
{
// mem[Rs + Im] = Rt
	u32 rt = _Rt_;

	iPushOfB();
	if (rt)
	{
		u32 r1 = regMipsToArm(rt, REG_LOAD, REG_REGISTER);
		ARM_MOV_REG_REG(ARM_POINTER, ARMREG_R1, r1);
	}
	else
	{
		ARM_MOV_REG_IMM8(ARM_POINTER, ARMREG_R1, 0);
	}
	CALLFunc_NoFlush((u32)psxMemWrite32);
}
#endif

#if 1
REC_FUNC_TEST(SWL);
#else
static void recSWL()
{
	/*
	mem = 1234 / reg = abcd

	0   123a (reg >> 24) | (mem & 0xffffff00)
	1   12ab (reg >> 16) | (mem & 0xffff0000)
	2   1abc (reg >>  8) | (mem & 0xff000000)
	3   abcd (reg      ) | (mem & 0x00000000)
	*/

	u32 rt = _Rt_;
	iPushOfB();
	u32 r1 = LoadRegister(rt);
	u32 r2 = AllocateTempRegister();
	u32 r3 = AllocateTempRegister();
	u32 r4 = AllocateTempRegister();
	ARM_MOV_REG_IMM(ARM_POINTER, T1, 16, 32-4);
	ARM_AND_REG_IMM8(ARM_POINTER, r2, T0, 3);
	ARM_BIC_REG_IMM8(ARM_POINTER, T0, T0, 3);
	ARM_RSB_REG_IMM8(ARM_POINTER, r2, r2, 3);
	ARM_ADD_REG_IMMSHIFT(ARM_POINTER, r3, T1, T0, ARMSHIFT_LSR, 28);
	ARM_MOV_REG_IMMSHIFT(ARM_POINTER, r2, r2, ARMSHIFT_LSL, 3);
	ARM_LDR_REG_REG_SHIFT(ARM_POINTER, T1, 11, r3, ARMSHIFT_LSL, 2);
	ARM_CMP_REG_IMM8(ARM_POINTER, T1, 0);
	ARM_MOV_REG_REG(ARM_POINTER, r4, T0);
	ARM_LDR_REG_REG_SHIFT_COND(ARM_POINTER, T0, T0, T1, ARMSHIFT_LSL, 0, ARMCOND_NE);
	ARM_BL_COND(ARM_POINTER, ARMCOND_EQ, arm_relative_offset(recMem, (u32)psxMemRead32, 8));
	ARM_MVN_REG_IMM8(ARM_POINTER, T1, 0);
	ARM_ADD_REG_IMM8(ARM_POINTER, T2, r3, 16);
	ARM_BIC_REG_REGSHIFT(ARM_POINTER, T1, T0, T1, ARMSHIFT_LSR, r2);
	ARM_MOV_REG_REG(ARM_POINTER, T0, r4);
	ARM_ORR_REG_REGSHIFT(ARM_POINTER, T1, T1, r1, ARMSHIFT_LSR, r2);
	ARM_LDR_REG_REG_SHIFT(ARM_POINTER, T2, 11, T2, ARMSHIFT_LSL, 2);
	ARM_CMP_REG_IMM8(ARM_POINTER, T2, 0);
	ARM_STR_REG_REG_SHIFT_COND(ARM_POINTER, T1, T0, T2, ARMSHIFT_LSL, 0, ARMCOND_NE);
	ARM_BL_COND(ARM_POINTER, ARMCOND_EQ, arm_relative_offset(recMem, (u32)psxMemWrite32, 8));
}
#endif

#if 1
REC_FUNC_TEST(SWR);
#endif

#else

REC_FUNC_TEST(LB);
REC_FUNC_TEST(LBU);
REC_FUNC_TEST(LH);
REC_FUNC_TEST(LHU);
REC_FUNC_TEST(LW);
REC_FUNC_TEST(SB);
REC_FUNC_TEST(SH);
REC_FUNC_TEST(SW);
REC_FUNC_TEST(LWL);
REC_FUNC_TEST(LWR);
REC_FUNC_TEST(SWL);
REC_FUNC_TEST(SWR);
#endif

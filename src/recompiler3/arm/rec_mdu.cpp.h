#if 1
static void recMULT() {
// Lo/Hi = Rs * Rt (signed)

//	iFlushRegs();
	if(!(_Rt_) || !(_Rs_))
	{
		u32 rsrt;
		if( !(_Rt_) )
		{
			rsrt = regMipsToArm(_Rt_, REG_LOAD, REG_REGISTER);
		}
		else
		{
			rsrt = regMipsToArm(_Rs_, REG_LOAD, REG_REGISTER);
		}
		ARM_STR_IMM(ARM_POINTER, rsrt, PERM_REG_1, 128);
		ARM_STR_IMM(ARM_POINTER, rsrt, PERM_REG_1, 132);		
	}
	else
	{
		u32 rs = regMipsToArm(_Rs_, REG_LOAD, REG_REGISTER);
		u32	rt = regMipsToArm(_Rt_, REG_LOAD, REG_REGISTER);
		
		ARM_SMULL(ARM_POINTER, TEMP_1, TEMP_2, rs, rt);
		ARM_STR_IMM(ARM_POINTER, TEMP_1, PERM_REG_1, 128);
		ARM_STR_IMM(ARM_POINTER, TEMP_2, PERM_REG_1, 132);
	}
}

static void recMULTU() {
// Lo/Hi = Rs * Rt (unsigned)

//	iFlushRegs();

	if(!(_Rt_) || !(_Rs_))
	{
		u32 rsrt;
		if( !(_Rt_) )
		{
			rsrt = regMipsToArm(_Rt_, REG_LOAD, REG_REGISTER);
		}
		else
		{
			rsrt = regMipsToArm(_Rs_, REG_LOAD, REG_REGISTER);
		}
		ARM_STR_IMM(ARM_POINTER, rsrt, PERM_REG_1, 128);
		ARM_STR_IMM(ARM_POINTER, rsrt, PERM_REG_1, 132);		
	}
	else
	{
		u32 rs = regMipsToArm(_Rs_, REG_LOAD, REG_REGISTER);
		u32 rt = regMipsToArm(_Rt_, REG_LOAD, REG_REGISTER);
		
		ARM_UMULL(ARM_POINTER, TEMP_1, TEMP_2, rs, rt);
		ARM_STR_IMM(ARM_POINTER, TEMP_1, PERM_REG_1, 128);
		ARM_STR_IMM(ARM_POINTER, TEMP_2, PERM_REG_1, 132);
	}
}

REC_FUNC_TEST(DIV);
REC_FUNC_TEST(DIVU);

static void recMFHI() {
// Rd = Hi
	if (!_Rd_) return;
	u32 rd = regMipsToArm(_Rd_, REG_FIND, REG_REGISTER);

	ARM_LDR_IMM(ARM_POINTER, rd, PERM_REG_1, 132);
	regMipsChanged(_Rd_);
}

static void recMTHI() {
// Hi = Rs
	u32 rs = regMipsToArm(_Rs_, REG_LOAD, REG_REGISTER);
	ARM_STR_IMM(ARM_POINTER, rs, PERM_REG_1, 132);
}

static void recMFLO() {
// Rd = Lo
	if (!_Rd_) return;
	u32 rd = regMipsToArm(_Rd_, REG_FIND, REG_REGISTER);

	ARM_LDR_IMM(ARM_POINTER, rd, PERM_REG_1, 128);
	regMipsChanged(_Rd_);
}

static void recMTLO() {
// Lo = Rs
	u32 rs = regMipsToArm(_Rs_, REG_LOAD, REG_REGISTER);
	ARM_STR_IMM(ARM_POINTER, rs, PERM_REG_1, 128);
}
#else


REC_FUNC_TEST(MULT);
REC_FUNC_TEST(MULTU);

REC_FUNC_TEST(MTLO);
REC_FUNC_TEST(MTHI);
REC_FUNC_TEST(MFLO);
REC_FUNC_TEST(MFHI);

REC_FUNC_TEST(DIV);
REC_FUNC_TEST(DIVU);

#endif

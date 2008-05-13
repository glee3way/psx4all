#if 1
static void recMFC0()
{
// Rt = Cop0->Rd
	if (!_Rt_) return;
	u32 rt = regMipsToArm(_Rt_, REG_FIND, REG_REGISTER);

	ARM_LDR_IMM(ARM_POINTER, rt, PERM_REG_1, CalcDispCP0(_Rd_));
	regMipsChanged(_Rt_);
}

static void recCFC0()
{
// Rt = Cop0->Rd

	recMFC0();
}

static void recMTC0()
{
// Cop0->Rd = Rt

	u32 rt = regMipsToArm(_Rt_, REG_LOAD, REG_REGISTER);
	ARM_STR_IMM(ARM_POINTER, rt, PERM_REG_1, CalcDispCP0(_Rd_));
}

static void recCTC0()
{
// Cop0->Rd = Rt

	recMTC0();
}

static void recRFE()
{
	ARM_MVN_REG_IMM(ARM_POINTER, TEMP_2, 0xf, 0);
	ARM_LDR_IMM(ARM_POINTER, TEMP_1, PERM_REG_1, 184);
	/* DEBUG LSR or ASR ? */
	ARM_AND_REG_REG(ARM_POINTER, TEMP_2, TEMP_1, TEMP_2);
	ARM_AND_REG_IMM(ARM_POINTER, TEMP_1, TEMP_1, 0x3c, 0);
	ARM_ORR_REG_IMMSHIFT(ARM_POINTER, TEMP_1, TEMP_2, TEMP_1, ARMSHIFT_LSR, 2);
	ARM_STR_IMM(ARM_POINTER, TEMP_1, PERM_REG_1, 184);
}
#else


REC_FUNC_TEST(RFE);
REC_FUNC_TEST(MTC0);
REC_FUNC_TEST(CTC0);
REC_FUNC_TEST(MFC0);
REC_FUNC_TEST(CFC0);

#endif

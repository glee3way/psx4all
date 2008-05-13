#pragma once

#ifndef NO_ZERO_REGISTER_OPTIMISATION

/* ALU : Arithmetic and Logical Unit */

extern INLINE u32 gen(MOVI16, u32 rt, s32 imm16)
{
  u32 stores, shifts;

  s32 n = gen(dissect_imm16_ex, imm16, ~imm16, stores, shifts);

  if (n < 0)
  {
    ARM_MVN_REG_IMM(ARM_POINTER, rt, stores, shifts);
	if (stores&-256) { ARM_EOR_REG_IMM(ARM_POINTER, rt, rt, (stores >> 8), (shifts >> 8)) };
		return -n;
  }
  if (n > 0)
  {
    ARM_MOV_REG_IMM(ARM_POINTER, rt, stores, shifts);
	if (stores&-256) { ARM_ORR_REG_IMM(ARM_POINTER, rt, rt, (stores >> 8), (shifts >> 8)) };
		return n;
  }
  ARM_MOV_REG_IMM8(ARM_POINTER, rt, 0);
	return 1;
}

extern INLINE u32 gen(MOVU16, u32 rt, u32 imm16)
{
  u32 stores, shifts;

  s32 n = gen(dissect_imm16, imm16, stores, shifts);

  ARM_MOV_REG_IMM(ARM_POINTER, rt, stores, shifts);
  if (stores&-256) { ARM_ORR_REG_IMM(ARM_POINTER, rt, rt, (stores >> 8), (shifts >> 8)) };

	return n ? n : 1;
}

extern INLINE u32 gen(MOVI32, u32 rt, u32 imm32)
{
  u32 stores, shifts;

  int n = gen(dissect_imm32_ex, imm32, ~imm32, stores, shifts);

  if (n < 0)
  {
    ARM_MVN_REG_IMM(ARM_POINTER, rt, stores, shifts);
	while (stores&-256) { ARM_EOR_REG_IMM(ARM_POINTER, rt, rt, (stores >>= 8), (shifts >>= 8)) };
		return -n;
  }
  if (n > 0)
  {
    ARM_MOV_REG_IMM(ARM_POINTER, rt, stores, shifts);
	while (stores&-256) { ARM_ORR_REG_IMM(ARM_POINTER, rt, rt, (stores >>= 8), (shifts >>= 8)) };
		return n;
  }
  ARM_MOV_REG_IMM8(ARM_POINTER, rt, 0);
	return 1;
}

extern INLINE u32 gen(MOV, u32 rd, u32 rs) { ARM_MOV_REG_REG(ARM_POINTER, rd, rs); return 1; }
extern INLINE u32 gen(CLR, u32 rd)         { ARM_MOV_REG_IMM8(ARM_POINTER, rd, 0); return 1; }
extern INLINE u32 gen(NEG, u32 rd, u32 rs) { ARM_RSB_REG_IMM8(ARM_POINTER, rd, rs, 0); return 1; }
extern INLINE u32 gen(NOT, u32 rd, u32 rs) { ARM_MVN_REG_REG(ARM_POINTER, rd, rs); return 1; }

extern INLINE u32 gen(LUI, u32 rt, u16 imm16)
{
  u32 stores, shifts;

  u32 n = gen(dissect_imm16, imm16, stores, shifts);

  ARM_MOV_REG_IMM(ARM_POINTER, rt, stores, (shifts - 16));
  if (stores&-256) { ARM_ORR_REG_IMM(ARM_POINTER, rt, rt, (stores >> 8), ((shifts >> 8) - 16)) };

	return n ? n : 1;
}

#define GEN_ARM_SHIFT(type, _rd_, _rt_, _sa_) \
	do { if (_sa_) { ARM_MOV_REG_IMMSHIFT(ARM_POINTER, _rd_, _rt_, type, _sa_); } else { ARM_MOV_REG_REG(ARM_POINTER, _rd_, _rt_); } } while (0)

extern INLINE u32 gen(SLL, u32 rd, u32 rt, u32 sa) { GEN_ARM_SHIFT(ARMSHIFT_LSL, rd, rt, sa); return 1; }
extern INLINE u32 gen(SRL, u32 rd, u32 rt, u32 sa) { GEN_ARM_SHIFT(ARMSHIFT_LSR, rd, rt, sa); return 1; }
extern INLINE u32 gen(SRA, u32 rd, u32 rt, u32 sa) { GEN_ARM_SHIFT(ARMSHIFT_ASR, rd, rt, sa); return 1; }

extern INLINE u32 gen(SLL_RT0, u32 rd, u32 rt, u32 sa) { return gen(CLR, rd); }
extern INLINE u32 gen(SRL_RT0, u32 rd, u32 rt, u32 sa) { return gen(CLR, rd); }
extern INLINE u32 gen(SRA_RT0, u32 rd, u32 rt, u32 sa) { return gen(CLR, rd); }

#define GEN_ARM_SHIFTV(type, _rd_, _rt_, _rs_) \
	do { ARM_AND_REG_IMM8(ARM_POINTER, T1, _rs_, 31); ARM_MOV_REG_REGSHIFT(ARM_POINTER, _rd_, _rt_, type, T1); } while (0)

extern INLINE u32 gen(SLLV, u32 rd, u32 rt, u32 rs) { GEN_ARM_SHIFTV(ARMSHIFT_LSL, rd, rt, rs); return 2; }
extern INLINE u32 gen(SRLV, u32 rd, u32 rt, u32 rs) { GEN_ARM_SHIFTV(ARMSHIFT_LSR, rd, rt, rs); return 2; }
extern INLINE u32 gen(SRAV, u32 rd, u32 rt, u32 rs) { GEN_ARM_SHIFTV(ARMSHIFT_ASR, rd, rt, rs); return 2; }

extern INLINE u32 gen(SLLV_RT0, u32 rd, u32 rt, u32 rs) { return gen(CLR, rd); }
extern INLINE u32 gen(SRLV_RT0, u32 rd, u32 rt, u32 rs) { return gen(CLR, rd); }
extern INLINE u32 gen(SRAV_RT0, u32 rd, u32 rt, u32 rs) { return gen(CLR, rd); }

extern INLINE u32 gen(SLLV_RS0, u32 rd, u32 rt, u32 rs) { return gen(MOV, rd, rt); }
extern INLINE u32 gen(SRLV_RS0, u32 rd, u32 rt, u32 rs) { return gen(MOV, rd, rt); }
extern INLINE u32 gen(SRAV_RS0, u32 rd, u32 rt, u32 rs) { return gen(MOV, rd, rt); }

extern INLINE u32 gen(SLLV_RT0_RS0, u32 rd, u32 rt, u32 rs) { return gen(CLR, rd); }
extern INLINE u32 gen(SRLV_RT0_RS0, u32 rd, u32 rt, u32 rs) { return gen(CLR, rd); }
extern INLINE u32 gen(SRAV_RT0_RS0, u32 rd, u32 rt, u32 rs) { return gen(CLR, rd); }

extern INLINE u32 gen(ADD, u32 rd, u32 rs, u32 rt) { ARM_ADD_REG_REG(ARM_POINTER, rd, rs, rt); return 1; }
extern INLINE u32 gen(ADD_RS0, u32 rd, u32 rs, u32 rt) { return gen(MOV, rd, rt); }
extern INLINE u32 gen(ADD_RT0, u32 rd, u32 rs, u32 rt) { return gen(MOV, rd, rs); }
extern INLINE u32 gen(ADD_RS0_RT0, u32 rd, u32 rs, u32 rt) { return gen(CLR, rd); }

extern INLINE u32 gen(SUB, u32 rd, u32 rs, u32 rt) { ARM_SUB_REG_REG(ARM_POINTER, rd, rs, rt); return 1; }
extern INLINE u32 gen(RSB, u32 rd, u32 rs, u32 rt) { ARM_RSB_REG_REG(ARM_POINTER, rd, rs, rt); return 1; }
extern INLINE u32 gen(SUB_RS0, u32 rd, u32 rs, u32 rt) { ARM_RSB_REG_IMM8(ARM_POINTER, rd, rt, 0); return 1; }
extern INLINE u32 gen(SUB_RT0, u32 rd, u32 rs, u32 rt) { return gen(MOV, rd, rs); }
extern INLINE u32 gen(SUB_RS0_RT0, u32 rd, u32 rs, u32 rt) { return gen(CLR, rd); }

extern INLINE u32 gen(AND, u32 rd, u32 rs, u32 rt) { ARM_AND_REG_REG(ARM_POINTER, rd, rs, rt); return 1; }
extern INLINE u32 gen(AND_RS0, u32 rd, u32 rs, u32 rt) { return gen(CLR, rd); }
extern INLINE u32 gen(AND_RT0, u32 rd, u32 rs, u32 rt) { return gen(CLR, rd); }
extern INLINE u32 gen(AND_RS0_RT0, u32 rd, u32 rs, u32 rt) { return gen(CLR, rd); }

extern INLINE u32 gen(OR, u32 rd, u32 rs, u32 rt) { ARM_ORR_REG_REG(ARM_POINTER, rd, rs, rt); return 1; }
extern INLINE u32 gen(OR_RS0, u32 rd, u32 rs, u32 rt) { return gen(MOV, rd, rt); }
extern INLINE u32 gen(OR_RT0, u32 rd, u32 rs, u32 rt) { return gen(MOV, rd, rs); }
extern INLINE u32 gen(OR_RS0_RT0, u32 rd, u32 rs, u32 rt) { return gen(CLR, rd); }

extern INLINE u32 gen(XOR, u32 rd, u32 rs, u32 rt) { ARM_EOR_REG_REG(ARM_POINTER, rd, rs, rt); return 1; }
extern INLINE u32 gen(XOR_RS0, u32 rd, u32 rs, u32 rt) { return gen(MOV, rd, rt); }
extern INLINE u32 gen(XOR_RT0, u32 rd, u32 rs, u32 rt) { return gen(MOV, rd, rs); }
extern INLINE u32 gen(XOR_RS0_RT0, u32 rd, u32 rs, u32 rt) { return gen(CLR, rd); }

extern INLINE u32 gen(NOR, u32 rd, u32 rs, u32 rt) { if (rs == rt) { return gen(NOT, rd, rs); } else { ARM_ORR_REG_REG(ARM_POINTER, rd, rs, rt); return 1+gen(NOT, rd, rd); } }
extern INLINE u32 gen(NOR_RS0, u32 rd, u32 rs, u32 rt) { return gen(NOT, rd, rt); } 
extern INLINE u32 gen(NOR_RT0, u32 rd, u32 rs, u32 rt) { return gen(NOT, rd, rs); } 
extern INLINE u32 gen(NOR_RS0_RT0, u32 rd, u32 rs, u32 rt) { ARM_MVN_REG_IMM8(ARM_POINTER, rd, 0); return 1; } 

extern INLINE u32 gen(SLT, u32 rd, u32 rs, u32 rt) { ARM_SUB_REG_REG(ARM_POINTER, rd, rs, rt); ARM_SHR_IMM(ARM_POINTER, rd, rd, 31); return 2; }
extern INLINE u32 gen(SLT_RS0, u32 rd, u32 rs, u32 rt) { ARM_RSB_REG_IMM8(ARM_POINTER, rd, rt, 0); ARM_SHR_IMM(ARM_POINTER, rd, rd, 31); return 2; }
extern INLINE u32 gen(SLT_RT0, u32 rd, u32 rs, u32 rt) { ARM_SHR_IMM(ARM_POINTER, rd, rs, 31); return 1; }
extern INLINE u32 gen(SLT_RS0_RT0, u32 rd, u32 rs, u32 rt) { return gen(CLR, rd); }

extern INLINE u32 gen(SLTU, u32 rd, u32 rs, u32 rt) { ARM_CMP_REG_REG(ARM_POINTER, rs, rt); ARM_MOV_REG_IMM_COND(ARM_POINTER, rd, 0, 0, ARMCOND_HS); ARM_MOV_REG_IMM_COND(ARM_POINTER, rd, 1, 0, ARMCOND_LO); return 3; }
extern INLINE u32 gen(SLTU_RS0, u32 rd, u32 rs, u32 rt) { ARM_ANDS_REG_REG(ARM_POINTER, rd, rt, rt); ARM_MOV_REG_IMM_COND(ARM_POINTER, rd, 1, 0, ARMCOND_NE); return 2; }
extern INLINE u32 gen(SLTU_RT0, u32 rd, u32 rs, u32 rt) { return gen(CLR, rd); }
extern INLINE u32 gen(SLTU_RS0_RT0, u32 rd, u32 rs, u32 rt) { return gen(CLR, rd); }

extern INLINE u32 gen(ADDI, u32 rt, u32 rs, s32 imm16)
{
	u32 stores, shifts;

	s32 n = gen(dissect_imm16_ex, imm16, -imm16, stores, shifts);

  	if (!n)
  	{
		ARM_MOV_REG_REG(ARM_POINTER, rt, rs);
		return 1;
  	}
	if (n < 0)
  	{
    		ARM_SUB_REG_IMM(ARM_POINTER, rt, rs, stores, shifts);
    		if (stores&-256) ARM_SUB_REG_IMM(ARM_POINTER, rt, rt, (stores >> 8), (shifts >> 8));
		return -n;
  	}

	ARM_ADD_REG_IMM(ARM_POINTER, rt, rs, stores, shifts);
  	if (stores&-256) ARM_ADD_REG_IMM(ARM_POINTER, rt, rt, (stores >> 8), (shifts >> 8));

	return n;
}
extern INLINE u32 gen(ADDI_RS0, u32 rt, u32 rs, s32 imm16) { return gen(MOVI16, rt, imm16); } 


extern INLINE u32 gen(SUBI, u32 rt, u32 rs, s32 imm16) { return gen(ADDI, rt, rs, -imm16); }
extern INLINE u32 gen(SUBI_RS0, u32 rt, u32 rs, s32 imm16) { return gen(MOVI16, rt, -imm16); }

extern INLINE u32 gen(SLTI, u32 rt, u32 rs, s32 imm16) { u32 n = gen(SUBI, rt, rs, imm16); ARM_SHR_IMM(ARM_POINTER, rt, rt, 31); return n+1; }
extern INLINE u32 gen(SLTI_RS0, u32 rt, u32 rs, s32 imm16) { u32 t = ((s32)0 < (s32)imm16); ARM_MOV_REG_IMM8(ARM_POINTER, rt, t); return 1; }

extern INLINE u32 gen(SLTIU, u32 rt, u32 rs, s32 imm16)
{
  // Rt = (u32)Rs < signextend(Imm16)
  u32 stores, shifts;

  s32 n = gen(dissect_imm16_ex, imm16, -imm16, stores, shifts);

  if (!n)
  {
    n = gen(CLR, rt);
	return n;
  }
	else if (n < 0)
  {
    ARM_ADDS_REG_IMM(ARM_POINTER, rt, rs, stores, shifts);
    if (stores&-256) ARM_ADDS_REG_IMM_COND(ARM_POINTER, rt, rt, (stores >> 8), (shifts >> 8), ARMCOND_HS);
		n = -n;
  }
	else
	{
		ARM_SUBS_REG_IMM(ARM_POINTER, rt, rs, stores, shifts);
		if (stores&-256) ARM_SUBS_REG_IMM_COND(ARM_POINTER, rt, rt, (stores >> 8), (shifts >> 8), ARMCOND_HS);
	}

  ARM_MOV_REG_IMM_COND(ARM_POINTER, rt, 0, 0, ARMCOND_HS);
  ARM_MOV_REG_IMM_COND(ARM_POINTER, rt, 1, 0, ARMCOND_LO);

	return n+2;
}
extern INLINE u32 gen(SLTIU_RS0, u32 rt, u32 rs, s32 imm16) { u32 t = ((u32)0 < (u32)imm16); ARM_MOV_REG_IMM8(ARM_POINTER, rt, t); return 1; }

extern INLINE u32 gen(ANDI, u32 rt, u32 rs, u32 imm16)
{
  // Rt = Rs & zeroextend(Imm16)
  u32 stores, shifts;

  s32 n = gen(dissect_imm16, imm16, stores, shifts);

  switch (n)
  {
  case 0:
		n = 1;
  case 1:
    ARM_AND_REG_IMM(ARM_POINTER, rt, rs, stores, shifts);
    break;
  case 2:
    ARM_MOV_REG_IMM(ARM_POINTER, T1, stores, shifts);
    ARM_ORR_REG_IMM(ARM_POINTER, T1, T1, (stores >> 8), (shifts >> 8));
    ARM_AND_REG_REG(ARM_POINTER, rt, rs, T1);
		n = 3;
    break;
  }

	return n;
}
extern INLINE u32 gen(ANDI_RS0, u32 rt, u32 rs, u32 imm16) { return gen(CLR, rt); }

extern INLINE u32 gen(ORI, u32 rt, u32 rs, u32 imm16)
{
  // Rt = Rs | zeroextend(Imm16)
  u32 stores, shifts;

  u32 n = gen(dissect_imm16, imm16, stores, shifts);

  ARM_ORR_REG_IMM(ARM_POINTER, rt, rs, stores, shifts);
  if (stores&-256) ARM_ORR_REG_IMM(ARM_POINTER, rt, rt, (stores >> 8), (shifts >> 8));

	return n ? n : 1;
}
extern INLINE u32 gen(ORI_RS0, u32 rt, u32 rs, u32 imm16) { return gen(MOVU16, rt, imm16); }

extern INLINE u32 gen(XORI, u32 rt, u32 rs, u32 imm16)
{
  // Rt = Rs ^ zeroextend(Imm16)
  u32 stores, shifts;

  u32 n = gen(dissect_imm16, imm16, stores, shifts);

  ARM_EOR_REG_IMM(ARM_POINTER, rt, rs, stores, shifts);
  if (stores&-256) ARM_EOR_REG_IMM(ARM_POINTER, rt, rt, (stores >> 8), (shifts >> 8));

	return n ? n : 1;
}
extern INLINE u32 gen(XORI_RS0, u32 rt, u32 rs, u32 imm16) { return gen(MOVU16, rt, imm16); }

#else /* NO_ZERO_REGISTER_OPTIMISATION */

/* ALU : Arithmetic and Logical Unit */

extern INLINE void gen(MOVI16, u32 rt, s32 imm16)
{
  u32 stores, shifts;

  s32 n = gen(dissect_imm16_ex, imm16, ~imm16, stores, shifts);

  if (n < 0)
  {
    ARM_MVN_REG_IMM(ARM_POINTER, rt, stores, shifts);
    if (stores&-256) ARM_EOR_REG_IMM(ARM_POINTER, rt, rt, (stores >> 8), (shifts >> 8));
  }
  else
  {
    ARM_MOV_REG_IMM(ARM_POINTER, rt, stores, shifts);
    if (stores&-256) ARM_ORR_REG_IMM(ARM_POINTER, rt, rt, (stores >> 8), (shifts >> 8));
  }
}

extern INLINE void gen(MOVU16, u32 rt, u32 imm16)
{
  u32 stores, shifts;

  s32 n = gen(dissect_imm16, imm16, stores, shifts);

  ARM_MOV_REG_IMM(ARM_POINTER, rt, stores, shifts);
  if (stores&-256) ARM_ORR_REG_IMM(ARM_POINTER, rt, rt, (stores >> 8), (shifts >> 8));
}

extern INLINE void gen(MOVI32, u32 rt, u32 imm32)
{
  u32 stores, shifts;

  int n = gen(dissect_imm32_ex, imm32, ~imm32, stores, shifts);

  if (n < 0)
  {
    ARM_MVN_REG_IMM(ARM_POINTER, rt, stores, shifts);
    while (stores&-256) ARM_EOR_REG_IMM(ARM_POINTER, rt, rt, (stores >>= 8), (shifts >>= 8));
  }
  else
  {
    ARM_MOV_REG_IMM(ARM_POINTER, rt, stores, shifts);
    while (stores&-256) ARM_ORR_REG_IMM(ARM_POINTER, rt, rt, (stores >>= 8), (shifts >>= 8));
  }
}

#define GEN_ARM_SHIFT(type, _rd_, _rt_, _sa_) \
	do { if (_sa_) { ARM_MOV_REG_IMMSHIFT(ARM_POINTER, _rd_, _rt_, type, _sa_); } else { ARM_MOV_REG_REG(ARM_POINTER, _rd_, _rt_); } } while (0)

extern INLINE void gen(SLL, u32 rd, u32 rt, u32 sa) { GEN_ARM_SHIFT(ARMSHIFT_LSL, rd, rt, sa); }
extern INLINE void gen(SRL, u32 rd, u32 rt, u32 sa) { GEN_ARM_SHIFT(ARMSHIFT_LSR, rd, rt, sa); }
extern INLINE void gen(SRA, u32 rd, u32 rt, u32 sa) { GEN_ARM_SHIFT(ARMSHIFT_ASR, rd, rt, sa); }

#define GEN_ARM_SHIFTV(type, _rd_, _rt_, _rs_) \
	do { ARM_AND_REG_IMM8(ARM_POINTER, T1, _rs_, 31); ARM_MOV_REG_REGSHIFT(ARM_POINTER, _rd_, _rt_, type, T1); } while (0)

extern INLINE void gen(SLLV, u32 rd, u32 rt, u32 rs) { GEN_ARM_SHIFTV(ARMSHIFT_LSL, rd, rt, rs); }
extern INLINE void gen(SRLV, u32 rd, u32 rt, u32 rs) { GEN_ARM_SHIFTV(ARMSHIFT_LSR, rd, rt, rs); }
extern INLINE void gen(SRAV, u32 rd, u32 rt, u32 rs) { GEN_ARM_SHIFTV(ARMSHIFT_ASR, rd, rt, rs); }

extern INLINE void gen(MOV, u32 rd, u32 rs) { ARM_MOV_REG_REG(ARM_POINTER, rd, rs); }

extern INLINE void gen(CLR, u32 rd) { ARM_MOV_REG_IMM8(ARM_POINTER, rd, 0); }

extern INLINE void gen(ADD, u32 rd, u32 rs, u32 rt) { ARM_ADD_REG_REG(ARM_POINTER, rd, rs, rt); }

extern INLINE void gen(SUB, u32 rd, u32 rs, u32 rt) { ARM_SUB_REG_REG(ARM_POINTER, rd, rs, rt); }

extern INLINE void gen(RSB, u32 rd, u32 rs, u32 rt) { ARM_RSB_REG_REG(ARM_POINTER, rd, rs, rt); }

extern INLINE void gen(NEG, u32 rd, u32 rs) { ARM_RSB_REG_IMM8(ARM_POINTER, rd, rs, 0); }

extern INLINE void gen(AND, u32 rd, u32 rs, u32 rt) { ARM_AND_REG_REG(ARM_POINTER, rd, rs, rt); }

extern INLINE void gen(OR, u32 rd, u32 rs, u32 rt) { ARM_ORR_REG_REG(ARM_POINTER, rd, rs, rt); }

extern INLINE void gen(XOR, u32 rd, u32 rs, u32 rt) { ARM_EOR_REG_REG(ARM_POINTER, rd, rs, rt); }

extern INLINE void gen(NOR, u32 rd, u32 rs, u32 rt) { if (rs == rt) { ARM_MVN_REG_REG(ARM_POINTER, rd, rs); } else { ARM_ORR_REG_REG(ARM_POINTER, rd, rs, rt); ARM_MVN_REG_REG(ARM_POINTER, rd, rd); } }

extern INLINE void gen(NOT, u32 rd, u32 rs) { ARM_MVN_REG_REG(ARM_POINTER, rd, rs); }

extern INLINE void gen(SLT, u32 rd, u32 rs, u32 rt) { ARM_SUB_REG_REG(ARM_POINTER, rd, rs, rt); ARM_SHR_IMM(ARM_POINTER, rd, rd, 31); }

extern INLINE void gen(SLTU, u32 rd, u32 rs, u32 rt) { ARM_CMP_REG_REG(ARM_POINTER, rs, rt); ARM_MOV_REG_IMM_COND(ARM_POINTER, rd, 0, 0, ARMCOND_CS); ARM_MOV_REG_IMM_COND(ARM_POINTER, rd, 1, 0, ARMCOND_CC); }

extern INLINE void gen(ADDI, u32 rt, u32 rs, s32 imm16)
{
  // Rt = Rs + signextend(Imm16)
  u32 stores, shifts;
  s32 n = gen(dissect_imm16_ex, imm16, -imm16, stores, shifts);

  if (!n)
  {
		ARM_MOV_REG_REG(ARM_POINTER, rt, rs);
  }
	else if (n < 0)
  {
    ARM_SUB_REG_IMM(ARM_POINTER, rt, rs, stores, shifts);
    if (stores&-256) ARM_SUB_REG_IMM(ARM_POINTER, rt, rt, (stores >> 8), (shifts >> 8));
  }
  else
  {
    ARM_ADD_REG_IMM(ARM_POINTER, rt, rs, stores, shifts);
    if (stores&-256) ARM_ADD_REG_IMM(ARM_POINTER, rt, rt, (stores >> 8), (shifts >> 8));
  }
}

extern INLINE void gen(SUBI, u32 rt, u32 rs, s32 imm16) { gen(ADDI, rt, rs, -imm16); }

extern INLINE void gen(ADDI32, u32 rt, u32 rs, s32 imm32)
{
  // Rt = Rs + (s32)Imm32
  u32 stores, shifts;

  s32 n = gen(dissect_imm32_ex, imm32, -imm32, stores, shifts);

  if (!n)
  {
    ARM_MOV_REG_REG(ARM_POINTER, rt, rs);
	return;
  }
  if (n < 0)
  {
    ARM_SUB_REG_IMM(ARM_POINTER, rt, rs, stores, shifts);
    while (stores&-256) ARM_SUB_REG_IMM(ARM_POINTER, rt, rt, (stores >>= 8), (shifts >>= 8));
  }
  else
  {
    ARM_ADD_REG_IMM(ARM_POINTER, rt, rs, stores, shifts);
    while (stores&-256) ARM_ADD_REG_IMM(ARM_POINTER, rt, rt, (stores >>= 8), (shifts >>= 8));
  }
}

extern INLINE void gen(SUBI32, u32 rt, u32 rs, s32 imm32) { gen(ADDI32, rt, rs, -imm32); } 

extern INLINE void gen(RSBI32, u32 rt, u32 rs, s32 imm32)
{
  // Rt = (s32)Imm32 - Rs;
  u32 stores, shifts;

  s32 n = gen(dissect_imm32_ex, imm32, -imm32, stores, shifts);

	if (n < 0)
	{
    ARM_ADD_REG_IMM(ARM_POINTER, rt, rs, stores, shifts);
    while (stores&-256) ARM_ADD_REG_IMM(ARM_POINTER, rt, rt, (stores >>= 8), (shifts >>= 8));
		ARM_RSB_REG_IMM8(ARM_POINTER, rt, rs, 0);
	}
	else
	{
		ARM_RSB_REG_IMM(ARM_POINTER, rt, rs, stores, shifts);
		if (!n) return;
    while (stores&-256) ARM_ADD_REG_IMM(ARM_POINTER, rt, rt, (stores >>= 8), (shifts >>= 8));
	}
}

extern INLINE void gen(SLTI, u32 rt, u32 rs, s32 imm16)
{
  // Rt = (s32)Rs < signextend(Imm16)  <=>  Rt = sign((s32)Rs - signextend(Imm16))

  gen(SUBI, rt, rs, imm16);

  ARM_SHR_IMM(ARM_POINTER, rt, rt, 31);
}

extern INLINE void gen(SLTI32, u32 rt, u32 rs, s32 imm32)
{
  // Rt = (s32)Rs < (s32)Imm32  <=>  Rt = sign((s32)Rs - (s32)Imm32)

  gen(SUBI32, rt, rs, imm32);

  ARM_SHR_IMM(ARM_POINTER, rt, rt, 31);
}

extern INLINE void gen(SLTIU, u32 rt, u32 rs, s32 imm16)
{
  // Rt = (u32)Rs < signextend(Imm16)
  u32 stores, shifts;

  s32 n = gen(dissect_imm16_ex, imm16, -imm16, stores, shifts);

  if (!n)
  {
    ARM_MOV_REG_IMM8(ARM_POINTER, rt, 0);
	return;
  }
  if (n < 0)
  {
    ARM_ADDS_REG_IMM(ARM_POINTER, rt, rs, stores, shifts);
    if (stores&-256) ARM_ADDS_REG_IMM_COND(ARM_POINTER, rt, rt, (stores >> 8), (shifts >> 8), ARMCOND_CS);
  }
  else
  {
		ARM_SUBS_REG_IMM(ARM_POINTER, rt, rs, stores, shifts);
		if (stores&-256) ARM_SUBS_REG_IMM_COND(ARM_POINTER, rt, rt, (stores >> 8), (shifts >> 8), ARMCOND_CS);
  }

  ARM_MOV_REG_IMM_COND(ARM_POINTER, rt, 0, 0, ARMCOND_CS);
  ARM_MOV_REG_IMM_COND(ARM_POINTER, rt, 1, 0, ARMCOND_CC);
}

extern INLINE void gen(SLTIU32, u32 rt, u32 rs, s32 imm32)
{
  // Rt = (u32)Rs < (s32)Imm32

  u32 stores, shifts;

  s32 n = gen(dissect_imm32_ex, imm32, -imm32, stores, shifts);

  if (!n)
  {
    ARM_MOV_REG_IMM8(ARM_POINTER, rt, 0);
	return;
  }
  if (n < 0)
  {
    ARM_ADDS_REG_IMM(ARM_POINTER, rt, rs, stores, shifts);
    while (stores&-256) ARM_ADDS_REG_IMM_COND(ARM_POINTER, rt, rt, (stores >>= 8), (shifts >>= 8), ARMCOND_CC);
  }
  else
  {
    ARM_SUBS_REG_IMM(ARM_POINTER, rt, rs, stores, shifts);
    while (stores&-256) ARM_SUBS_REG_IMM_COND(ARM_POINTER, rt, rt, (stores >>= 8), (shifts >>= 8), ARMCOND_CC);
  }

  ARM_MOV_REG_IMM_COND(ARM_POINTER, rt, 0, 0, ARMCOND_CS);
  ARM_MOV_REG_IMM_COND(ARM_POINTER, rt, 1, 0, ARMCOND_CC);
}

extern INLINE void gen(ANDI, u32 rt, u32 rs, u32 imm16)
{
  // Rt = Rs & zeroextend(Imm16)
  u32 stores, shifts;

  s32 n = gen(dissect_imm16, imm16, stores, shifts);

  switch (n)
  {
  case 0:
  case 1:
    ARM_AND_REG_IMM(ARM_POINTER, rt, rs, stores, shifts);
    break;
  case 2:
    ARM_MOV_REG_IMM(ARM_POINTER, T1, stores, shifts);
    ARM_ORR_REG_IMM(ARM_POINTER, T1, T1, (stores >> 8), (shifts >> 8));
    ARM_AND_REG_REG(ARM_POINTER, rt, rs, T1);
    break;
  }
}

extern INLINE void gen(ANDI32, u32 rt, u32 rs, u32 imm32)
{
  // Rt = Rs & (u32)Imm32
  u32 stores, shifts;

  if (imm32 == -1) return;

  s32 n = gen(dissect_imm32_ex, imm32, ~imm32, stores, shifts);

  switch (n)
  {
  case -4:
  case -3:
  case -2:
    ARM_MOV_REG_IMM(ARM_POINTER, T1, stores, shifts);
    while (stores&-256) ARM_ORR_REG_IMM(ARM_POINTER, T1, T1, (stores >>= 8), (shifts >>= 8));
    ARM_BIC_REG_REG(ARM_POINTER, rt, rs, T1);
    break;
  case -1:
    ARM_BIC_REG_IMM(ARM_POINTER, rt, rs, stores, shifts);
    break;
  case  0:
  case +1:
    ARM_AND_REG_IMM(ARM_POINTER, rt, rs, stores, shifts);
    break;
  case +2:
  case +3:
  case +4:
    ARM_MOV_REG_IMM(ARM_POINTER, T1, stores, shifts);
    while (stores&-256) ARM_ORR_REG_IMM(ARM_POINTER, T1, T1, (stores >>= 8), (shifts >>= 8));
    ARM_AND_REG_REG(ARM_POINTER, rt, rs, T1);
    break;
  }
}

extern INLINE void gen(ORI, u32 rt, u32 rs, u32 imm16)
{
  // Rt = Rs | zeroextend(Imm16)
  u32 stores, shifts;

  if (imm16 == 0) return;

  gen(dissect_imm16, imm16, stores, shifts);

  ARM_ORR_REG_IMM(ARM_POINTER, rt, rs, stores, shifts);
  if (stores&-256) ARM_ORR_REG_IMM(ARM_POINTER, rt, rt, (stores >> 8), (shifts >> 8));
}

extern INLINE void gen(ORI32, u32 rt, u32 rs, u32 imm32)
{
  // Rt = Rs | (u32)imm32
  u32 stores, shifts;

  if (imm32 == 0) return;

  if (imm32 == -1)
  {
    ARM_MVN_REG_IMM8(ARM_POINTER, rt, 0);
    return;
  }

  gen(dissect_imm32, imm32, stores, shifts);

  ARM_ORR_REG_IMM(ARM_POINTER, rt, rs, stores, shifts);
  while (stores&-256) ARM_ORR_REG_IMM(ARM_POINTER, rt, rt, (stores >>= 8), (shifts >>= 8));
}

extern INLINE void gen(XORI, u32 rt, u32 rs, u32 imm16)
{
  // Rt = Rs ^ zeroextend(Imm16)
  u32 stores, shifts;

  if (imm16 == 0)
  {
    if (rt != rs) ARM_MOV_REG_REG(ARM_POINTER, rt, rs);
    return;
  }

  gen(dissect_imm16, imm16, stores, shifts);

  ARM_EOR_REG_IMM(ARM_POINTER, rt, rs, stores, shifts);
  if (stores&-256) ARM_EOR_REG_IMM(ARM_POINTER, rt, rt, (stores >> 8), (shifts >> 8));
}

extern INLINE void gen(XORI32, u32 rt, u32 rs, u32 imm32)
{
  // Rt = Rs ^ (u32)Imm32
  u32 stores, shifts;

  if (imm32 == 0)
  {
    if (rt != rs) ARM_MOV_REG_REG(ARM_POINTER, rt, rs);
    return;
  }

  if (imm32 == -1)
  {
    ARM_MVN_REG_REG(ARM_POINTER, rt, rs);
    return;
  }

  gen(dissect_imm32, imm32, stores, shifts);

  ARM_EOR_REG_IMM(ARM_POINTER, rt, rs, stores, shifts);
  while (stores&-256) ARM_EOR_REG_IMM(ARM_POINTER, rt, rt, (stores >>= 8), (shifts >>= 8));
}

extern INLINE void gen(NORI32, u32 rt, u32 rs, u32 imm32)
{
  // Rt = ~(Rs | (u32)Imm32)
  gen(XORI32, rt, rs, imm32);
  ARM_MVN_REG_REG(ARM_POINTER, rt, rt);
}

extern INLINE void gen(LUI, u32 rt, u32 imm16)
{
  u32 stores, shifts;

  gen(dissect_imm16, imm16, stores, shifts);

  ARM_MOV_REG_IMM(ARM_POINTER, rt, stores, (shifts - 16));
  if (stores&-256) ARM_ORR_REG_IMM(ARM_POINTER, rt, rt, (stores >> 8), ((shifts >> 8) - 16));
}

#endif

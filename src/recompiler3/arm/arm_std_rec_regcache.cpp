#include "../common.h"

static INLINE void regClearJump(void)
{
	int i;
	for(i = 0; i < 32; i++)
	{
		if( regcache.mips[i].ismapped )
		{
			int mappedto = regcache.mips[i].mappedto;
			if( i != 0 && regcache.mips[i].mips_ischanged )
			{
				ARM_STR_IMM(ARM_POINTER, mappedto, PERM_REG_1, CalcDisp(i));
			}
			regcache.mips[i].mips_ischanged = false;
			regcache.arm[mappedto].ismapped = regcache.mips[i].ismapped = false;
			regcache.arm[mappedto].mappedto = regcache.mips[i].mappedto = 0;
			regcache.arm[mappedto].arm_type = REG_EMPTY;
			regcache.arm[mappedto].arm_age = 0;
			regcache.arm[mappedto].arm_use = 0;
			regcache.arm[mappedto].arm_islocked = false;
		}
	}
}

static INLINE void regFreeRegs(void)
{
	int i = 0;
	int firstfound = 0;
	while(regcache.reglist[i] != 0xFF)
	{
		int armreg = regcache.reglist[i];

		if( regcache.arm[armreg].arm_islocked == false )
		{
			int mipsreg = regcache.arm[armreg].mappedto;
			if( mipsreg != 0 && regcache.mips[mipsreg].mips_ischanged )
			{
				ARM_STR_IMM(ARM_POINTER, armreg, PERM_REG_1, CalcDisp(mipsreg));
			}
			regcache.mips[mipsreg].mips_ischanged = false;
			regcache.arm[armreg].ismapped = regcache.mips[mipsreg].ismapped = false;
			regcache.arm[armreg].mappedto = regcache.mips[mipsreg].mappedto = 0;
			regcache.arm[armreg].arm_type = REG_EMPTY;
			regcache.arm[armreg].arm_age = 0;
			regcache.arm[armreg].arm_use = 0;
			regcache.arm[armreg].arm_islocked = false;
			if(firstfound == 0)
			{
				regcache.reglist_cnt = i;
				firstfound = 1;
			}
		}
		
		i++;
	}
}

static u32 regMipsToArmHelper(u32 regmips, u32 action, u32 type)
{
	int regnum = regcache.reglist[regcache.reglist_cnt];
	
	while( regnum != 0xFF )
	{
		if(regcache.arm[regnum].arm_type == REG_EMPTY)
		{
			break;
		}
		regcache.reglist_cnt++;
		regnum = regcache.reglist[regcache.reglist_cnt];
	}

	if( regnum == 0xFF )
	{
		regFreeRegs();
		regnum = regcache.reglist[regcache.reglist_cnt];
	}

	regcache.arm[regnum].arm_type = type;
	regcache.arm[regnum].arm_islocked = true;
	regcache.mips[regmips].mips_ischanged = false;

	if( action != REG_LOADBRANCH )
	{
		regcache.arm[regnum].arm_age = 0;
		regcache.arm[regnum].arm_use = 0;
		regcache.arm[regnum].ismapped = true;
		regcache.arm[regnum].mappedto = regmips;
		regcache.mips[regmips].ismapped = true;
		regcache.mips[regmips].mappedto = regnum;
	}
	else
	{
		regcache.arm[regnum].arm_age = 0;
		regcache.arm[regnum].arm_use = 0xFF;
		regcache.arm[regnum].ismapped = false;
		regcache.arm[regnum].mappedto = 0;
		if( regmips != 0 )
		{
			ARM_LDR_IMM(ARM_POINTER, regnum, PERM_REG_1, CalcDisp(regmips));
		}
		else
		{
			ARM_MOV_REG_IMM8(ARM_POINTER, regnum, 0);
		}
	
		regcache.reglist_cnt++;
	
		return regnum;
	}

	if(action == REG_LOAD)
	{
		if( regmips != 0 )
		{
			ARM_LDR_IMM(ARM_POINTER, regcache.mips[regmips].mappedto, PERM_REG_1, CalcDisp(regmips));
		}
		else
		{
			ARM_MOV_REG_IMM8(ARM_POINTER, regcache.mips[regmips].mappedto, 0);
		}
	}

	regcache.reglist_cnt++;

	return regnum;
}

static INLINE u32 regMipsToArm(u32 regmips, u32 action, u32 type)
{
	if( regcache.mips[regmips].ismapped )
	{
		if( action != REG_LOADBRANCH )
		{
			int armreg = regcache.mips[regmips].mappedto;
			regcache.arm[armreg].arm_islocked = true;
			return armreg;
		}
		else
		{
			u32 mappedto = regcache.mips[regmips].mappedto;
			if( regmips != 0 && regcache.mips[regmips].mips_ischanged )
			{
				ARM_STR_IMM(ARM_POINTER, mappedto, PERM_REG_1, CalcDisp(regmips));
			}
			regcache.mips[regmips].mips_ischanged = false;
			regcache.mips[regmips].ismapped = false;
			regcache.mips[regmips].mappedto = 0;

			regcache.arm[mappedto].arm_type = type;
			regcache.arm[mappedto].arm_age = 0;
			regcache.arm[mappedto].arm_use = 0xFF;
			regcache.arm[mappedto].ismapped = false;
			regcache.arm[mappedto].arm_islocked = true;
			regcache.arm[mappedto].mappedto = 0;

			return mappedto;
		}
	}

	return regMipsToArmHelper(regmips, action, type);
}

static INLINE void regMipsChanged(u32 regmips)
{
	regcache.mips[regmips].mips_ischanged = true;
}

static INLINE void regBranchUnlock(u32 regarm)
{
	regcache.arm[regarm].arm_islocked = false;
}

static INLINE void regClearBranch(void)
{
	int i;
	for(i = 1; i < 32; i++)
	{
		if( regcache.mips[i].ismapped )
		{
			if( regcache.mips[i].mips_ischanged )
			{
				ARM_STR_IMM(ARM_POINTER, regcache.mips[i].mappedto, PERM_REG_1, CalcDisp(i));
			}
		}
	}
}

static INLINE void regReset()
{
	int i, i2;
	for(i = 0; i < 32; i++)
	{
		regcache.mips[i].mips_ischanged = false;
		regcache.mips[i].ismapped = false;
		regcache.mips[i].mappedto = 0;
	}

	for(i = 0; i < 16; i++)
	{
		regcache.arm[i].arm_type = REG_EMPTY;
		regcache.arm[i].arm_age = 0;
		regcache.arm[i].arm_use = 0;
		regcache.arm[i].arm_islocked = false;
		regcache.arm[i].ismapped = false;
		regcache.arm[i].mappedto = 0;
	}

	regcache.arm[0].arm_type = REG_RESERVED;
	regcache.arm[1].arm_type = REG_RESERVED;
	regcache.arm[2].arm_type = REG_RESERVED;
	regcache.arm[3].arm_type = REG_RESERVED;
	regcache.arm[TEMP_1].arm_type = REG_RESERVED;
	regcache.arm[TEMP_2].arm_type = REG_RESERVED;
#ifdef IPHONE
	regcache.arm[9].arm_type = REG_RESERVED;
#endif
	regcache.arm[PERM_REG_1].arm_type = REG_RESERVED;
	regcache.arm[13].arm_type = REG_RESERVED;
	regcache.arm[15].arm_type = REG_RESERVED;

	regcache.arm[12].arm_type = REG_RESERVED;
	regcache.arm[14].arm_type = REG_RESERVED;
	
	for(i = 0, i2 = 0; i < 16; i++)
	{
		if(regcache.arm[i].arm_type == REG_EMPTY)
		{
			regcache.reglist[i2] = i;
			i2++;
		}
	}
	regcache.reglist[i2] = 0xFF;
}


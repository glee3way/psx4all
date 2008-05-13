#ifndef __CDROM_H__
#define __CDROM_H__

#include "common.h"
#include "decode_xa.h"

struct CdrStat
{
 unsigned long Type;
 unsigned long Status;
 unsigned char Time[3]; // current playing time
};

typedef struct {
	u8 OCUP;
	u8 Reg1Mode;
	u8 Reg2;
	u8 CmdProcess;
	u8 Ctrl;
	u8 Stat;

	u8 StatP;

	u8 Transfer[2352];
	u8 *pTransfer;

	u8 Prev[4];
	u8 Param[8];
	u8 Result[8];

	u8 ParamC;
	u8 ParamP;
	u8 ResultC;
	u8 ResultP;
	u8 ResultReady;
	u8 Cmd;
	u8 Readed;
	u32 Reading;

	u8 ResultTN[6];
#ifdef __DREAMCAST__
	__declspec(align( 4 )) u8 ResultTD[4];
#else
	unsigned char ResultTD[4] __attribute__ ((aligned (4)));
	//u8 ResultTD[4];
#endif
	u8 SetSector[4];
	u8 SetSectorSeek[4];
	u8 Track;
	s32 Play;
	s32 CurTrack;
	s32 Mode, File, Channel, Muted;
	s32 Reset;
	s32 RErr;
	s32 FirstSector;

	xa_decode_t Xa;

	s32 Init;

	u8 Irq;
	u32 eCycle;

	s32 Seeked;

	char Unused[4083];
} cdrStruct;

extern cdrStruct cdr;

void cdrReset();
void cdrInterrupt();
void cdrReadInterrupt();
u8 cdrRead0(void);
u8 cdrRead1(void);
u8 cdrRead2(void);
u8 cdrRead3(void);
void cdrWrite0(u8 rt);
void cdrWrite1(u8 rt);
void cdrWrite2(u8 rt);
void cdrWrite3(u8 rt);
s32 cdrFreeze(gzFile f, s32 Mode);

#endif /* __CDROM_H__ */

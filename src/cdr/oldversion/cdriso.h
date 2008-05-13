#ifndef __CDRISO_H__
#define __CDRISO_H__

extern s8* IsoFile;
extern FILE *cdHandle;

#define CD_FRAMESIZE_RAW	2352
#define	BUFFER_DISABLED		0xFFFFFFFF

#define BUFFER_SECTORS		(1)

#define BUFFER_SIZE 	(BUFFER_SECTORS*CD_FRAMESIZE_RAW)
#define DATA_SIZE	(CD_FRAMESIZE_RAW-12)

#define itob(i)		((i)/10*16 + (i)%10)	/* u_char to BCD */
#define btoi(b)		((b)/16*10 + (b)%16)	/* BCD to u_char */

#define MSF2SECT(m,s,f)	(((m)*60+(s)-2)*75+(f))

extern s32 fmode;						// 0 - file / 1 - Zfile


s32 UpdateZmode();
void CfgOpenFile();

s32 CDR_init(void);
s32 CDR_shutdown(void);
s32 CDR_open(void);
s32 CDR_close(void);
s32 CDR_getTN(u8 *);
s32 CDR_getTD(u8 , u8 *);
s32 CDR_readTrack(u8 *);
u8 *CDR_getBuffer(void);
s32 CDR_configure(void);
s32 CDR_test(void);
void CDR_about(void);
s32 CDR_play(u8 *);
s32 CDR_stop(void);

s32 CDR_getStatus(struct CdrStat *);
struct SubQ {
	s8 res0[11];
	u8 ControlAndADR;
	u8 TrackNumber;
	u8 IndexNumber;
	u8 TrackRelativeAddress[3];
	u8 Filler;
	u8 AbsoluteAddress[3];
	s8 res1[72];
};
u8* CDR_getBufferSub(void);

#endif /* __CDRISO_H__ */

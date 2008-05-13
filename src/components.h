#ifndef __PLUGINS_H__
#define __PLUGINS_H__

#include "decode_xa.h"

s32  InitComponents();
void CloseComponents();
/*
///GPU PLUGIN STUFF
s32 GPU_open(s32);
s32 GPU_init(void);
s32 GPU_shutdown(void);
s32 GPU_close(void);
void GPU_writeStatus(u32);
void GPU_writeData(u32);
void GPU_writeDataMem(u32 *, s32);
u32 GPU_readStatus(void);
u32 GPU_readData(void);
void GPU_readDataMem(u32 *, s32);
s32 GPU_dmaChain(u32 *,u32);
void GPU_updateLace(void);
s32 GPU_configure(void);
s32 GPU_test(void);
void GPU_about(void);
void GPU_makeSnapshot(void);
void GPU_keypressed(s32);
void GPU_displayText(s8 *);

typedef struct {
	u32 ulFreezeVersion;
	u32 ulStatus;
	u32 ulControl[256];
	u8 psxVRam[1024*512*2];
} GPUFreeze_t;

void GPU_getScreenPic(u8 *);
void GPU_showScreenPic(u8 *);
*/
//cd rom plugin ;)
s32 CDR_init(void);
s32 CDR_shutdown(void);
s32 CDR_open(void);
s32 CDR_close(void);
s32 CDR_getTN(u8 *);
s32 CDR_getTD(u8, u8 *);
s32 CDR_readTrack(u8 *);
u8* CDR_getBuffer(void);
s32 CDR_configure(void);
s32 CDR_test(void);
void CDR_about(void);
s32 CDR_play(u8 *);
s32 CDR_stop(void);
s32 CDR_getStatus(CdrStat *);
s8* CDR_getDriveLetter(void);
u8* CDR_getBufferSub(void);

// SPU_ plugin
s32 SPU_open(void);
s32 SPU_init(void);
s32 SPU_shutdown(void);
s32 SPU_close(void);
void SPU_playSample(u8);
void SPU_startChannels1(u16);
void SPU_startChannels2(u16);
void SPU_stopChannels1(u16);
void SPU_stopChannels2(u16);
void SPU_putOne(u32,u16);
u16 SPU_getOne(u32);
void SPU_setAddr(u8, u16);
void SPU_setPitch(u8, u16);
void SPU_setVolumeL(u8, s16 );
void SPU_setVolumeR(u8, s16 );
//psemu pro 2 functions from now..
void SPU_writeRegister(u32, u16);
u16 SPU_readRegister(u32);
void SPU_writeDMA(u16);
u16 SPU_readDMA(void);
void SPU_writeDMAMem(unsigned short *, int);
void SPU_readDMAMem(unsigned short *, int);
void SPU_playADPCMchannel(xa_decode_t *);
void SPU_registerCallback(void);
s32 SPU_configure(void);
s32 SPU_test(void);
void SPU_about(void);

typedef struct {
	u32 Addr;
	u32 Size;
	u32 ulFreezeVersion;
	u32 ulFreezeSize;
	u8 cSPUPort[0x200];
	u8 cSPURam[0x80000];
	xa_decode_t xaS;
} SPUFreeze_t;

s32 SPU_freeze(u32, SPUFreeze_t *);
void SPU_async(u32);

// PAD Functions
s32 PAD1_open(void);
s32 PAD1_configure(void);
void PAD1_about(void);
s32 PAD1_init(s32);
s32 PAD1_shutdown(void);
s32 PAD1_test(void);
s32 PAD1_close(void);
s32 PAD1_query(void);
s32 PAD1_readPort1(PadDataS*);
s32 PAD1_readPort2(PadDataS*);
s32 PAD1_keypressed(void);
u8 PAD1_startPoll(s32);
u8 PAD1_poll(unsigned char);
void PAD1_setSensitive(s32);

s32 PAD2_open(void);
s32 PAD2_configure(void);
void PAD2_about(void);
s32 PAD2_init(s32);
s32 PAD2_shutdown(void);
s32 PAD2_test(void);
s32 PAD2_close(void);
s32 PAD2_query(void);
s32 PAD2_readPort1(PadDataS*);
s32 PAD2_readPort2(PadDataS*);
s32 PAD2_keypressed(void);
u8 PAD2_startPoll(s32);
u8 PAD2_poll(u8);
void PAD2_setSensitive(s32);

#endif /* __PLUGINS_H__ */

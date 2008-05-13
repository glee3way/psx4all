#ifndef GPU_INTRF_H
#define GPU_INTRF_H

#include "../common.h"
#include "sdl/videogl.h"

#include<SDL.h>
#include<SDL_opengl.h>

#define MAXSKIP			6
#define FRAME_HEIGHT            512
#define FRAME_OFFSET(x,y)       (((y)<<10)+(x))

typedef union {
	s8 S1[64];
	s16 S2[32];
	s32 S4[16];
	u8 U1[64];
	u16 U2[32];
	u32 U4[16];
} PacketBuffer_t;

typedef struct {
	u32 Version;
	u32 GP1;
	u32 Control[256];
	u8 gpu_frame_buffer[0x100000];
} GPUFREEZE;

extern u32 frameRate;
extern double frameRateAvg;
extern s32 framesToSkip;
extern s32 framesSkipped;
extern u32 displayFrameInfo;
extern s32 frameRateCounter;
extern u32 framesTotal;
extern u32 autoFrameSkip;
extern unsigned systicks;
extern s32 GP0;
extern s32 GP1;
extern s32 FrameToRead;
extern s32 FrameToWrite;
extern s32 FrameWidth;
extern s32 FrameCount;
extern s32 FrameIndex;
extern PacketBuffer_t PacketBuffer;
extern s32 PacketCount;
extern s32 PacketIndex;
extern s32 TextureWindow[4];
extern s32 DrawingArea[4];
extern s32 DrawingOffset[2];
extern s32 DisplayArea[8];
extern s32 OtherEnv[16];
extern u8 PacketSize[256];
extern u16 HorizontalResolution[8];
extern u16 VerticalResolution[4];
extern s32 isPAL;
extern s32 skip_this_frame;

s32 GPU_open(s32);
void GPU_reset(void);
s32 GPU_init(void);
s32 GPU_shutdown(void);
s32 GPU_close(void);
void GPU_writeStatus(u32);
u32 GPU_readStatus(void);
void GPU_writeData(u32);
void GPU_writeDataMem(u32 *, s32);
u32 GPU_readData(void);
void GPU_readDataMem(u32 *, s32);
s32 GPU_dmaChain(u32 *,u32);
s32 GPU_configure(void);
s32 GPU_test(void);
void GPU_about(void);
void GPU_makeSnapshot(void);
void GPU_keypressed(s32);
void GPU_displayText(s8 *);
s32 GPU_freeze(u32, GPUFREEZE *);
void GPU_getScreenPic(u8 *);
void GPU_updateLace(void);
s32  GPU_getMode(void);
void  GPU_setMode(u32 p1);


extern u16 *gpu_frame_buffer;
extern u16 *gpu_pvram;
extern s32 gpu_x_start, gpu_y_start, gpu_x_end, gpu_y_end;
extern s32 gpu_px, gpu_py;
extern s32 gpu_updateLace;
extern u32 gpu_writeDmaWidth, gpu_writeDmaHeight;
void gpu_videoOutput(void);
void gpu_sendPacket(void);
void gpu_reset(void);
void gpu_openVideo(long);
void gpu_closeVideo(void);

// Primitives 
void gpuClearImage(void);
void gpuDrawF3(void);


#endif

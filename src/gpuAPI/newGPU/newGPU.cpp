///////////////////////////////////////////////////////////////////////////////
//  NEWGPU main file
#include "newGPU.h"


///////////////////////////////////////////////////////////////////////////////
//  Tweaks and Hacks
int  skipCount          = 0;
int  skipRate           = 1;
bool enableFrameLimit   = false;
bool enableAbbeyHack    = false;
bool displayFrameInfo   = true;
bool displayGpuStats    = false;
bool displayVideoMemory = false;
bool primitiveDebugMode = false;
bool activeNullGPU      = false;
bool activeGPULog       = false;

///////////////////////////////////////////////////////////////////////////////
//  interlaced rendering
int linesInterlace      = 0;  //  0, 1, 3, 7
int linesInterlace_user = linesInterlace;

///////////////////////////////////////////////////////////////////////////////
//  GPU internal inits
bool  gpuInnerInit();


///////////////////////////////////////////////////////////////////////////////
//  GPU registering function
bool  register_NEWGPU()
{
  //  GPU inicialization/deinicialization functions
  GPU_init    = NEWGPU_init;
  GPU_done    = NEWGPU_done;
  GPU_freeze  = NEWGPU_freeze;

  //  GPU Vsinc Notification
  GPU_vSinc = NEWGPU_vSinc;

  //  GPU DMA comunication
  GPU_dmaChain      = NEWGPU_dmaChain;
  GPU_writeDataMem  = NEWGPU_writeDataMem;
  GPU_readDataMem   = NEWGPU_readDataMem;

  //  GPU Memory comunication
  GPU_writeData   = NEWGPU_writeData;
  GPU_writeStatus = NEWGPU_writeStatus;
  GPU_readData    = NEWGPU_readData;

  return gpuInnerInit();
}


///////////////////////////////////////////////////////////////////////////////
//  GPU Global data
///////////////////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////////////////
  //  Dma Transfers info
  s32		px,py;
  s32		x_start,y_start,x_end,y_end;
  u16*  pvram;

  s32 FrameToRead;
  s32 FrameToWrite;
  s32 FrameCount;
  s32 FrameIndex;

  u32 GP0;
  u32 OtherEnv[16];
  s32 PacketCount;
  s32 PacketIndex;

  ///////////////////////////////////////////////////////////////////////////////
  //  Stadistic and Timming
  u32 systime;
  u32 isSkip;
  u32 skipFrame			= 0;
  s32 vsincRateCounter  = 0;
  s32 frameRateCounter  = 0;
  s32 frameRealCounter  = 0;
  s32 vsincRate         = 60;
  s32 frameRate         = 60;
  s32 realRate          = 60;
  u32 framesTotal       = 0;

  u32 statF3            = 0;
  u32 statFT3           = 0;
  u32 statG3            = 0;
  u32 statGT3           = 0;
  u32 statLF            = 0;
  u32 statLG            = 0;
  u32 statS             = 0;
  u32 statT             = 0;

  u32 gpuPolyTime       = 0;
  u32 gpuPolyCount      = 0;
  u32 gpuRasterTime     = 0;
  u32 gpuRasterCount    = 0;
  u32 gpuPixelTime      = 0;
  u32 gpuPixelCount     = 0;
  u32 dmaChainTime      = 0;
  u32 dmaChainCount     = 0;
  u32 dmaMemTime        = 0;
  u32 dmaMemCount       = 0;
  u32 dmaPacketTime [256]= {0};
  u32 dmaPacketCount[256]= {0};


  ///////////////////////////////////////////////////////////////////////////////
  //  Display status
  s32	isPAL;
  u32 isDisplaySet;
  u32 DisplayArea   [8];
  u32 DirtyArea     [4];
  u32 LastDirtyArea [4];
  u32 CheckArea     [4];

  ///////////////////////////////////////////////////////////////////////////////
  //  Rasterizer status
  u32 TextureWindow [4];
  u32 DrawingArea   [4];
  u32 DrawingOffset [2];
  u32 MaskU;
  u32 MaskV;
  
  ///////////////////////////////////////////////////////////////////////////////
  //  Rasterizer status
  u8  Masking;
  u16 PixelMSB;

  u16* TBA;
  u16* CBA;
  u8*  TA;

  u32 BLEND_MODE;
  u32 TEXT_MODE;

  ///////////////////////////////////////////////////////////////////////////////
  //  Inner Loops
  u16*  Pixel;
  u16*  PixelEnd;
  s32   u4, du4;
  s32   v4, dv4;
  s32   r4, dr4;
  s32   g4, dg4;
  s32   b4, db4;
  u32   lInc;
  u32   tInc, tMsk;

  u32	  PixelData;
//u32   TextureU,TextureV;
//u8    LightR,LightG,LightB;
  long  skipNum = 1;
  long  SkipReset = 0;
  bool  isNewDisplay = false;


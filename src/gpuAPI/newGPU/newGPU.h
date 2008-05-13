#ifndef NEW_GPU_H
#define NEW_GPU_H

///////////////////////////////////////////////////////////////////////////////
//  NEWGPU INTERNAL main  file
#include "../gpuAPI.h"
#include "fixed.h"

  ///////////////////////////////////////////////////////////////////////////////
  //  Tweaks and Hacks
  extern  int  skipCount;
  extern  int  skipRate;
  extern  bool enableFrameLimit;
  extern  bool enableAbbeyHack;
  extern  bool displayFrameInfo;
  extern  bool displayGpuStats;
  extern  bool displayVideoMemory;
  extern  bool primitiveDebugMode;
  extern  bool activeNullGPU;
  extern  bool activeGPULog;

  ///////////////////////////////////////////////////////////////////////////////
  //  interlaced rendering
  extern  int linesInterlace;      //  0, 1, 3, 7
  extern  int linesInterlace_user;

  ///////////////////////////////////////////////////////////////////////////////
  //  Compile Options
  #define ENABLE_GPU_COMMAND_TABLE  // Optimization, must be on by default

//#define ENABLE_GPU_PRIM_STATS     // Enable GPU stats
//#define GPU_ARM_ASM               // Experimental functions
//#define ENABLE_GPU_PROFILLER      // Enable GPU profiling
//#define ENABLE_GPU_PRIMITIVE_DBG  // Enables primitive y primitive rendering
//#define ENABLE_GPU_NULL_SUPPORT   // Enables NullGPU support
//#define ENABLE_GPU_LOG_SUPPORT    // Enables gpu logger, very slow only for windows debugging

#ifdef ENABLE_GPU_PROFILLER
  #define ENABLE_PROFILLER
#endif
#include "profiller.h"

///////////////////////////////////////////////////////////////////////////////
//  GPU Interface functions declaration
///////////////////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////////////////
  //  GPU inicialization/deinicialization functions
  bool  NEWGPU_init          (void);
  void  NEWGPU_done          (void);
  bool  NEWGPU_freeze        (bool bWrite , GPUFreeze *pFreeze);

  ///////////////////////////////////////////////////////////////////////////////
  //  GPU Vsinc Notification
  void  NEWGPU_vSinc         (void);

  ///////////////////////////////////////////////////////////////////////////////
  //  GPU DMA comunication
  void  NEWGPU_dmaChain      (u32* baseAddr,   u32 dmaVAddr);
  void  NEWGPU_writeDataMem  (u32* dmaAddress, s32 dmaCount);
  void  NEWGPU_readDataMem   (u32* dmaAddress, s32 dmaCount);

  ///////////////////////////////////////////////////////////////////////////////
  //  GPU Memory comunication
  void  NEWGPU_writeData     (u32  data);
  void  NEWGPU_writeStatus   (u32  data);
  u32   NEWGPU_readData      (void);


///////////////////////////////////////////////////////////////////////////////
//  GPU Implementations shared functions
  void gpuReset           (void);
  void gpuDisplayVideoMem (void);
  void gpuSkipUpdate      (void);

///////////////////////////////////////////////////////////////////////////////
//  GPU Benchmarking
  void  gpuRecordReplay   (void);
  void  gpuRecordReset    (void);


///////////////////////////////////////////////////////////////////////////////
//  GPU Drivers
///////////////////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////////////////
  //  GPU gpuSendPacket driver
  typedef void (*PF)();
  extern  PF  gpuSendPackedDriver[256];

  ///////////////////////////////////////////////////////////////////////////////
  //  Pixel drawing drivers, for lines (only blending)
  extern  PF  gpuPixelDriver;
  extern  PF  gpuPixelDrivers[16];

  ///////////////////////////////////////////////////////////////////////////////
  //  Sprite innerloops driver
  extern  PF gpuSpriteSpanDriver;
  extern  PF gpuSpriteSpanDrivers[128];

  ///////////////////////////////////////////////////////////////////////////////
  //  Poligon innerloops driver
  extern  PF gpuPolySpanDriver;
  extern  PF gpuPolySpanDrivers[256];

  ///////////////////////////////////////////////////////////////////////////////
  //  Pixelops Tables
  extern  u8 gpuLitT[32*32];
  extern  u8 gpuSubT[32*32];
  extern  u8 gpuAddT[32*32];
//extern  u8 gpuAd2T[32*32];


///////////////////////////////////////////////////////////////////////////////
//  GPU Global data
///////////////////////////////////////////////////////////////////////////////
  // TODO: This is not the final way to go but I'll rearange all this soon

  ///////////////////////////////////////////////////////////////////////////////
  //  Dma Transfers info
  extern  s32		px,py;
  extern  s32		x_start,y_start,x_end,y_end;
  extern  u16*  pvram;

  extern  s32 FrameToRead;
  extern  s32 FrameToWrite;
  extern  s32 FrameCount;
  extern  s32 FrameIndex;

  extern  u32 GP0;
  extern  u32 OtherEnv[16];
  extern  s32 PacketCount;
  extern  s32 PacketIndex;


  ///////////////////////////////////////////////////////////////////////////////
  //  Stadistic and Timming
  extern  u32 systime;
  extern  u32 isSkip;
  extern  u32 skipFrame;

  extern  u32 framesTotal;
  extern  s32	vsincRateCounter;
  extern  s32	frameRateCounter;
  extern  s32	frameRealCounter;
  extern  s32 vsincRate;
  extern  s32 frameRate;
  extern  s32 realRate;

  extern  u32 statF3;
  extern  u32 statFT3;
  extern  u32 statG3;
  extern  u32 statGT3;
  extern  u32 statLF;
  extern  u32 statLG;
  extern  u32 statS;
  extern  u32 statT;

  extern  u32 gpuPolyTime;
  extern  u32 gpuPolyCount;
  extern  u32 gpuRasterTime;
  extern  u32 gpuRasterCount;
  extern  u32 gpuPixelTime;
  extern  u32 gpuPixelCount;
  extern  u32 dmaChainTime;
  extern  u32 dmaChainCount;
  extern  u32 dmaMemTime;
  extern  u32 dmaMemCount;
  extern  u32 dmaPacketTime [256];
  extern  u32 dmaPacketCount[256];


  ///////////////////////////////////////////////////////////////////////////////
  //  Display status
  extern  s32	isPAL;
  extern  u32 isDisplaySet;
  extern  u32 DisplayArea   [8];
  extern  u32 DirtyArea     [4];
  extern  u32 LastDirtyArea [4];
  extern  u32 CheckArea     [4];

  ///////////////////////////////////////////////////////////////////////////////
  //  Rasterizer status
  extern  u32 TextureWindow [4];
  extern  u32 DrawingArea   [4];
  extern  u32 DrawingOffset [2];
  extern  u32 MaskU;
  extern  u32 MaskV;
  
  ///////////////////////////////////////////////////////////////////////////////
  //  Rasterizer status
  extern  u8  Masking;
  extern  u16 PixelMSB;

  extern  u16* TBA;
  extern  u16* CBA;
  extern  u8*  TA;

  extern  u32 BLEND_MODE;
  extern  u32 TEXT_MODE;



  ///////////////////////////////////////////////////////////////////////////////
  //  Inner Loops
  extern  u16*  Pixel;
  extern  u16*  PixelEnd;
  extern  s32   u4, du4;
  extern  s32   v4, dv4;
  extern  s32   r4, dr4;
  extern  s32   g4, dg4;
  extern  s32   b4, db4;
  extern  u32   lInc;
  extern  u32   tInc, tMsk;

  extern  u32	  PixelData;
//extern  u32   TextureU,TextureV;
//extern  u8    LightR,LightG,LightB;

  extern  long	skipNum;
  extern  long  SkipReset;
  extern  bool  isNewDisplay;
///////////////////////////////////////////////////////////////////////////////
#endif  // NEW_GPU_H

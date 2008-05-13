#ifndef _GPU_API_
#define _GPU_API_
///////////////////////////////////////////////////////////////////////////////
#include "../psxTypes.h"

///////////////////////////////////////////////////////////////////////////////
//  GPU global definitions
#define	FRAME_BUFFER_SIZE	(1024*512*2)
#define	FRAME_WIDTH			  1024
#define	FRAME_HEIGHT		  512
#define	FRAME_OFFSET(x,y)	(((y)<<10)+(x))

///////////////////////////////////////////////////////////////////////////////
//  GPU defined interface for save states
struct  GPUFreeze
{
	u32 Version;
	u32 GP1;
	u32 Control[256];
	u8  FrameBuffer[0x100000];
};

struct  GPUTumbnail
{
  u8  Data[128*96*3];
};

struct  GPUPacket
{
  union
  {
    u32 U4[16];
    s32 S4[16];
    u16 U2[32];
    s16 S2[32];
    u8  U1[64];
	  s8  S1[64];
  };
};


///////////////////////////////////////////////////////////////////////////////
//  GPU common data for fast access.
extern bool GPU_IsInit;
extern u32  GPU_GP1;
#ifdef PSP
extern u16*  GPU_FrameBuffer;
#else
extern u16  GPU_FrameBuffer[FRAME_BUFFER_SIZE/2];    // FRAME_BUFFER_SIZE is defined in bytes
#endif
//#if defined(IPHONE)
//extern u32  GP1;
//#else
extern GPUPacket PacketBuffer;
extern GPUPacket PacketBufferBackup;
//#endif

///////////////////////////////////////////////////////////////////////////////
//  GPU Pluggin Enumeration/selection
u32         GPU_GetNumPluggin();
const char* GPU_GetPluginName(u32 idx);
bool        GPU_SelectPluggin(u32 idx);


///////////////////////////////////////////////////////////////////////////////
//  GPU inicialization/deinicialization functions
extern  bool  (*GPU_init)         (void);
extern  void  (*GPU_done)         (void);
extern  bool  (*GPU_freeze)       (bool bWrite , GPUFreeze *pFreeze);

///////////////////////////////////////////////////////////////////////////////
//  GPU Vsinc Notification
extern  void  (*GPU_vSinc)        (void);

///////////////////////////////////////////////////////////////////////////////
//  GPU DMA comunication
extern  void  (*GPU_dmaChain)     (u32* baseAddr,   u32 dmaVAddr);
extern  void  (*GPU_writeDataMem) (u32* dmaAddress, s32 dmaCount);
extern  void  (*GPU_readDataMem)  (u32* dmaAddress, s32 dmaCount);

///////////////////////////////////////////////////////////////////////////////
//  GPU Memory comunication
extern  void  (*GPU_writeData)    (u32  data);
extern  void  (*GPU_writeStatus)  (u32  data);
extern  u32   (*GPU_readData)     (void);
//#if defined(IPHONE)
//inline  u32     GPU_readStatus    (void)  { return GP1; }
//#else
inline  u32     GPU_readStatus    (void)  { return GPU_GP1; }
//#endif

///////////////////////////////////////////////////////////////////////////////
#endif //_GPU_API_

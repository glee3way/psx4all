#include "gpuAPI.h"


///////////////////////////////////////////////////////////////////////////////
//  GPU common data for fast access.
bool  GPU_IsInit = false;
u32   GPU_GP1;
#ifdef PSP
u16*   GPU_FrameBuffer = (u16*)(0x04000000 + 0x100000);
#else
u16   GPU_FrameBuffer[FRAME_BUFFER_SIZE/2];    // FRAME_BUFFER_SIZE is defined in bytes
#endif
//#if !defined(IPHONE)
GPUPacket PacketBuffer;
GPUPacket PacketBufferBackup;
//#endif

///////////////////////////////////////////////////////////////////////////////
//  GPU inicialization/deinicialization functions
bool  (*GPU_init)         (void);
void  (*GPU_done)         (void);
bool  (*GPU_freeze)       (bool bWrite , GPUFreeze *pFreeze);

///////////////////////////////////////////////////////////////////////////////
//  GPU Vsinc Notification
void  (*GPU_vSinc)        (void);

///////////////////////////////////////////////////////////////////////////////
//  GPU DMA comunication
void  (*GPU_dmaChain)     (u32* baseAddr,   u32 dmaVAddr);
void  (*GPU_writeDataMem) (u32* dmaAddress, s32 dmaCount);
void  (*GPU_readDataMem)  (u32* dmaAddress, s32 dmaCount);

///////////////////////////////////////////////////////////////////////////////
//  GPU Memory comunication
void  (*GPU_writeData)    (u32  data);
void  (*GPU_writeStatus)  (u32  data);
u32   (*GPU_readData)     (void);



///////////////////////////////////////////////////////////////////////////////
//  GPU Pluggin Enumeration/selection
enum
{
  E_GPU_NEWGPU = 0,
  E_GPU_GL,
  E_GPU_PSP,
  E_GPU_DRHELL,
  E_GPU_DUMMY,
  E_GPU_COUNT
};

static  char  s_gpuNames[E_GPU_COUNT][256] =
{
  "newGpu",
  "glGpu",
  "pspGpu",
  "drhellGpu",
  "dummyGpu",
};

bool  register_NEWGPU   ()
#if (defined(PSP) && defined(PSP_GPU))
{ return true;  }
#else
;
#endif
bool  register_GLGPU    ()    { return true;  }
bool  register_DUMMYGPU ()    { return true;  }
bool  register_PSPGPU   ()
#if defined(PSP) && defined(PSP_GPU)
;
#else
{ return true;  }
#endif
bool  register_DRHELLGPU   ()
//#if defined(IPHONE)
//;
//#else
{ return true;  }
//#endif
bool  register_NOGPU    ();

typedef bool  (*PFI)();
static  PFI s_gpuRegisters[E_GPU_COUNT] =
{
  register_NEWGPU,
  register_GLGPU,
  register_PSPGPU,
  register_DRHELLGPU,
  register_DUMMYGPU
};


///////////////////////////////////////////////////////////////////////////////
u32         GPU_GetNumPluggin()
{
  return E_GPU_COUNT;
}


///////////////////////////////////////////////////////////////////////////////
const char* GPU_GetPluginName(u32 idx)
{
  if(idx<E_GPU_COUNT)
    return s_gpuNames[idx];
  return "unknownGPU";
}


///////////////////////////////////////////////////////////////////////////////
bool        GPU_SelectPluggin(u32 idx)
{
  bool bInit = GPU_IsInit;
  bool bOk   = true;
  //GPUFreeze  local_freeze;
  if(bInit)
  {
    //bOk = GPU_freeze(1, &local_freeze);
    GPU_done();
  }

  if(bOk && idx<E_GPU_COUNT)
    bOk = s_gpuRegisters[idx]();
  else
    bOk = false;

  if(!bOk)
    register_NOGPU();

  if(bInit && bOk)
  {
    bOk = GPU_init();
    //bOk = bOk && GPU_freeze(0, &local_freeze);
  }

  return bOk;
}


///////////////////////////////////////////////////////////////////////////////
bool  register_NOGPU()
{
  //  GPU inicialization/deinicialization functions
  GPU_init    = 0;
  GPU_done    = 0;
  GPU_freeze  = 0;

  //  GPU Vsinc Notification
  GPU_vSinc = 0;

  //  GPU DMA comunication
  GPU_dmaChain      = 0;
  GPU_writeDataMem  = 0;
  GPU_readDataMem   = 0;

  //  GPU Memory comunication
  GPU_writeData   = 0;
  GPU_writeStatus = 0;
  GPU_readData    = 0;

  return true;
}



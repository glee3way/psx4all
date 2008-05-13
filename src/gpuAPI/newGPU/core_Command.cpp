///////////////////////////////////////////////////////////////////////////////
//  GPU "command buffer" execution/store
#include "newGPU.h"

///////////////////////////////////////////////////////////////////////////////
u8  TextureMask[32] = {
	255, 7, 15, 7, 31, 7, 15, 7, 63, 7, 15, 7, 31, 7, 15, 7,	//
	127, 7, 15, 7, 31, 7, 15, 7, 63, 7, 15, 7, 31, 7, 15, 7	  //
};


///////////////////////////////////////////////////////////////////////////////
//  Includes for Real Primitives
#include  "raster.h"


///////////////////////////////////////////////////////////////////////////////
void gpuSetTexture(u16 tpage)
{
	long tp;
	long tx, ty;
	GPU_GP1 = (GPU_GP1 & ~0x7FF) | (tpage & 0x7FF);

  MaskU	= TextureWindow[2];
  MaskV = TextureWindow[3];
  TextureWindow[0]&= ~TextureWindow[2];
  TextureWindow[1]&= ~TextureWindow[3];

  tp = (tpage >> 7) & 3;
	tx = (tpage & 0x0F) << 6;
	ty = (tpage & 0x10) << 4;

  tx += (TextureWindow[0] >> (2 - tp));
  ty += TextureWindow[1];
	
  BLEND_MODE  = (((tpage>>5)&0x3)     ) << 3;
  TEXT_MODE   = (((tpage>>7)&0x3) + 1 ) << 5; // +1 el cero no lo usamos

  TBA = &((u16*)GPU_FrameBuffer)[FRAME_OFFSET(tx, ty)];

}

///////////////////////////////////////////////////////////////////////////////
void gpuSetCLUT(u16 clut)
{
	CBA = &((u16*)GPU_FrameBuffer)[(clut & 0x7FFF) << 4];
}

#ifdef  ENABLE_GPU_NULL_SUPPORT
  #define NULL_GPU()  do { if(activeNullGPU)  return; } while(false)
#else
  #define NULL_GPU()  do {} while(false)
#endif

#ifdef  ENABLE_GPU_PRIMITIVE_DBG
  #define PRIM_DBG()  do { if(primitiveDebugMode)  gpuDisplayVideoMem(); } while(false)
#else
  #define PRIM_DBG()  do {} while(false)
#endif

#ifdef  ENABLE_GPU_LOG_SUPPORT
  #define DO_LOG(expr)   do { if(activeGPULog) printf expr; } while(false)
#else
  #define DO_LOG(expr)   do {} while(false)
#endif

#ifdef  ENABLE_GPU_PRIM_STATS
  #define DO_STAT(STAT)   do { ++stat##STAT; } while(false)
#else
  #define DO_STAT(STAT)   do {} while(false)
#endif

///////////////////////////////////////////////////////////////////////////////
//  Templated generated functino table.
#ifdef  ENABLE_GPU_COMMAND_TABLE
template<int PRIM>
void gpuSendPacketFunction(void)
{
  const u32 temp = PacketBuffer.U4[0];
#else
void gpuSendPacket(void)
{
  const u32 temp = PacketBuffer.U4[0];
  const u8 PRIM = temp>>24;
#endif

//printf("0x%x\n",PRIM);

  switch (PRIM)
  {
    case 0x00:
      DO_LOG(("Reset(%d)",PRIM));
      return;
    case 0x01:
      DO_LOG(("Clear Cache(%d)",PRIM));
      return;
    case 0x02:
      NULL_GPU();
      gpuClearImage();    //  prim handles updateLace && skip
      PRIM_DBG();
      DO_LOG(("gpuClearImage(%d)",PRIM));
      return;
    case 0x20:
    case 0x21:
    case 0x22:
    case 0x23:
      NULL_GPU();
      gpuPolySpanDriver  = gpuPolySpanDrivers [BLEND_MODE | 0 | Masking | (PRIM&0x2) | 0];
      gpuDrawF3();
      PRIM_DBG();
      DO_LOG(("gpuDrawF3(%d)",PRIM));
      DO_STAT(F3);
      return;
    case 0x24:
    case 0x25:
    case 0x26:
    case 0x27:
      NULL_GPU();
      gpuSetCLUT    (PacketBuffer.U4[2] >> 16);
      gpuSetTexture (PacketBuffer.U4[4] >> 16);
      gpuPolySpanDriver  = gpuPolySpanDrivers [BLEND_MODE | TEXT_MODE | Masking | (PRIM&0x2) | ((~PRIM)&0x1)];
      gpuDrawFT3();
      PRIM_DBG();
      DO_LOG(("gpuDrawFT3(%d)",PRIM));
      DO_STAT(FT3);
      return;
    case 0x28:
    case 0x29:
    case 0x2A:
    case 0x2B:
      NULL_GPU();
      gpuPolySpanDriver  = gpuPolySpanDrivers [BLEND_MODE | 0 | Masking | (PRIM&0x2) | 0];
    //--PacketBuffer.S2[6];
      gpuDrawF3();
      PacketBuffer.U4[1] = PacketBuffer.U4[4];
    //--PacketBuffer.S2[2];
      gpuDrawF3();
      PRIM_DBG();
      DO_LOG(("gpuDrawF4(%d)",PRIM));
      DO_STAT(F3);
      DO_STAT(F3);
      return;
    case 0x2C:
    case 0x2D:
    case 0x2E:
    case 0x2F:
      NULL_GPU();
      gpuSetCLUT    (PacketBuffer.U4[2] >> 16);
      gpuSetTexture (PacketBuffer.U4[4] >> 16);
      gpuPolySpanDriver  = gpuPolySpanDrivers [BLEND_MODE | TEXT_MODE | Masking | (PRIM&0x2) | ((~PRIM)&0x1)];
    //--PacketBuffer.S2[6];
      gpuDrawFT3();
      PacketBuffer.U4[1] = PacketBuffer.U4[7];
      PacketBuffer.U4[2] = PacketBuffer.U4[8];
    //--PacketBuffer.S2[2];
      gpuDrawFT3();
      PRIM_DBG();
      DO_LOG(("gpuDrawFT4(%d)",PRIM));
      DO_STAT(FT3);
      DO_STAT(FT3);
      return;
    case 0x30:
    case 0x31:
    case 0x32:
    case 0x33:
      NULL_GPU();
      gpuPolySpanDriver  = gpuPolySpanDrivers [BLEND_MODE | 0 | Masking | (PRIM&0x2) | 129];
      gpuDrawG3();
      PRIM_DBG();
      DO_LOG(("gpuDrawG3(%d)",PRIM));
      DO_STAT(G3);
      return;
    case 0x34:
    case 0x35:
    case 0x36:
    case 0x37:
      NULL_GPU();
      gpuSetCLUT    (PacketBuffer.U4[2] >> 16);
      gpuSetTexture (PacketBuffer.U4[5] >> 16);
      gpuPolySpanDriver  = gpuPolySpanDrivers [BLEND_MODE | TEXT_MODE | Masking | (PRIM&0x2) | ((~PRIM)&0x1) | 128];
      gpuDrawGT3();
      PRIM_DBG();
      DO_LOG(("gpuDrawGT3(%d)",PRIM));
      DO_STAT(GT3);
      return;
    case 0x38:
    case 0x39:
    case 0x3A:
    case 0x3B:
      NULL_GPU();
      gpuPolySpanDriver  = gpuPolySpanDrivers [BLEND_MODE | 0 | Masking | (PRIM&0x2) | 129];
    //--PacketBuffer.S2[6];
      gpuDrawG3();
      PacketBuffer.U4[0] = PacketBuffer.U4[6];
      PacketBuffer.U4[1] = PacketBuffer.U4[7];
    //--PacketBuffer.S2[2];
      gpuDrawG3();
      PRIM_DBG();
      DO_LOG(("gpuDrawG4(%d)",PRIM));
      DO_STAT(G3);
      DO_STAT(G3);
      return;
    case 0x3C:
    case 0x3D:
    case 0x3E:
    case 0x3F:
      NULL_GPU();
      gpuSetCLUT    (PacketBuffer.U4[2] >> 16);
      gpuSetTexture (PacketBuffer.U4[5] >> 16);
      gpuPolySpanDriver  = gpuPolySpanDrivers [BLEND_MODE | TEXT_MODE | Masking | (PRIM&0x2) | ((~PRIM)&0x1) | 128];
    //--PacketBuffer.S2[6];
      gpuDrawGT3();
      PacketBuffer.U4[0] = PacketBuffer.U4[9];
      PacketBuffer.U4[1] = PacketBuffer.U4[10];
      PacketBuffer.U4[2] = PacketBuffer.U4[11];
    //--PacketBuffer.S2[2];
      gpuDrawGT3();
      PRIM_DBG();
      DO_LOG(("gpuDrawGT4(%d)",PRIM));
      DO_STAT(GT3);
      DO_STAT(GT3);
      return;
    case 0x40:
    case 0x41:
    case 0x42:
    case 0x43:
      NULL_GPU();
      gpuPixelDriver = gpuPixelDrivers [ (BLEND_MODE | Masking | (PRIM&0x2)) >> 1];
      gpuDrawLF();
      PRIM_DBG();
      DO_LOG(("gpuDrawLF(%d)",PRIM));
      DO_STAT(LF);
      return;
    case 0x48:
    case 0x49:
    case 0x4A:
    case 0x4B:
    case 0x4C:
    case 0x4D:
    case 0x4E:
    case 0x4F:
      NULL_GPU();
      gpuPixelDriver = gpuPixelDrivers [ (BLEND_MODE | Masking | (PRIM&0x2)) >> 1];
      gpuDrawLF();
      if ((PacketBuffer.U4[3] & 0xF000F000) != 0x50005000)
      {
        PacketBuffer.U4[1] = PacketBuffer.U4[2];
        PacketBuffer.U4[2] = PacketBuffer.U4[3];
        PacketCount = 1;
        PacketIndex = 3;
      }
      PRIM_DBG();
      DO_LOG(("gpuDrawLF(%d)",PRIM));
      DO_STAT(LF);
      return;
    case 0x50:
    case 0x51:
    case 0x52:
    case 0x53:
      NULL_GPU();
      gpuPixelDriver = gpuPixelDrivers [ (BLEND_MODE | Masking | (PRIM&0x2)) >> 1];  // gouraud i shandled by primitive
      gpuDrawLG();
      PRIM_DBG();
      DO_LOG(("gpuDrawLG(%d)",PRIM));
      DO_STAT(LG);
      return;
    case 0x58:
    case 0x59:
    case 0x5A:
    case 0x5B:
    case 0x5C:
    case 0x5D:
    case 0x5E:
    case 0x5F:
      NULL_GPU();
      gpuPixelDriver = gpuPixelDrivers [ (BLEND_MODE | Masking | (PRIM&0x2)) >> 1];  // gouraud i shandled by primitive
      gpuDrawLG();
      if ((PacketBuffer.U4[4] & 0xF000F000) != 0x50005000)
      {
        PacketBuffer.U1[3 + (2 * 4)] = PacketBuffer.U1[3 + (0 * 4)];
        PacketBuffer.U4[0] = PacketBuffer.U4[2];
        PacketBuffer.U4[1] = PacketBuffer.U4[3];
        PacketBuffer.U4[2] = PacketBuffer.U4[4];
        PacketCount = 2;
        PacketIndex = 3;
      }
      PRIM_DBG();
      DO_LOG(("gpuDrawLG(%d)",PRIM));
      DO_STAT(LG);
      return;
    case 0x60:
    case 0x61:
    case 0x62:
    case 0x63:
      NULL_GPU();
      gpuSpriteSpanDriver  = gpuSpriteSpanDrivers [BLEND_MODE | 0 | Masking | (PRIM&0x2)];
      gpuDrawT();
      PRIM_DBG();
      DO_LOG(("gpuDrawT(%d)",PRIM));
      DO_STAT(T);
      return;
    case 0x64:
    case 0x65:
    case 0x66:
    case 0x67:
      NULL_GPU();
      gpuSetCLUT    (PacketBuffer.U4[2] >> 16);
      gpuSetTexture (GPU_GP1);
      gpuSpriteSpanDriver  = gpuSpriteSpanDrivers [BLEND_MODE | TEXT_MODE | Masking | (PRIM&0x2) | ((~PRIM)&0x1) ];
      gpuDrawS();
      PRIM_DBG();
      DO_LOG(("gpuDrawS(%d)",PRIM));
      DO_STAT(S);
      return;
    case 0x68:
    case 0x69:
    case 0x6A:
    case 0x6B:
      NULL_GPU();
      PacketBuffer.U4[2] = 0x00010001;
      gpuSpriteSpanDriver  = gpuSpriteSpanDrivers [BLEND_MODE | 0 | Masking | (PRIM&0x2)];
      gpuDrawT();
      PRIM_DBG();
      DO_LOG(("gpuDrawT(%d)",PRIM));
      DO_STAT(T);
      return;
    case 0x6C:
    case 0x6D:
    case 0x6E:
    case 0x6F:
      NULL_GPU();
      PacketBuffer.U4[3] = 0x00010001;
      gpuSetCLUT    (PacketBuffer.U4[2] >> 16);
      gpuSetTexture (GPU_GP1);
      gpuSpriteSpanDriver  = gpuSpriteSpanDrivers [BLEND_MODE | TEXT_MODE | Masking | (PRIM&0x2) | ((~PRIM)&0x1) ];
      gpuDrawS();
      PRIM_DBG();
      DO_LOG(("gpuDrawS(%d)",PRIM));
      DO_STAT(S);
      return;
    case 0x70:
    case 0x71:
    case 0x72:
    case 0x73:
      NULL_GPU();
      PacketBuffer.U4[2] = 0x00080008;
      gpuSpriteSpanDriver  = gpuSpriteSpanDrivers [BLEND_MODE | 0 | Masking | (PRIM&0x2)];
      gpuDrawT();
      PRIM_DBG();
      DO_LOG(("gpuDrawT(%d)",PRIM));
      DO_STAT(T);
      return;
    case 0x74:
    case 0x75:
    case 0x76:
    case 0x77:
      NULL_GPU();
      PacketBuffer.U4[3] = 0x00080008;
      gpuSetCLUT    (PacketBuffer.U4[2] >> 16);
      gpuSetTexture (GPU_GP1);
      gpuSpriteSpanDriver  = gpuSpriteSpanDrivers [BLEND_MODE | TEXT_MODE | Masking | (PRIM&0x2) | ((~PRIM)&0x1) ];
      gpuDrawS();
      PRIM_DBG();
      DO_LOG(("gpuDrawS(%d)",PRIM));
      DO_STAT(S);
      return;
    case 0x78:
    case 0x79:
    case 0x7A:
    case 0x7B:
      NULL_GPU();
      PacketBuffer.U4[2] = 0x00100010;
      gpuSpriteSpanDriver  = gpuSpriteSpanDrivers [BLEND_MODE | 0 | Masking | (PRIM&0x2)];
      gpuDrawT();
      PRIM_DBG();
      DO_LOG(("gpuDrawT(%d)",PRIM));
      DO_STAT(T);
      return;
    case 0x7C:
    case 0x7D:
    case 0x7E:
    case 0x7F:
      NULL_GPU();
      PacketBuffer.U4[3] = 0x00100010;
      gpuSetCLUT    (PacketBuffer.U4[2] >> 16);
      gpuSetTexture (GPU_GP1);
      gpuSpriteSpanDriver  = gpuSpriteSpanDrivers [BLEND_MODE | TEXT_MODE | Masking | (PRIM&0x2) | ((~PRIM)&0x1) ];
      gpuDrawS();
      PRIM_DBG();
      DO_LOG(("gpuDrawS(%d)",PRIM));
      DO_STAT(S);
      return;
    case 0x80:          //  vid -> vid
      gpuMoveImage();   //  prim handles updateLace && skip
      PRIM_DBG();
      DO_LOG(("gpuMoveImage(%d)",PRIM));
      return;
    case 0xA0:          //  sys ->vid
      gpuLoadImage();   //  prim handles updateLace && skip
      PRIM_DBG();
      DO_LOG(("gpuLoadImage(%d)",PRIM));
      return;
    case 0xC0:          //  vid -> sys
      gpuStoreImage();  //  prim handles updateLace && skip
      PRIM_DBG();
      DO_LOG(("gpuStoreImage(%d)",PRIM));
      return;
    case 0xE1:
	  //isJustDraw++;
      GPU_GP1 = (GPU_GP1 & ~0x000007FF) | (temp & 0x000007FF);
      gpuSetTexture(temp);
      
    //Masking = ((temp>>11) & 0x2) <<  1;
    //PixelMSB =((temp>>11) & 0x1) << 15;

      DO_LOG(("gpuSetTexture(%d)",PRIM));
      return;
    case 0xE2:	  
	  //isJustDraw++;
	TextureWindow[0] = ((temp >> 10) & 0x1F) << 3;
	TextureWindow[1] = ((temp >> 15) & 0x1F) << 3;
	TextureWindow[2] = TextureMask[(temp >> 0) & 0x1F];
	TextureWindow[3] = TextureMask[(temp >> 5) & 0x1F];
	gpuSetTexture(GPU_GP1);
	isSkip = 0;
	DO_LOG(("TextureWindow(%d)",PRIM));
	return;
    case 0xE3:
	DrawingArea[0] = temp         & 0x3FF;
	DrawingArea[1] = (temp >> 10) & 0x3FF;
	isSkip = 0;
	//isNewDisplay = true;
	//SysPrintf("E3 %x %x", DrawingArea[0], DrawingArea[1]); usleep(100000);// EDIT TEMP
	DO_LOG(("DrawingArea_Pos(%d)",PRIM));
	return;
    case 0xE4:
	DrawingArea[2] = (temp         & 0x3FF) + 1;
	DrawingArea[3] = ((temp >> 10) & 0x3FF) + 1;
	isSkip = 0;
	//SysPrintf("E4 %x %x", DrawingArea[2], DrawingArea[3]); usleep(100000);// EDIT TEMP
	DO_LOG(("DrawingArea_Size(%d)",PRIM));
	return;
    case 0xE5:
      //SysMessage("E5 %x", temp); // EDIT TEMP
	  //isJustDraw++;
	DrawingOffset[0] = ((long)temp<<(32-11))>>(32-11);
	DrawingOffset[1] = ((long)temp<<(32-22))>>(32-11);
	isSkip = 0;
      DO_LOG(("DrawingOffset(%d)",PRIM));
      return;
    case 0xE6:
    //GPU_GP1 = (GPU_GP1 & ~0x00001800) | ((temp&3) << 11);
      Masking = (temp & 0x2) <<  1;
      PixelMSB =(temp & 0x1) << 15;
      DO_LOG(("SetMask(%d)",PRIM));
      return;
  }
}


#ifdef  ENABLE_GPU_COMMAND_TABLE
typedef void (*PSPF)();
PSPF  gpuSendPackedDriver[256] =
{
  gpuSendPacketFunction<0x00>,gpuSendPacketFunction<0x01>,gpuSendPacketFunction<0x02>,gpuSendPacketFunction<0x03>,  gpuSendPacketFunction<0x04>,gpuSendPacketFunction<0x05>,gpuSendPacketFunction<0x06>,gpuSendPacketFunction<0x07>,  gpuSendPacketFunction<0x08>,gpuSendPacketFunction<0x09>,gpuSendPacketFunction<0x0A>,gpuSendPacketFunction<0x0B>,  gpuSendPacketFunction<0x0C>,gpuSendPacketFunction<0x0D>,gpuSendPacketFunction<0x0E>,gpuSendPacketFunction<0x0F>,
  gpuSendPacketFunction<0x10>,gpuSendPacketFunction<0x11>,gpuSendPacketFunction<0x12>,gpuSendPacketFunction<0x13>,  gpuSendPacketFunction<0x14>,gpuSendPacketFunction<0x15>,gpuSendPacketFunction<0x16>,gpuSendPacketFunction<0x17>,  gpuSendPacketFunction<0x18>,gpuSendPacketFunction<0x19>,gpuSendPacketFunction<0x1A>,gpuSendPacketFunction<0x1B>,  gpuSendPacketFunction<0x1C>,gpuSendPacketFunction<0x1D>,gpuSendPacketFunction<0x1E>,gpuSendPacketFunction<0x1F>,
  gpuSendPacketFunction<0x20>,gpuSendPacketFunction<0x21>,gpuSendPacketFunction<0x22>,gpuSendPacketFunction<0x23>,  gpuSendPacketFunction<0x24>,gpuSendPacketFunction<0x25>,gpuSendPacketFunction<0x26>,gpuSendPacketFunction<0x27>,  gpuSendPacketFunction<0x28>,gpuSendPacketFunction<0x29>,gpuSendPacketFunction<0x2A>,gpuSendPacketFunction<0x2B>,  gpuSendPacketFunction<0x2C>,gpuSendPacketFunction<0x2D>,gpuSendPacketFunction<0x2E>,gpuSendPacketFunction<0x2F>,
  gpuSendPacketFunction<0x30>,gpuSendPacketFunction<0x31>,gpuSendPacketFunction<0x32>,gpuSendPacketFunction<0x33>,  gpuSendPacketFunction<0x34>,gpuSendPacketFunction<0x35>,gpuSendPacketFunction<0x36>,gpuSendPacketFunction<0x37>,  gpuSendPacketFunction<0x38>,gpuSendPacketFunction<0x39>,gpuSendPacketFunction<0x3A>,gpuSendPacketFunction<0x3B>,  gpuSendPacketFunction<0x3C>,gpuSendPacketFunction<0x3D>,gpuSendPacketFunction<0x3E>,gpuSendPacketFunction<0x3F>,
  gpuSendPacketFunction<0x40>,gpuSendPacketFunction<0x41>,gpuSendPacketFunction<0x42>,gpuSendPacketFunction<0x43>,  gpuSendPacketFunction<0x44>,gpuSendPacketFunction<0x45>,gpuSendPacketFunction<0x46>,gpuSendPacketFunction<0x47>,  gpuSendPacketFunction<0x48>,gpuSendPacketFunction<0x49>,gpuSendPacketFunction<0x4A>,gpuSendPacketFunction<0x4B>,  gpuSendPacketFunction<0x4C>,gpuSendPacketFunction<0x4D>,gpuSendPacketFunction<0x4E>,gpuSendPacketFunction<0x4F>,
  gpuSendPacketFunction<0x50>,gpuSendPacketFunction<0x51>,gpuSendPacketFunction<0x52>,gpuSendPacketFunction<0x53>,  gpuSendPacketFunction<0x54>,gpuSendPacketFunction<0x55>,gpuSendPacketFunction<0x56>,gpuSendPacketFunction<0x57>,  gpuSendPacketFunction<0x58>,gpuSendPacketFunction<0x59>,gpuSendPacketFunction<0x5A>,gpuSendPacketFunction<0x5B>,  gpuSendPacketFunction<0x5C>,gpuSendPacketFunction<0x5D>,gpuSendPacketFunction<0x5E>,gpuSendPacketFunction<0x5F>,
  gpuSendPacketFunction<0x60>,gpuSendPacketFunction<0x61>,gpuSendPacketFunction<0x62>,gpuSendPacketFunction<0x63>,  gpuSendPacketFunction<0x64>,gpuSendPacketFunction<0x65>,gpuSendPacketFunction<0x66>,gpuSendPacketFunction<0x67>,  gpuSendPacketFunction<0x68>,gpuSendPacketFunction<0x69>,gpuSendPacketFunction<0x6A>,gpuSendPacketFunction<0x6B>,  gpuSendPacketFunction<0x6C>,gpuSendPacketFunction<0x6D>,gpuSendPacketFunction<0x6E>,gpuSendPacketFunction<0x6F>,
  gpuSendPacketFunction<0x70>,gpuSendPacketFunction<0x71>,gpuSendPacketFunction<0x72>,gpuSendPacketFunction<0x73>,  gpuSendPacketFunction<0x74>,gpuSendPacketFunction<0x75>,gpuSendPacketFunction<0x76>,gpuSendPacketFunction<0x77>,  gpuSendPacketFunction<0x78>,gpuSendPacketFunction<0x79>,gpuSendPacketFunction<0x7A>,gpuSendPacketFunction<0x7B>,  gpuSendPacketFunction<0x7C>,gpuSendPacketFunction<0x7D>,gpuSendPacketFunction<0x7E>,gpuSendPacketFunction<0x7F>,

  gpuSendPacketFunction<0x80>,gpuSendPacketFunction<0x81>,gpuSendPacketFunction<0x82>,gpuSendPacketFunction<0x83>,  gpuSendPacketFunction<0x84>,gpuSendPacketFunction<0x85>,gpuSendPacketFunction<0x86>,gpuSendPacketFunction<0x87>,  gpuSendPacketFunction<0x88>,gpuSendPacketFunction<0x89>,gpuSendPacketFunction<0x8A>,gpuSendPacketFunction<0x8B>,  gpuSendPacketFunction<0x8C>,gpuSendPacketFunction<0x8D>,gpuSendPacketFunction<0x8E>,gpuSendPacketFunction<0x8F>,
  gpuSendPacketFunction<0x90>,gpuSendPacketFunction<0x91>,gpuSendPacketFunction<0x92>,gpuSendPacketFunction<0x93>,  gpuSendPacketFunction<0x94>,gpuSendPacketFunction<0x95>,gpuSendPacketFunction<0x96>,gpuSendPacketFunction<0x97>,  gpuSendPacketFunction<0x98>,gpuSendPacketFunction<0x99>,gpuSendPacketFunction<0x9A>,gpuSendPacketFunction<0x9B>,  gpuSendPacketFunction<0x9C>,gpuSendPacketFunction<0x9D>,gpuSendPacketFunction<0x9E>,gpuSendPacketFunction<0x9F>,
  gpuSendPacketFunction<0xa0>,gpuSendPacketFunction<0xa1>,gpuSendPacketFunction<0xa2>,gpuSendPacketFunction<0xa3>,  gpuSendPacketFunction<0xa4>,gpuSendPacketFunction<0xa5>,gpuSendPacketFunction<0xa6>,gpuSendPacketFunction<0xa7>,  gpuSendPacketFunction<0xa8>,gpuSendPacketFunction<0xa9>,gpuSendPacketFunction<0xaA>,gpuSendPacketFunction<0xaB>,  gpuSendPacketFunction<0xaC>,gpuSendPacketFunction<0xaD>,gpuSendPacketFunction<0xaE>,gpuSendPacketFunction<0xaF>,
  gpuSendPacketFunction<0xb0>,gpuSendPacketFunction<0xb1>,gpuSendPacketFunction<0xb2>,gpuSendPacketFunction<0xb3>,  gpuSendPacketFunction<0xb4>,gpuSendPacketFunction<0xb5>,gpuSendPacketFunction<0xb6>,gpuSendPacketFunction<0xb7>,  gpuSendPacketFunction<0xb8>,gpuSendPacketFunction<0xb9>,gpuSendPacketFunction<0xbA>,gpuSendPacketFunction<0xbB>,  gpuSendPacketFunction<0xbC>,gpuSendPacketFunction<0xbD>,gpuSendPacketFunction<0xbE>,gpuSendPacketFunction<0xbF>,
  gpuSendPacketFunction<0xc0>,gpuSendPacketFunction<0xc1>,gpuSendPacketFunction<0xc2>,gpuSendPacketFunction<0xc3>,  gpuSendPacketFunction<0xc4>,gpuSendPacketFunction<0xc5>,gpuSendPacketFunction<0xc6>,gpuSendPacketFunction<0xc7>,  gpuSendPacketFunction<0xc8>,gpuSendPacketFunction<0xc9>,gpuSendPacketFunction<0xcA>,gpuSendPacketFunction<0xcB>,  gpuSendPacketFunction<0xcC>,gpuSendPacketFunction<0xcD>,gpuSendPacketFunction<0xcE>,gpuSendPacketFunction<0xcF>,
  gpuSendPacketFunction<0xd0>,gpuSendPacketFunction<0xd1>,gpuSendPacketFunction<0xd2>,gpuSendPacketFunction<0xd3>,  gpuSendPacketFunction<0xd4>,gpuSendPacketFunction<0xd5>,gpuSendPacketFunction<0xd6>,gpuSendPacketFunction<0xd7>,  gpuSendPacketFunction<0xd8>,gpuSendPacketFunction<0xd9>,gpuSendPacketFunction<0xdA>,gpuSendPacketFunction<0xdB>,  gpuSendPacketFunction<0xdC>,gpuSendPacketFunction<0xdD>,gpuSendPacketFunction<0xdE>,gpuSendPacketFunction<0xdF>,
  gpuSendPacketFunction<0xe0>,gpuSendPacketFunction<0xe1>,gpuSendPacketFunction<0xe2>,gpuSendPacketFunction<0xe3>,  gpuSendPacketFunction<0xe4>,gpuSendPacketFunction<0xe5>,gpuSendPacketFunction<0xe6>,gpuSendPacketFunction<0xe7>,  gpuSendPacketFunction<0xe8>,gpuSendPacketFunction<0xe9>,gpuSendPacketFunction<0xeA>,gpuSendPacketFunction<0xeB>,  gpuSendPacketFunction<0xeC>,gpuSendPacketFunction<0xeD>,gpuSendPacketFunction<0xeE>,gpuSendPacketFunction<0xeF>,
  gpuSendPacketFunction<0xf0>,gpuSendPacketFunction<0xf1>,gpuSendPacketFunction<0xf2>,gpuSendPacketFunction<0xf3>,  gpuSendPacketFunction<0xf4>,gpuSendPacketFunction<0xf5>,gpuSendPacketFunction<0xf6>,gpuSendPacketFunction<0xf7>,  gpuSendPacketFunction<0xf8>,gpuSendPacketFunction<0xf9>,gpuSendPacketFunction<0xfA>,gpuSendPacketFunction<0xfB>,  gpuSendPacketFunction<0xfC>,gpuSendPacketFunction<0xfD>,gpuSendPacketFunction<0xfE>,gpuSendPacketFunction<0xfF>
};
#endif

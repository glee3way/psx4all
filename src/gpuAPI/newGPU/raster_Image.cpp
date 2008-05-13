///////////////////////////////////////////////////////////////////////////////
//  GPU internal image manipulation, load, store, move, clear
#include "newGPU.h"
#include "raster.h"
#include "fixed.h"


///////////////////////////////////////////////////////////////////////////////
void gpuLoadImage(void)
{
	u16 x0, y0, w0, h0;
	x0 = PacketBuffer.U2[2] & 1023;
	y0 = PacketBuffer.U2[3] & 511;
	w0 = PacketBuffer.U2[4];
	h0 = PacketBuffer.U2[5];

	//isJustDraw = 0;
	FrameIndex = FRAME_OFFSET(x0, y0);
	if ((y0 + h0) > FRAME_HEIGHT)
	{
		h0 = FRAME_HEIGHT - y0;
	}

	FrameToWrite = w0 * h0;
	FrameCount  =  w0;

	px = x_start = x0;
	py = y_start = y0;
	x_end = x_start + w0;
	y_end = y_start + h0;
	pvram = &((u16*)GPU_FrameBuffer)[py*1024];

	GPU_GP1 |= 0x08000000;
}

///////////////////////////////////////////////////////////////////////////////
void gpuStoreImage(void)
{
	u16 x0, y0, w0, h0;
	x0 = PacketBuffer.U2[2] & 1023;
	y0 = PacketBuffer.U2[3] & 511;
	w0 = PacketBuffer.U2[4];
	h0 = PacketBuffer.U2[5];

	FrameIndex = FRAME_OFFSET(x0, y0);
	if ((y0 + h0) > FRAME_HEIGHT)
	{
		h0 = FRAME_HEIGHT - y0;
	}
	FrameToRead = w0 * h0;
	FrameCount  = w0;

	px = x_start = x0;
	py = y_start = y0;
	x_end = x_start + w0;
	y_end = y_start + h0;
	pvram = &((u16*)GPU_FrameBuffer)[py*1024];
	
	GPU_GP1 |= 0x08000000;
}

///////////////////////////////////////////////////////////////////////////////
extern "C" void ARM_MoveImage( unsigned char* src, unsigned char* dst, unsigned long src_advance, unsigned long w0, unsigned long h1 );

#if !defined(GP2X) && !defined(ZAURUS) && !defined(IPHONE)
///////////////////////////////////////////////////////////////////////////////
void gpuMoveImage(void)
{
  s32 x0, y0, x1, y1, w0, h0;
  u16 *lpDst, *lpSrc;
  x0 = PacketBuffer.U2[2] & 1023;
  y0 = PacketBuffer.U2[3] & 511;
  x1 = PacketBuffer.U2[4] & 1023;
  y1 = PacketBuffer.U2[5] & 511;
  w0 = PacketBuffer.U2[6];
  h0 = PacketBuffer.U2[7];

  if( (x0==x1) && (y0==y1) )
    return;
  if ((y0 + h0) > FRAME_HEIGHT)
    return;
  if ((y1 + h0) > FRAME_HEIGHT)
    return;

  //isJustDraw = 0;
  lpDst = lpSrc = (u16*)GPU_FrameBuffer;
  lpSrc += FRAME_OFFSET(x0, y0);
  lpDst += FRAME_OFFSET(x1, y1);
  x1 = FRAME_WIDTH - w0;

  for (; h0; h0--)
  {
    for (x0 = w0; x0; x0--)
      *lpDst++ = *lpSrc++;
    lpDst += x1;
    lpSrc += x1;
  }
}

#else

void gpuMoveImage(void)
{
  s32   x0, y0, x1, y1, w0, h0;
  u16*  lpDst, *lpSrc;
  x0 = PacketBuffer.U2[2] & 1023;
  y0 = PacketBuffer.U2[3] & 511;
  x1 = PacketBuffer.U2[4] & 1023;
  y1 = PacketBuffer.U2[5] & 511;
  w0 = PacketBuffer.U2[6];
  h0 = PacketBuffer.U2[7];

  if( (x0==x1) && (y0==y1) )
    return;
  if ((y0 + h0) > FRAME_HEIGHT)
    return;
  if ((y1 + h0) > FRAME_HEIGHT)
    return;

  //isJustDraw = 0;
  lpDst = lpSrc = (u16*)GPU_FrameBuffer;
  lpSrc += FRAME_OFFSET(x0, y0);
  lpDst += FRAME_OFFSET(x1, y1);
  x1 = FRAME_WIDTH - w0;

  ARM_MoveImage( (unsigned char*)lpSrc, (unsigned char*)lpDst, x1<<1, w0,  h0 );

}
#endif

///////////////////////////////////////////////////////////////////////////////
extern "C" void ARM_ClearImage_Fast ( unsigned char* dst, unsigned long src_advance, unsigned long rgb, unsigned long w0, unsigned long h1 );
extern "C" void ARM_ClearImage      ( unsigned char* dst, unsigned long src_advance, unsigned long rgb, unsigned long w0, unsigned long h1 );

#if !defined(GP2X) && !defined(ZAURUS) && !defined(IPHONE)
///////////////////////////////////////////////////////////////////////////////
void gpuClearImage(void)
{
  s32   x0, y0, w0, h0;
  u16*  pixel;
  u32   rgb;

	if (isSkip)
	{
		return;
	}

  rgb = GPU_RGB16(PacketBuffer.S4[0]);
  rgb|= rgb<<16;

  x0 = PacketBuffer.S2[2];
  y0 = PacketBuffer.S2[3];
  w0 = PacketBuffer.S2[4];
  h0 = PacketBuffer.S2[5];
 
  w0 += x0;
  if (x0 < 0)
    x0 = 0;
  if (w0 > FRAME_WIDTH)
    w0 = FRAME_WIDTH;
  w0 -= x0;
  if (w0 <= 0)
    return;
  h0 += y0;
  if (y0 < 0)
    y0 = 0;
  if (h0 > FRAME_HEIGHT)
    h0 = FRAME_HEIGHT;
  h0 -= y0;
  if (h0 <= 0)
    return;

  pixel = (u16*)GPU_FrameBuffer + FRAME_OFFSET(x0, y0);
  y0 = FRAME_WIDTH - w0;
  for (; h0; --h0)
  {
    for (x0 = w0; x0; --x0)
      *pixel++ = rgb;
    pixel += y0;
  }
}
#else

///////////////////////////////////////////////////////////////////////////////
void gpuClearImage(void)
{
  s32   x0, y0, w0, h0;
  u16*  pixel;
  u32   rgb;

	if (isSkip)
	{
		return;
	}

  rgb = GPU_RGB16(PacketBuffer.S4[0]);
  rgb|= rgb<<16;
  
  x0 = PacketBuffer.S2[2];
  y0 = PacketBuffer.S2[3];
  w0 = PacketBuffer.S2[4];
  h0 = PacketBuffer.S2[5];

  w0 += x0;
  if (x0 < 0)
    x0 = 0;
  if (w0 > FRAME_WIDTH)
    w0 = FRAME_WIDTH;
  w0 -= x0;
  if (w0 <= 0)
    return;
  h0 += y0;
  if (y0 < 0)
    y0 = 0;
  if (h0 > FRAME_HEIGHT)
    h0 = FRAME_HEIGHT;
  h0 -= y0;
  if (h0 <= 0)
    return;

  pixel = (u16*)GPU_FrameBuffer + FRAME_OFFSET(x0, y0);
  y0 = FRAME_WIDTH - w0;

  //if(w0&0xf)
  //  ARM_ClearImage     ( (unsigned char*)pixel, y0<<1, rgb, w0, h0 );
  //else
    ARM_ClearImage_Fast( (unsigned char*)pixel, y0<<1, rgb, w0, h0 );
}

#endif

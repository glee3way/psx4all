///////////////////////////////////////////////////////////////////////////////
//  GPU internal poligon drawing functions
#include "newGPU.h"
#include "raster.h"

/*
void gpuDrawS(void)
{
}
void gpuDrawT(void)
{
}
*/

///////////////////////////////////////////////////////////////////////////////
#define	GPU_RGB16(rgb) ((((rgb)&0xF80000)>>9)|(((rgb)&0xF800)>>6)|(((rgb)&0xF8)>>3))

/*
///////////////////////////////////////////////////////////////////////////////
static  bool  HitDrawingArea(int x0,int y0, int x1, int y1)
{
  s32 xmin = DrawingArea[0];  s32 xmax = DrawingArea[2];
	s32 ymin = DrawingArea[1];  s32 ymax = DrawingArea[3];
  if(x0<xmin) x0=xmin;
  if(y0<ymin) y0=ymin;
  if(x1>xmax) x1=xmax;
  if(y1>ymax) y1=ymax;
  x1-=x0;
  y1-=y0;
  
  return (x1>0 && y1>0);
}
*/

///////////////////////////////////////////////////////////////////////////////
void gpuDrawS(void)
{
	s32 temp;
	s32 xmin, xmax;
	s32 ymin, ymax;
	s32 x0, x1;
	s32 y0, y1;
	s32 u0;
	s32 v0;

	x1 = x0 = GPU_EXPANDSIGN_SPRT(PacketBuffer.S2[2]) + DrawingOffset[0];
  y1 = y0 = GPU_EXPANDSIGN_SPRT(PacketBuffer.S2[3]) + DrawingOffset[1];
  x1+= PacketBuffer.S2[6];
  y1+= PacketBuffer.S2[7];

	xmin = DrawingArea[0];	xmax = DrawingArea[2];
	ymin = DrawingArea[1];	ymax = DrawingArea[3];

  int rx0 = Max2(xmin,Min2(x0,x1));
  int ry0 = Max2(ymin,Min2(y0,y1));
  int rx1 = Min2(xmax,Max2(x0,x1));
  int ry1 = Min2(ymax,Max2(y0,y1));
  if( rx0>=rx1 || ry0>=ry1)
    return;

  //AddDirtyArea(rx0,ry0,rx1,ry1);
  if(isSkip) return;

  u0 = PacketBuffer.U1[8];
  v0 = PacketBuffer.U1[9];

  r4 = s32(PacketBuffer.U1[0]);
  g4 = s32(PacketBuffer.U1[1]);
  b4 = s32(PacketBuffer.U1[2]);

	temp = ymin - y0;
	if (temp > 0) {
		y0 = ymin;
		v0 += temp;
	}
	if (y1 > ymax)
		y1 = ymax;

	temp = xmin - x0;
	if (temp > 0) {
		x0 = xmin;
		u0 += temp;
	}
	if (x1 > xmax)
		x1 = xmax;
  x1 -= x0;
	if (x1 < 0)
		x1 = 0;
	
	Pixel = &((u16*)GPU_FrameBuffer)[FRAME_OFFSET(x0, y0)];
  v4    = v0;
	for (;y0<y1;++y0)
  {
    u4  = u0;
    PixelEnd = Pixel+x1; 
    if( 0 == (y0&linesInterlace) )
      gpuSpriteSpanDriver();
    Pixel += FRAME_WIDTH;
  	v4 = (++v4)&MaskV;
	}

}

///////////////////////////////////////////////////////////////////////////////
void gpuDrawT(void)
{
	s32 xmin, xmax;
	s32 ymin, ymax;
	s32 x0, y0;
	s32 x1, y1;

	x1 = x0 = GPU_EXPANDSIGN_SPRT(PacketBuffer.S2[2]) + DrawingOffset[0];
  y1 = y0 = GPU_EXPANDSIGN_SPRT(PacketBuffer.S2[3]) + DrawingOffset[1];
  x1+= PacketBuffer.S2[4];
  y1+= PacketBuffer.S2[5];

	xmin = DrawingArea[0];	xmax = DrawingArea[2];
	ymin = DrawingArea[1];	ymax = DrawingArea[3];

  int rx0 = Max2(xmin,Min2(x0,x1));
  int ry0 = Max2(ymin,Min2(y0,y1));
  int rx1 = Min2(xmax,Max2(x0,x1));
  int ry1 = Min2(ymax,Max2(y0,y1));
  if(rx0>=rx1 || ry0>=ry1)
    return;

  //AddDirtyArea(rx0,ry0,rx1,ry1);
  if(isSkip) return;

	PixelData = GPU_RGB16(PacketBuffer.U4[0]);

	if (y0 < ymin)
		y0 = ymin;
	if (y1 > ymax)
		y1 = ymax;

	if (x0 < xmin)
		x0 = xmin;
	if (x1 > xmax)
		x1 = xmax;

  x1 -= x0;
	if (x1 < 0)
		x1 = 0;
	Pixel = &((u16*)GPU_FrameBuffer)[FRAME_OFFSET(x0, y0)];
	for (; y0<y1; ++y0)
  {
    PixelEnd = Pixel+x1; 
    if( 0 == (y0&linesInterlace) )
      gpuSpriteSpanDriver();
    Pixel += FRAME_WIDTH;
	}
}

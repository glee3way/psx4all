/***********************************************************************
*
*	Dr.Hell's WinGDI GPU Plugin
*	Version 0.8
*	Copyright (C)Dr.Hell, 2002-2004
*
*	Device Depended
*
***********************************************************************/

#include "minimal.h"

/*----------------------------------------------------------------------
Macro
----------------------------------------------------------------------*/

#define	GetMem(p1)	malloc(p1)
#define	FreeMem(p1)	free(p1)
#define	CopyMem(p1,p2,p3)	memcpy((p1),(p2),(p3))
#define	ZeroMem(p1,p2)		memset((p1), 0, (p2))


void gpuDrawScreen(void)
{
	if (GP1 & 0x00800000)
		return;

	static s16 old_res_horz, old_res_vert, old_rgb24;
	s16 h0, x0, y0, w0, h1;

	x0 = DisplayArea[0];
	y0 = DisplayArea[1];

	w0 = DisplayArea[2];
	h0 = DisplayArea[3];  // video mode

	h1 = DisplayArea[7] - DisplayArea[5]; // display needed
	if (h0 == 480)
		h1 = Min2(h1*2,480);

#ifdef ZAURUS
	if(SDL_MUSTLOCK(gp2x_sdlwrapper_screen)) SDL_LockSurface(gp2x_sdlwrapper_screen);
#endif

	u16* dest_screen16 = gp2x_screen16;
	u16* src_screen16  = &((u16*)FrameBuffer)[FRAME_OFFSET(x0,y0)];
	u32 isRGB24 = (GP1 & 0x00200000 ? 32 : 0);
	/* Clear the screen if resolution changed to prevent interlacing and clipping to clash */
	if( (w0 != old_res_horz || h1 != old_res_vert || (s16)isRGB24 != old_rgb24) )
	{
		// Update old resolution
		old_res_horz = w0;
		old_res_vert = h1;
		old_rgb24 = (s16)isRGB24;
		// Finally, clear the screen for this special case
		gp2x_video_RGB_clearscreen16();
	}

	//  Height centering
  int sizeShift = 1;
  if(h0==256)
    h0 = 240;
  else
  if(h0==480)
    sizeShift = 2;

  if(h1>h0)
  {
		src_screen16  += ((h1-h0)>>sizeShift)*1024;
    h1 = h0;
  }
  else
  if(h1<h0)
    dest_screen16 += ((h0-h1)>>sizeShift)*PSX4ALL_WIDTH;

	/* Main blitter */
  int incY = (h0==480) ? 2 : 1;
//y0 -= framesProgresiveInt ? incY : 0;
  for(int y1=y0+h1; y0<y1; y0+=incY)
	{
		/* Blit span */
		switch( w0 )
		{
		case 256:
				GPU_BlitWWDWW(	src_screen16,	dest_screen16, isRGB24);
			break;
		case 368:
				GPU_BlitWWWWWWWWS(	src_screen16,	dest_screen16, isRGB24, 4);
			break;
		case 320:
				GPU_BlitWW(	src_screen16,	dest_screen16, isRGB24);
			break;
		case 384:
				GPU_BlitWWWWWS(	src_screen16,	dest_screen16, isRGB24);
			break;
		case 512:
				GPU_BlitWWSWWSWS(	src_screen16, dest_screen16, isRGB24);
			break;
		case 640:
				GPU_BlitWS(	src_screen16, dest_screen16, isRGB24);
			break;
		}

		dest_screen16 += PSX4ALL_WIDTH;
		src_screen16  += h0==480 ? 2048 : 1024;
	}


#ifdef ZAURUS
	if(SDL_MUSTLOCK(gp2x_sdlwrapper_screen)) SDL_UnlockSurface(gp2x_sdlwrapper_screen);
#endif
}


/*----------------------------------------------------------------------
gpuVideoOutput
----------------------------------------------------------------------*/
int curDelay     = 0 ;
int curDelay_inc = gp2x_timer_raw_second()/1000;
///////////////////////////////////////////////////////////////////////////////
void  gpuVideoOutput()
{
  ++frameRateCounter;

  static u32 s_LastFrame=0;
  u32 curFlip  = gp2x_timer_raw();
  u32 curFrame = curFlip-s_LastFrame;
  if(Skip == 0)
  {
    //isNewDisplay = false;
    ++frameRealCounter;
    gpuDrawScreen();
  }
  curFlip  = gp2x_timer_raw()-curFlip;
  if( displayFrameInfo && (Skip == 0))
  {
    gp2x_printf(NULL, 0, 0,"VS:%04.4g fps:%04.4g real:%04.4g fs(%d/%d) (%3d,%2d,%2d)ms", float(vsincRate)/100.0f, float(frameRate)/100.0f, float(realRate)/100.0f, skipCount, skipRate, gp2x_timer_raw_to_ticks(curFrame),gp2x_timer_raw_to_ticks(curFlip),gp2x_timer_raw_to_ticks(curDelay));
  }

  if(Skip == 0)
    gp2x_video_flip();

  s_LastFrame = gp2x_timer_raw();

  if(skCount-->0)
  {
    Skip = 1;
  }
  else
  {
    Skip = 0;
  }

  if(--skRate<=0)
  {
		skCount = Min2(skipCount,skipRate?skipRate-1:0);
		skRate  = skipRate;
  }
}

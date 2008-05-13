
#include "gpu.h"

u32 frameRate=60;
double frameRateAvg=0.0;
s32 framesToSkip=0;
s32 framesSkipped=0;
u32 displayFrameInfo=1;
s32 frameRateCounter=0;
u32 framesTotal=0;
u32 autoFrameSkip = 0;
unsigned systicks=0;

int framesInterlace=0;
int framesProgresiveInt=1;

s32 GP0=0;
s32 GP1=0;
s32 FrameToRead=0;
s32 FrameToWrite=0;
s32 FrameWidth;
s32 FrameCount;
s32 FrameIndex;
PacketBuffer_t PacketBuffer;
s32 PacketCount;
s32 PacketIndex;
s32 isPAL = 0;
s32 TextureWindow[4];
s32 DrawingArea[4];
s32 DrawingOffset[2];
s32 DisplayArea[8];
s32 OtherEnv[16];

u8 PacketSize[256] = {
	0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	//		0-15
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	//		16-31
	3, 3, 3, 3, 6, 6, 6, 6, 4, 4, 4, 4, 8, 8, 8, 8,	//		32-47
	5, 5, 5, 5, 8, 8, 8, 8, 7, 7, 7, 7, 11, 11, 11, 11,	//	48-63
	2, 2, 2, 2, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3,	//		64-79
	3, 3, 3, 3, 0, 0, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4,	//		80-95
	2, 2, 2, 2, 3, 3, 3, 3, 1, 1, 1, 1, 2, 2, 2, 2,	//		96-111
	1, 1, 1, 1, 2, 2, 2, 2, 1, 1, 1, 1, 2, 2, 2, 2,	//		112-127
	3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	//		128-
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	//		144
	2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	//		160
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	//
	2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	//
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	//
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	//
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	//
};

u16 HorizontalResolution[8] = {
	256, 368, 320, 384, 512, 512, 640, 640
};

u16 VerticalResolution[4] = {
	240, 480, 256, 480
};



/* GPU_reset */
void GPU_reset(void)
{
	//ZeroMem(&gpuFreezeBegin,
	//		(long) &gpuFreezeEnd - (long) &gpuFreezeBegin);
	GP1 = 0x14802000;
	TextureWindow[2] = 255;
	TextureWindow[3] = 255;
	DrawingArea[2] = 256;
	DrawingArea[3] = 240;
	DisplayArea[2] = 256;
	DisplayArea[3] = 240;
	DisplayArea[6] = 256;
	DisplayArea[7] = 240;
	gpu_reset();
}


s32 GPU_init(void) {
	gpu_openVideo(0);
	GPU_reset();
	return 0;
}

s32 GPU_shutdown(void) {
	gpu_closeVideo();
	return 0;
}

s32 GPU_open(s32) {
	systicks=gp2x_timer_read();
	return 0; 
}

s32 GPU_close(void) {
	return 0;
}

s32 GPU_configure(void) {
	return 0;
}

s32 GPU_test(void) {
	return 0;
}

void GPU_about(void) {
}

void GPU_makeSnapshot(void) {
}

void GPU_keypressed(s32) {
}

void GPU_displayText(s8 *) {
}

s32 GPU_freeze(u32, GPUFREEZE *){
	return 0;
}

void GPU_getScreenPic(u8 *) {
}

/* GPUgetMode */
s32  GPU_getMode(void)
{
	/* Support Discontinued */
	return (0);
}

/* GPUsetMode */
void  GPU_setMode(u32 p1)
{
	/* Support Discontinued */
}


s32 skip_this_frame=0;
static s32 lastframerate = 0;
static s32 frameskipChange = 2;	// must be an even number so both frames in the buffer are cleared

static void FrameSkip(void)
{
	static u32 firstTime = 1;
	static s32 palhz = 50;
	static s32 ntschz = 60;

	static s32 pollcount = 0;
	s32 hz = (isPAL ? palhz : ntschz);
	s32 previousframeskip = framesToSkip;

	if( firstTime )
	{
		palhz = 50;
		ntschz = 60;
		framesToSkip = 0;
		frameskipChange = 2;
		lastframerate = 0;
		firstTime = 0;
		return;
	}

	if( frameRate * framesToSkip < hz )			// are we below 50/60 FPS?
	{
		pollcount++;
		if( pollcount > MAXSKIP )				// if we tried fixing the frameskip over MAXSKIP times
		{
			pollcount = 0;
			framesToSkip = 2;					// reset/reduce the frameskip
			if( isPAL ) palhz -= 5;				// reduce target framerate by half
			else ntschz -= 5;
			
			if( palhz < 10 ) palhz = 10;
			if( ntschz < 10 ) ntschz = 10;
		}
		else if( lastframerate > frameRate * framesToSkip )			// check if the previous change was effective
		{										// it wasn't effective
			frameskipChange = -frameskipChange; // reverse the direction of frameskipping
			framesToSkip += frameskipChange;	// add the framerate change
		}
		else
		{
			framesToSkip += frameskipChange;	// everything going good? keep going
			pollcount = 0;						// and reset pollcount
		}
	}
	else										// were above or equal to the correct framerate
	{
		if( palhz < 50 || ntschz < 60 )			// did we reduce the target rate before?
		{
			if( isPAL ) palhz += 5;				// then up the target framerate
			else		ntschz += 5;

			if( palhz > 50 ) palhz = 50;
			if( ntschz > 60 ) ntschz = 60;
		}
/*
		else
		{											// we already are at the target framerate
			framesToSkip -= 2;						// try going down a frame
		}
*/
		pollcount = 0;								// we made our target framerate, so resset pollcount
	}

	if( framesToSkip < 0 ) framesToSkip = 0;
	if( framesToSkip > MAXSKIP ) framesToSkip = MAXSKIP;

	lastframerate = frameRate * previousframeskip;
}





/* GPUupdateLace */
void  GPU_updateLace(void)
{
	unsigned newticks;
	unsigned diffticks = 0;	

	if(skip_this_frame)
	{
		--skip_this_frame;
		if(!skip_this_frame)
			gpu_updateLace = 0;
		return;
	}

	skip_this_frame = framesToSkip;

	psx4all_prof_start(PROFILER_GPU);

	frameRateCounter++;
	framesTotal++;
	newticks=gp2x_timer_read();

	if( (diffticks = (newticks-systicks)) >= 1000 ) // poll every second
	{
		double rate=(((double)frameRateCounter) / (((double)diffticks)/1000.0));
		frameRate = (u32)rate;
		systicks += 1000;
		frameRateCounter = 0;
		if (frameRateAvg!=0.0)
			frameRateAvg=(frameRateAvg+rate)/2.0;
		else
			frameRateAvg=rate;

		// when the framerate is updated, update the autoframeskip as well
		// CHUI: Bad method, its necesary update frameskip per frame.
		if( autoFrameSkip )
		{
			FrameSkip(); // auto frameskip for FramesToSkip setting	
		}

#ifndef GP2X
		if( displayFrameInfo )
			printf("FrameRate: %d, Avg=%.2f\n", frameRate, frameRateAvg);
#endif
	}
	if(gpu_updateLace)
	{
		gpu_videoOutput();
#ifdef GP2X
		if( displayFrameInfo )
			gp2x_printf(NULL, 0,0,"Frameskip: %d FrameRate: %d", framesToSkip, frameRate);
#endif
		gpu_updateLace = 0;
	}

#ifdef MAXFRAMES
	if (framesTotal>MAXFRAMES)
		exit(0);
#endif
	psx4all_prof_end(PROFILER_GPU);
}




/* GPUwriteStatus */
void GPU_writeStatus(u32 data)
{
        psx4all_prof_end(PROFILER_GPU);
	switch (data >> 24) {
		case 0x00:
			GPU_reset();
			break;
		case 0x01:
			GP1 &= ~0x08000000;
			PacketCount = FrameToRead = FrameToWrite = 0;
			break;
		case 0x02:
			GP1 &= ~0x08000000;
			PacketCount = FrameToRead = FrameToWrite = 0;
			break;
		case 0x03:
			GP1 = (GP1 & ~0x00800000) | ((data & 1) << 23);
			break;
		case 0x04:
			if (data == 0x04000000)
				PacketCount = 0;
			GP1 = (GP1 & ~0x60000000) | ((data & 3) << 29);
			break;
		case 0x05:
			DisplayArea[0] = data & 0x000003FF; //(short)(data & 0x3ff);
			DisplayArea[1] = (data & 0x000FFC00) >> 10; //(short)((data>>10)&0x1ff);
			break;
		case 0x06:
			DisplayArea[4] = data & 0x00000FFF; //(short)(data & 0x7ff);
			DisplayArea[6] = (data & 0x00FFF000) >> 12; //(short)((data>>12) & 0xfff);
			break;
		case 0x07:
			{		
				//int iT;
				DisplayArea[5] = data & 0x000003FF; //(short)(data & 0x3ff);
				DisplayArea[7] = (data & 0x000FFC00) >> 10; //(short)((data>>10) & 0x3ff);
				
				/*
				drawHeight = DisplayArea[7] - DisplayArea[5];

				if(isPAL) iT=48; else iT=28;

				if(DisplayArea[5]>=iT)
				{
					DisplayArea[5] = (short)(DisplayArea[5]-iT-4);
					if(DisplayArea[5]<0)
					{
						DisplayArea[5]=0;
					}
					drawHeight += DisplayArea[5];
				}
				else 
				{
					DisplayArea[5] = 0;
				}
				*/
			}
			break;
		case 0x08:
			OtherEnv[0x08] = (data >> 7) & 1;	//	reverse(?)
			GP1 =
				(GP1 & ~0x007F0000) | ((data & 0x3F) << 17) |
				((data & 0x40) << 10);

			{
				long oldResX = DisplayArea[2];
				long oldResY = DisplayArea[3];

				DisplayArea[2] = HorizontalResolution[(GP1 >> 16) & 7];
				DisplayArea[3] = VerticalResolution[(GP1 >> 19) & 3];
			}

			isPAL = (data & 0x08) ? 1 : 0; // if 1 - PAL mode, else NTSC
			break;
		case 0x09:
			OtherEnv[0x09] = data & 1;			//	gpub(?)
			break;
		case 0x10:
			switch (data & 0xffff) {
				case 0:
				case 1:
				case 3:
					GP0 = (DrawingArea[1] << 10) | DrawingArea[0];
					break;
				case 4:
					GP0 =
						((DrawingArea[3] - 1) << 10) | (DrawingArea[2] -
														1);
					break;
				case 6:
				case 5:
					GP0 = (DrawingOffset[1] << 11) | DrawingOffset[0];
					break;
				case 7:
					GP0 = 2;
					break;
				default:
					GP0 = 0;
			}
			break;
	}
        psx4all_prof_end(PROFILER_GPU);
}


/* GPUreadStatus */
u32  GPU_readStatus(void)
{
   static int iNumRead=0;                              // odd/even hack
   if((iNumRead++)==2)
    {
     iNumRead=0;
     GP1^=0x80000000;                        // interlaced bit toggle... we do it on every 3 read status... needed by some games (like ChronoCross) with old epsxe versions (1.5.2 and older)
    }

	return (GP1);
}


/* GPUwriteData */
void  GPU_writeData(u32 data)
{
	psx4all_prof_start(PROFILER_GPU);
	GP1 &= ~0x14000000;
	if (FrameToWrite > 0) {
          gpu_pvram[gpu_px]=(u16)data;
          if (++gpu_px>=gpu_x_end) {
               gpu_px = gpu_x_start;
               gpu_pvram += 1024;
               if (++gpu_py>=gpu_y_end) FrameToWrite=0;
          }
          if (FrameToWrite > 0) {
               gpu_pvram[gpu_px]=data>>16;
               if (++gpu_px>=gpu_x_end) {
                    gpu_px = gpu_x_start;
                    gpu_pvram += 1024;
                    if (++gpu_py>=gpu_y_end) FrameToWrite=0;
               }
          }
	} else {
		if (PacketCount) {
			PacketCount--;
			PacketBuffer.U4[PacketIndex++] = data;
		} else {
			PacketBuffer.U4[0] = data;
			PacketCount = PacketSize[data >> 24];
			PacketIndex = 1;
		}
		if (!PacketCount)
		{
			gpu_sendPacket();
		}
	}
	GP1 |= 0x14000000;
	psx4all_prof_end(PROFILER_GPU);
}

void  GPU_writeDataMem(u32 * dmaAddress, s32 dmaCount)
{
	u32 temp, temp2;

	psx4all_prof_start(PROFILER_GPU);

	GP1 &= ~0x14000000;

	while (dmaCount) {
		if (FrameToWrite > 0) {
			while (dmaCount--) 
			{
				u32 data = *dmaAddress++;

				if (gpu_px<1024 && gpu_py<512)
					gpu_pvram[gpu_px] = data;
				if (++gpu_px>=gpu_x_end) 
				{
					gpu_px = gpu_x_start;
					gpu_pvram += 1024;
					if (++gpu_py>=gpu_y_end) 
					{
						FrameToWrite = 0;
						GP1 &= ~0x08000000;
						break;
					}
				}
				if (gpu_px<1024 && gpu_py<512)
					gpu_pvram[gpu_px] = data>>16;
				if (++gpu_px>=gpu_x_end) 
				{
					gpu_px = gpu_x_start;
					gpu_pvram += 1024;
					if (++gpu_py>=gpu_y_end) 
					{
						FrameToWrite = 0;
						GP1 &= ~0x08000000;
						break;
					}
				}
			}
		}
		else
		{
			temp = *dmaAddress++;
			dmaCount--;
			if (PacketCount) {
				PacketBuffer.U4[PacketIndex++] = temp;
				PacketCount--;
			} else {
				PacketBuffer.U4[0] = temp;
				PacketCount = PacketSize[temp >> 24];
				PacketIndex = 1;
			}
			if (!PacketCount)
			{
				gpu_sendPacket();
			}
		}
	}
	GP1 = (GP1 | 0x14000000) & ~0x60000000;
	psx4all_prof_end(PROFILER_GPU);
}

u32  GPU_readData(void)
{
        psx4all_prof_start(PROFILER_GPU);
	GP1 &= ~0x14000000;
	if (FrameToRead)
	{
		GP0 = gpu_pvram[gpu_px];
		if (++gpu_px>=gpu_x_end) {
		   gpu_px = gpu_x_start;
		   gpu_pvram += 1024;
		   if (++gpu_py>=gpu_y_end) FrameToRead=0;
		}
		GP0 |= gpu_pvram[gpu_px]<<16;
		if (++gpu_px>=gpu_x_end) {
		   gpu_px = gpu_x_start;
		   gpu_pvram +=1024;
		   if (++gpu_py>=gpu_y_end) FrameToRead=0;
		}

		if( FrameToRead == 0 ) GP1 &= ~0x08000000;
	}
	GP1 |= 0x14000000;
        psx4all_prof_end(PROFILER_GPU);
	return (GP0);
}

void  GPU_readDataMem(u32 * dmaAddress, s32 dmaCount)
{
	if( FrameToRead == 0 ) return;

	psx4all_prof_start(PROFILER_GPU);
	GP1 &= ~0x14000000;

	do 
	{
		// lower 16 bit
		u32 data = (unsigned long)gpu_pvram[gpu_px];

		if (++gpu_px>=gpu_x_end) 
		{
			gpu_px = gpu_x_start;
			gpu_pvram += 1024;
		}

		// higher 16 bit (always, even if it's an odd width)
		data |= (unsigned long)(gpu_pvram[gpu_px])<<16;
    
		*dmaAddress++ = data;

		if (++gpu_px>=gpu_x_end) 
		{
			gpu_px = gpu_x_start;
			gpu_pvram += 1024;
			if (++gpu_py>=gpu_y_end) 
			{
				FrameToRead = 0;
				GP1 &= ~0x08000000;
				break;
			}
		}
	} while (--dmaCount);

	GP1 = (GP1 | 0x14000000) & ~0x60000000;
	psx4all_prof_end(PROFILER_GPU);
}

s32 GPU_dmaChain(u32 * baseAddr, u32 dmaVAddr)
{
	u32 temp, data, *address, count, offset;
	psx4all_prof_start(PROFILER_GPU);
	GP1 &= ~0x14000000;
	dmaVAddr &= 0x00FFFFFF;
	while (dmaVAddr != 0xFFFFFF) {
		address = (baseAddr + (dmaVAddr >> 2));
		data = *address++;
		count = (data >> 24);
		offset = data & 0x00FFFFFF;
		if (dmaVAddr != offset)
			dmaVAddr = offset;
		else
			dmaVAddr = 0xFFFFFF;
		while (count) {
			data = *address++;
			count--;
			//temp = PacketCount;
			if (PacketCount) {
				PacketCount--;
				PacketBuffer.U4[PacketIndex++] = data;
			} else {
				PacketBuffer.U4[0] = data;
				PacketCount = PacketSize[data >> 24];
				PacketIndex = 1;
			}
			//PacketCount = temp;
			if (!PacketCount)
			{
				gpu_sendPacket();
			}
		}
	}
	GP1 = (GP1 | 0x14000000) & ~0x60000000;
	psx4all_prof_end(PROFILER_GPU);
	return (0);
}


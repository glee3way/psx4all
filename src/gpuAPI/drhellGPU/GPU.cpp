/***********************************************************************
*
*	Dr.Hell's WinGDI GPU Plugin
*	Version 0.8
*	Copyright (C)Dr.Hell, 2002-2004
*
***********************************************************************/

#include "minimal.h"
#include "inner_Blit.h"
#include "../gpuAPI.h"

/*----------------------------------------------------------------------
Type
----------------------------------------------------------------------*/

typedef u8 uchar;
typedef s8 schar;
typedef u16 ushort;
typedef u32 ulong;

/*----------------------------------------------------------------------
Constant
----------------------------------------------------------------------*/

#define	GPU_LIBTYPE			2
#define	GPU_VERSION			1
#define	GPU_REVISION		0
#define	GPU_BUILD			8

#define	FRAME_BUFFER_SIZE	(1024*512*2)
#define	FRAME_WIDTH			1024
#define	FRAME_HEIGHT		512

/*----------------------------------------------------------------------
Macro
----------------------------------------------------------------------*/

#define	FRAME_OFFSET(x,y)	(((y)<<10)+(x))

template<typename T>
INLINE  T Min2 (const T _a, const T _b)             { return (_a<_b)?_a:_b; }

/*----------------------------------------------------------------------
Global
----------------------------------------------------------------------*/

char LIBNAME[] = "Dr.Hell's GDI Driver";
long gpuFreezeBegin;
u32 GP0;
u32 GP1;
long FrameToRead;
long FrameToWrite;
long FrameWidth;
long FrameCount;
long FrameIndex;
union {
	schar S1[64];
	short S2[32];
	long S4[16];
	uchar U1[64];
	ushort U2[32];
	ulong U4[16];
} PacketBuffer;
long PacketCount;
long PacketIndex;
long TextureWindow[4];
long DrawingArea[4];
long DrawingOffset[2];
long DisplayArea[8];
uchar Masking;
ushort PixelMSB;
long OtherEnv[16];
long gpuFreezeEnd;
long Skip;
long SkipReset;
long DrawingCount[4];
long DisplayCount[4];
ushort *FrameBuffer;

int skCount = 0;
int skRate  = 0;
int  skipCount = 0;
int  skipRate = 1;
bool enableFrameLimit = false;
bool enableAbbeyHack = false;
bool displayFrameInfo = true;
bool displayGpuStats = false;
bool displayVideoMemory = false;
bool primitiveDebugMode = false;
bool activeNullGPU = false;
bool activeGPULog = false;
u32 systime;
s32 vsincRateCounter  = 0;
s32 frameRateCounter  = 0;
s32 frameRealCounter  = 0;
s32 vsincRate         = 60;
s32 frameRate         = 60;
s32 realRate          = 60;
u32 framesTotal       = 0;
int linesInterlace      = 0;  //  0, 1, 3, 7
int linesInterlace_user = linesInterlace;
long justdrawCount		= 1;

/*----------------------------------------------------------------------
Table
----------------------------------------------------------------------*/

uchar PacketSize[256] = {
	0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	//
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	//
	3, 3, 3, 3, 6, 6, 6, 6, 4, 4, 4, 4, 8, 8, 8, 8,	//
	5, 5, 5, 5, 8, 8, 8, 8, 7, 7, 7, 7, 11, 11, 11, 11,	//
	2, 2, 2, 2, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3,	//
	3, 3, 3, 3, 0, 0, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4,	//
	2, 2, 2, 2, 3, 3, 3, 3, 1, 1, 1, 1, 2, 2, 2, 2,	//
	1, 1, 1, 1, 2, 2, 2, 2, 1, 1, 1, 1, 2, 2, 2, 2,	//
	3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	//
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	//
	2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	//
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	//
	2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	//
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	//
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	//
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	//
};

ushort HorizontalResolution[8] = {
	256, 368, 320, 384, 512, 512, 640, 640
};

ushort VerticalResolution[4] = {
	240, 480, 256, 480
};

/*----------------------------------------------------------------------
Device Depended
----------------------------------------------------------------------*/

#include "GPU_DEV.cpp"

/*

CALLBACK
GetMem(p1)
FreeMem(p1)
CopyMem(p1,p2,p3)
ZeroMem(p1,p2)

long gpuGetTime(void);
void gpuLoadConfig(void);
void gpuSaveConfig(void);
void gpuOpenVideo(long);
void gpuCloseVideo(void);
void gpuVideoOutput(void);
void gpuOpenAbout(void);
void gpuOpenConfigure(void);

*/

/*----------------------------------------------------------------------
Drawing
----------------------------------------------------------------------*/

#include "GPU_DRAW.cpp"

/*

void gpuSendPacket(void);

*/

/*----------------------------------------------------------------------
Other
----------------------------------------------------------------------*/

/* gpuReset */
void gpuReset(void)
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
	justdrawCount	= 1;
}

/* ReadFrameBuffer */
void ReadFrameBuffer(ushort * lpBuffer, long nCount)
{
	long temp, nCount2, nCount3;
	ushort *lpBuffer2;
	temp = FrameToRead;
	if (temp <= nCount) {
		nCount = temp;
		GP1 &= ~0x08000000;
	}
	FrameToRead = temp - nCount;
	lpBuffer2 = &FrameBuffer[FrameIndex];
	temp = FrameCount;
	while (nCount) {
		if (nCount >= temp) {
			nCount2 = temp;
			nCount3 = temp + 1024 - FrameWidth;
			temp = FrameWidth;
		} else {
			nCount3 = nCount2 = nCount;
			temp -= nCount;
		}
		CopyMem(lpBuffer, lpBuffer2, nCount2 + nCount2);
		lpBuffer += nCount2;
		lpBuffer2 += nCount3;
		nCount -= nCount2;
	}
	FrameCount = temp;
	FrameIndex = lpBuffer2 - FrameBuffer;
}

/* WriteFrameBuffer */
void WriteFrameBuffer(ushort * lpBuffer, long nCount)
{
	long temp, nCount2, nCount3;
	ushort *lpBuffer2;
	temp = FrameToWrite;
	if (temp <= nCount) {
		nCount = temp;
		GP1 &= ~0x08000000;
	}
	FrameToWrite = temp - nCount;
	lpBuffer2 = &FrameBuffer[FrameIndex];
	temp = FrameCount;
	while (nCount) {
		if (nCount >= temp) {
			nCount2 = temp;
			nCount3 = temp + 1024 - FrameWidth;
			temp = FrameWidth;
		} else {
			nCount3 = nCount2 = nCount;
			temp -= nCount;
		}
		CopyMem(lpBuffer2, lpBuffer, nCount2 + nCount2);
		lpBuffer += nCount2;
		lpBuffer2 += nCount3;
		nCount -= nCount2;
	}
	FrameCount = temp;
	FrameIndex = lpBuffer2 - FrameBuffer;
}

/* GPUinit */
bool DRHELLGPU_init(void)
{
	FrameBuffer = (ushort*) GetMem(FRAME_BUFFER_SIZE);
	gpuReset();

	return (0);
}

/* GPUshutdown */
void DRHELLGPU_done(void)
{
	FreeMem(FrameBuffer);
}

/* GPUwriteData */
void DRHELLGPU_writeData(ulong data)
{
	GP1 &= ~0x14000000;
	if (FrameToWrite) {
		WriteFrameBuffer((ushort *) &data, 2);
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
			gpuSendPacket();
	}
	GP1 |= 0x14000000;
}

/* GPUwriteStatus */
void DRHELLGPU_writeStatus(ulong data)
{
	switch (data >> 24) {
		case 0x00:
			gpuReset();
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
			DisplayArea[0] = data & 0x000003FF;
			DisplayArea[1] = (data & 0x000FFC00) >> 10;
			break;
		case 0x06:
			DisplayArea[4] = data & 0x00000FFF;
			DisplayArea[6] = (data & 0x00FFF000) >> 12;
			break;
		case 0x07:
			DisplayArea[5] = data & 0x000003FF;
			DisplayArea[7] = (data & 0x000FFC00) >> 10;
			break;
		case 0x08:
			OtherEnv[0x08] = (data >> 7) & 1;	//	reverse(?)
			GP1 =
				(GP1 & ~0x007F0000) | ((data & 0x3F) << 17) |
				((data & 0x40) << 10);
			DisplayArea[2] = HorizontalResolution[(GP1 >> 16) & 7];
			DisplayArea[3] = VerticalResolution[(GP1 >> 19) & 3];
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
}

/* GPUreadData */
ulong DRHELLGPU_readData(void)
{
	GP1 &= ~0x14000000;
	if (FrameToRead)
		ReadFrameBuffer((ushort *) &GP0, 2);
	GP1 |= 0x14000000;
	return (GP0);
}

/* GPUreadStatus */
ulong DRHELLGPU_readStatus(void)
{
	return (GP1);
}

/* GPUdmaChain */
void DRHELLGPU_dmaChain(ulong * baseAddr, ulong dmaVAddr)
{
	ulong temp, data, pkts, *address, count, offset;
	GP1 &= ~0x14000000;
	dmaVAddr &= 0x00FFFFFF;
	pkts = 0;
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
			temp = PacketCount;
			if (temp) {
				temp--;
				PacketBuffer.U4[PacketIndex++] = data;
			} else {
				PacketBuffer.U4[0] = data;
				temp = PacketSize[data >> 24];
				PacketIndex = 1;
			}
			PacketCount = temp;
			if (!temp)
				gpuSendPacket();
		}
		pkts++;
		if (pkts > 0x100000) return;
	}
	GP1 = (GP1 | 0x14000000) & ~0x60000000;
}

/* GPUupdateLace */
void DRHELLGPU_vSinc(void)
{
	u32    newtime;
	u32    diffintime = 0;
	long temp;

	GP1 ^= 0x80000000;

	++vsincRateCounter;
#if 0
	//DisplayCount[1] += DisplayCount[0];
	DrawingCount[2] &= 0xFF;
	DrawingCount[3] &= 0xFF;
	Skip = DrawingCount[2];
	if (DrawingCount[2]) {
		DrawingCount[2]--;
		Skip = DrawingCount[2];
		return;
	}
	if (DrawingCount[0]) {
		if (DisplayArea[3] > 240) {
			if (DrawingCount[3]) {
				DrawingCount[3]--;
				return;
			}
			DrawingCount[3] = DrawingCount[1];
		}
	}
	DrawingCount[2] = DrawingCount[0];
#endif

	gpuVideoOutput();

#if 0
	temp = gpuGetTime() * 1000;
	if ((ulong) (temp - DisplayCount[2]) > (ulong) 10000000) {
		DisplayCount[1] = 0;
		DisplayCount[2] = temp;
	} else {
		while ((temp - DisplayCount[2]) < DisplayCount[1]) {
			temp = gpuGetTime() * 1000;
		}
		if (DisplayCount[1] > 5000000) {
			DisplayCount[1] = 0;
			DisplayCount[2] = temp;
		}
	}
#endif

	newtime = gp2x_timer_raw();
	if( (diffintime=newtime-systime) >= (gp2x_timer_raw_second()) ) // poll 2 times per second
	{
	    vsincRate = (u64)(vsincRateCounter*100)*gp2x_timer_raw_second() / diffintime;
		frameRate = (u64)(frameRateCounter*100)*gp2x_timer_raw_second() / diffintime;
		realRate  = (u64)(frameRealCounter*100)*gp2x_timer_raw_second() / diffintime;
		vsincRateCounter = 0;
		frameRateCounter = 0;
		frameRealCounter = 0;
		systime = gp2x_timer_raw();
	}
}

/* GPUwriteDataMem */
void DRHELLGPU_writeDataMem(ulong * dmaAddress, long dmaCount)
{
	ulong temp, temp2;
	GP1 &= ~0x14000000;
	while (dmaCount) {
		temp = FrameToWrite;
		if (temp) {
			temp2 = dmaCount << 1;
			if (temp2 > temp)
				temp2 = temp;
			WriteFrameBuffer((ushort *) dmaAddress, temp2);
			temp2 = (temp2 + 1) >> 1;
			dmaAddress += temp2;
			dmaCount -= temp2;
		} else {
			temp = *dmaAddress++;
			dmaCount--;
			temp2 = PacketCount;
			if (temp2) {
				PacketBuffer.U4[PacketIndex++] = temp;
				temp2--;
			} else {
				PacketBuffer.U4[0] = temp;
				temp2 = PacketSize[temp >> 24];
				PacketIndex = 0;
			}
			PacketCount = temp2;
			if (!temp2)
				gpuSendPacket();
		}
	}
	GP1 = (GP1 | 0x14000000) & ~0x60000000;
}

/* GPUreadDataMem */
void DRHELLGPU_readDataMem(ulong * dmaAddress, long dmaCount)
{
	ulong temp, temp2;
	GP1 &= ~0x14000000;
	temp = FrameToRead;
	if (temp) {
		temp2 = dmaCount << 1;
		if (temp2 > temp)
			temp2 = temp;
		ReadFrameBuffer((ushort *) dmaAddress, temp2);
		temp2 = (temp2 + 1) >> 1;
		dmaAddress += temp2;
		dmaCount -= temp2;
	}
	for (; dmaCount; dmaCount--) {
		*dmaAddress++ = GP0;
	}
	GP1 = (GP1 | 0x14000000) & ~0x60000000;
}

/* GPUfreeze */
bool  DRHELLGPU_freeze(bool bWrite, GPUFreeze* p2)
{
//u32 temp;
  /*
	if (p1 == 2) {
		temp = *(u32 *) p2;
		if ((temp < 0) || (temp > 8))
			return (0);
		return (1);
	}
  */
	if (!p2)
		return (0);

  if (p2->Version != 1)
		return (0);

  if (bWrite)
  {
		p2->GP1 = GP1;
		//CopyMem(p2->Control, &gpuFreezeBegin,
		//		(long) &gpuFreezeEnd - (long) &gpuFreezeBegin);
		CopyMem(p2->FrameBuffer, (ushort*)FrameBuffer, FRAME_BUFFER_SIZE);
		return (1);
	}
  else
	{
		GP1 = p2->GP1;
		//CopyMem(&gpuFreezeBegin, p2->Control,
		//		(long) &gpuFreezeEnd - (long) &gpuFreezeBegin);
		CopyMem((ushort*)FrameBuffer, p2->FrameBuffer, FRAME_BUFFER_SIZE);
		return (1);
	}
	return (0);
}

///////////////////////////////////////////////////////////////////////////////
//  GPU registering function
bool  register_DRHELLGPU()
{
  //  GPU inicialization/deinicialization functions
  GPU_init    = DRHELLGPU_init;
  GPU_done    = DRHELLGPU_done;
  GPU_freeze  = DRHELLGPU_freeze;

  //  GPU Vsinc Notification
  GPU_vSinc = DRHELLGPU_vSinc;

  //  GPU DMA comunication
  GPU_dmaChain      = DRHELLGPU_dmaChain;
  GPU_writeDataMem  = DRHELLGPU_writeDataMem;
  GPU_readDataMem   = DRHELLGPU_readDataMem;

  //  GPU Memory comunication
  GPU_writeData   = DRHELLGPU_writeData;
  GPU_writeStatus = DRHELLGPU_writeStatus;
  GPU_readData    = DRHELLGPU_readData;

  return true;
}

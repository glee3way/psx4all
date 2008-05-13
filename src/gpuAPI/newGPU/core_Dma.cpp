///////////////////////////////////////////////////////////////////////////////
//  GPU DMA comunication
#include "newGPU.h"
#include "raster.h"

#include "minimal.h"

///////////////////////////////////////////////////////////////////////////////
u8 PacketSize[256] =
{
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

///////////////////////////////////////////////////////////////////////////////
u32 HorizontalResolution[8] =
{
	256, 368, 320, 384, 512, 512, 640, 640
};

///////////////////////////////////////////////////////////////////////////////
u32 VerticalResolution[4] =
{
	240, 480, 256, 480
};


///////////////////////////////////////////////////////////////////////////////
#ifdef  ENABLE_GPU_COMMAND_TABLE
void INLINE gpuSendPacket()
{
//PROFILE_FENCE(dmaPacketTime[PacketBuffer.U4[0]>>24],dmaPacketCount[PacketBuffer.U4[0]>>24]);
  gpuSendPackedDriver[PacketBuffer.U4[0]>>24]();
}
#else
  extern void gpuSendPacket();
#endif

///////////////////////////////////////////////////////////////////////////////
void INLINE gpuCheckPacket(u32 uData)
{
	if (PacketCount)
  {
		PacketBuffer.U4[PacketIndex++] = uData;
		--PacketCount;
	}
  else
  {
		PacketBuffer.U4[0] = uData;
		PacketCount = PacketSize[uData >> 24];
		PacketIndex = 1;
	}
	if (!PacketCount)
		gpuSendPacket();
}

///////////////////////////////////////////////////////////////////////////////
static u32* s_LastBase = 0;
static u32  s_LastDma  = 0;

static u32* s_recordBase[32];
static u32  s_recordDma [32];
static u32  s_recordCount =0;

///////////////////////////////////////////////////////////////////////////////
void  gpuRecordDmaChain(u32* baseAddr, u32 dmaVAddr)
{
  s_recordBase[s_recordCount] = baseAddr;
  s_recordDma [s_recordCount] = dmaVAddr;
  ++s_recordCount;
}

///////////////////////////////////////////////////////////////////////////////
void  gpuRecordReplay()
{
  s_LastBase = 0;
  s_LastDma  = 0;
  for(u32 r=0;r<s_recordCount;++r)
    NEWGPU_dmaChain(s_recordBase[r], s_recordDma[r]);
}

///////////////////////////////////////////////////////////////////////////////
void  gpuRecordReset()
{
  s_recordCount = 0;
}

#if 1
///////////////////////////////////////////////////////////////////////////////
void  NEWGPU_dmaChain(u32* baseAddr, u32 dmaVAddr)
{
  PROFILE_FENCE(dmaChainTime,dmaChainCount);

  u32 data, *address, count, offset;
	GPU_GP1 &= ~0x14000000;
	dmaVAddr &= 0x00FFFFFF;
	while (dmaVAddr != 0xFFFFFF)
  {
		address = (baseAddr + (dmaVAddr >> 2));
		data = *address++;
		count = (data >> 24);
		offset = data & 0x00FFFFFF;
		if (dmaVAddr != offset)
			dmaVAddr = offset;
		else
			dmaVAddr = 0xFFFFFF;

    while (count--)
      gpuCheckPacket(*address++);
	}
  GPU_GP1 = (GPU_GP1 | 0x14000000) & ~0x60000000;
}
#else
///////////////////////////////////////////////////////////////////////////////
void  NEWGPU_dmaChain(u32* baseAddr, u32 dmaVAddr)
{
	PROFILE_FENCE(dmaChainTime,dmaChainCount);

	u32 data, *address, count, i;
	u32 baseVAddr = (u32)baseAddr;
	u32 stopnext = 0;
	
	GPU_GP1 &= ~0x14000000;
	PacketCount = 0;
	
	address = (u32*)(baseVAddr + (dmaVAddr & 0x001FFFFF));
	data = *address++;
	if( data & 0x00FFFFFF == 0x00FFFFFF ) stopnext = 1;

	while( 1 )
	{
		count = (data >> 24);
		if( (dmaVAddr & 0x00FFFFFF) == (data & 0x00FFFFFF) ) break;
		dmaVAddr = data;

		while(count > 0)
		{
			u32 packetsize;
			PacketBuffer.U4[0] = *address++;
			packetsize = PacketSize[PacketBuffer.U4[0] >> 24];
			for(i = 1; i <= packetsize; i++)
			{
				PacketBuffer.U4[i] = *address++;
			}
			gpuSendPacket();
			count -= packetsize + 1;
		}
		if( stopnext == 1 ) break;

		address = (u32*)(baseVAddr + (dmaVAddr & 0x001FFFFF));
		data = *address++;
		if( data & 0x00FFFFFF == 0x00FFFFFF ) stopnext = 1;
	}
	GPU_GP1 = (GPU_GP1 | 0x14000000) & ~0x60000000;
}
#endif

///////////////////////////////////////////////////////////////////////////////
void  NEWGPU_writeDataMem(u32* dmaAddress, s32 dmaCount)
{
  PROFILE_FENCE(dmaMemTime,dmaMemCount);

	u32 data;
	GPU_GP1 &= ~0x14000000;

	while (dmaCount) 
	{
		if (FrameToWrite > 0) 
		{
			/*AddDirtyArea(x_start,y_start,x_end,y_end);
			if(isSkip[1]) 
			{
				dmaCount = 0;
				FrameToWrite = 0;
				GPU_GP1 &= ~0x08000000;
			}
			else */while (dmaCount--) 
			{
				data = *dmaAddress++;
				if (px<1024 && py<512)
					pvram[px] = data;
				if (++px>=x_end) 
				{
					px = x_start;
					pvram += 1024;
					if (++py>=y_end) 
					{
						FrameToWrite = 0;
						GPU_GP1 &= ~0x08000000;
						break;
					}
				}
				if (px<1024 && py<512)
					pvram[px] = data>>16;
				if (++px>=x_end) 
				{
					px = x_start;
					pvram += 1024;
					if (++py>=y_end) 
					{
						FrameToWrite = 0;
						GPU_GP1 &= ~0x08000000;
						break;
					}
				}
			}
		}
		else
		{
			data = *dmaAddress++;
			dmaCount--;
			gpuCheckPacket(data);
		}
	}

  GPU_GP1 = (GPU_GP1 | 0x14000000) & ~0x60000000;
}


///////////////////////////////////////////////////////////////////////////////
void  NEWGPU_writeData(u32 data)
{
  PROFILE_FENCE(dmaMemTime,dmaMemCount);

  GPU_GP1 &= ~0x14000000;

	if (FrameToWrite > 0)
  {
		/*AddDirtyArea(x_start,y_start,x_end,y_end);
		if(isSkip[1]) 
		{
			FrameToWrite = 0;
			GPU_GP1 &= ~0x08000000;
		}
		else */
		{
			pvram[px]=(u16)data;
			if (++px>=x_end)
			{
			   px = x_start;
			   pvram += 1024;
						 if (++py>=y_end) 
			   {
							 FrameToWrite = 0;
							 GPU_GP1 &= ~0x08000000;
			   }
			}
			if (FrameToWrite > 0)
			{
			   pvram[px]=data>>16;
			   if (++px>=x_end)
			   {
					px = x_start;
					pvram += 1024;
							  if (++py>=y_end) 
					{
								  FrameToWrite = 0;
								  GPU_GP1 &= ~0x08000000;
					}
			   }
			}
		}
	}
  else
  {
    gpuCheckPacket(data);
	}
	GPU_GP1 |= 0x14000000;

}


///////////////////////////////////////////////////////////////////////////////
void  NEWGPU_readDataMem(u32* dmaAddress, s32 dmaCount)
{
  PROFILE_FENCE(dmaMemTime,dmaMemCount);

  if( FrameToRead == 0 )
    return;

	GPU_GP1 &= ~0x14000000;
	do 
	{
		// lower 16 bit
		u32 data = (unsigned long)pvram[px];

		if (++px>=x_end) 
		{
			px = x_start;
			pvram += 1024;
		}

		// higher 16 bit (always, even if it's an odd width)
		data |= (unsigned long)(pvram[px])<<16;
    
		*dmaAddress++ = data;

		if (++px>=x_end) 
		{
			px = x_start;
			pvram += 1024;
			if (++py>=y_end) 
			{
				FrameToRead = 0;
				GPU_GP1 &= ~0x08000000;
				break;
			}
		}
	} while (--dmaCount);

	GPU_GP1 = (GPU_GP1 | 0x14000000) & ~0x60000000;
}



///////////////////////////////////////////////////////////////////////////////
u32  NEWGPU_readData(void)
{
  PROFILE_FENCE(dmaMemTime,dmaMemCount);

  GPU_GP1 &= ~0x14000000;
	if (FrameToRead)
	{
		GP0 = pvram[px];
		if (++px>=x_end)
    {
		   px = x_start;
		   pvram += 1024;
 	     if (++py>=y_end) 
  		 {
			    FrameToRead = 0;
				  GPU_GP1 &= ~0x08000000;
       }
		}
		GP0 |= pvram[px]<<16;
		if (++px>=x_end)
    {
		   px = x_start;
		   pvram +=1024;
 	     if (++py>=y_end) 
  		 {
			    FrameToRead = 0;
				  GPU_GP1 &= ~0x08000000;
       }
		}

	}
	GPU_GP1 |= 0x14000000;

  return (GP0);
}

extern void  gpuVideoOutput(void);

///////////////////////////////////////////////////////////////////////////////
void  NEWGPU_writeStatus(u32 data)
{
  PROFILE_FENCE(dmaMemTime,dmaMemCount);

  switch (data >> 24) {
		case 0x00:
			gpuReset();
      //ResetDirtyArea();
      //ResetCheckArea();
      break;
		case 0x01:
			GPU_GP1 &= ~0x08000000;
			PacketCount = FrameToRead = FrameToWrite = 0;
			break;
		case 0x02:
			GPU_GP1 &= ~0x08000000;
			PacketCount = FrameToRead = FrameToWrite = 0;
			break;
		case 0x03:
			GPU_GP1 = (GPU_GP1 & ~0x00800000) | ((data & 1) << 23);
			break;
		case 0x04:
			if (data == 0x04000000)
				PacketCount = 0;
			GPU_GP1 = (GPU_GP1 & ~0x60000000) | ((data & 3) << 29);
			break;
		case 0x05:
			DisplayArea[0] = (data & 0x000003FF); //(short)(data & 0x3ff);
 			DisplayArea[1] = ((data & 0x0007FC00)>>10); //(data & 0x000FFC00) >> 10; //(short)((data>>10)&0x1ff);
			break;
		case 0x06:
			DisplayArea[4] = data & 0x00000FFF; //(short)(data & 0x7ff);
			DisplayArea[6] = (data & 0x00FFF000) >> 12; //(short)((data>>12) & 0xfff);
			//gpuSkipUpdate();
			break;
		case 0x07:
			DisplayArea[5] = data & 0x000003FF; //(short)(data & 0x3ff);
			DisplayArea[7] = (data & 0x000FFC00) >> 10; //(short)((data>>10) & 0x3ff);
			break;
		case 0x08:
			OtherEnv[0x08] = (data >> 7) & 1;	//	reverse(?)
			GPU_GP1 =
				(GPU_GP1 & ~0x007F0000) | ((data & 0x3F) << 17) |
				((data & 0x40) << 10);

			DisplayArea[2] = HorizontalResolution[(GPU_GP1 >> 16) & 7];
			DisplayArea[3] = VerticalResolution[(GPU_GP1 >> 19) & 3];
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
					GP0 = ((DrawingArea[3]-1) << 10) | (DrawingArea[2]-1);
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


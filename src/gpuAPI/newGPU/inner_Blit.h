#ifndef _INNER_BLIT_H_
#define _INNER_BLIT_H_
///////////////////////////////////////////////////////////////////////////////
//  GPU Blitting code with rescale and interlace support.


/*
  
  struct  HeightTable
  {
    int   Height;
    int   IncY;
    int   SrcLineStep; 
    int   ClipHeight;
  };

  struct  WithTable
  {
    int With;
    int bpp;
    int SrcOffsetByte;
    Pfn BlitterFn;
  };

*/


extern "C" void convert_frame_buffer_bgr555_to_rgb555(u32 source);

#define write2xrot16(n) dst16[pixelstart+(480*n-1)] = dst16[pixelstart+(480*n)]

#define writerot16(n) dst16[pixelstart+(480*n)]

#define bgr15torgb16(n) ((src16[n]&(0x7C00))>>10) | ((src16[n]&(0x03E0))<<1) | ((src16[n]&(0x1f))<<11)

#define rgb24torgb16(n, r, g, b) gp2x_video_RGB_color16( src8[r], src8[g], src8[b] )

#define LOOP_START_16_BASE(count)				\
		u16* src16 = (u16*)src + (i<<10);		\
		uCount = count;											\

#define LOOP_START_16(count)						\
	for(int i = 0; i < h1; i++)						\
	{																			\
		u32 pixelstart = 479 - i;						\
		LOOP_START_16_BASE(count)						\


#define LOOP_START_16_240(count)				\
	for(int i = 0; i < h1; i++)						\
	{																			\
		u32 pixelstart = 479 - (i<<1);			\
		LOOP_START_16_BASE(count)						\

#define LOOP_END_16																						\
	}																														\

/*	
	convert_frame_buffer_bgr555_to_rgb555((u32)gp2x_screen16);	\
	memcpy(gp2xscreen, gp2x_screen16, 480*640*2);								\

*/

#define LOOP_START_8_BASE(count)				\
		u8* src8 = (u8*)src + (i<<11);			\
		uCount = count;											\

#define LOOP_START_8(count)							\
	for(int i = 0; i < h1; i++)						\
	{																			\
		u32 pixelstart = 479 - i;						\
		LOOP_START_8_BASE(count)						\
		
#define LOOP_START_8_240(count)					\
	for(int i = 0; i < h1; i++)						\
	{																			\
		u32 pixelstart = 479 - (i<<1);			\
		LOOP_START_8_BASE(count)						\

#define LOOP_END_8											\
	}																			\

void GPU_Blit_512_480_ROT90CW_2X(void* src, u16* dst16, u32 h1)
{ 
  u32 uCount;
  if((GPU_GP1 & 0x00200000) == 0)
  {
		LOOP_START_16(32)
    do{
    	writerot16(0) = bgr15torgb16(0);
    	writerot16(1) = bgr15torgb16(1);
 			writerot16(3) = writerot16(2) = bgr15torgb16(2);
 			writerot16(4) = bgr15torgb16(3);
 			
    	writerot16(5) = bgr15torgb16(4);
    	writerot16(6) = bgr15torgb16(5);
 			writerot16(8) = writerot16(7) = bgr15torgb16(6);
 			writerot16(9) = bgr15torgb16(7);
 			
 			writerot16(10) = bgr15torgb16(8);
    	writerot16(11) = bgr15torgb16(9);
 			writerot16(13) = writerot16(12) = bgr15torgb16(10);
 			writerot16(14) = bgr15torgb16(11);
 			
 			writerot16(15) = bgr15torgb16(12);
    	writerot16(16) = bgr15torgb16(13);
 			writerot16(18) = writerot16(17) = bgr15torgb16(14);
 			writerot16(19) = bgr15torgb16(15);
 			
      pixelstart += 480*20;
      src16 += 16;
    }while(--uCount);
    LOOP_END_16
  }
  else
  {
    LOOP_START_8(32)
    do{
    	writerot16(0) = rgb24torgb16(0, 0,  1,  2 );
    	writerot16(1) = rgb24torgb16(1, 3,  4,  5 );
 			writerot16(3) = writerot16(2) = rgb24torgb16(2, 6,  7,  8 );
 			writerot16(4) = rgb24torgb16(3, 9, 10, 11 );
 			
    	writerot16(5) = rgb24torgb16(4, 12, 13, 14 );
    	writerot16(6) = rgb24torgb16(5, 15, 16, 17 );
 			writerot16(8) = writerot16(7) = rgb24torgb16(6, 18, 19, 20 );
 			writerot16(9) = rgb24torgb16(7, 21, 22, 23 );
 			
 			writerot16(10) = rgb24torgb16(8, 24, 25, 26 );
    	writerot16(11) = rgb24torgb16(9, 27, 28, 29 );
 			writerot16(13) = writerot16(12) = rgb24torgb16(10, 30, 31, 32 );
 			writerot16(14) = rgb24torgb16(11, 33, 34, 35 );
 			
 			writerot16(15) = rgb24torgb16(12, 36, 37, 38 );
    	writerot16(16) = rgb24torgb16(13, 39, 40, 41 );
 			writerot16(18) = writerot16(17) =rgb24torgb16(14, 42, 43, 44 );
 			writerot16(19) = rgb24torgb16(15, 45, 46, 47 );
 			
      pixelstart += 480*20;
      src8 += 48;
    }while(--uCount);
    LOOP_END_8
  }
}

void GPU_Blit_384_480_ROT90CW_2X(void* src, u16* dst16, u32 h1)
{ 
  u32 uCount;
  if((GPU_GP1 & 0x00200000) == 0)
  {
    LOOP_START_16(32)
    do{
    	writerot16(1) = writerot16(0) = bgr15torgb16(0);
 			writerot16(3) = writerot16(2) = bgr15torgb16(1);
 			writerot16(4) = bgr15torgb16(2);
 			
    	writerot16(6) = writerot16(5) = bgr15torgb16(3);
 			writerot16(8) = writerot16(7) = bgr15torgb16(4);
 			writerot16(9) = bgr15torgb16(5);
 			
    	writerot16(11) = writerot16(10) = bgr15torgb16(6);
 			writerot16(13) = writerot16(12) = bgr15torgb16(7);
 			writerot16(14) = bgr15torgb16(8);
 			
    	writerot16(16) = writerot16(15) = bgr15torgb16(9);
 			writerot16(18) = writerot16(17) = bgr15torgb16(10);
 			writerot16(19) = bgr15torgb16(11);
 			
      pixelstart += 480*20;
      src16 += 12;
    }while(--uCount);
    LOOP_END_16
  }
  else
  {
    LOOP_START_8(32)
    do{
    	writerot16(1) = writerot16(0) = rgb24torgb16(0, 0,  1,  2 );
 			writerot16(3) = writerot16(2) = rgb24torgb16(1, 3,  4,  5 );
 			writerot16(4) = rgb24torgb16(2, 6,  7,  8 );
 			
    	writerot16(6) = writerot16(5) = rgb24torgb16(3, 9, 10, 11 );
 			writerot16(8) = writerot16(7) = rgb24torgb16(4, 12, 13, 14 );
 			writerot16(9) = rgb24torgb16(5, 15, 16, 17 );
 			
    	writerot16(11) = writerot16(10) = rgb24torgb16(6, 18, 19, 20 );
 			writerot16(13) = writerot16(12) = rgb24torgb16(7, 21, 22, 23 );
 			writerot16(14) = rgb24torgb16(8, 24, 25, 26 );
 			
    	writerot16(16) = writerot16(15) = rgb24torgb16(9, 27, 28, 29 );
 			writerot16(18) = writerot16(17) = rgb24torgb16(10, 30, 31, 32 );
 			writerot16(19) = rgb24torgb16(11, 33, 34, 35 );
 			
      pixelstart += 480*20;
      src8 += 36;
    }while(--uCount);
    LOOP_END_8
  }
}

void GPU_Blit_368_480_ROT90CW_2X(void* src, u16* dst16, u32 h1)
{ 
  u32 uCount;
  if((GPU_GP1 & 0x00200000) == 0)
  {
    LOOP_START_16(16)
    do{
    	writerot16(1) = writerot16(0) = bgr15torgb16(0);
    	writerot16(3) = writerot16(2) = bgr15torgb16(1);
    	writerot16(5) = writerot16(4) = bgr15torgb16(2);
    	writerot16(6) = bgr15torgb16(3);
    	writerot16(7) = bgr15torgb16(4);
    	writerot16(9) =  writerot16(8) = bgr15torgb16(5);
    	writerot16(11) = writerot16(10) = bgr15torgb16(6);
    	writerot16(13) = writerot16(12) = bgr15torgb16(7);
    	writerot16(15) = writerot16(14) = bgr15torgb16(8);
    	writerot16(17) = writerot16(16) = bgr15torgb16(9);
    	writerot16(18) = bgr15torgb16(10);
    	writerot16(19) = bgr15torgb16(11);
    	writerot16(21) = writerot16(20) = bgr15torgb16(12);
    	writerot16(23) = writerot16(22) = bgr15torgb16(13);
    	writerot16(25) = writerot16(24) = bgr15torgb16(14);
    	writerot16(27) = writerot16(26) = bgr15torgb16(15);
    	writerot16(29) = writerot16(28) = bgr15torgb16(16);
    	writerot16(30) = bgr15torgb16(17);
    	writerot16(31) = bgr15torgb16(18);
    	writerot16(33) = writerot16(32) = bgr15torgb16(19);
    	writerot16(35) = writerot16(34) = bgr15torgb16(20);
    	writerot16(37) = writerot16(36) = bgr15torgb16(21);
    	writerot16(39) = writerot16(38) = bgr15torgb16(22);

      pixelstart += 480*40;
      src16 += 23;
    }while(--uCount);
    LOOP_END_16
  }
  else
  {
    LOOP_START_8(16)
    do{
    	writerot16(1) = writerot16(0) = rgb24torgb16(0, 0,  1,  2 );
    	writerot16(3) = writerot16(2) = rgb24torgb16(1, 3, 4,  5 );
    	writerot16(5) = writerot16(4) = rgb24torgb16(2, 6, 7,  8 );
    	writerot16(6) = rgb24torgb16(3, 9, 10, 11 );
    	writerot16(7) = rgb24torgb16(4, 12, 13, 14 );
    	writerot16(9) =  writerot16(8) = rgb24torgb16(5, 15, 16, 17 );
    	writerot16(11) = writerot16(10) = rgb24torgb16(6, 18, 19, 20 );
    	writerot16(13) = writerot16(12) = rgb24torgb16(7, 21, 22, 23 );
    	writerot16(15) = writerot16(14) = rgb24torgb16(8, 24, 25, 26 );
    	writerot16(17) = writerot16(16) = rgb24torgb16(9, 27, 28, 29 );
    	writerot16(18) = rgb24torgb16(10, 30, 31, 32 );
    	writerot16(19) = rgb24torgb16(11, 33, 34, 35 );
    	writerot16(21) = writerot16(20) = rgb24torgb16(12, 36, 37, 38 );
    	writerot16(23) = writerot16(22) = rgb24torgb16(13, 39, 40, 41 );
    	writerot16(25) = writerot16(24) = rgb24torgb16(14, 42, 43, 44 );
    	writerot16(27) = writerot16(26) = rgb24torgb16(15, 45, 46, 47 );
    	writerot16(29) = writerot16(28) = rgb24torgb16(16, 48, 49, 50 );
    	writerot16(30) = rgb24torgb16(17, 51, 52, 53 );
    	writerot16(31) = rgb24torgb16(18, 54, 55, 56 );
    	writerot16(33) = writerot16(32) = rgb24torgb16(19, 57, 58, 59 );
    	writerot16(35) = writerot16(34) = rgb24torgb16(20, 60, 61, 62 );
    	writerot16(37) = writerot16(36) = rgb24torgb16(21, 63, 64, 65 );
    	writerot16(39) = writerot16(38) = rgb24torgb16(22, 66, 67, 68 );

      pixelstart += 480*40;
      src8 += 69;
    }while(--uCount);
    LOOP_END_8
  }
}

void GPU_Blit_256_480_ROT90CW_2X(void* src, u16* dst16, u32 h1)
{ 
  u32 uCount;
  if((GPU_GP1 & 0x00200000) == 0)
  {
    LOOP_START_16(32)
    do{
    	writerot16(1) = writerot16(0) = bgr15torgb16(0);
      writerot16(3) = writerot16(2) = bgr15torgb16(1);
      writerot16(7) = writerot16(6) = writerot16(5) = writerot16(4) = bgr15torgb16(2);
      writerot16(9) = writerot16(8) = bgr15torgb16(3);
      writerot16(11) = writerot16(10) = bgr15torgb16(4);
      writerot16(15) = writerot16(14) = writerot16(13) = writerot16(12) = bgr15torgb16(5);
      writerot16(17) = writerot16(16) = bgr15torgb16(6);
      writerot16(19) = writerot16(18) = bgr15torgb16(7);

      pixelstart += 480*20;
      src16 += 8;
    }while(--uCount);
    LOOP_END_16
  }
  else
  {
    LOOP_START_8(32)
    do{
    	writerot16(1) = writerot16(0) = rgb24torgb16(0, 0,  1,  2 );
      writerot16(3) = writerot16(2) = rgb24torgb16(1, 3,  4,  5 );
      writerot16(7) = writerot16(6) = writerot16(5) = writerot16(4) = rgb24torgb16(2, 6,  7,  8 );
      writerot16(9) = writerot16(8) = rgb24torgb16(3, 9, 10, 11 );
      writerot16(11) = writerot16(10) = rgb24torgb16(4, 12, 13, 14 );
      writerot16(15) = writerot16(14) = writerot16(13) = writerot16(12) = rgb24torgb16(5, 15, 16, 17 );
      writerot16(17) = writerot16(16) = rgb24torgb16(6, 18, 19, 20 );
      writerot16(19) = writerot16(18) = rgb24torgb16(7, 21, 22, 23 );

      pixelstart += 480*20;
      src8 += 24;
    }while(--uCount);
    LOOP_END_8
  }
}

void GPU_Blit_320_480_ROT90CW_2X(void* src, u16* dst16, u32 h1)
{
  u32 uCount;
  if((GPU_GP1 & 0x00200000) == 0)
  {
    LOOP_START_16(32)
    do{
    	writerot16(1) = writerot16(0) = bgr15torgb16(0);
      writerot16(3) = writerot16(2) = bgr15torgb16(1);
      writerot16(5) = writerot16(4) = bgr15torgb16(2);
      writerot16(7) = writerot16(6) = bgr15torgb16(3);
      writerot16(9) = writerot16(8) = bgr15torgb16(4);
      writerot16(11) = writerot16(10) = bgr15torgb16(5);
      writerot16(13) = writerot16(12) = bgr15torgb16(6);
      writerot16(15) = writerot16(14) = bgr15torgb16(7);
      writerot16(17) = writerot16(16) = bgr15torgb16(8);
      writerot16(19) = writerot16(18) = bgr15torgb16(9);

      pixelstart += 480*20;
      src16 += 10;
    }while(--uCount);
    LOOP_END_16
  }
  else
  {
    LOOP_START_8(32)
    do{

    	writerot16(1) = writerot16(0) = rgb24torgb16(0, 0,  1,  2 );
      writerot16(3) = writerot16(2) = rgb24torgb16(1, 3,  4,  5 );
      writerot16(5) = writerot16(4) = rgb24torgb16(2, 6,  7,  8 );
      writerot16(7) = writerot16(6) = rgb24torgb16(3, 9, 10, 11 );
      writerot16(9) = writerot16(8) = rgb24torgb16(4, 12, 13, 14 );
      writerot16(11) = writerot16(10) = rgb24torgb16(5, 15, 16, 17 );
      writerot16(13) = writerot16(12) = rgb24torgb16(6, 18, 19, 20 );
      writerot16(15) = writerot16(14) = rgb24torgb16(7, 21, 22, 23 );
      writerot16(17) = writerot16(16) = rgb24torgb16(8, 24, 25, 26 );
      writerot16(19) = writerot16(18) = rgb24torgb16(9, 27, 28, 29 );

      pixelstart += 480*20;
      src8 += 30;
    }while(--uCount);
    LOOP_END_8
  }
}

void GPU_Blit_640_480_ROT90CW(void* src, u16* dst16, u32 h1)
{
  u32 uCount;
  if((GPU_GP1 & 0x00200000) == 0)
  {
		LOOP_START_16(160)
    do
    {
      writerot16(0) = bgr15torgb16(0);
			writerot16(1) = bgr15torgb16(1);
			writerot16(2) = bgr15torgb16(2);
			writerot16(3) = bgr15torgb16(3);
			
      pixelstart += 480<<2;
      src16 += 4;
    }while(--uCount);
    LOOP_END_16
  }
	else
  {
  	LOOP_START_8(160)
    do
    {
      writerot16(0) = rgb24torgb16(0, 0,  1,  2 );
      writerot16(0) = rgb24torgb16(1, 3,  4,  5 );
      writerot16(0) = rgb24torgb16(2, 6,  7,  8 );
      writerot16(0) = rgb24torgb16(3, 9, 10, 11 );

      pixelstart += 480<<2;
      src8 += 12;
    }while(--uCount);
    LOOP_END_8
  }
}

void GPU_Blit_512_240_ROT90CW_2X(void* src, u16* dst16, u32 h1)
{ 
  u32 uCount;
  if((GPU_GP1 & 0x00200000) == 0)
  {
    LOOP_START_16_240(32)
    do{
    	write2xrot16(0) = bgr15torgb16(0);
    	write2xrot16(1) = bgr15torgb16(1);
 			write2xrot16(3) = write2xrot16(2) = bgr15torgb16(2);
 			write2xrot16(4) = bgr15torgb16(3);
 			
    	write2xrot16(5) = bgr15torgb16(4);
    	write2xrot16(6) = bgr15torgb16(5);
 			write2xrot16(8) = write2xrot16(7) = bgr15torgb16(6);
 			write2xrot16(9) = bgr15torgb16(7);
 			
 			write2xrot16(10) = bgr15torgb16(8);
    	write2xrot16(11) = bgr15torgb16(9);
 			write2xrot16(13) = write2xrot16(12) = bgr15torgb16(10);
 			write2xrot16(14) = bgr15torgb16(11);
 			
 			write2xrot16(15) = bgr15torgb16(12);
    	write2xrot16(16) = bgr15torgb16(13);
 			write2xrot16(18) = write2xrot16(17) = bgr15torgb16(14);
 			write2xrot16(19) = bgr15torgb16(15);
 			
      pixelstart += 480*20;
      src16 += 16;
    }while(--uCount);
    LOOP_END_16
  }
  else
  {
    LOOP_START_8_240(32)
    do{
    	write2xrot16(0) = rgb24torgb16(0, 0,  1,  2 );
    	write2xrot16(1) = rgb24torgb16(1, 3,  4,  5 );
 			write2xrot16(3) = write2xrot16(2) = rgb24torgb16(2, 6,  7,  8 );
 			write2xrot16(4) = rgb24torgb16(3, 9, 10, 11 );
 			
    	write2xrot16(5) = rgb24torgb16(4, 12, 13, 14 );
    	write2xrot16(6) = rgb24torgb16(5, 15, 16, 17 );
 			write2xrot16(8) = write2xrot16(7) = rgb24torgb16(6, 18, 19, 20 );
 			write2xrot16(9) = rgb24torgb16(7, 21, 22, 23 );
 			
 			write2xrot16(10) = rgb24torgb16(8, 24, 25, 26 );
    	write2xrot16(11) = rgb24torgb16(9, 27, 28, 29 );
 			write2xrot16(13) = write2xrot16(12) = rgb24torgb16(10, 30, 31, 32 );
 			write2xrot16(14) = rgb24torgb16(11, 33, 34, 35 );
 			
 			write2xrot16(15) = rgb24torgb16(12, 36, 37, 38 );
    	write2xrot16(16) = rgb24torgb16(13, 39, 40, 41 );
 			write2xrot16(18) = write2xrot16(17) =rgb24torgb16(14, 42, 43, 44 );
 			write2xrot16(19) = rgb24torgb16(15, 45, 46, 47 );
 			
      pixelstart += 480*20;
      src8 += 48;
    }while(--uCount);
    LOOP_END_8
  }
}

void GPU_Blit_384_240_ROT90CW_2X(void* src, u16* dst16, u32 h1)
{ 
  u32 uCount;
  if((GPU_GP1 & 0x00200000) == 0)
  {
    LOOP_START_16_240(32)
    do{
    	write2xrot16(1) = write2xrot16(0) = bgr15torgb16(0);
 			write2xrot16(3) = write2xrot16(2) = bgr15torgb16(1);
 			write2xrot16(4) = bgr15torgb16(2);
 			
    	write2xrot16(6) = write2xrot16(5) = bgr15torgb16(3);
 			write2xrot16(8) = write2xrot16(7) = bgr15torgb16(4);
 			write2xrot16(9) = bgr15torgb16(5);
 			
    	write2xrot16(11) = write2xrot16(10) = bgr15torgb16(6);
 			write2xrot16(13) = write2xrot16(12) = bgr15torgb16(7);
 			write2xrot16(14) = bgr15torgb16(8);
 			
    	write2xrot16(16) = write2xrot16(15) = bgr15torgb16(9);
 			write2xrot16(18) = write2xrot16(17) = bgr15torgb16(10);
 			write2xrot16(19) = bgr15torgb16(11);
 			
      pixelstart += 480*20;
      src16 += 12;
    }while(--uCount);
    LOOP_END_16
  }
  else
  {
    LOOP_START_8_240(32)
    do{
    	write2xrot16(1) = write2xrot16(0) = rgb24torgb16(0, 0,  1,  2 );
 			write2xrot16(3) = write2xrot16(2) = rgb24torgb16(1, 3,  4,  5 );
 			write2xrot16(4) = rgb24torgb16(2, 6,  7,  8 );
 			
    	write2xrot16(6) = write2xrot16(5) = rgb24torgb16(3, 9, 10, 11 );
 			write2xrot16(8) = write2xrot16(7) = rgb24torgb16(4, 12, 13, 14 );
 			write2xrot16(9) = rgb24torgb16(5, 15, 16, 17 );
 			
    	write2xrot16(11) = write2xrot16(10) = rgb24torgb16(6, 18, 19, 20 );
 			write2xrot16(13) = write2xrot16(12) = rgb24torgb16(7, 21, 22, 23 );
 			write2xrot16(14) = rgb24torgb16(8, 24, 25, 26 );
 			
    	write2xrot16(16) = write2xrot16(15) = rgb24torgb16(9, 27, 28, 29 );
 			write2xrot16(18) = write2xrot16(17) = rgb24torgb16(10, 30, 31, 32 );
 			write2xrot16(19) = rgb24torgb16(11, 33, 34, 35 );
 			
      pixelstart += 480*20;
      src8 += 36;
    }while(--uCount);
    LOOP_END_8
  }
}

void GPU_Blit_368_240_ROT90CW_2X(void* src, u16* dst16, u32 h1)
{
  u32 uCount;
  if((GPU_GP1 & 0x00200000) == 0)
  {
    LOOP_START_16_240(16)
    do{
    	write2xrot16(1) = write2xrot16(0) = bgr15torgb16(0);
    	write2xrot16(3) = write2xrot16(2) = bgr15torgb16(1);
    	write2xrot16(5) = write2xrot16(4) = bgr15torgb16(2);
    	write2xrot16(6) = bgr15torgb16(3);
    	write2xrot16(7) = bgr15torgb16(4);
    	write2xrot16(9) =  write2xrot16(8) = bgr15torgb16(5);
    	write2xrot16(11) = write2xrot16(10) = bgr15torgb16(6);
    	write2xrot16(13) = write2xrot16(12) = bgr15torgb16(7);
    	write2xrot16(15) = write2xrot16(14) = bgr15torgb16(8);
    	write2xrot16(17) = write2xrot16(16) = bgr15torgb16(9);
    	write2xrot16(18) = bgr15torgb16(10);
    	write2xrot16(19) = bgr15torgb16(11);
    	write2xrot16(21) = write2xrot16(20) = bgr15torgb16(12);
    	write2xrot16(23) = write2xrot16(22) = bgr15torgb16(13);
    	write2xrot16(25) = write2xrot16(24) = bgr15torgb16(14);
    	write2xrot16(27) = write2xrot16(26) = bgr15torgb16(15);
    	write2xrot16(29) = write2xrot16(28) = bgr15torgb16(16);
    	write2xrot16(30) = bgr15torgb16(17);
    	write2xrot16(31) = bgr15torgb16(18);
    	write2xrot16(33) = write2xrot16(32) = bgr15torgb16(19);
    	write2xrot16(35) = write2xrot16(34) = bgr15torgb16(20);
    	write2xrot16(37) = write2xrot16(36) = bgr15torgb16(21);
    	write2xrot16(39) = write2xrot16(38) = bgr15torgb16(22);

      pixelstart += 480*40;
      src16 += 23;
    }while(--uCount);
    LOOP_END_16
  }
  else
  {
    LOOP_START_8_240(16)
    do{
    	write2xrot16(1) = write2xrot16(0) = rgb24torgb16(0, 0,  1,  2 );
    	write2xrot16(3) = write2xrot16(2) = rgb24torgb16(1, 3, 4,  5 );
    	write2xrot16(5) = write2xrot16(4) = rgb24torgb16(2, 6, 7,  8 );
    	write2xrot16(6) = rgb24torgb16(3, 9, 10, 11 );
    	write2xrot16(7) = rgb24torgb16(4, 12, 13, 14 );
    	write2xrot16(9) =  write2xrot16(8) = rgb24torgb16(5, 15, 16, 17 );
    	write2xrot16(11) = write2xrot16(10) = rgb24torgb16(6, 18, 19, 20 );
    	write2xrot16(13) = write2xrot16(12) = rgb24torgb16(7, 21, 22, 23 );
    	write2xrot16(15) = write2xrot16(14) = rgb24torgb16(8, 24, 25, 26 );
    	write2xrot16(17) = write2xrot16(16) = rgb24torgb16(9, 27, 28, 29 );
    	write2xrot16(18) = rgb24torgb16(10, 30, 31, 32 );
    	write2xrot16(19) = rgb24torgb16(11, 33, 34, 35 );
    	write2xrot16(21) = write2xrot16(20) = rgb24torgb16(12, 36, 37, 38 );
    	write2xrot16(23) = write2xrot16(22) = rgb24torgb16(13, 39, 40, 41 );
    	write2xrot16(25) = write2xrot16(24) = rgb24torgb16(14, 42, 43, 44 );
    	write2xrot16(27) = write2xrot16(26) = rgb24torgb16(15, 45, 46, 47 );
    	write2xrot16(29) = write2xrot16(28) = rgb24torgb16(16, 48, 49, 50 );
    	write2xrot16(30) = rgb24torgb16(17, 51, 52, 53 );
    	write2xrot16(31) = rgb24torgb16(18, 54, 55, 56 );
    	write2xrot16(33) = write2xrot16(32) = rgb24torgb16(19, 57, 58, 59 );
    	write2xrot16(35) = write2xrot16(34) = rgb24torgb16(20, 60, 61, 62 );
    	write2xrot16(37) = write2xrot16(36) = rgb24torgb16(21, 63, 64, 65 );
    	write2xrot16(39) = write2xrot16(38) = rgb24torgb16(22, 66, 67, 68 );

      pixelstart += 480*40;
      src8 += 69;
    }while(--uCount);
    LOOP_END_8
  }
}

void GPU_Blit_256_240_ROT90CW_2X(void* src, u16* dst16, u32 h1)
{ 
  u32 uCount;
  if((GPU_GP1 & 0x00200000) == 0)
  {
    LOOP_START_16_240(32)
    do{
    	write2xrot16(1) = write2xrot16(0) = bgr15torgb16(0);
      write2xrot16(3) = write2xrot16(2) = bgr15torgb16(1);
      write2xrot16(7) = write2xrot16(6) = write2xrot16(5) = write2xrot16(4) = bgr15torgb16(2);
      write2xrot16(9) = write2xrot16(8) = bgr15torgb16(3);
      write2xrot16(11) = write2xrot16(10) = bgr15torgb16(4);
      write2xrot16(15) = write2xrot16(14) = write2xrot16(13) = write2xrot16(12) = bgr15torgb16(5);
      write2xrot16(17) = write2xrot16(16) = bgr15torgb16(6);
      write2xrot16(19) = write2xrot16(18) = bgr15torgb16(7);

      pixelstart += 480*20;
      src16 += 8;
    }while(--uCount);
    LOOP_END_16
  }
  else
  {
    LOOP_START_8_240(32)
    do{
    	write2xrot16(1) = write2xrot16(0) = rgb24torgb16(0, 0,  1,  2 );
      write2xrot16(3) = write2xrot16(2) = rgb24torgb16(1, 3,  4,  5 );
      write2xrot16(7) = write2xrot16(6) = write2xrot16(5) = write2xrot16(4) = rgb24torgb16(2, 6,  7,  8 );
      write2xrot16(9) = write2xrot16(8) = rgb24torgb16(3, 9, 10, 11 );
      write2xrot16(11) = write2xrot16(10) = rgb24torgb16(4, 12, 13, 14 );
      write2xrot16(15) = write2xrot16(14) = write2xrot16(13) = write2xrot16(12) = rgb24torgb16(5, 15, 16, 17 );
      write2xrot16(17) = write2xrot16(16) = rgb24torgb16(6, 18, 19, 20 );
      write2xrot16(19) = write2xrot16(18) = rgb24torgb16(7, 21, 22, 23 );

      pixelstart += 480*20;
      src8 += 24;
    }while(--uCount);
    LOOP_END_8
  }
}

void GPU_Blit_320_240_ROT90CW_2X(void* src, u16* dst16, u32 h1)
{
  u32 uCount;
  if((GPU_GP1 & 0x00200000) == 0)
  {
  	LOOP_START_16_240(32)
    do{
    	write2xrot16(1) = write2xrot16(0) = bgr15torgb16(0);
      write2xrot16(3) = write2xrot16(2) = bgr15torgb16(1);
      write2xrot16(5) = write2xrot16(4) = bgr15torgb16(2);
      write2xrot16(7) = write2xrot16(6) = bgr15torgb16(3);
      write2xrot16(9) = write2xrot16(8) = bgr15torgb16(4);
      write2xrot16(11) = write2xrot16(10) = bgr15torgb16(5);
      write2xrot16(13) = write2xrot16(12) = bgr15torgb16(6);
      write2xrot16(15) = write2xrot16(14) = bgr15torgb16(7);
      write2xrot16(17) = write2xrot16(16) = bgr15torgb16(8);
      write2xrot16(19) = write2xrot16(18) = bgr15torgb16(9);

      pixelstart += 480*20;
      src16 += 10;
    }while(--uCount);
  	LOOP_END_16
  }
  else
  {
    LOOP_START_8_240(32)
    do{

    	write2xrot16(1) = write2xrot16(0) = rgb24torgb16(0, 0,  1,  2 );
      write2xrot16(3) = write2xrot16(2) = rgb24torgb16(1, 3,  4,  5 );
      write2xrot16(5) = write2xrot16(4) = rgb24torgb16(2, 6,  7,  8 );
      write2xrot16(7) = write2xrot16(6) = rgb24torgb16(3, 9, 10, 11 );
      write2xrot16(9) = write2xrot16(8) = rgb24torgb16(4, 12, 13, 14 );
      write2xrot16(11) = write2xrot16(10) = rgb24torgb16(5, 15, 16, 17 );
      write2xrot16(13) = write2xrot16(12) = rgb24torgb16(6, 18, 19, 20 );
      write2xrot16(15) = write2xrot16(14) = rgb24torgb16(7, 21, 22, 23 );
      write2xrot16(17) = write2xrot16(16) = rgb24torgb16(8, 24, 25, 26 );
      write2xrot16(19) = write2xrot16(18) = rgb24torgb16(9, 27, 28, 29 );

      pixelstart += 480*20;
      src8 += 30;
    }while(--uCount);
    LOOP_END_8
  }
}

INLINE void GPU_BlitWW_SDL(const void* src, u16* dst16, u32 isRGB24, u32 width)
{
  u32 uCount;
  if(isRGB24 == 0)
  {
    uCount = width/16;
    const u32* src32 = (const u32*) src; 
          u32* dst32 = (      u32*) dst16;
    do{
                            //B                                 //G                          //R
      dst32[0] = ((src32[0]&(0x1f001f<<10))>>10) | ((src32[0]&(0x1f001f<<5))<<1) | ((src32[0]&(0x1f001f<<0))<<11);
      dst32[1] = ((src32[1]&(0x1f001f<<10))>>10) | ((src32[1]&(0x1f001f<<5))<<1) | ((src32[1]&(0x1f001f<<0))<<11);
      dst32[2] = ((src32[2]&(0x1f001f<<10))>>10) | ((src32[2]&(0x1f001f<<5))<<1) | ((src32[2]&(0x1f001f<<0))<<11);
      dst32[3] = ((src32[3]&(0x1f001f<<10))>>10) | ((src32[3]&(0x1f001f<<5))<<1) | ((src32[3]&(0x1f001f<<0))<<11);
      dst32[4] = ((src32[4]&(0x1f001f<<10))>>10) | ((src32[4]&(0x1f001f<<5))<<1) | ((src32[4]&(0x1f001f<<0))<<11);
      dst32[5] = ((src32[5]&(0x1f001f<<10))>>10) | ((src32[5]&(0x1f001f<<5))<<1) | ((src32[5]&(0x1f001f<<0))<<11);
      dst32[6] = ((src32[6]&(0x1f001f<<10))>>10) | ((src32[6]&(0x1f001f<<5))<<1) | ((src32[6]&(0x1f001f<<0))<<11);
      dst32[7] = ((src32[7]&(0x1f001f<<10))>>10) | ((src32[7]&(0x1f001f<<5))<<1) | ((src32[7]&(0x1f001f<<0))<<11);
      dst32 += 8;
      src32 += 8;
    }while(--uCount);
  }
	else
  {
    uCount = width / 16;
    const u8* src8 = (const u8*)src;
    do{
      dst16[ 0] = gp2x_video_RGB_color16(src8[ 0], src8[ 1], src8[ 2] );
      dst16[ 1] = gp2x_video_RGB_color16(src8[ 3], src8[ 4], src8[ 5] );
      dst16[ 2] = gp2x_video_RGB_color16(src8[ 6], src8[ 7], src8[ 8] );
      dst16[ 3] = gp2x_video_RGB_color16(src8[ 9], src8[10], src8[11] );
      dst16[ 4] = gp2x_video_RGB_color16(src8[12], src8[13], src8[14] );
      dst16[ 5] = gp2x_video_RGB_color16(src8[15], src8[16], src8[17] );
      dst16[ 6] = gp2x_video_RGB_color16(src8[18], src8[19], src8[20] );
      dst16[ 7] = gp2x_video_RGB_color16(src8[21], src8[22], src8[23] );

      dst16[ 8] = gp2x_video_RGB_color16(src8[24], src8[25], src8[26] );
      dst16[ 9] = gp2x_video_RGB_color16(src8[27], src8[28], src8[29] );
      dst16[10] = gp2x_video_RGB_color16(src8[30], src8[31], src8[32] );
      dst16[11] = gp2x_video_RGB_color16(src8[33], src8[34], src8[35] );
      dst16[12] = gp2x_video_RGB_color16(src8[36], src8[37], src8[38] );
      dst16[13] = gp2x_video_RGB_color16(src8[39], src8[40], src8[41] );
      dst16[14] = gp2x_video_RGB_color16(src8[42], src8[43], src8[44] );
      dst16[15] = gp2x_video_RGB_color16(src8[45], src8[46], src8[47] );
      dst16 += 16;
      src8  += 48;
    }while(--uCount);
  }
}


INLINE void GPU_BlitWW(const void* src, u16* dst16, u32 isRGB24)
{
  u32 uCount;
  if(isRGB24 == 0)
  {
    uCount = 20;
    const u32* src32 = (const u32*) src; 
          u32* dst32 = (      u32*) dst16;
    do{
                            //B                                 //G                          //R
      dst32[0] = ((src32[0]&(0x1f001f<<10))>>10) | ((src32[0]&(0x1f001f<<5))<<1) | ((src32[0]&(0x1f001f<<0))<<11);
      dst32[1] = ((src32[1]&(0x1f001f<<10))>>10) | ((src32[1]&(0x1f001f<<5))<<1) | ((src32[1]&(0x1f001f<<0))<<11);
      dst32[2] = ((src32[2]&(0x1f001f<<10))>>10) | ((src32[2]&(0x1f001f<<5))<<1) | ((src32[2]&(0x1f001f<<0))<<11);
      dst32[3] = ((src32[3]&(0x1f001f<<10))>>10) | ((src32[3]&(0x1f001f<<5))<<1) | ((src32[3]&(0x1f001f<<0))<<11);
      dst32[4] = ((src32[4]&(0x1f001f<<10))>>10) | ((src32[4]&(0x1f001f<<5))<<1) | ((src32[4]&(0x1f001f<<0))<<11);
      dst32[5] = ((src32[5]&(0x1f001f<<10))>>10) | ((src32[5]&(0x1f001f<<5))<<1) | ((src32[5]&(0x1f001f<<0))<<11);
      dst32[6] = ((src32[6]&(0x1f001f<<10))>>10) | ((src32[6]&(0x1f001f<<5))<<1) | ((src32[6]&(0x1f001f<<0))<<11);
      dst32[7] = ((src32[7]&(0x1f001f<<10))>>10) | ((src32[7]&(0x1f001f<<5))<<1) | ((src32[7]&(0x1f001f<<0))<<11);
      dst32 += 8;
      src32 += 8;
    }while(--uCount);
  }
	else
  {
    uCount = 20;
    const u8* src8 = (const u8*)src;
    do{
      dst16[ 0] = gp2x_video_RGB_color16(src8[ 0], src8[ 1], src8[ 2] );
      dst16[ 1] = gp2x_video_RGB_color16(src8[ 3], src8[ 4], src8[ 5] );
      dst16[ 2] = gp2x_video_RGB_color16(src8[ 6], src8[ 7], src8[ 8] );
      dst16[ 3] = gp2x_video_RGB_color16(src8[ 9], src8[10], src8[11] );
      dst16[ 4] = gp2x_video_RGB_color16(src8[12], src8[13], src8[14] );
      dst16[ 5] = gp2x_video_RGB_color16(src8[15], src8[16], src8[17] );
      dst16[ 6] = gp2x_video_RGB_color16(src8[18], src8[19], src8[20] );
      dst16[ 7] = gp2x_video_RGB_color16(src8[21], src8[22], src8[23] );

      dst16[ 8] = gp2x_video_RGB_color16(src8[24], src8[25], src8[26] );
      dst16[ 9] = gp2x_video_RGB_color16(src8[27], src8[28], src8[29] );
      dst16[10] = gp2x_video_RGB_color16(src8[30], src8[31], src8[32] );
      dst16[11] = gp2x_video_RGB_color16(src8[33], src8[34], src8[35] );
      dst16[12] = gp2x_video_RGB_color16(src8[36], src8[37], src8[38] );
      dst16[13] = gp2x_video_RGB_color16(src8[39], src8[40], src8[41] );
      dst16[14] = gp2x_video_RGB_color16(src8[42], src8[43], src8[44] );
      dst16[15] = gp2x_video_RGB_color16(src8[45], src8[46], src8[47] );
      dst16 += 16;
      src8  += 48;
    }while(--uCount);
  }
}

INLINE void GPU_BlitWWSWWSWS(const void* src, u16* dst16, u32 isRGB24)
{
  u32 uCount;
  if(isRGB24 == 0)
  {
    uCount = 32;
    const u16* src16 = (const u16*) src; 
    do{
                            //B                                 //G                          //R
      dst16[ 0] = ((src16[ 0]&(0x1f001f<<10))>>10) | ((src16[ 0]&(0x1f001f<<5))<<1) | ((src16[ 0]&(0x1f001f<<0))<<11);
      dst16[ 1] = ((src16[ 1]&(0x1f001f<<10))>>10) | ((src16[ 1]&(0x1f001f<<5))<<1) | ((src16[ 1]&(0x1f001f<<0))<<11);
      dst16[ 2] = ((src16[ 3]&(0x1f001f<<10))>>10) | ((src16[ 3]&(0x1f001f<<5))<<1) | ((src16[ 3]&(0x1f001f<<0))<<11);
      dst16[ 3] = ((src16[ 4]&(0x1f001f<<10))>>10) | ((src16[ 4]&(0x1f001f<<5))<<1) | ((src16[ 4]&(0x1f001f<<0))<<11);
      dst16[ 4] = ((src16[ 6]&(0x1f001f<<10))>>10) | ((src16[ 6]&(0x1f001f<<5))<<1) | ((src16[ 6]&(0x1f001f<<0))<<11);
      dst16[ 5] = ((src16[ 8]&(0x1f001f<<10))>>10) | ((src16[ 8]&(0x1f001f<<5))<<1) | ((src16[ 8]&(0x1f001f<<0))<<11);
      dst16[ 6] = ((src16[ 9]&(0x1f001f<<10))>>10) | ((src16[ 9]&(0x1f001f<<5))<<1) | ((src16[ 9]&(0x1f001f<<0))<<11);
      dst16[ 7] = ((src16[11]&(0x1f001f<<10))>>10) | ((src16[11]&(0x1f001f<<5))<<1) | ((src16[11]&(0x1f001f<<0))<<11);
      dst16[ 8] = ((src16[12]&(0x1f001f<<10))>>10) | ((src16[12]&(0x1f001f<<5))<<1) | ((src16[12]&(0x1f001f<<0))<<11);
	    dst16[ 9] = ((src16[14]&(0x1f001f<<10))>>10) | ((src16[14]&(0x1f001f<<5))<<1) | ((src16[14]&(0x1f001f<<0))<<11);
	    dst16 += 10;
      src16 += 16;
    }while(--uCount);
  }
else
  {
    uCount = 32;
    const u8* src8 = (const u8*)src;
    do{
      dst16[ 0] = gp2x_video_RGB_color16(src8[ 0], src8[ 1], src8[ 2] );
      dst16[ 1] = gp2x_video_RGB_color16(src8[ 3], src8[ 4], src8[ 5] );
      dst16[ 2] = gp2x_video_RGB_color16(src8[ 9], src8[10], src8[11] );
      dst16[ 3] = gp2x_video_RGB_color16(src8[12], src8[13], src8[14] );
      dst16[ 4] = gp2x_video_RGB_color16(src8[18], src8[19], src8[20] );

      dst16[ 5] = gp2x_video_RGB_color16(src8[24], src8[25], src8[26] );
      dst16[ 6] = gp2x_video_RGB_color16(src8[27], src8[28], src8[29] );
      dst16[ 7] = gp2x_video_RGB_color16(src8[33], src8[34], src8[35] );
      dst16[ 8] = gp2x_video_RGB_color16(src8[36], src8[37], src8[38] );
      dst16[ 9] = gp2x_video_RGB_color16(src8[42], src8[43], src8[44] );

      dst16 += 10;
      src8  += 48;
    }while(--uCount);
  }
}

INLINE void GPU_BlitWWWWWS(const void* src, u16* dst16, u32 isRGB24)
{
  u32 uCount;
  if(isRGB24 == 0)
  {
    uCount = 32;
    const u16* src16 = (const u16*) src; 
    do{
							//B                                 //G                          //R
      dst16[ 0] = ((src16[0]&(0x1f001f<<10))>>10) | ((src16[0]&(0x1f001f<<5))<<1) | ((src16[0]&(0x1f001f<<0))<<11);
      dst16[ 1] = ((src16[1]&(0x1f001f<<10))>>10) | ((src16[1]&(0x1f001f<<5))<<1) | ((src16[1]&(0x1f001f<<0))<<11);
      dst16[ 2] = ((src16[2]&(0x1f001f<<10))>>10) | ((src16[2]&(0x1f001f<<5))<<1) | ((src16[2]&(0x1f001f<<0))<<11);
      dst16[ 3] = ((src16[3]&(0x1f001f<<10))>>10) | ((src16[3]&(0x1f001f<<5))<<1) | ((src16[3]&(0x1f001f<<0))<<11);
	    dst16[ 4] = ((src16[4]&(0x1f001f<<10))>>10) | ((src16[4]&(0x1f001f<<5))<<1) | ((src16[4]&(0x1f001f<<0))<<11);
      dst16[ 5] = ((src16[6]&(0x1f001f<<10))>>10) | ((src16[6]&(0x1f001f<<5))<<1) | ((src16[6]&(0x1f001f<<0))<<11);
      dst16[ 6] = ((src16[7]&(0x1f001f<<10))>>10) | ((src16[7]&(0x1f001f<<5))<<1) | ((src16[7]&(0x1f001f<<0))<<11);
      dst16[ 7] = ((src16[8]&(0x1f001f<<10))>>10) | ((src16[8]&(0x1f001f<<5))<<1) | ((src16[8]&(0x1f001f<<0))<<11);
      dst16[ 8] = ((src16[9]&(0x1f001f<<10))>>10) | ((src16[9]&(0x1f001f<<5))<<1) | ((src16[9]&(0x1f001f<<0))<<11);
	    dst16[ 9] = ((src16[10]&(0x1f001f<<10))>>10) | ((src16[10]&(0x1f001f<<5))<<1) | ((src16[10]&(0x1f001f<<0))<<11);
	    dst16 += 10;
      src16 += 12;
    }while(--uCount);
  }
  else
  {
    uCount = 32;
    const u8* src8 = (const u8*)src;
    do{
      dst16[0] = gp2x_video_RGB_color16(src8[ 0], src8[ 1], src8[ 2] );
      dst16[1] = gp2x_video_RGB_color16(src8[ 3], src8[ 4], src8[ 5] );
      dst16[2] = gp2x_video_RGB_color16(src8[ 6], src8[ 7], src8[ 8] );
      dst16[3] = gp2x_video_RGB_color16(src8[ 9], src8[10], src8[11] );
	    dst16[4] = gp2x_video_RGB_color16(src8[12], src8[13], src8[14] );
      dst16[5] = gp2x_video_RGB_color16(src8[18], src8[19], src8[20] );
      dst16[6] = gp2x_video_RGB_color16(src8[21], src8[22], src8[23] );
      dst16[7] = gp2x_video_RGB_color16(src8[24], src8[25], src8[26] );
      dst16[8] = gp2x_video_RGB_color16(src8[27], src8[28], src8[29] );
	    dst16[9] = gp2x_video_RGB_color16(src8[30], src8[31], src8[32] );
      dst16 += 10;
      src8  += 36;
    }while(--uCount);
  }
}

INLINE void GPU_BlitWWWWWWWWS(const void* src, u16* dst16, u32 isRGB24, u32 uClip_src)
{
  u32 uCount;
  if(isRGB24 == 0)
  {
    uCount = 20;
    const u16* src16 = ((const u16*) src) + uClip_src; 
    do{                         //B                                 //G                          //R
      dst16[ 0] = ((src16[ 0]&(0x1f001f<<10))>>10) | ((src16[ 0]&(0x1f001f<<5))<<1) | ((src16[ 0]&(0x1f001f<<0))<<11);
      dst16[ 1] = ((src16[ 1]&(0x1f001f<<10))>>10) | ((src16[ 1]&(0x1f001f<<5))<<1) | ((src16[ 1]&(0x1f001f<<0))<<11);
      dst16[ 2] = ((src16[ 2]&(0x1f001f<<10))>>10) | ((src16[ 2]&(0x1f001f<<5))<<1) | ((src16[ 2]&(0x1f001f<<0))<<11);
      dst16[ 3] = ((src16[ 3]&(0x1f001f<<10))>>10) | ((src16[ 3]&(0x1f001f<<5))<<1) | ((src16[ 3]&(0x1f001f<<0))<<11);
	    dst16[ 4] = ((src16[ 4]&(0x1f001f<<10))>>10) | ((src16[ 4]&(0x1f001f<<5))<<1) | ((src16[ 4]&(0x1f001f<<0))<<11);
	    dst16[ 5] = ((src16[ 5]&(0x1f001f<<10))>>10) | ((src16[ 5]&(0x1f001f<<5))<<1) | ((src16[ 5]&(0x1f001f<<0))<<11);
	    dst16[ 6] = ((src16[ 6]&(0x1f001f<<10))>>10) | ((src16[ 6]&(0x1f001f<<5))<<1) | ((src16[ 6]&(0x1f001f<<0))<<11);
	    dst16[ 7] = ((src16[ 7]&(0x1f001f<<10))>>10) | ((src16[ 7]&(0x1f001f<<5))<<1) | ((src16[ 7]&(0x1f001f<<0))<<11);

      dst16[ 8] = ((src16[ 9]&(0x1f001f<<10))>>10) | ((src16[ 9]&(0x1f001f<<5))<<1) | ((src16[ 9]&(0x1f001f<<0))<<11);
      dst16[ 9] = ((src16[10]&(0x1f001f<<10))>>10) | ((src16[10]&(0x1f001f<<5))<<1) | ((src16[10]&(0x1f001f<<0))<<11);
      dst16[10] = ((src16[11]&(0x1f001f<<10))>>10) | ((src16[11]&(0x1f001f<<5))<<1) | ((src16[11]&(0x1f001f<<0))<<11);
      dst16[11] = ((src16[12]&(0x1f001f<<10))>>10) | ((src16[12]&(0x1f001f<<5))<<1) | ((src16[12]&(0x1f001f<<0))<<11);
	    dst16[12] = ((src16[13]&(0x1f001f<<10))>>10) | ((src16[13]&(0x1f001f<<5))<<1) | ((src16[13]&(0x1f001f<<0))<<11);
	    dst16[13] = ((src16[14]&(0x1f001f<<10))>>10) | ((src16[14]&(0x1f001f<<5))<<1) | ((src16[14]&(0x1f001f<<0))<<11);
	    dst16[14] = ((src16[15]&(0x1f001f<<10))>>10) | ((src16[15]&(0x1f001f<<5))<<1) | ((src16[15]&(0x1f001f<<0))<<11);
	    dst16[15] = ((src16[16]&(0x1f001f<<10))>>10) | ((src16[16]&(0x1f001f<<5))<<1) | ((src16[16]&(0x1f001f<<0))<<11);
	  	dst16 += 16;
      src16 += 18;
    }while(--uCount);
  }
  else
  {
    uCount = 20;
    const u8* src8 = (const u8*)src + (uClip_src<<1) + uClip_src;
    do{
      dst16[ 0] = gp2x_video_RGB_color16(src8[ 0], src8[ 1], src8[ 2] );
      dst16[ 1] = gp2x_video_RGB_color16(src8[ 3], src8[ 4], src8[ 5] );
      dst16[ 2] = gp2x_video_RGB_color16(src8[ 6], src8[ 7], src8[ 8] );
      dst16[ 3] = gp2x_video_RGB_color16(src8[ 9], src8[10], src8[11] );
      dst16[ 4] = gp2x_video_RGB_color16(src8[12], src8[13], src8[14] );
      dst16[ 5] = gp2x_video_RGB_color16(src8[15], src8[16], src8[17] );
      dst16[ 6] = gp2x_video_RGB_color16(src8[18], src8[19], src8[20] );
      dst16[ 7] = gp2x_video_RGB_color16(src8[21], src8[22], src8[23] );

      dst16[ 8] = gp2x_video_RGB_color16(src8[27], src8[28], src8[29] );
      dst16[ 9] = gp2x_video_RGB_color16(src8[30], src8[31], src8[32] );
      dst16[10] = gp2x_video_RGB_color16(src8[33], src8[34], src8[35] );
      dst16[11] = gp2x_video_RGB_color16(src8[36], src8[37], src8[38] );
      dst16[12] = gp2x_video_RGB_color16(src8[39], src8[40], src8[41] );
      dst16[13] = gp2x_video_RGB_color16(src8[42], src8[43], src8[44] );
      dst16[14] = gp2x_video_RGB_color16(src8[45], src8[46], src8[47] );
      dst16[15] = gp2x_video_RGB_color16(src8[48], src8[49], src8[50] );
      dst16 += 16;
      src8  += 54;
    }while(--uCount);
  }
}

INLINE void GPU_BlitWWDWW(const void* src, u16* dst16, u32 isRGB24)
{
  u32 uCount;
  if(isRGB24 == 0)
  {
    uCount = 32;
    const u16* src16 = (const u16*) src; 
    do{                            //B                                 //G                          //R
      dst16[ 0] = ((src16[0]&(0x1f001f<<10))>>10) | ((src16[0]&(0x1f001f<<5))<<1) | ((src16[0]&(0x1f001f<<0))<<11);
      dst16[ 1] = ((src16[1]&(0x1f001f<<10))>>10) | ((src16[1]&(0x1f001f<<5))<<1) | ((src16[1]&(0x1f001f<<0))<<11);
      dst16[ 2] = dst16[1];
      dst16[ 3] = ((src16[2]&(0x1f001f<<10))>>10) | ((src16[2]&(0x1f001f<<5))<<1) | ((src16[2]&(0x1f001f<<0))<<11);
      dst16[ 4] = ((src16[3]&(0x1f001f<<10))>>10) | ((src16[3]&(0x1f001f<<5))<<1) | ((src16[3]&(0x1f001f<<0))<<11);
      dst16[ 5] = ((src16[4]&(0x1f001f<<10))>>10) | ((src16[4]&(0x1f001f<<5))<<1) | ((src16[4]&(0x1f001f<<0))<<11);
      dst16[ 6] = ((src16[5]&(0x1f001f<<10))>>10) | ((src16[5]&(0x1f001f<<5))<<1) | ((src16[5]&(0x1f001f<<0))<<11);
      dst16[ 7] = dst16[6];
      dst16[ 8] = ((src16[6]&(0x1f001f<<10))>>10) | ((src16[6]&(0x1f001f<<5))<<1) | ((src16[6]&(0x1f001f<<0))<<11);
      dst16[ 9] = ((src16[7]&(0x1f001f<<10))>>10) | ((src16[7]&(0x1f001f<<5))<<1) | ((src16[7]&(0x1f001f<<0))<<11);
      dst16 += 10;
      src16 +=  8;
    }while(--uCount);
  }
  else
  {
    uCount = 32;
    const u8* src8 = (const u8*)src;
    do{
      dst16[ 0] = gp2x_video_RGB_color16(src8[0], src8[ 1], src8[ 2] );
      dst16[ 1] = gp2x_video_RGB_color16(src8[3], src8[ 4], src8[ 5] );
	    dst16[ 2] = dst16[1];
      dst16[ 3] = gp2x_video_RGB_color16(src8[6], src8[ 7], src8[ 8] );
      dst16[ 4] = gp2x_video_RGB_color16(src8[9], src8[10], src8[11] );

      dst16[ 5] = gp2x_video_RGB_color16(src8[12], src8[13], src8[14] );
      dst16[ 6] = gp2x_video_RGB_color16(src8[15], src8[16], src8[17] );
	    dst16[ 7] = dst16[6];
      dst16[ 8] = gp2x_video_RGB_color16(src8[18], src8[19], src8[20] );
      dst16[ 9] = gp2x_video_RGB_color16(src8[21], src8[22], src8[23] );
      dst16 += 10;
      src8  += 24;
    }while(--uCount);
  }
}


INLINE void GPU_BlitWS(const void* src, u16* dst16, u32 isRGB24)
{
  u32 uCount;
  if(isRGB24 == 0)
  {
    uCount = 20;
    const u16* src16 = (const u16*) src; 
    do{                            //B                                 //G                          //R
      dst16[ 0] = ((src16[ 0]&(0x1f001f<<10))>>10) | ((src16[ 0]&(0x1f001f<<5))<<1) | ((src16[ 0]&(0x1f001f<<0))<<11);
      dst16[ 1] = ((src16[ 2]&(0x1f001f<<10))>>10) | ((src16[ 2]&(0x1f001f<<5))<<1) | ((src16[ 2]&(0x1f001f<<0))<<11);
      dst16[ 2] = ((src16[ 4]&(0x1f001f<<10))>>10) | ((src16[ 4]&(0x1f001f<<5))<<1) | ((src16[ 4]&(0x1f001f<<0))<<11);
      dst16[ 3] = ((src16[ 6]&(0x1f001f<<10))>>10) | ((src16[ 6]&(0x1f001f<<5))<<1) | ((src16[ 6]&(0x1f001f<<0))<<11);

      dst16[ 4] = ((src16[ 8]&(0x1f001f<<10))>>10) | ((src16[ 8]&(0x1f001f<<5))<<1) | ((src16[ 8]&(0x1f001f<<0))<<11);
      dst16[ 5] = ((src16[10]&(0x1f001f<<10))>>10) | ((src16[10]&(0x1f001f<<5))<<1) | ((src16[10]&(0x1f001f<<0))<<11);
      dst16[ 6] = ((src16[12]&(0x1f001f<<10))>>10) | ((src16[12]&(0x1f001f<<5))<<1) | ((src16[12]&(0x1f001f<<0))<<11);
      dst16[ 7] = ((src16[14]&(0x1f001f<<10))>>10) | ((src16[14]&(0x1f001f<<5))<<1) | ((src16[14]&(0x1f001f<<0))<<11);

      dst16[ 8] = ((src16[16]&(0x1f001f<<10))>>10) | ((src16[16]&(0x1f001f<<5))<<1) | ((src16[16]&(0x1f001f<<0))<<11);
      dst16[ 9] = ((src16[18]&(0x1f001f<<10))>>10) | ((src16[18]&(0x1f001f<<5))<<1) | ((src16[18]&(0x1f001f<<0))<<11);
      dst16[10] = ((src16[20]&(0x1f001f<<10))>>10) | ((src16[20]&(0x1f001f<<5))<<1) | ((src16[20]&(0x1f001f<<0))<<11);
      dst16[11] = ((src16[22]&(0x1f001f<<10))>>10) | ((src16[22]&(0x1f001f<<5))<<1) | ((src16[22]&(0x1f001f<<0))<<11);

      dst16[12] = ((src16[24]&(0x1f001f<<10))>>10) | ((src16[24]&(0x1f001f<<5))<<1) | ((src16[24]&(0x1f001f<<0))<<11);
      dst16[13] = ((src16[26]&(0x1f001f<<10))>>10) | ((src16[26]&(0x1f001f<<5))<<1) | ((src16[26]&(0x1f001f<<0))<<11);
      dst16[14] = ((src16[28]&(0x1f001f<<10))>>10) | ((src16[28]&(0x1f001f<<5))<<1) | ((src16[28]&(0x1f001f<<0))<<11);
      dst16[15] = ((src16[30]&(0x1f001f<<10))>>10) | ((src16[30]&(0x1f001f<<5))<<1) | ((src16[30]&(0x1f001f<<0))<<11);

	    dst16 += 16;
      src16 += 32;
    }while(--uCount);
  }
  else
  {
    uCount = 20;
    const u8* src8 = (const u8*) src; 
    do{
      dst16[ 0] = gp2x_video_RGB_color16(src8[ 0], src8[ 1], src8[ 2] );
      dst16[ 1] = gp2x_video_RGB_color16(src8[ 6], src8[ 7], src8[ 8] );
      dst16[ 2] = gp2x_video_RGB_color16(src8[12], src8[13], src8[14] );
      dst16[ 3] = gp2x_video_RGB_color16(src8[18], src8[19], src8[20] );

      dst16[ 4] = gp2x_video_RGB_color16(src8[24], src8[25], src8[26] );
      dst16[ 5] = gp2x_video_RGB_color16(src8[30], src8[31], src8[32] );
      dst16[ 6] = gp2x_video_RGB_color16(src8[36], src8[37], src8[38] );
      dst16[ 7] = gp2x_video_RGB_color16(src8[42], src8[43], src8[44] );

      dst16[ 8] = gp2x_video_RGB_color16(src8[48], src8[49], src8[50] );
      dst16[ 9] = gp2x_video_RGB_color16(src8[54], src8[55], src8[56] );
      dst16[10] = gp2x_video_RGB_color16(src8[60], src8[61], src8[62] );
      dst16[11] = gp2x_video_RGB_color16(src8[66], src8[67], src8[68] );

      dst16[12] = gp2x_video_RGB_color16(src8[72], src8[73], src8[74] );
      dst16[13] = gp2x_video_RGB_color16(src8[78], src8[79], src8[80] );
      dst16[14] = gp2x_video_RGB_color16(src8[84], src8[85], src8[86] );
      dst16[15] = gp2x_video_RGB_color16(src8[90], src8[91], src8[92] );

      dst16 += 16;
      src8  += 96;
    }while(--uCount);
  }
}


INLINE void GPU_BlitWSSWSSWSSWSSWSSS(const void* src, u16* dst16, u32 isRGB24)
{
  u32 uCount;
  if(isRGB24 == 0)
  {
    uCount = 32;
    const u16* src16 = (const u16*) src; 
    do{                            //B                                 //G                          //R
      dst16[ 0] = ((src16[ 0]&(0x1f001f<<10))>>10) | ((src16[ 0]&(0x1f001f<<5))<<1) | ((src16[ 0]&(0x1f001f<<0))<<11);
      dst16[ 1] = ((src16[ 3]&(0x1f001f<<10))>>10) | ((src16[ 3]&(0x1f001f<<5))<<1) | ((src16[ 3]&(0x1f001f<<0))<<11);
      dst16[ 2] = ((src16[ 6]&(0x1f001f<<10))>>10) | ((src16[ 6]&(0x1f001f<<5))<<1) | ((src16[ 6]&(0x1f001f<<0))<<11);
      dst16[ 3] = ((src16[ 9]&(0x1f001f<<10))>>10) | ((src16[ 9]&(0x1f001f<<5))<<1) | ((src16[ 9]&(0x1f001f<<0))<<11);
      dst16[ 4] = ((src16[12]&(0x1f001f<<10))>>10) | ((src16[12]&(0x1f001f<<5))<<1) | ((src16[12]&(0x1f001f<<0))<<11);
                                                                                              
      dst16[ 5] = ((src16[16]&(0x1f001f<<10))>>10) | ((src16[16]&(0x1f001f<<5))<<1) | ((src16[16]&(0x1f001f<<0))<<11);
      dst16[ 6] = ((src16[19]&(0x1f001f<<10))>>10) | ((src16[19]&(0x1f001f<<5))<<1) | ((src16[19]&(0x1f001f<<0))<<11);
      dst16[ 7] = ((src16[22]&(0x1f001f<<10))>>10) | ((src16[22]&(0x1f001f<<5))<<1) | ((src16[22]&(0x1f001f<<0))<<11);
      dst16[ 8] = ((src16[25]&(0x1f001f<<10))>>10) | ((src16[25]&(0x1f001f<<5))<<1) | ((src16[25]&(0x1f001f<<0))<<11);
      dst16[ 9] = ((src16[28]&(0x1f001f<<10))>>10) | ((src16[28]&(0x1f001f<<5))<<1) | ((src16[28]&(0x1f001f<<0))<<11);

      dst16 += 10;
      src16 += 32;
    }while(--uCount);
  }
}

#endif //_INNER_BLIT_H_

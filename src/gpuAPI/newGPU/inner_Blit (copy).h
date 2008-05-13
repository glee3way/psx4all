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

#define write2xrot16(n) dst16[pixelstart+(480*n-1)] = dst16[pixelstart+(480*n)]

#define writerot16(n) dst16[pixelstart+(480*n)]

#define bgr15torgb16(n) ((src16[n]&(0x1f001f<<10))>>10) | ((src16[n]&(0x1f001f<<5))<<1) | ((src16[n]&(0x1f001f<<0))<<11)



INLINE void GPU_Blit_512_480_ROT90CW_2X(void* src, u16* dst16, u32 isRGB24, u32 linenum)
{ 
  u32 uCount;
  if(isRGB24 == 0)
  {
    uCount = 32;
		u16* src16 = (u16*)src;
    u32 pixelstart = 479 - linenum;
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
      src16 +=  16;
    }while(--uCount);
  }
  else
  {
    uCount = 32;
    u8* src8 = (u8*)src;
    u32 pixelstart = 479 - linenum;
    do{
    	writerot16(0) = gp2x_video_RGB_color16(src8[0], src8[ 1], src8[ 2] );
    	writerot16(1) = gp2x_video_RGB_color16(src8[3], src8[ 4], src8[ 5] );
 			writerot16(3) = writerot16(2) = gp2x_video_RGB_color16(src8[6], src8[ 7], src8[ 8] );
 			writerot16(4) = gp2x_video_RGB_color16(src8[9], src8[10], src8[11] );
 			
    	writerot16(5) = gp2x_video_RGB_color16(src8[12], src8[13], src8[14] );
    	writerot16(6) = gp2x_video_RGB_color16(src8[15], src8[16], src8[17] );
 			writerot16(8) = writerot16(7) = gp2x_video_RGB_color16(src8[18], src8[19], src8[20] );
 			writerot16(9) = gp2x_video_RGB_color16(src8[21], src8[22], src8[23] );
 			
 			writerot16(10) = gp2x_video_RGB_color16(src8[24], src8[25], src8[26] );
    	writerot16(11) = gp2x_video_RGB_color16(src8[27], src8[28], src8[29] );
 			writerot16(13) = writerot16(12) = gp2x_video_RGB_color16(src8[30], src8[31], src8[32] );
 			writerot16(14) = gp2x_video_RGB_color16(src8[33], src8[34], src8[35] );
 			
 			writerot16(15) = gp2x_video_RGB_color16(src8[36], src8[37], src8[38] );
    	writerot16(16) = gp2x_video_RGB_color16(src8[39], src8[40], src8[41] );
 			writerot16(18) = writerot16(17) =gp2x_video_RGB_color16(src8[42], src8[43], src8[44] );
 			writerot16(19) = gp2x_video_RGB_color16(src8[45], src8[46], src8[47] );
 			
      pixelstart += 480*20;
      src8 +=  48;
    }while(--uCount);
  }
}

INLINE void GPU_Blit_384_480_ROT90CW_2X(void* src, u16* dst16, u32 isRGB24, u32 linenum)
{ 
  u32 uCount;
  if(isRGB24 == 0)
  {
    uCount = 32;
		u16* src16 = (u16*)src;
    u32 pixelstart = 479 - linenum;
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
      src16 +=  12;
    }while(--uCount);
  }
  else
  {
    uCount = 32;
    u8* src8 = (u8*)src;
    u32 pixelstart = 479 - linenum;
    do{
    	writerot16(1) = writerot16(0) = gp2x_video_RGB_color16(src8[0], src8[ 1], src8[ 2] );
 			writerot16(3) = writerot16(2) = gp2x_video_RGB_color16(src8[3], src8[ 4], src8[ 5] );
 			writerot16(4) = gp2x_video_RGB_color16(src8[6], src8[ 7], src8[ 8] );
 			
    	writerot16(6) = writerot16(5) = gp2x_video_RGB_color16(src8[9], src8[10], src8[11] );
 			writerot16(8) = writerot16(7) = gp2x_video_RGB_color16(src8[12], src8[13], src8[14] );
 			writerot16(9) = gp2x_video_RGB_color16(src8[15], src8[16], src8[17] );
 			
    	writerot16(11) = writerot16(10) = gp2x_video_RGB_color16(src8[18], src8[19], src8[20] );
 			writerot16(13) = writerot16(12) = gp2x_video_RGB_color16(src8[21], src8[22], src8[23] );
 			writerot16(14) = gp2x_video_RGB_color16(src8[24], src8[25], src8[26] );
 			
    	writerot16(16) = writerot16(15) = gp2x_video_RGB_color16(src8[27], src8[28], src8[29] );
 			writerot16(18) = writerot16(17) = gp2x_video_RGB_color16(src8[30], src8[31], src8[32] );
 			writerot16(19) = gp2x_video_RGB_color16(src8[33], src8[34], src8[35] );
 			
      pixelstart += 480*20;
      src8 += 36;
    }while(--uCount);
  }
}

INLINE void GPU_Blit_368_480_ROT90CW_2X(void* src, u16* dst16, u32 isRGB24, u32 linenum)
{ 
  u32 uCount;
  if(isRGB24 == 0)
  {
    uCount = 16;
		u16* src16 = (u16*)src;
    u32 pixelstart = 479 - linenum;
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
      src16 +=  23;
    }while(--uCount);
  }
  else
  {
    uCount = 16;
    u8* src8 = (u8*)src;
    u32 pixelstart = 479 - linenum;
    do{
    	writerot16(1) = writerot16(0) = gp2x_video_RGB_color16(src8[0], src8[ 1], src8[ 2] );
    	writerot16(3) = writerot16(2) = gp2x_video_RGB_color16(src8[3], src8[4], src8[ 5] );
    	writerot16(5) = writerot16(4) = gp2x_video_RGB_color16(src8[6], src8[7], src8[ 8] );
    	writerot16(6) = gp2x_video_RGB_color16(src8[9], src8[10], src8[11] );
    	writerot16(7) = gp2x_video_RGB_color16(src8[12], src8[13], src8[14] );
    	writerot16(9) =  writerot16(8) = gp2x_video_RGB_color16(src8[15], src8[16], src8[17] );
    	writerot16(11) = writerot16(10) = gp2x_video_RGB_color16(src8[18], src8[19], src8[20] );
    	writerot16(13) = writerot16(12) = gp2x_video_RGB_color16(src8[21], src8[22], src8[23] );
    	writerot16(15) = writerot16(14) = gp2x_video_RGB_color16(src8[24], src8[25], src8[26] );
    	writerot16(17) = writerot16(16) = gp2x_video_RGB_color16(src8[27], src8[28], src8[29] );
    	writerot16(18) = gp2x_video_RGB_color16(src8[30], src8[31], src8[32] );
    	writerot16(19) = gp2x_video_RGB_color16(src8[33], src8[34], src8[35] );
    	writerot16(21) = writerot16(20) = gp2x_video_RGB_color16(src8[36], src8[37], src8[38] );
    	writerot16(23) = writerot16(22) = gp2x_video_RGB_color16(src8[39], src8[40], src8[41] );
    	writerot16(25) = writerot16(24) = gp2x_video_RGB_color16(src8[42], src8[43], src8[44] );
    	writerot16(27) = writerot16(26) = gp2x_video_RGB_color16(src8[45], src8[46], src8[47] );
    	writerot16(29) = writerot16(28) = gp2x_video_RGB_color16(src8[48], src8[49], src8[50] );
    	writerot16(30) = gp2x_video_RGB_color16(src8[51], src8[52], src8[53] );
    	writerot16(31) = gp2x_video_RGB_color16(src8[54], src8[55], src8[56] );
    	writerot16(33) = writerot16(32) = gp2x_video_RGB_color16(src8[57], src8[58], src8[59] );
    	writerot16(35) = writerot16(34) = gp2x_video_RGB_color16(src8[60], src8[61], src8[62] );
    	writerot16(37) = writerot16(36) = gp2x_video_RGB_color16(src8[63], src8[64], src8[65] );
    	writerot16(39) = writerot16(38) = gp2x_video_RGB_color16(src8[66], src8[67], src8[68] );

      pixelstart += 480*40;
      src8  += 69;
    }while(--uCount);
  }
}

INLINE void GPU_Blit_256_480_ROT90CW_2X(void* src, u16* dst16, u32 isRGB24, u32 linenum)
{ 
  u32 uCount;
  if(isRGB24 == 0)
  {
    uCount = 32;
		u16* src16 = (u16*)src;
    u32 pixelstart = 479 - linenum;
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
      src16 +=  8;
    }while(--uCount);
  }
  else
  {
    uCount = 32;
    u8* src8 = (u8*)src;
    u32 pixelstart = 479 - linenum;
    do{
    	writerot16(1) = writerot16(0) = gp2x_video_RGB_color16(src8[0], src8[ 1], src8[ 2] );
      writerot16(3) = writerot16(2) = gp2x_video_RGB_color16(src8[3], src8[ 4], src8[ 5] );
      writerot16(7) = writerot16(6) = writerot16(5) = writerot16(4) = gp2x_video_RGB_color16(src8[6], src8[ 7], src8[ 8] );
      writerot16(9) = writerot16(8) = gp2x_video_RGB_color16(src8[9], src8[10], src8[11] );
      writerot16(11) = writerot16(10) = gp2x_video_RGB_color16(src8[12], src8[13], src8[14] );
      writerot16(15) = writerot16(14) = writerot16(13) = writerot16(12) = gp2x_video_RGB_color16(src8[15], src8[16], src8[17] );
      writerot16(17) = writerot16(16) = gp2x_video_RGB_color16(src8[18], src8[19], src8[20] );
      writerot16(19) = writerot16(18) = gp2x_video_RGB_color16(src8[21], src8[22], src8[23] );

      pixelstart += 480*20;
      src8  += 24;
    }while(--uCount);
  }
}

INLINE void GPU_Blit_320_480_ROT90CW_2X(void* src, u16* dst16, u32 isRGB24, u32 linenum)
{
  u32 uCount;
  if(isRGB24 == 0)
  {
    uCount = 32;
		u16* src16 = (u16*)src;
    u32 pixelstart = 479 - linenum;
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
      src16 +=  10;
    }while(--uCount);
  }
  else
  {
    uCount = 32;
    u8* src8 = (u8*)src;
    u32 pixelstart = 479 - linenum;
    do{

    	writerot16(1) = writerot16(0) = gp2x_video_RGB_color16(src8[0], src8[ 1], src8[ 2] );
      writerot16(3) = writerot16(2) = gp2x_video_RGB_color16(src8[3], src8[ 4], src8[ 5] );
      writerot16(5) = writerot16(4) = gp2x_video_RGB_color16(src8[6], src8[ 7], src8[ 8] );
      writerot16(7) = writerot16(6) = gp2x_video_RGB_color16(src8[9], src8[10], src8[11] );
      writerot16(9) = writerot16(8) = gp2x_video_RGB_color16(src8[12], src8[13], src8[14] );
      writerot16(11) = writerot16(10) = gp2x_video_RGB_color16(src8[15], src8[16], src8[17] );
      writerot16(13) = writerot16(12) = gp2x_video_RGB_color16(src8[18], src8[19], src8[20] );
      writerot16(15) = writerot16(14) = gp2x_video_RGB_color16(src8[21], src8[22], src8[23] );
      writerot16(17) = writerot16(16) = gp2x_video_RGB_color16(src8[24], src8[25], src8[26] );
      writerot16(19) = writerot16(18) = gp2x_video_RGB_color16(src8[27], src8[28], src8[29] );

      pixelstart += 480*20;
      src8  += 30;
    }while(--uCount);
  }
}

INLINE void GPU_Blit_640_480_ROT90CW(void* src, u16* dst16, u32 isRGB24, u32 linenum)
{
  u32 ucount;
  if(isRGB24 == 0)
  {
  	u16* src16 = (u16*)src;
    u32 pixelstart = 479 - linenum;
    for(ucount = 0; ucount < 640; ucount++)
    {
      dst16[pixelstart] = ((src16[0]&(0x1f001f<<10))>>10) | ((src16[0]&(0x1f001f<<5))<<1) | ((src16[0]&(0x1f001f<<0))<<11);
      pixelstart += 480;
      src16++;
    }
  }
	else
  {
  	u8* src8 = (u8*)src;
    u32 pixelstart = 479 - linenum;
    for(ucount = 0; ucount < 640; ucount++)
    {
      dst16[pixelstart] = gp2x_video_RGB_color16(src8[ 0], src8[ 1], src8[ 2] );
      pixelstart += 480;
      src8 += 3;
    }
  }
}


INLINE void GPU_Blit_512_240_ROT90CW_2X(void* src, u16* dst16, u32 isRGB24, u32 linenum)
{ 
  u32 uCount;
  if(isRGB24 == 0)
  {
    uCount = 32;
		u16* src16 = (u16*)src;
    u32 pixelstart = 479 - (linenum<<1);
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
      src16 +=  16;
    }while(--uCount);
  }
  else
  {
    uCount = 32;
    u8* src8 = (u8*)src;
    u32 pixelstart = 479 - (linenum<<1);
    do{
    	write2xrot16(0) = gp2x_video_RGB_color16(src8[0], src8[ 1], src8[ 2] );
    	write2xrot16(1) = gp2x_video_RGB_color16(src8[3], src8[ 4], src8[ 5] );
 			write2xrot16(3) = write2xrot16(2) = gp2x_video_RGB_color16(src8[6], src8[ 7], src8[ 8] );
 			write2xrot16(4) = gp2x_video_RGB_color16(src8[9], src8[10], src8[11] );
 			
    	write2xrot16(5) = gp2x_video_RGB_color16(src8[12], src8[13], src8[14] );
    	write2xrot16(6) = gp2x_video_RGB_color16(src8[15], src8[16], src8[17] );
 			write2xrot16(8) = write2xrot16(7) = gp2x_video_RGB_color16(src8[18], src8[19], src8[20] );
 			write2xrot16(9) = gp2x_video_RGB_color16(src8[21], src8[22], src8[23] );
 			
 			write2xrot16(10) = gp2x_video_RGB_color16(src8[24], src8[25], src8[26] );
    	write2xrot16(11) = gp2x_video_RGB_color16(src8[27], src8[28], src8[29] );
 			write2xrot16(13) = write2xrot16(12) = gp2x_video_RGB_color16(src8[30], src8[31], src8[32] );
 			write2xrot16(14) = gp2x_video_RGB_color16(src8[33], src8[34], src8[35] );
 			
 			write2xrot16(15) = gp2x_video_RGB_color16(src8[36], src8[37], src8[38] );
    	write2xrot16(16) = gp2x_video_RGB_color16(src8[39], src8[40], src8[41] );
 			write2xrot16(18) = write2xrot16(17) =gp2x_video_RGB_color16(src8[42], src8[43], src8[44] );
 			write2xrot16(19) = gp2x_video_RGB_color16(src8[45], src8[46], src8[47] );
 			
      pixelstart += 480*20;
      src8 +=  48;
    }while(--uCount);
  }
}

INLINE void GPU_Blit_384_240_ROT90CW_2X(void* src, u16* dst16, u32 isRGB24, u32 linenum)
{ 
  u32 uCount;
  if(isRGB24 == 0)
  {
    uCount = 32;
		u16* src16 = (u16*)src;
    u32 pixelstart = 479 - (linenum<<1);
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
      src16 +=  12;
    }while(--uCount);
  }
  else
  {
    uCount = 32;
    u8* src8 = (u8*)src;
    u32 pixelstart = 479 - (linenum<<1);
    do{
    	write2xrot16(1) = write2xrot16(0) = gp2x_video_RGB_color16(src8[0], src8[ 1], src8[ 2] );
 			write2xrot16(3) = write2xrot16(2) = gp2x_video_RGB_color16(src8[3], src8[ 4], src8[ 5] );
 			write2xrot16(4) = gp2x_video_RGB_color16(src8[6], src8[ 7], src8[ 8] );
 			
    	write2xrot16(6) = write2xrot16(5) = gp2x_video_RGB_color16(src8[9], src8[10], src8[11] );
 			write2xrot16(8) = write2xrot16(7) = gp2x_video_RGB_color16(src8[12], src8[13], src8[14] );
 			write2xrot16(9) = gp2x_video_RGB_color16(src8[15], src8[16], src8[17] );
 			
    	write2xrot16(11) = write2xrot16(10) = gp2x_video_RGB_color16(src8[18], src8[19], src8[20] );
 			write2xrot16(13) = write2xrot16(12) = gp2x_video_RGB_color16(src8[21], src8[22], src8[23] );
 			write2xrot16(14) = gp2x_video_RGB_color16(src8[24], src8[25], src8[26] );
 			
    	write2xrot16(16) = write2xrot16(15) = gp2x_video_RGB_color16(src8[27], src8[28], src8[29] );
 			write2xrot16(18) = write2xrot16(17) = gp2x_video_RGB_color16(src8[30], src8[31], src8[32] );
 			write2xrot16(19) = gp2x_video_RGB_color16(src8[33], src8[34], src8[35] );
 			
      pixelstart += 480*20;
      src8 += 36;
    }while(--uCount);
  }
}

INLINE void GPU_Blit_368_240_ROT90CW_2X(void* src, u16* dst16, u32 isRGB24, u32 linenum)
{ 
  u32 uCount;
  if(isRGB24 == 0)
  {
    uCount = 16;
		u16* src16 = (u16*)src;
    u32 pixelstart = 479 - (linenum<<1);
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
      src16 +=  23;
    }while(--uCount);
  }
  else
  {
    uCount = 16;
    u8* src8 = (u8*)src;
    u32 pixelstart = 479 - (linenum<<1);
    do{
    	write2xrot16(1) = write2xrot16(0) = gp2x_video_RGB_color16(src8[0], src8[ 1], src8[ 2] );
    	write2xrot16(3) = write2xrot16(2) = gp2x_video_RGB_color16(src8[3], src8[4], src8[ 5] );
    	write2xrot16(5) = write2xrot16(4) = gp2x_video_RGB_color16(src8[6], src8[7], src8[ 8] );
    	write2xrot16(6) = gp2x_video_RGB_color16(src8[9], src8[10], src8[11] );
    	write2xrot16(7) = gp2x_video_RGB_color16(src8[12], src8[13], src8[14] );
    	write2xrot16(9) =  write2xrot16(8) = gp2x_video_RGB_color16(src8[15], src8[16], src8[17] );
    	write2xrot16(11) = write2xrot16(10) = gp2x_video_RGB_color16(src8[18], src8[19], src8[20] );
    	write2xrot16(13) = write2xrot16(12) = gp2x_video_RGB_color16(src8[21], src8[22], src8[23] );
    	write2xrot16(15) = write2xrot16(14) = gp2x_video_RGB_color16(src8[24], src8[25], src8[26] );
    	write2xrot16(17) = write2xrot16(16) = gp2x_video_RGB_color16(src8[27], src8[28], src8[29] );
    	write2xrot16(18) = gp2x_video_RGB_color16(src8[30], src8[31], src8[32] );
    	write2xrot16(19) = gp2x_video_RGB_color16(src8[33], src8[34], src8[35] );
    	write2xrot16(21) = write2xrot16(20) = gp2x_video_RGB_color16(src8[36], src8[37], src8[38] );
    	write2xrot16(23) = write2xrot16(22) = gp2x_video_RGB_color16(src8[39], src8[40], src8[41] );
    	write2xrot16(25) = write2xrot16(24) = gp2x_video_RGB_color16(src8[42], src8[43], src8[44] );
    	write2xrot16(27) = write2xrot16(26) = gp2x_video_RGB_color16(src8[45], src8[46], src8[47] );
    	write2xrot16(29) = write2xrot16(28) = gp2x_video_RGB_color16(src8[48], src8[49], src8[50] );
    	write2xrot16(30) = gp2x_video_RGB_color16(src8[51], src8[52], src8[53] );
    	write2xrot16(31) = gp2x_video_RGB_color16(src8[54], src8[55], src8[56] );
    	write2xrot16(33) = write2xrot16(32) = gp2x_video_RGB_color16(src8[57], src8[58], src8[59] );
    	write2xrot16(35) = write2xrot16(34) = gp2x_video_RGB_color16(src8[60], src8[61], src8[62] );
    	write2xrot16(37) = write2xrot16(36) = gp2x_video_RGB_color16(src8[63], src8[64], src8[65] );
    	write2xrot16(39) = write2xrot16(38) = gp2x_video_RGB_color16(src8[66], src8[67], src8[68] );

      pixelstart += 480*40;
      src8  += 69;
    }while(--uCount);
  }
}

INLINE void GPU_Blit_256_240_ROT90CW_2X(void* src, u16* dst16, u32 isRGB24, u32 linenum)
{ 
  u32 uCount;
  if(isRGB24 == 0)
  {
    uCount = 32;
		u16* src16 = (u16*)src;
    u32 pixelstart = 479 - (linenum<<1);
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
      src16 +=  8;
    }while(--uCount);
  }
  else
  {
    uCount = 32;
    u8* src8 = (u8*)src;
    u32 pixelstart = 479 - (linenum<<1);
    do{
    	write2xrot16(1) = write2xrot16(0) = gp2x_video_RGB_color16(src8[0], src8[ 1], src8[ 2] );
      write2xrot16(3) = write2xrot16(2) = gp2x_video_RGB_color16(src8[3], src8[ 4], src8[ 5] );
      write2xrot16(7) = write2xrot16(6) = write2xrot16(5) = write2xrot16(4) = gp2x_video_RGB_color16(src8[6], src8[ 7], src8[ 8] );
      write2xrot16(9) = write2xrot16(8) = gp2x_video_RGB_color16(src8[9], src8[10], src8[11] );
      write2xrot16(11) = write2xrot16(10) = gp2x_video_RGB_color16(src8[12], src8[13], src8[14] );
      write2xrot16(15) = write2xrot16(14) = write2xrot16(13) = write2xrot16(12) = gp2x_video_RGB_color16(src8[15], src8[16], src8[17] );
      write2xrot16(17) = write2xrot16(16) = gp2x_video_RGB_color16(src8[18], src8[19], src8[20] );
      write2xrot16(19) = write2xrot16(18) = gp2x_video_RGB_color16(src8[21], src8[22], src8[23] );

      pixelstart += 480*20;
      src8  += 24;
    }while(--uCount);
  }
}

INLINE void GPU_Blit_320_240_ROT90CW_2X(void* src, u16* dst16, u32 isRGB24, u32 linenum)
{
  u32 uCount;
  if(isRGB24 == 0)
  {
    uCount = 32;
		u16* src16 = (u16*)src;
    u32 pixelstart = 479 - (linenum<<1);
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
      src16 +=  10;
    }while(--uCount);
  }
  else
  {
    uCount = 32;
    u8* src8 = (u8*)src;
    u32 pixelstart = 479 - (linenum<<1);
    do{

    	write2xrot16(1) = write2xrot16(0) = gp2x_video_RGB_color16(src8[0], src8[ 1], src8[ 2] );
      write2xrot16(3) = write2xrot16(2) = gp2x_video_RGB_color16(src8[3], src8[ 4], src8[ 5] );
      write2xrot16(5) = write2xrot16(4) = gp2x_video_RGB_color16(src8[6], src8[ 7], src8[ 8] );
      write2xrot16(7) = write2xrot16(6) = gp2x_video_RGB_color16(src8[9], src8[10], src8[11] );
      write2xrot16(9) = write2xrot16(8) = gp2x_video_RGB_color16(src8[12], src8[13], src8[14] );
      write2xrot16(11) = write2xrot16(10) = gp2x_video_RGB_color16(src8[15], src8[16], src8[17] );
      write2xrot16(13) = write2xrot16(12) = gp2x_video_RGB_color16(src8[18], src8[19], src8[20] );
      write2xrot16(15) = write2xrot16(14) = gp2x_video_RGB_color16(src8[21], src8[22], src8[23] );
      write2xrot16(17) = write2xrot16(16) = gp2x_video_RGB_color16(src8[24], src8[25], src8[26] );
      write2xrot16(19) = write2xrot16(18) = gp2x_video_RGB_color16(src8[27], src8[28], src8[29] );

      pixelstart += 480*20;
      src8  += 30;
    }while(--uCount);
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

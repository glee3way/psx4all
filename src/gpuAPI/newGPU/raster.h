#ifndef RASTER_H
#define RASTER_H
///////////////////////////////////////////////////////////////////////////////
//  GPU Render Primitives


//  Images
void gpuLoadImage   (void);
void gpuStoreImage  (void);
void gpuMoveImage   (void);
void gpuClearImage  (void);

//  Lines
void gpuDrawLF(void);
void gpuDrawLG(void);

//  Sprite
void gpuDrawS(void);
void gpuDrawT(void);


//  Poligon
void  gpuDrawF3 ();
void  gpuDrawFT3();
void  gpuDrawG3 ();
void  gpuDrawGT3();

///////////////////////////////////////////////////////////////////////////////
//  GPU Raster Macros
#define	GPU_RGB16(rgb)        ((((rgb)&0xF80000)>>9)|(((rgb)&0xF800)>>6)|(((rgb)&0xF8)>>3))

#define GPU_EXPANDSIGN_POLY(x)  (((s32)(x)<<20)>>20)
#define GPU_EXPANDSIGN_SPRT(x)  (((s32)(x)<<21)>>21)

#define	GPU_TESTRANGE(x)      { if((u32)(x+1024) > 2047) return; }

#define	GPU_SWAP(a,b,t)	{(t)=(a);(a)=(b);(b)=(t);}

///////////////////////////////////////////////////////////////////////////////
INLINE  void  AddDirtyArea(u32 x0, u32 y0, u32 x1, u32 y1)
{
#if 0
	isSkip[1] = 0;

	if( isSkip[0] == 0 ) 
	{
		return; 
	}

	u32 xmin = DisplayArea[0];  u32 xmax = xmin + DisplayArea[2];
	u32 ymin = DisplayArea[1];  u32 ymax = ymin + ( (DisplayArea[7]-DisplayArea[5]) << ( (DisplayArea[3]==480)?1:0) );
	
	if( (	( (x0 < xmin && x1 < xmin) || (x0 > xmax && x1 > xmax) ) &&
			( (y0 < ymin && y1 < ymin) || (y0 > ymax && y1 > ymax) ) )	)
	{
		isSkip[1] = 1; 
		return;
	}

	//if( (x1 < xmin || x0 > xmax) && ((y1 < ymin || y0 > ymax) || (y0 < ymin && y1 > ymax)) ) { isSkip[1] = 0; return; }
	//if( (x0 < xmin && x1 > xmax) && ((y1 < ymin || y0 > ymax) || (y0 < ymin && y1 > ymax)) ) { isSkip[1] = 0; return; }
#endif
}

///////////////////////////////////////////////////////////////////////////////
#endif  //RASTER_H

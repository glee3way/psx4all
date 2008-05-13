#ifndef _INNER_POLY_H_
#define _INNER_POLY_H_
///////////////////////////////////////////////////////////////////////////////
//  GPU Polygon innerloops generator
#include  "op_Texture.h"
#include  "op_Light.h"
#include  "op_Blend.h"


///////////////////////////////////////////////////////////////////////////////
#define gpuPolySpanFn gpuPolySpanFn_Ex
//#define gpuPolySpanFn gpuPolySpanFn_Std

///////////////////////////////////////////////////////////////////////////////
template<int CF>
void  gpuPolySpanFn_Std()
{
//PROFILE_FENCE(gpuPixelTime,gpuPixelCount);

  u32 u,v;
  u32 mu,mv;
  u32 r,g,b;
  u16 uDst;
  u16 uSrc;

  if(TM)       { u=u4;  v=v4;  mu=MaskU; mv=MaskV; }

  if(L && !G)  { r = r4>>3; g = g4>>3;  b = b4>>3; } else
  if(L &&  G)  { r = r4;    g = g4;     b = b4;    }

  u16* pDst = Pixel;
  u16* pEnd = PixelEnd;
  for(;pDst<pEnd;++pDst,(TM?u+=du4,v+=dv4:1),(L&&G?r+=dr4,g+=dg4,b+=db4:1))
  {
    //  MASKING
    if(M) { uDst = *pDst;  if (uDst&0x8000) continue;  }

    //  TEXTURE MAPPING
	switch(TM)
	{
	case (1): { uSrc = gpuTextureMappingW0(x2i(u)&mu,x2i(v)&mv);  if(!uSrc)  continue; }  break;
	case (2): { uSrc = gpuTextureMappingW1(x2i(u)&mu,x2i(v)&mv);  if(!uSrc)  continue; }  break;
	case (3): { uSrc = gpuTextureMappingW2(x2i(u)&mu,x2i(v)&mv);  if(!uSrc)  continue; }  break;
	default:  { uSrc = PixelData;                                                      }  break;
	}
    //  BLEND
    if( B && ( (!TM) || (uSrc&0x8000) ) )
    {
      //  LIGHTING CALCULATIONS
      if(L && !G && !TM)  { uSrc = gpuLightingRGB(     b,g,r);                          } else
      if(L && !G &&  TM)  { uSrc = gpuLightingTXT(uSrc,b,g,r);                          } else
      if(L &&  G && !TM)  { uSrc = gpuLightingRGB(     x2i(b>>3),x2i(g>>3),x2i(r>>3));  } else
      if(L &&  G &&  TM)  { uSrc = gpuLightingTXT(uSrc,x2i(b>>3),x2i(g>>3),x2i(r>>3));  }

      if(!M)    { uDst = *pDst; }
	  switch(BM)
	  {
	  case (0): { uSrc = gpuBlending00(uDst, uSrc);  } break;
	  case (1): { uSrc = gpuBlending01(uDst, uSrc);  } break;
	  case (2): { uSrc = gpuBlending02(uDst, uSrc);  } break;
	  case (3): { uSrc = gpuBlending03(uDst, uSrc);  } break;
      }
	}
    else
    {
      //  LIGHTING CALCULATIONS
      if(L && !G && !TM)  { uSrc = gpuLightingRGB(     b,g,r);                          } else
      if(L && !G &&  TM)  { uSrc = gpuLightingTXT(uSrc,b,g,r);                          } else
      if(L &&  G && !TM)  { uSrc = gpuLightingRGB(     x2i(b>>3),x2i(g>>3),x2i(r>>3));  } else
      if(L &&  G &&  TM)  { uSrc = gpuLightingTXT(uSrc,x2i(b>>3),x2i(g>>3),x2i(r>>3));  } else
      if(            TM)  { uSrc&= 0x7fff;                                              }
    }

    *pDst = uSrc | PixelMSB;
  }
}

///////////////////////////////////////////////////////////////////////////////
template<int CF>
void  gpuPolySpanFn_ExOLD()
{
//PROFILE_FENCE(gpuPixelTime,gpuPixelCount);

  u16 uDst;
  u16 uSrc;

  u32 tCor;//, tInc, tMsk;
  u32 lCol;//, lInc;

  if(TM)
  {
    tCor = ((u32)( u4<<7)&0x7fff0000) | ((u32)( v4>>9)&0x00007fff);
    tCor&= tMsk;
    /*
    tInc = ((u32)(du4<<7)&0x7fff0000) | ((u32)(dv4>>9)&0x00007fff);
    tMsk = (MaskU<<23) | (MaskV<<7) | 0x00ff00ff;
    */
  }

  if(L && !G)
  {
    lCol = ((u32)(b4<< 2)&(0x03ff)) | ((u32)(g4<<13)&(0x07ff<<10)) | ((u32)(r4<<24)&(0x07ff<<21));
  }
  else
  if(L && G)
  {
    /*    working 01
    u32 dr = (u32(dr4<< 8)&(0x07ff<<22));
    u32 dg = (u32(dg4>> 3)&(0x07ff<<11));
    u32 db = (u32(db4>>14)&(0x07ff    ));
    if(dr4<0) dr+= 1<<22;
    if(dg4<0) dg+= 1<<11;
    if(db4<0) db+= 1<< 0;
    lMsk =~((1<<10) | (1<<21));
    lCol = (u32( b4>>14)&(0x03ff)) | (u32( g4>> 3)&(0x03ff<<11)) | (u32( r4<< 8)&(0x03ff<<22));
    lInc = (db | dg | dr) & lMsk;
    */

    /*  workin 02
    u32 dr = (u32(dr4<< 8)&(0x03ff<<22));
    u32 dg = (u32(dg4>> 3)&(0x03ff<<11));
    u32 db = (u32(db4>>14)&(0x03ff    ));
    if(dr4<0) dr+= 1<<22;
    if(dg4<0) dg+= 1<<11;
    if(db4<0) db+= 1<< 0;

    lMsk =~((1<<10) | (1<<21));
    lCol = (u32( b4>>14)&(0x03ff)) | (u32( g4>> 3)&(0x03ff<<11)) | (u32( r4<< 8)&(0x03ff<<22));
    lInc = (db | dg | dr) & lMsk;
    */

    /*  fine
    u32 dr = u32(dr4<< 8)&(0xffffffff<<21);   if(dr4<0) dr+= 1<<21;
    u32 dg = u32(dg4>> 3)&(0xffffffff<<10);   if(dg4<0) dg+= 1<<10;
    u32 db = u32(db4>>14)&(0xffffffff    );   if(db4<0) db+= 1<< 0;
    lCol = (u32(b4>>14)&(0x03ff)) | (u32(g4>>3)&(0x07ff<<10)) | (u32(r4<<8)&(0x07ff<<21));
    lInc = db + dg + dr;
    */

    lCol = ((u32)(b4>>14)&(0x03ff)) | ((u32)(g4>>3)&(0x07ff<<10)) | ((u32)(r4<<8)&(0x07ff<<21));
    /*
    u32 dr = u32(dr4<< 8)&(0xffffffff<<21);   if(dr4<0) dr+= 1<<21;
    u32 dg = u32(dg4>> 3)&(0xffffffff<<10);   if(dg4<0) dg+= 1<<10;
    u32 db = u32(db4>>14)&(0xffffffff    );   if(db4<0) db+= 1<< 0;
    lInc = db + dg + dr;
    */
  }

  u16* pDst = Pixel;
  u16* pEnd = PixelEnd;
//for(;pDst<pEnd;++pDst, (TM?(tCor=(tCor+tInc)&tMsk):1), (L&&G?(lCol=(lCol+lInc)&lMsk):1) )
  for(;pDst<pEnd;++pDst, (TM?(tCor=(tCor+tInc)&tMsk):1), (L&&G?(lCol=(lCol+lInc)):1) )
  {
    //  MASKING
    if(M) { uDst = *pDst;  if (uDst&0x8000) continue;  }

    //  TEXTURE MAPPING
	switch(TM)
	{
	case (1): { uSrc = gpuTextureMappingW0_Ex(tCor);    if(!uSrc)  continue; }  break;
	case (2): { uSrc = gpuTextureMappingW1_Ex(tCor);    if(!uSrc)  continue; }  break;
	case (3): { uSrc = gpuTextureMappingW2_Ex(tCor);    if(!uSrc)  continue; }  break;
	default:  { uSrc = PixelData;                                            }  break;
	}

    //  BLEND
    if( B && ( (!TM) || (uSrc&0x8000) ) )
    {
      //  LIGHTING CALCULATIONS
      if(L && !TM)  { uSrc = gpuLightingRGB_Ex(      lCol); } else
      if(L &&  TM)  { uSrc = gpuLightingTXT_Ex(uSrc, lCol); }

      if(!M)    { uDst = *pDst; }
	  switch(BM)
	  {
	  case (0): { uSrc = gpuBlending00(uDst, uSrc);  } break;
	  case (1): { uSrc = gpuBlending01(uDst, uSrc);  } break;
	  case (2): { uSrc = gpuBlending02(uDst, uSrc);  } break;
	  case (3): { uSrc = gpuBlending03(uDst, uSrc);  } break;
      }
	}
    else
    {
      //  LIGHTING CALCULATIONS
      if(L && !TM)  { uSrc = gpuLightingRGB_Ex(      lCol); } else
      if(L &&  TM)  { uSrc = gpuLightingTXT_Ex(uSrc, lCol); } else
      if(      TM)  { uSrc&= 0x7fff;                        }
  }

    *pDst = uSrc | PixelMSB;
  }
}

///////////////////////////////////////////////////////////////////////////////
template<int CF>
void  gpuPolySpanFn_Ex()
{
//PROFILE_FENCE(gpuPixelTime,gpuPixelCount);

  u16 uDst;
  u16 uSrc;

  u32 tCor;
  u32 lCol;

  if(TM)
  {
    tCor = ((u32)( u4<<7)&0x7fff0000) | ((u32)( v4>>9)&0x00007fff);
    tCor&= tMsk;
  }

  if(L && !G)
  {
    lCol = ((u32)(b4<< 2)&(0x03ff)) | ((u32)(g4<<13)&(0x07ff<<10)) | ((u32)(r4<<24)&(0x07ff<<21));
  }
  else
  if(L && G)
  {
    lCol = ((u32)(b4>>14)&(0x03ff)) | ((u32)(g4>>3)&(0x07ff<<10)) | ((u32)(r4<<8)&(0x07ff<<21));
  }

  u16* pDst = Pixel;
  u16* pEnd = PixelEnd;
  for(;pDst<pEnd;++pDst, (TM?(tCor=(tCor+tInc)&tMsk):1), (L&&G?(lCol=(lCol+lInc)):1) )
  {
    //  MASKING
    if(M) { uDst = *pDst;  if (uDst&0x8000) continue;  }

    //  TEXTURE MAPPING
	switch(TM)
	{
	case (1): { uSrc = gpuTextureMappingW0_Ex(tCor);    if(!uSrc)  continue; }  break;
	case (2): { uSrc = gpuTextureMappingW1_Ex(tCor);    if(!uSrc)  continue; }  break;
	case (3): { uSrc = gpuTextureMappingW2_Ex(tCor);    if(!uSrc)  continue; }  break;
	default:  { uSrc = PixelData;                                            }  break;
	}
    //  BLEND
    if( B && ( (!TM) || (uSrc&0x8000) ) )
    {
      //  LIGHTING CALCULATIONS
      if(L && !TM)  { uSrc = gpuLightingRGB_Ex(      lCol); } else
      if(L &&  TM)  { uSrc = gpuLightingTXT_Ex(uSrc, lCol); }

      if(!M)    { uDst = *pDst; }
	  switch(BM)
	  {
	  case (0): { uSrc = gpuBlending00(uDst, uSrc);  } break;
	  case (1): { uSrc = gpuBlending01(uDst, uSrc);  } break;
	  case (2): { uSrc = gpuBlending02(uDst, uSrc);  } break;
	  case (3): { uSrc = gpuBlending03(uDst, uSrc);  } break;
      }
	}
    else
    {
      //  LIGHTING CALCULATIONS
      if(L && !TM)  { uSrc = gpuLightingRGB_Ex(      lCol); } else
      if(L &&  TM)  { uSrc = gpuLightingTXT_Ex(uSrc, lCol); } else
      if(      TM)  { uSrc&= 0x7fff;                        }
    }

    *pDst = uSrc | PixelMSB;
  }
}

///////////////////////////////////////////////////////////////////////////////
#endif  //_INNER_POLY_H_

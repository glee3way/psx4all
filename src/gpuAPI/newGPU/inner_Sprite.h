#ifndef _INNER_SPRITE_H_
#define _INNER_SPRITE_H_
///////////////////////////////////////////////////////////////////////////////
//  GPU Sprites innerloops generator
#include  "op_Light.h"
#include  "op_Blend.h"

///////////////////////////////////////////////////////////////////////////////
#define gpuSpriteSpanFn gpuSpriteSpanFn_Ex
//#define gpuSpriteSpanFn gpuSpriteSpanFn_Std

#if 0
///////////////////////////////////////////////////////////////////////////////
template<int CF>
INLINE  const u8*  BeginSprite()
{
  switch(TM)
  {
  case (1): { return    &((u8*)TBA)[FRAME_OFFSET(0 , v4<<1)];  } break;
  case (2): { return    &((u8*)TBA)[FRAME_OFFSET(0 , v4<<1)];  } break;
  case (3): { return      (u8*)&TBA[FRAME_OFFSET(0 , v4)   ];  } break;
  }
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
template<int CF>
INLINE  u16 StepSprite(const u8* pSrc)
{
  /*
  if(TM==1) { u8 rgb = pSrc[(u4&MaskU)>>1];         return CBA[    (u4++)&1 ? (rgb>>4) : (rgb&0xf) ];   } else
  if(TM==2) {                                       return CBA[pSrc[u4++]];                             } else
  if(TM==3) { u16 r= ((const u16*)pSrc)[u4&MaskU];                  ++u4;   return r;                   } else
  return 0;
  */
  /**/
  u4 &= MaskU;
  switch(TM)
  {
  case (1): { u8  c = pSrc[u4>>1];              return CBA[    (u4++)&1 ? (c>>4) : (c&0xf) ];   } break;
  case (2): {                                   return CBA[pSrc[u4++]];                         } break;
  case (3): { u16 c = ((const u16*)pSrc)[u4++]; return c;                                       } break;
  }
  return 0;
  /**/
}
#endif

#define BeginSprite()															\
{																				\
  switch(TM)																	\
  {																				\
  case (1): { pTxt =     &((u8*)TBA)[FRAME_OFFSET(0 , v4<<1)];  } break;		\
  case (2): { pTxt =     &((u8*)TBA)[FRAME_OFFSET(0 , v4<<1)];  } break;		\
  case (3): { pTxt =       (u8*)&TBA[FRAME_OFFSET(0 , v4)   ];  } break;		\
  default:  { pTxt =										0;  } break;		\
  }																				\
}

#define StepSprite(pSrc)																						\
{																												\
  u4 &= MaskU;																									\
  switch(TM)																									\
  {																												\
  case (1): { u8  c = pSrc[u4>>1];              uSrc =  CBA[    (u4++)&1 ? (c>>4) : (c&0xf) ];   } break;		\
  case (2): {                                   uSrc =  CBA[pSrc[u4++]];                         } break;		\
  case (3): { uSrc = ((const u16*)pSrc)[u4++];				                                     } break;		\
  default:  {									uSrc =	0;										 } break;		\
  }																												\
}

///////////////////////////////////////////////////////////////////////////////
template<int CF>
static INLINE  void  gpuSpriteSpanFn_Std()
{
  u16 uSrc;
  u16 uDst;
  u32 r,g,b;
  const u8* pTxt;

  if(TM)  { BeginSprite(); }
  if( L)  { r = (u8)(r4>>3);    g = (u8)(g4>>3);    b = (u8)(b4>>3); }

  u16* pDst = Pixel;
  u16* pEnd = PixelEnd;
  for(;pDst<pEnd;++pDst)
  {
    //  TEXTURE MAPPING
    if(TM)  { StepSprite(pTxt);  }  else
            { uSrc = PixelData;             }

    //  MASKING
    if(M)   { uDst = *pDst;   if (uDst&0x8000) continue;  }

    //  BLEND
    if( B && ( (!TM) || (uSrc&0x8000) ) )
    {
      //  LIGHTING CALCULATIONS
      if(L && !TM)  { uSrc = gpuLightingRGB(     b,g,r);  } else
      if(L &&  TM)    uSrc = gpuLightingTXT(uSrc,b,g,r);

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
      if(L && !TM)  { uSrc = gpuLightingRGB(     b,g,r);  } else
      if(L &&  TM)  { uSrc = gpuLightingTXT(uSrc,b,g,r);  } else
      if(      TM)  { uSrc&= 0x7fff;                      }
    }

    *pDst = uSrc | PixelMSB;
  }
}

///////////////////////////////////////////////////////////////////////////////
template<int CF>
static INLINE  void  gpuSpriteSpanFn_Ex()
{
  u16 uSrc;
  u16 uDst;
  u32 lCol;
  const u8* pTxt;

  if(TM)  { BeginSprite(); }
  if( L)  { lCol = ((u32)(b4<< 2)&(0x03ff)) | ((u32)(g4<<13)&(0x07ff<<10)) | ((u32)(r4<<24)&(0x07ff<<21));  }

  u16* pDst = Pixel;
  u16* pEnd = PixelEnd;
  for(;pDst<pEnd;++pDst)
  {
    //  TEXTURE MAPPING
    if(TM)  { StepSprite(pTxt);  if(!enableAbbeyHack && !uSrc) continue; }  else
            { uSrc = PixelData;                                                     }


    //  MASKING
    if(M)   { uDst = *pDst;   if (uDst&0x8000) continue;  }

    //  BLEND
    if( B && ( (!TM) || (uSrc&0x8000) ) )
    {
      //  LIGHTING CALCULATIONS
      if(L && !TM)  { uSrc = gpuLightingRGB_Ex(      lCol);   } else
      if(L &&  TM)  { uSrc = gpuLightingTXT_Ex(uSrc, lCol);   }

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
      if(L && !TM)  { uSrc = gpuLightingRGB_Ex(      lCol);   } else
      if(L &&  TM)  { uSrc = gpuLightingTXT_Ex(uSrc, lCol);   } else
      if(      TM)  { uSrc&= 0x7fff;                          }
    }

    *pDst = uSrc | PixelMSB;
  }
}

///////////////////////////////////////////////////////////////////////////////
#endif  //_INNER_SPRITE_H_



#ifndef _INNER_PIXEL_H_
#define _INNER_PIXEL_H_
///////////////////////////////////////////////////////////////////////////////
//  GPU Pixel opperations generator

//  Aux Functions
#include "op_Blend.h"

//  PixelFn
template<int CF>
INLINE void gpuPixelFn()
{
  u16 uDst;
  u16 uSrc = PixelData;

  //  MASKING
  if(M) { uDst = *Pixel;  if (uDst&0x8000) return;  }

  //  BLEND
  if(B)
  {
    if(!M)    { uDst = *Pixel; }
	switch(BM)
	{
	case (0): { uSrc = gpuBlending00(uDst, uSrc);  } break;
	case (1): { uSrc = gpuBlending01(uDst, uSrc);  } break;
	case (2): { uSrc = gpuBlending02(uDst, uSrc);  } break;
	case (3): { uSrc = gpuBlending03(uDst, uSrc);  } break;
	}
  }

  //  OUTPUT
  *Pixel = uSrc | PixelMSB;
}


///////////////////////////////////////////////////////////////////////////////
#endif  //_INNER_PIXEL_H_

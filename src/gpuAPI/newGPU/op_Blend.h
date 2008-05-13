#ifndef _OP_BLEND_H_
#define _OP_BLEND_H_
///////////////////////////////////////////////////////////////////////////////
//  GPU Blending operations functions

#ifdef    GPU_ARM_ASM
///////////////////////////////////////////////////////////////////////////////
INLINE  u32 gpuBlending00(u32 uDst, u32 uSrc)
{
//return (((uDst & 0x7BDE) + (uSrc & 0x7BDE)) >> 1);

  u32 uMsk;
  asm volatile ("mov  %[msk], #0x7B00         " /*  msk = 0x7BDE      */  : [msk] "=r" (uMsk)                                                 );
  asm volatile ("orr  %[msk], %[msk], #0xDE   " /*                    */  : [msk] "=r" (uMsk) : "0" (uMsk)                                    );
  asm volatile ("and  %[src], %[src], %[msk]  " /*  src = uSrc & msk  */  : [src] "=r" (uSrc) : "0" (uSrc), [msk] "r" (uMsk)                  );
  asm volatile ("and  %[dst], %[dst], %[msk]  " /*  dst = uDst & msk  */  : [dst] "=r" (uDst) : "0" (uDst), [msk] "r" (uMsk)                  );
  asm volatile ("add  %[msk], %[dst], %[src]  " /*  msk = src  + dst  */  : [msk] "=r" (uMsk) :             [dst] "r" (uDst), [src] "r" (uSrc));
  asm volatile ("mov  %[msk], %[msk], lsr #1  " /*  msk = msk >> 1    */  : [msk] "=r" (uMsk) : "0" (uMsk)                                    );
  
  return uMsk;

}

///////////////////////////////////////////////////////////////////////////////
//	1.0 x Back + 1.0 x Forward
INLINE  u32 gpuBlending01(u32 uDst, u32 uSrc)
{
  /*
  u32 rr, gg, bb;
  bb = (uDst & 0x7C00) + (uSrc & 0x7C00);   if (bb > 0x7C00)  bb = 0x7C00;
  gg = (uDst & 0x03E0) + (uSrc & 0x03E0);   if (gg > 0x03E0)  gg = 0x03E0;  bb |= gg;
  rr = (uDst & 0x001F) + (uSrc & 0x001F);   if (rr > 0x001F)  rr = 0x001F;  bb |= rr;
  return bb;
  */

  u32 st,dt,out;
  asm volatile ("and    %[dt],  %[dst],   #0x7C00             "   /* dt  = uDst & 0x7C00        */  : [dt]  "=r" (dt)   :             [dst] "r" (uDst)                    );
  asm volatile ("and    %[st],  %[src],   #0x7C00             "   /* st  = uSrc & 0x7C00        */  : [st]  "=r" (st)   :             [src] "r" (uSrc)                    );
  asm volatile ("add    %[out], %[dt],    %[st]               "   /* out = dt + st              */  : [out] "=r" (out)  :             [dt]  "r" (dt),   [st]  "r" (st)    );
  asm volatile ("cmp    %[out], #0x7C00                       "   /* if(out>0x7C00)             */  :                   :             [out] "r" (out)                     );
  asm volatile ("movhi  %[out], #0x7C00                       "   /*   out = 0x7C00             */  : [out] "=r" (out)  : "0" (out)                                       );

  asm volatile ("and    %[dt],  %[dst],   #0x03E0             "   /* dt  = uDst & 0x03E0        */  : [dt]  "=r" (dt)   :             [dst] "r" (uDst)                    );
  asm volatile ("and    %[st],  %[src],   #0x03E0             "   /* st  = uSrc & 0x03E0        */  : [st]  "=r" (st)   :             [src] "r" (uSrc)                    );
  asm volatile ("add    %[dt],  %[dt],    %[st]               "   /* dt  = dt + st              */  : [dt]  "=r" (dt)   : "0" (dt),   [st]  "r" (st)                      );
  asm volatile ("cmp    %[dt],  #0x03E0                       "   /* if(dt>0x03E0)              */  :                   :             [dt]  "r" (dt)                      );
  asm volatile ("movhi  %[dt],  #0x03E0                       "   /*   dt = 0x03E0              */  : [dt]  "=r" (dt)   : "0" (dt)                                        );
  asm volatile ("orr    %[out], %[out],   %[dt]               "   /* out = out | dt             */  : [out] "=r" (out)  : "0" (out),  [dt]  "r" (dt)                      );

  asm volatile ("and    %[dt],  %[dst],   #0x001F             "   /* dt  = uDst & 0x001F        */  : [dt]  "=r" (dt)   :             [dst] "r" (uDst)                    );
  asm volatile ("and    %[st],  %[src],   #0x001F             "   /* st  = uSrc & 0x001F        */  : [st]  "=r" (st)   :             [src] "r" (uSrc)                    );
  asm volatile ("add    %[dt],  %[dt],    %[st]               "   /* dt  = dt + st              */  : [dt]  "=r" (dt)   : "0" (dt),   [st]  "r" (st)                      );
  asm volatile ("cmp    %[dt],  #0x001F                       "   /* if(dt>0x001F)              */  :                   :             [dt]  "r" (dt)                      );
  asm volatile ("movhi  %[dt],  #0x001F                       "   /*   dt = 0x001F              */  : [dt]  "=r" (dt)   : "0" (dt)                                        );
  asm volatile ("orr    %[out], %[out],   %[dt]               "   /* out = out | dt             */  : [out] "=r" (out)  : "0" (out),  [dt]  "r" (dt)                      );
  return out;

  /*
  u16 rc  = gpuAddT[ ((uDst&0x7C00)>>5) + ((uSrc       )>>10) ] << 10;
      rc |= gpuAddT[ ((uDst&0x03E0)>>0) + ((uSrc&0x03E0)>> 5) ] <<  5;
      rc |= gpuAddT[ ((uDst&0x001F)<<5) + ((uSrc&0x001F)    ) ];
	return rc;
  */
}

///////////////////////////////////////////////////////////////////////////////
//	1.0 x Back + 0.25 x Forward	*/
INLINE  u32 gpuBlending03(u32 uDst, u32 uSrc)
{
  /*
  s32 rr, gg, bb;
  uSrc >>= 2;
  bb = (uDst & 0x7C00) + (uSrc & 0x1C00);   if (bb > 0x7C00)  bb = 0x7C00;
  gg = (uDst & 0x03E0) + (uSrc & 0x00E0);   if (gg > 0x03E0)  gg = 0x03E0;  bb |= gg;
  rr = (uDst & 0x001F) + (uSrc & 0x0007);   if (rr > 0x001F)  rr = 0x001F;  bb |= rr;
  return bb;
  */

  u32 st,dt,out;
  asm volatile ("mov    %[src], %[src],               lsr #2  "   /* src = uSrc>>2              */  : [src] "=r" (uSrc) : "0" (uSrc)                                      );
  asm volatile ("and    %[dt],  %[dst],   #0x7C00             "   /* dt  = uDst & 0x7C00        */  : [dt]  "=r" (dt)   :             [dst] "r" (uDst)                    );
  asm volatile ("and    %[st],  %[src],   #0x1C00             "   /* st  = uSrc & 0x1C00        */  : [st]  "=r" (st)   :             [src] "r" (uSrc)                    );
  asm volatile ("add    %[out], %[dt],    %[st]               "   /* out = dt + st              */  : [out] "=r" (out)  :             [dt]  "r" (dt),   [st]  "r" (st)    );
  asm volatile ("cmp    %[out], #0x7C00                       "   /* if(out>0x7C00)             */  :                   :             [out] "r" (out)                     );
  asm volatile ("movhi  %[out], #0x7C00                       "   /*   out = 0x7C00             */  : [out] "=r" (out)  : "0" (out)                                       );

  asm volatile ("and    %[dt],  %[dst],   #0x03E0             "   /* dt  = uDst & 0x03E0        */  : [dt]  "=r" (dt)   :             [dst] "r" (uDst)                    );
  asm volatile ("and    %[st],  %[src],   #0x00E0             "   /* st  = uSrc & 0x00E0        */  : [st]  "=r" (st)   :             [src] "r" (uSrc)                    );
  asm volatile ("add    %[dt],  %[dt],    %[st]               "   /* dt  = dt + st              */  : [dt]  "=r" (dt)   : "0" (dt),   [st]  "r" (st)                      );
  asm volatile ("cmp    %[dt],  #0x03E0                       "   /* if(dt>0x03E0)              */  :                   :             [dt]  "r" (dt)                      );
  asm volatile ("movhi  %[dt],  #0x03E0                       "   /*   dt = 0x03E0              */  : [dt]  "=r" (dt)   : "0" (dt)                                        );
  asm volatile ("orr    %[out], %[out],   %[dt]               "   /* out = out | dt             */  : [out] "=r" (out)  : "0" (out),  [dt]  "r" (dt)                      );

  asm volatile ("and    %[dt],  %[dst],   #0x001F             "   /* dt  = uDst & 0x001F        */  : [dt]  "=r" (dt)   :             [dst] "r" (uDst)                    );
  asm volatile ("and    %[st],  %[src],   #0x0007             "   /* st  = uSrc & 0x0007        */  : [st]  "=r" (st)   :             [src] "r" (uSrc)                    );
  asm volatile ("add    %[dt],  %[dt],    %[st]               "   /* dt  = dt + st              */  : [dt]  "=r" (dt)   : "0" (dt),   [st]  "r" (st)                      );
  asm volatile ("cmp    %[dt],  #0x001F                       "   /* if(dt>0x001F)              */  :                   :             [dt]  "r" (dt)                      );
  asm volatile ("movhi  %[dt],  #0x001F                       "   /*   dt = 0x001F              */  : [dt]  "=r" (dt)   : "0" (dt)                                        );
  asm volatile ("orr    %[out], %[out],   %[dt]               "   /* out = out | dt             */  : [out] "=r" (out)  : "0" (out),  [dt]  "r" (dt)                      );
  return out;

  /*
  u16 rc  = gpuAddT[ ((uDst&0x7C00)>>5) + ((uSrc       )>>12) ] << 10;
      rc |= gpuAddT[ ((uDst&0x03E0)>>0) + ((uSrc&0x03E0)>> 7) ] <<  5;
      rc |= gpuAddT[ ((uDst&0x001F)<<5) + ((uSrc&0x001F)>> 2) ];
  return rc;
  */
}

///////////////////////////////////////////////////////////////////////////////
//	1.0 x Back - 1.0 x Forward	*/
INLINE  u32 gpuBlending02(u32 uDst, u32 uSrc)
{
  /*
  s32 rr, gg, bb;
  bb = (uDst & 0x7C00) - (uSrc & 0x7C00);   if (bb < 0)  bb  =  0;
  gg = (uDst & 0x03E0) - (uSrc & 0x03E0);   if (gg > 0)  bb |= gg;
  rr = (uDst & 0x001F) - (uSrc & 0x001F);   if (rr > 0)  bb |= rr;
  return bb;
  */

  u32 st,dt,out;
  asm volatile ("and    %[dt],  %[dst],   #0x7C00             "   /* dt  = uDst & 0x7C00        */  : [dt]  "=r" (dt)   :             [dst] "r" (uDst)                    );
  asm volatile ("and    %[st],  %[src],   #0x7C00             "   /* st  = uSrc & 0x7C00        */  : [st]  "=r" (st)   :             [src] "r" (uSrc)                    );
  asm volatile ("subs   %[out], %[dt],    %[st]               "   /* out = dt-st -> flag(mi)    */  : [out] "=r" (out)  :             [dt]  "r" (dt),   [st]  "r" (st)    );
  asm volatile ("movmi  %[out], #0x0000                       "   /* if(mi)  out = 0x0000       */  : [out] "=r" (out)  : "0" (out)                                       );

  asm volatile ("and    %[dt],  %[dst],   #0x03E0             "   /* dt  = uDst & 0x03E0        */  : [dt]  "=r" (dt)   :             [dst] "r" (uDst)                    );
  asm volatile ("and    %[st],  %[src],   #0x03E0             "   /* st  = uSrc & 0x03E0        */  : [st]  "=r" (st)   :             [src] "r" (uSrc)                    );
  asm volatile ("subs   %[dt],  %[dt],    %[st]               "   /* dt  = dt-st -> flag(pl)    */  : [dt]  "=r" (dt)   : "0" (dt),   [st]  "r" (st)                      );
  asm volatile ("orrpl  %[out], %[out],   %[dt]               "   /* if(pl) out = out | dt      */  : [out] "=r" (out)  : "0" (out),  [dt]  "r" (dt)                      );

  asm volatile ("and    %[dt],  %[dst],   #0x001F             "   /* dt  = uDst & 0x001F        */  : [dt]  "=r" (dt)   :             [dst] "r" (uDst)                    );
  asm volatile ("and    %[st],  %[src],   #0x001F             "   /* st  = uSrc & 0x001F        */  : [st]  "=r" (st)   :             [src] "r" (uSrc)                    );
  asm volatile ("subs   %[dt],  %[dt],    %[st]               "   /* dt  = dt-st - flat(pl)     */  : [dt]  "=r" (dt)   : "0" (dt),   [st]  "r" (st)                      );
  asm volatile ("orrpl  %[out], %[out],   %[dt]               "   /* if(pl) out = out | dt      */  : [out] "=r" (out)  : "0" (out),  [dt]  "r" (dt)                      );
  return out;

  /*
  u16 rc  = gpuSubT[ ((uDst&0x7C00)>>5) + ((uSrc       )>>10) ] << 10;
      rc |= gpuSubT[ ((uDst&0x03E0)>>0) + ((uSrc&0x03E0)>> 5) ] <<  5;
      rc |= gpuSubT[ ((uDst&0x001F)<<5) + ((uSrc&0x001F)    ) ];
	return rc;
  */
}


#else   //GPU_ARM_ASM
///////////////////////////////////////////////////////////////////////////////
INLINE  u32 gpuBlending00(u32 uDst, u32 uSrc)
{
  return (((uDst & 0x7BDE) + (uSrc & 0x7BDE)) >> 1);
}

///////////////////////////////////////////////////////////////////////////////
//	1.0 x Back + 1.0 x Forward
INLINE  u32 gpuBlending01(u32 uDst, u32 uSrc)
{
  u32 rr, gg, bb;
  bb = (uDst & 0x7C00) + (uSrc & 0x7C00);   if (bb > 0x7C00)  bb = 0x7C00;
  gg = (uDst & 0x03E0) + (uSrc & 0x03E0);   if (gg > 0x03E0)  gg = 0x03E0;  bb |= gg;
  rr = (uDst & 0x001F) + (uSrc & 0x001F);   if (rr > 0x001F)  rr = 0x001F;  bb |= rr;
  return bb;

  /*
  s32 rr, gg, bb;
  bb = (uDst & 0x7C00) + (uSrc & 0x7C00);   if (bb > 0x7C00)  bb = 0x7C00;
  gg = (uDst & 0x03E0) + (uSrc & 0x03E0);   bb |= (gg < 0x03E0) ? gg : 0x03E0;
  rr = (uDst & 0x001F) + (uSrc & 0x001F);   bb |= (rr < 0x001F) ? rr : 0x001F;
  return bb;
  */
  /*
  u32 rc  = gpuAddT[ ((uDst&0x7C00)>>5) + ((uSrc       )>>10) ] << 10;
      rc |= gpuAddT[ ((uDst&0x03E0)>>0) + ((uSrc&0x03E0)>> 5) ] <<  5;
      rc |= gpuAddT[ ((uDst&0x001F)<<5) + ((uSrc&0x001F)    ) ];
	return rc;
  */
}

///////////////////////////////////////////////////////////////////////////////
//	1.0 x Back - 1.0 x Forward	*/
INLINE  u32 gpuBlending02(u32 uDst, u32 uSrc)
{
  /**/
  s32 rr, gg, bb;
  bb = (uDst & 0x7C00) - (uSrc & 0x7C00);   if (bb < 0)  bb  =  0;
  gg = (uDst & 0x03E0) - (uSrc & 0x03E0);   if (gg > 0)  bb |= gg;
  rr = (uDst & 0x001F) - (uSrc & 0x001F);   if (rr > 0)  bb |= rr;
  return bb;
  /**/
  /*
  u32 rc  = gpuSubT[ ((uDst&0x7C00)>>5) + ((uSrc       )>>10) ] << 10;
      rc |= gpuSubT[ ((uDst&0x03E0)>>0) + ((uSrc&0x03E0)>> 5) ] <<  5;
      rc |= gpuSubT[ ((uDst&0x001F)<<5) + ((uSrc&0x001F)    ) ];
	return rc;
  */
}

///////////////////////////////////////////////////////////////////////////////
//	1.0 x Back + 0.25 x Forward	*/
INLINE  u32 gpuBlending03(u32 uDst, u32 uSrc)
{
  u32 rr, gg, bb;
  uSrc >>= 2;
  bb = (uDst & 0x7C00) + (uSrc & 0x1C00);   if (bb > 0x7C00)  bb = 0x7C00;
  gg = (uDst & 0x03E0) + (uSrc & 0x00E0);   if (gg > 0x03E0)  gg = 0x03E0;  bb |= gg;
  rr = (uDst & 0x001F) + (uSrc & 0x0007);   if (rr > 0x001F)  rr = 0x001F;  bb |= rr;
  return bb;

  /*
  s32 rr, gg, bb;
  uSrc >>= 2;
  bb = (uDst & 0x7C00) + (uSrc & 0x1C00);   if (bb > 0x7C00)  bb = 0x7C00;
  gg = (uDst & 0x03E0) + (uSrc & 0x00E0);   bb |= (gg < 0x03E0) ? gg : 0x03E0;
  rr = (uDst & 0x001F) + (uSrc & 0x0007);   bb |= (rr < 0x001F) ? rr : 0x001F;
  return bb;
  /*
  /*
  u32 rc  = gpuAddT[ ((uDst&0x7C00)>>5) + ((uSrc       )>>12) ] << 10;
      rc |= gpuAddT[ ((uDst&0x03E0)>>0) + ((uSrc&0x03E0)>> 7) ] <<  5;
      rc |= gpuAddT[ ((uDst&0x001F)<<5) + ((uSrc&0x001F)>> 2) ];
  return rc;
  */
}

///////////////////////////////////////////////////////////////////////////////
#endif  //GPU_ARM_ASM

///////////////////////////////////////////////////////////////////////////////
#endif  //_OP_BLEND_H_

#ifndef _OP_LIGHT_H_
#define _OP_LIGHT_H_
///////////////////////////////////////////////////////////////////////////////
//  GPU color operations for lightng calculations

#ifdef    GPU_ARM_ASM
///////////////////////////////////////////////////////////////////////////////
INLINE  u32 gpuLightingTXT(u32 uDst, u32 cb, u32 cg, u32 cr)
{
//return uDst;
  
  /**/
  u16 rc;
  rc  = gpuLitT[ ((uDst&0x7C00)>>5) + cb ] << 10;
  rc |= gpuLitT[ ((uDst&0x03E0)>>0) + cg ] <<  5;
  rc |= gpuLitT[ ((uDst&0x001F)<<5) + cr ];
	return rc;
  /**/

  /*
  u32 rr, gg, bb;
  bb = ((uDst&0x7C00)*(b)) >> 4;   if (bb>0x7C00) bb = 0x7C00;  bb &= 0x7C00;
  gg = ((uDst&0x03E0)*(g)) >> 4;   if (gg>0x03E0) gg = 0x03E0;  gg &= 0x03E0;  bb |= gg;
  rr = ((uDst&0x001F)*(r)) >> 4;   if (rr>0x001F) rr = 0x001F;  rr &= 0x001F;  bb |= rr;
  return bb;
  */

#if 0
  // todo  quitar out = uDst & 0x8000        
  u32 out;
  asm volatile ("orr    %[cb],  %[cr],  %[cb],    lsl #10   "   /* cb = light color           */  : [cb]  "=r" (cb)   : "0" (cb),   [cr]  "r" (cr)                        );
  asm volatile ("orr    %[cb],  %[cb],  %[cg],    lsl #5    "   /*                            */  : [cb]  "=r" (cb)   : "0" (cb),   [cg]  "r" (cg)                        );
  asm volatile ("and    %[out], %[col], #0x8000             "   /* out = uDst & 0x8000        */  : [out] "=r" (out)  : "0" (out),  [col] "r" (uDst)                      );

  asm volatile ("and    %[ct],  %[col], #0x7C00             "   /* ct  = col & 0x7C00         */  : [ct]  "=r" (cg)   :             [col] "r" (uDst)                      );
  asm volatile ("and    %[lt],  %[lit], #0x7C00             "   /* lt  = lit & 0x7C00         */  : [lt]  "=r" (cr)   :             [lit] "r" (cb)                        );
  asm volatile ("orr    %[ct],  %[ct],  %[lt],    lsr #5    "   /* ct  =((cb<<5) | (lb))<<5   */  : [ct]  "=r" (cg)   : "0" (cg),   [lt]  "r" (cr)                        );
  asm volatile ("ldrb   %[lt], [%[pal], %[ct],    lsr #5  ] "   /* lt  = gpuLitT[ct>>5]       */  : [lt]  "=r" (cr)   : "0" (cr),   [ct]  "r" (cg),   [pal] "r" (gpuLitT) );
  asm volatile ("orr    %[out], %[out], %[lt],    lsl #10   "   /* out = out | (lt<<10)       */  : [out] "=r" (out)  : "0" (out),  [lt]  "r" (cr)                        );

  asm volatile ("and    %[ct],  %[col], #0x03E0             "   /* ct  = col & 0x03E0         */  : [ct]  "=r" (cg)   :             [col] "r" (uDst)                      );
  asm volatile ("and    %[lt],  %[lit], #0x03E0             "   /* lt  = lit & 0x03E0         */  : [lt]  "=r" (cr)   :             [lit] "r" (cb)                        );
  asm volatile ("orr    %[ct],  %[ct],  %[lt],    lsr #5    "   /* ct  = (cg<<5) | (lg)       */  : [ct]  "=r" (cg)   : "0" (cg),   [lt]  "r" (cr)                        );
  asm volatile ("ldrb   %[lt], [%[pal], %[ct] ]             "   /* lt  = gpuLitT[ct]          */  : [lt]  "=r" (cr)   : "0" (cr),   [ct]  "r" (cg),   [pal] "r" (gpuLitT) );
  asm volatile ("orr    %[out], %[out], %[lt],    lsl #5    "   /* out = out | (lt<<5)        */  : [out] "=r" (out)  : "0" (out),  [lt]  "r" (cr)                        );

  asm volatile ("and    %[lt],  %[lit], #0x001F             "   /* lt  = lit & 0x001F         */  : [lt]  "=r" (cr)   :             [lit] "r" (cb)                        );
  asm volatile ("and    %[ct],  %[col], #0x001F             "   /* ct  = col & 0x001F         */  : [ct]  "=r" (cg)   :             [col] "r" (uDst)                      );
  asm volatile ("orr    %[lt],  %[lt],  %[ct],    lsl #5    "   /* lt  = (cr<<5) | (lr)       */  : [lt]  "=r" (cr)   : "0" (cr),   [ct]  "r" (cg)                        );
  asm volatile ("ldrb   %[ct], [%[pal], %[lt] ]             "   /* ct  = gpuLitT[lt]          */  : [ct]  "=r" (cg)   : "0" (cg),   [lt]  "r" (cr),   [pal] "r" (gpuLitT) );
  asm volatile ("orr    %[out], %[out], %[ct]               "   /* out = out | (ct)           */  : [out] "=r" (out)  : "0" (out),  [ct]  "r" (cg)                        );
  return out;
#endif

#if 0
  //todo quitar out = uDst & 0x8000        
  u32 out,ct;
  asm volatile ("and    %[out], %[col],   #0x8000           "   /* out = uDst & 0x8000        */  : [out] "=r" (out)  : "0" (out),  [col] "r" (uDst)                      );

  asm volatile ("and    %[ct],  %[col],   #0x7C00           "   /* ct  = col & 0x7C00         */  : [ct]  "=r" (ct)   : "0" (ct),   [col] "r" (uDst)                      );
  asm volatile ("mul    %[lb],  %[ct],    %[lb]             "   /* lb  =  cb  * lb            */  : [lb]  "=r" (cb)   : "0" (cb),   [ct]  "r" (ct)                        );
  asm volatile ("cmp    %[lb],  #0x7C000                    "   /* if((lb>>4)>0x7C00)         */  :                   :             [lb]  "r" (cb)                        );
  asm volatile ("movhi  %[lb],  #0x7C000                    "   /*   lb = 0x7C000             */  : [lb]  "=r" (cb)   : "0" (cb)                                          );
  asm volatile ("and    %[lb],  %[lb],    #0x7C000          "   /* lb &=  0x7C000             */  : [lb]  "=r" (cb)   : "0" (cb)                                          );
  asm volatile ("orr    %[out], %[out],   %[lb],    lsr #4  "   /* out = out | (gB)           */  : [out] "=r" (out)  : "0" (out),  [lb]  "r" (cb)                        );

  asm volatile ("and    %[ct],  %[col],   #0x03E0           "   /* ct  = col & 0x03E0         */  : [ct]  "=r" (ct)   : "0" (ct),   [col] "r" (uDst)                      );
  asm volatile ("mul    %[lg],  %[ct],    %[lg]             "   /* lg  =  cg  * lg            */  : [lg]  "=r" (cg)   : "0" (cg),   [ct]  "r" (ct)                        );
  asm volatile ("cmp    %[lg],  #0x03E00                    "   /* if((lg>>4)>0x03E0)         */  :                   :             [lg]  "r" (cg)                        );
  asm volatile ("movhi  %[lg],  #0x03E00                    "   /*   lg = 0x03E00             */  : [lg]  "=r" (cg)   : "0" (cg)                                          );
  asm volatile ("and    %[lg],  %[lg],    #0x03E00          "   /* lg &=  0x03E00             */  : [lg]  "=r" (cg)   : "0" (cg)                                          );
  asm volatile ("orr    %[out], %[out],   %[lg],    lsr #4  "   /* out = out | (gG)           */  : [out] "=r" (out)  : "0" (out),  [lg]  "r" (cg)                        );

  asm volatile ("and    %[ct],  %[col],   #0x001F           "   /* ct  = col & 0x001F         */  : [ct]  "=r" (ct)   : "0" (ct),   [col] "r" (uDst)                      );
  asm volatile ("mul    %[lr],  %[ct],    %[lr]             "   /* lr  =  cr  * lr            */  : [lr]  "=r" (cr)   : "0" (cr),   [ct]  "r" (ct)                        );
  asm volatile ("cmp    %[lr],  #0x001F0                    "   /* if((lr>>4)>0x001F)         */  :                   :             [lr]  "r" (cr)                        );
  asm volatile ("movhi  %[lr],  #0x001F0                    "   /*   lr = 0x001F0             */  : [lr]  "=r" (cr)   : "0" (cr)                                          );
  asm volatile ("and    %[lr],  %[lr],    #0x001F0          "   /* lr &=  0x001F0             */  : [lr]  "=r" (cr)   : "0" (cr)                                          );
  asm volatile ("orr    %[out], %[out],   %[lr],    lsr #4  "   /* out = out | (gR)           */  : [out] "=r" (out)  : "0" (out),  [lr]  "r" (cr)                        );
  return out;
#endif

}

INLINE  u32  gpuLightingTXT_Ex(u32 uDst, u32 lCol)
{
  /*  workin 01 - 02
  u16 rc;
  rc  = gpuLitT[ ((uDst&0x7C00)>>5) + ((lCol>> 5)&0x1f) ] << 10;
  rc |= gpuLitT[ ((uDst&0x03E0)>>0) + ((lCol>>16)&0x1f) ] <<  5;
  rc |= gpuLitT[ ((uDst&0x001F)<<5) + ( lCol>>27)       ];
	return rc;
  */

  /*  fine
  u16 rc;
  rc  = gpuLitT[ ((uDst&0x7C00)>>5) + ((lCol>> 5)&0x1f) ] << 10;
  rc |= gpuLitT[ ((uDst&0x03E0)>>0) + ((lCol>>16)&0x1f) ] <<  5;
  rc |= gpuLitT[ ((uDst&0x001F)<<5) + ( lCol>>27)       ];
	return rc;
  */

  u16 rc;
  rc  = gpuLitT[ ((uDst&0x7C00)>>5) + ((lCol>> 5)&0x1f) ] << 10;
  rc |= gpuLitT[ ((uDst&0x03E0)>>0) + ((lCol>>16)&0x1f) ] <<  5;
  rc |= gpuLitT[ ((uDst&0x001F)<<5) + ( lCol>>27)       ];
	return rc;

}


/**/
///////////////////////////////////////////////////////////////////////////////
INLINE u32  gpuLightingRGB(u32 cb, u32 cg, u32 cr) 
{
//return (cb << 10) | (cg << 5) | (cr);
  asm volatile ("orr  %[cb], %[cr], %[cb], lsl #10" : [cb] "=r" (cb)  : [cr] "r" (cr), "0" (cb) );
  asm volatile ("orr  %[cb], %[cb], %[cg], lsl #5 " : [cb] "=r" (cb)  : [cg] "r" (cg), "0" (cb) );
  return cb;
}

INLINE  u32  gpuLightingRGB_Ex(u32 lCol)
{
  /*  workin 01 - 02
	return ((lCol<<5)&0x7C00) | ((lCol>>11)&0x3E0) | (lCol>>27);
  */
  /*  fine
  return ((lCol<<5)&0x7C00) | ((lCol>>11)&0x3E0) | (lCol>>27);
  */
  u32 cb,cg;
  asm volatile ("and  %[cb],    %[lCol], #0x7C00*32       " : [cb]    "=r" (cb)   :             [lCol] "r" (lCol) );
  asm volatile ("and  %[cg],    %[lCol], #0x03E0*2048     " : [cg]    "=r" (cg)   :             [lCol] "r" (lCol) );
  asm volatile ("mov  %[lCol],  %[lCol],          lsr #27 " : [lCol]  "=r" (lCol) : "0" (lCol)                    );
  asm volatile ("orr  %[lCol],  %[lCol], %[cb],   lsl #5  " : [lCol]  "=r" (lCol) : "0" (lCol), [cb]   "r" (cb)   );
  asm volatile ("orr  %[lCol],  %[lCol], %[cg],   lsr #11 " : [lCol]  "=r" (lCol) : "0" (lCol), [cg]   "r" (cg)   );
  return lCol;
}

#else   //GPU_ARM_ASM
///////////////////////////////////////////////////////////////////////////////
INLINE  u32  gpuLightingTXT(u32 uDst, u32 b, u32 g, u32 r)
{
  /*
  u32 rr, gg, bb;
  bb = ((uDst&0x7C00)*u8(b)) >> 4;   bb  = (bb<0x7C00) ? (bb&0x7C00) : 0x7C00;
  gg = ((uDst&0x03E0)*u8(g)) >> 4;   bb |= (gg<0x03E0) ? (gg&0x03E0) : 0x03E0;
  rr = ((uDst&0x001F)*u8(r)) >> 4;   bb |= (rr<0x001F) ? (rr&0x001F) : 0x001F;
  return bb;
  */
  /**/
  u16 rc;
  rc  = gpuLitT[ ((uDst&0x7C00)>>5) + b ] << 10;
  rc |= gpuLitT[ ((uDst&0x03E0)>>0) + g ] <<  5;
  rc |= gpuLitT[ ((uDst&0x001F)<<5) + r ];
	return rc;
  /**/
}

INLINE  u32  gpuLightingTXT_Ex(u32 uDst, u32 lCol)
{
  /*  workin 01 - 02
  u16 rc;
  rc  = gpuLitT[ ((uDst&0x7C00)>>5) + ((lCol>> 5)&0x1f) ] << 10;
  rc |= gpuLitT[ ((uDst&0x03E0)>>0) + ((lCol>>16)&0x1f) ] <<  5;
  rc |= gpuLitT[ ((uDst&0x001F)<<5) + ( lCol>>27)       ];
	return rc;
  */

  /*  fine
  u16 rc;
  rc  = gpuLitT[ ((uDst&0x7C00)>>5) + ((lCol>> 5)&0x1f) ] << 10;
  rc |= gpuLitT[ ((uDst&0x03E0)>>0) + ((lCol>>16)&0x1f) ] <<  5;
  rc |= gpuLitT[ ((uDst&0x001F)<<5) + ( lCol>>27)       ];
	return rc;
  */

  u16 rc;
  rc  = gpuLitT[ ((uDst&0x7C00)>>5) + ((lCol>> 5)&0x1f) ] << 10;
  rc |= gpuLitT[ ((uDst&0x03E0)>>0) + ((lCol>>16)&0x1f) ] <<  5;
  rc |= gpuLitT[ ((uDst&0x001F)<<5) + ( lCol>>27)       ];
	return rc;
}

INLINE  u32  gpuLightingRGB(u32 b, u32 g, u32 r)
{
	return (b << 10) | (g << 5) | (r);
}

INLINE  u32  gpuLightingRGB_Ex(u32 lCol)
{
  /*  workin 01 - 02
	return ((lCol<<5)&0x7C00) | ((lCol>>11)&0x3E0) | (lCol>>27);
  */
  /*  fine
  return ((lCol<<5)&0x7C00) | ((lCol>>11)&0x3E0) | (lCol>>27);
  */

  return ((lCol<<5)&0x7C00) | ((lCol>>11)&0x3E0) | (lCol>>27);
}

///////////////////////////////////////////////////////////////////////////////
#endif  //GPU_ARM_ASM

///////////////////////////////////////////////////////////////////////////////
#endif  //_OP_LIGHT_H_

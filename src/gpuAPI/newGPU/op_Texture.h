#ifndef _OP_TEXTURE_H_
#define _OP_TEXTURE_H_
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//  GPU Texture Addresing

#ifdef    GPU_ARM_ASM
///////////////////////////////////////////////////////////////////////////////
//  4bitCLUT
INLINE  u32 gpuTextureMappingW0(u32 tu, u32 tv)
{
  /*
  u8  rgb = ((u8*)TBA)[FRAME_OFFSET(tu>>1 , tv<<1)];
  return CBA[(tu&1) ? (rgb>>4) : (rgb&0xf) ];
  */
  asm volatile ("movs   %[tu],   %[tu],               lsr #1  " :  [tu] "=r" (tu) :        "0" (tu)                   : "cc"  );
  asm volatile ("add    %[off],  %[tu],     %[tv],    lsl #11 " : [off] "=r" (tv) :  [tv]  "0" (tv), [tu]   "r" (tu)          );
  asm volatile ("ldrb   %[idx], [%[addr],   %[idx] ]          " : [idx] "=r" (tv) :        "0" (tv), [addr] "r" (TBA)         );
  asm volatile ("movcs  %[idx],  %[idx],              lsr #4  " : [idx] "=r" (tv) :        "0" (tv)                           );
  asm volatile ("and    %[idx],  %[idx],    #0xf              " : [idx] "=r" (tv) :        "0" (tv), [tu]   "r" (tu)          );
  asm volatile ("mov    %[off],  %[off],              lsl #1  " : [off] "=r" (tv) :        "0" (tv)                           );
  asm volatile ("ldrh   %[col], [%[addr],   %[off]           ]" : [col] "=r" (tu) :  [off] "r" (tv), [addr] "r" (CBA)         );
//asm volatile ("ldrh   %[col], [%[addr],   %[off],   lsl #1 ]" : [col] "=r" (tu) :  [off] "r" (tv), [addr] "r" (CBA)         );
  return tu;
}

#if 0
INLINE  u16 gpuTextureMappingW0_Ex(u32 tCor)
{
  /*
  u32 tu  = (tCor>>23);
  u32 tv  = (tCor<< 4)&(0xff<<11);
  u8  rgb = ((u8*)TBA)[tv + (tu>>1)];
  return CBA[(tu&1) ? (rgb>>4) : (rgb&0xf) ];
  */
  u32 off;
  asm volatile ("movs   %[off],  %[tCor],             lsr #24 " : [off]   "=r" (off)  :        "0" (off)                  : "cc"  );
  asm volatile ("and    %[tCor], %[tCor],   #0xff00           " : [tCor]  "=r" (tCor) :
  asm volatile ("add    %[off],  %[tu],     %[tv],    lsl #11 " : [off]   "=r" (tv)   :  [tv]  "0" (tv), [tu]   "r" (tu)          );

  asm volatile ("ldrb   %[idx], [%[addr],   %[idx] ]          " : [idx] "=r" (tv) :        "0" (tv), [addr] "r" (TBA)         );
  asm volatile ("movcs  %[idx],  %[idx],              lsr #4  " : [idx] "=r" (tv) :        "0" (tv)                           );
  asm volatile ("and    %[idx],  %[idx],    #0xf              " : [idx] "=r" (tv) :        "0" (tv), [tu]   "r" (tu)          );
  asm volatile ("mov    %[off],  %[off],              lsl #1  " : [off] "=r" (tv) :        "0" (tv)                           );
  asm volatile ("ldrh   %[col], [%[addr],   %[off]           ]" : [col] "=r" (tu) :  [off] "r" (tv), [addr] "r" (CBA)         );
//asm volatile ("ldrh   %[col], [%[addr],   %[off],   lsl #1 ]" : [col] "=r" (tu) :  [off] "r" (tv), [addr] "r" (CBA)         );
  return tu;

}
#else
INLINE  u16 gpuTextureMappingW0_Ex(u32 tCor)
{
  u32 tu  = (tCor>>23);
  u32 tv  = (tCor<< 4)&(0xff<<11);
  u8  rgb = ((u8*)TBA)[tv + (tu>>1)];
  return CBA[(tu&1) ? (rgb>>4) : (rgb&0xf) ];
}
#endif

///////////////////////////////////////////////////////////////////////////////
//  8bitCLUT
INLINE  u32 gpuTextureMappingW1(u32 tu, u32 tv)
{
  /*
  u8  rgb = ((u8*)TBA)[FRAME_OFFSET(tu , tv<<1)];
  return CBA[rgb];
  */
  asm volatile ("add    %[off],  %[tu],     %[tv],    lsl #11 " : [off] "=r" (tv) :  [tv]  "0" (tv), [tu]   "r" (tu) );
  asm volatile ("ldrb   %[idx], [%[addr],   %[idx] ]          " : [idx] "=r" (tv) :        "0" (tv), [addr] "r" (TBA));
  asm volatile ("and    %[idx],  %[idx],    #0xff             " : [idx] "=r" (tv) :        "0" (tv)                  );
  asm volatile ("mov    %[off],  %[off],              lsl #1  " : [off] "=r" (tv) :        "0" (tv)                  );
  asm volatile ("ldrh   %[col], [%[addr],   %[off]           ]" : [col] "=r" (tu) :  [off] "r" (tv), [addr] "r" (CBA));
//asm volatile ("ldrh   %[col], [%[addr],   %[off],   lsl #1 ]" : [col] "=r" (tu) :  [off] "r" (tv), [addr] "r" (CBA));
  return tu;
}

INLINE  u16 gpuTextureMappingW1_Ex(u32 tCor)
{
  u32 tu  = (tCor>>23);
  u32 tv  = (tCor<< 4)&(0xff<<11);
  u8  rgb = ((u8*)TBA)[tv+tu];
  return CBA[rgb];
}

///////////////////////////////////////////////////////////////////////////////
//  15bit
INLINE  u32 gpuTextureMappingW2(u32 tu, u32 tv)
{
  /*
  return TBA[FRAME_OFFSET(tu, tv)];
  */
  asm volatile ("add    %[off],   %[tu],    %[tv],    lsl #10 " : [off] "=r" (tv) :  [tv]  "0" (tv), [tu]   "r" (tu) );
  asm volatile ("mov    %[off],   %[off],             lsl #1  " : [off] "=r" (tv) :        "0" (tv)                  );
  asm volatile ("ldrh   %[col], [ %[addr],  %[off]           ]" : [col] "=r" (tu) :  [off] "r" (tv), [addr] "r" (TBA));
//asm volatile ("ldrh   %[col], [%[addr],   %[off],   lsl #1 ]" : [col] "=r" (tu) :  [off] "r" (tv), [addr] "r" (CBA));
  return tu;
}

INLINE  u16 gpuTextureMappingW2_Ex(u32 tCor)
{
  u32 tu  = (tCor>>23);
  u32 tv  = (tCor<< 3)&(0xff<<10);
  return TBA[tu+tv];
}

#else   //GPU_ARM_ASM
///////////////////////////////////////////////////////////////////////////////
//  4bitCLUT
INLINE  u16 gpuTextureMappingW0(u32 tu, u32 tv)
{
  u8  rgb = ((u8*)TBA)[FRAME_OFFSET(tu>>1 , tv<<1)];
  return CBA[(tu&1) ? (rgb>>4) : (rgb&0xf) ];
}

INLINE  u16 gpuTextureMappingW0_Ex(u32 tCor)
{
  u32 tu  = (tCor>>23);
  u32 tv  = (tCor<< 4)&(0xff<<11);
  u8  rgb = ((u8*)TBA)[tv + (tu>>1)];
  return CBA[(tu&1) ? (rgb>>4) : (rgb&0xf) ];
}


///////////////////////////////////////////////////////////////////////////////
//  8bitCLUT
INLINE  u16 gpuTextureMappingW1(u32 tu, u32 tv)
{
  u8  rgb = ((u8*)TBA)[FRAME_OFFSET(tu , tv<<1)];
  return CBA[rgb];
}

INLINE  u16 gpuTextureMappingW1_Ex(u32 tCor)
{
  u32 tu  = (tCor>>23);
  u32 tv  = (tCor<< 4)&(0xff<<11);
  u8  rgb = ((u8*)TBA)[tv+tu];
  return CBA[rgb];
}

///////////////////////////////////////////////////////////////////////////////
//  15bit
INLINE  u16 gpuTextureMappingW2(u32 tu, u32 tv)
{
  return TBA[FRAME_OFFSET(tu, tv)];
}

INLINE  u16 gpuTextureMappingW2_Ex(u32 tCor)
{
  u32 tu  = (tCor>>23);
  u32 tv  = (tCor<< 3)&(0xff<<10);
  return TBA[tu+tv];
}

///////////////////////////////////////////////////////////////////////////////
#endif  //GPU_ARM_ASM


///////////////////////////////////////////////////////////////////////////////
#endif  //_OP_TEXTURE_H_

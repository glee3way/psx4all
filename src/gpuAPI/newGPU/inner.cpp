///////////////////////////////////////////////////////////////////////////////
//  Inner loop driver instanciation file
#include "newGPU.h"

///////////////////////////////////////////////////////////////////////////////
//  Code generation "hints"
#ifdef __WIN32__
#   undef GPU_ARM_ASM
#endif

///////////////////////////////////////////////////////////////////////////////
//  Pixelops Table
u8 gpuLitT[32*32];
u8 gpuSubT[32*32];
u8 gpuAddT[32*32];
//u8 gpuAd2T[32*32];

///////////////////////////////////////////////////////////////////////////////
bool gpuInnerInit()
{
  //  gpuLitT
  for(int i=0;i<1024;++i)
  {
    int a = i>>5;
    int b = i&0x1f;
    gpuLitT[i] = ((a*b)>>4) < 0x1f ? ((a*b)>>4) : 0x1f;
  }

  //  gpuSubT
  for(int i=0;i<1024;++i)
  {
    int a = i>>5;
    int b = i&0x1f;
    gpuSubT[i] = (a-b)>0 ? (a-b): 0;
  }

  //  gpuAddT
  for(int i=0;i<1024;++i)
  {
    int a = i>>5;
    int b = i&0x1f;
    gpuAddT[i] = (a+b)<0x1f ? (a+b): 0x1f;
  }

  /*
  //  gpuAd2T
  for(int i=0;i<1024;++i)
  {
    int a = i>>5;
    int b = (i&0x1f)>>2;
    gpuAd2T[i] = (a+b)<0x1f ? (a+b): 0x1f;
  }
  */
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//  Option Masks
#define   L ((CF>>0)&1)
#define   B ((CF>>1)&1)
#define   M ((CF>>2)&1)
#define  BM ((CF>>3)&3)
#define  TM ((CF>>5)&3)
#define   G ((CF>>7)&1)


///////////////////////////////////////////////////////////////////////////////
//  Pixel drawing drivers, for lines (only blending)
#include "inner_Pixel.h"

PF  gpuPixelDriver;
PF  gpuPixelDrivers[16] =   //  We only generate pixel op for MASKING/BLEND_ENABLE/BLEND_MODE
{ 
  gpuPixelFn<0x00<<1>,gpuPixelFn<0x01<<1>,gpuPixelFn<0x02<<1>,gpuPixelFn<0x03<<1>,  
  gpuPixelFn<0x04<<1>,gpuPixelFn<0x05<<1>,gpuPixelFn<0x06<<1>,gpuPixelFn<0x07<<1>,
  gpuPixelFn<0x08<<1>,gpuPixelFn<0x09<<1>,gpuPixelFn<0x0A<<1>,gpuPixelFn<0x0B<<1>,
  gpuPixelFn<0x0C<<1>,gpuPixelFn<0x0D<<1>,gpuPixelFn<0x0E<<1>,gpuPixelFn<0x0F<<1>,
};


///////////////////////////////////////////////////////////////////////////////
//  Sprite innerloops driver
#include "inner_Sprite.h"

PF gpuSpriteSpanDriver;
PF gpuSpriteSpanDrivers[128] = 
{
  gpuSpriteSpanFn<0x00>,gpuSpriteSpanFn<0x01>,gpuSpriteSpanFn<0x02>,gpuSpriteSpanFn<0x03>,  gpuSpriteSpanFn<0x04>,gpuSpriteSpanFn<0x05>,gpuSpriteSpanFn<0x06>,gpuSpriteSpanFn<0x07>,  gpuSpriteSpanFn<0x08>,gpuSpriteSpanFn<0x09>,gpuSpriteSpanFn<0x0A>,gpuSpriteSpanFn<0x0B>,  gpuSpriteSpanFn<0x0C>,gpuSpriteSpanFn<0x0D>,gpuSpriteSpanFn<0x0E>,gpuSpriteSpanFn<0x0F>,
  gpuSpriteSpanFn<0x10>,gpuSpriteSpanFn<0x11>,gpuSpriteSpanFn<0x12>,gpuSpriteSpanFn<0x13>,  gpuSpriteSpanFn<0x14>,gpuSpriteSpanFn<0x15>,gpuSpriteSpanFn<0x16>,gpuSpriteSpanFn<0x17>,  gpuSpriteSpanFn<0x18>,gpuSpriteSpanFn<0x19>,gpuSpriteSpanFn<0x1A>,gpuSpriteSpanFn<0x1B>,  gpuSpriteSpanFn<0x1C>,gpuSpriteSpanFn<0x1D>,gpuSpriteSpanFn<0x1E>,gpuSpriteSpanFn<0x1F>,
  gpuSpriteSpanFn<0x20>,gpuSpriteSpanFn<0x21>,gpuSpriteSpanFn<0x22>,gpuSpriteSpanFn<0x23>,  gpuSpriteSpanFn<0x24>,gpuSpriteSpanFn<0x25>,gpuSpriteSpanFn<0x26>,gpuSpriteSpanFn<0x27>,  gpuSpriteSpanFn<0x28>,gpuSpriteSpanFn<0x29>,gpuSpriteSpanFn<0x2A>,gpuSpriteSpanFn<0x2B>,  gpuSpriteSpanFn<0x2C>,gpuSpriteSpanFn<0x2D>,gpuSpriteSpanFn<0x2E>,gpuSpriteSpanFn<0x2F>,
  gpuSpriteSpanFn<0x30>,gpuSpriteSpanFn<0x31>,gpuSpriteSpanFn<0x32>,gpuSpriteSpanFn<0x33>,  gpuSpriteSpanFn<0x34>,gpuSpriteSpanFn<0x35>,gpuSpriteSpanFn<0x36>,gpuSpriteSpanFn<0x37>,  gpuSpriteSpanFn<0x38>,gpuSpriteSpanFn<0x39>,gpuSpriteSpanFn<0x3A>,gpuSpriteSpanFn<0x3B>,  gpuSpriteSpanFn<0x3C>,gpuSpriteSpanFn<0x3D>,gpuSpriteSpanFn<0x3E>,gpuSpriteSpanFn<0x3F>,
  gpuSpriteSpanFn<0x40>,gpuSpriteSpanFn<0x41>,gpuSpriteSpanFn<0x42>,gpuSpriteSpanFn<0x43>,  gpuSpriteSpanFn<0x44>,gpuSpriteSpanFn<0x45>,gpuSpriteSpanFn<0x46>,gpuSpriteSpanFn<0x47>,  gpuSpriteSpanFn<0x48>,gpuSpriteSpanFn<0x49>,gpuSpriteSpanFn<0x4A>,gpuSpriteSpanFn<0x4B>,  gpuSpriteSpanFn<0x4C>,gpuSpriteSpanFn<0x4D>,gpuSpriteSpanFn<0x4E>,gpuSpriteSpanFn<0x4F>,
  gpuSpriteSpanFn<0x50>,gpuSpriteSpanFn<0x51>,gpuSpriteSpanFn<0x52>,gpuSpriteSpanFn<0x53>,  gpuSpriteSpanFn<0x54>,gpuSpriteSpanFn<0x55>,gpuSpriteSpanFn<0x56>,gpuSpriteSpanFn<0x57>,  gpuSpriteSpanFn<0x58>,gpuSpriteSpanFn<0x59>,gpuSpriteSpanFn<0x5A>,gpuSpriteSpanFn<0x5B>,  gpuSpriteSpanFn<0x5C>,gpuSpriteSpanFn<0x5D>,gpuSpriteSpanFn<0x5E>,gpuSpriteSpanFn<0x5F>,
  gpuSpriteSpanFn<0x60>,gpuSpriteSpanFn<0x61>,gpuSpriteSpanFn<0x62>,gpuSpriteSpanFn<0x63>,  gpuSpriteSpanFn<0x64>,gpuSpriteSpanFn<0x65>,gpuSpriteSpanFn<0x66>,gpuSpriteSpanFn<0x67>,  gpuSpriteSpanFn<0x68>,gpuSpriteSpanFn<0x69>,gpuSpriteSpanFn<0x6A>,gpuSpriteSpanFn<0x6B>,  gpuSpriteSpanFn<0x6C>,gpuSpriteSpanFn<0x6D>,gpuSpriteSpanFn<0x6E>,gpuSpriteSpanFn<0x6F>,
  gpuSpriteSpanFn<0x70>,gpuSpriteSpanFn<0x71>,gpuSpriteSpanFn<0x72>,gpuSpriteSpanFn<0x73>,  gpuSpriteSpanFn<0x74>,gpuSpriteSpanFn<0x75>,gpuSpriteSpanFn<0x76>,gpuSpriteSpanFn<0x77>,  gpuSpriteSpanFn<0x78>,gpuSpriteSpanFn<0x79>,gpuSpriteSpanFn<0x7A>,gpuSpriteSpanFn<0x7B>,  gpuSpriteSpanFn<0x7C>,gpuSpriteSpanFn<0x7D>,gpuSpriteSpanFn<0x7E>,gpuSpriteSpanFn<0x7F>,
};


///////////////////////////////////////////////////////////////////////////////
//  Poligon innerloops driver
#include "inner_Poly.h"

PF gpuPolySpanDriver;
PF gpuPolySpanDrivers[256] =
{
  gpuPolySpanFn<0x00>,gpuPolySpanFn<0x01>,gpuPolySpanFn<0x02>,gpuPolySpanFn<0x03>,  gpuPolySpanFn<0x04>,gpuPolySpanFn<0x05>,gpuPolySpanFn<0x06>,gpuPolySpanFn<0x07>,  gpuPolySpanFn<0x08>,gpuPolySpanFn<0x09>,gpuPolySpanFn<0x0A>,gpuPolySpanFn<0x0B>,  gpuPolySpanFn<0x0C>,gpuPolySpanFn<0x0D>,gpuPolySpanFn<0x0E>,gpuPolySpanFn<0x0F>,
  gpuPolySpanFn<0x10>,gpuPolySpanFn<0x11>,gpuPolySpanFn<0x12>,gpuPolySpanFn<0x13>,  gpuPolySpanFn<0x14>,gpuPolySpanFn<0x15>,gpuPolySpanFn<0x16>,gpuPolySpanFn<0x17>,  gpuPolySpanFn<0x18>,gpuPolySpanFn<0x19>,gpuPolySpanFn<0x1A>,gpuPolySpanFn<0x1B>,  gpuPolySpanFn<0x1C>,gpuPolySpanFn<0x1D>,gpuPolySpanFn<0x1E>,gpuPolySpanFn<0x1F>,
  gpuPolySpanFn<0x20>,gpuPolySpanFn<0x21>,gpuPolySpanFn<0x22>,gpuPolySpanFn<0x23>,  gpuPolySpanFn<0x24>,gpuPolySpanFn<0x25>,gpuPolySpanFn<0x26>,gpuPolySpanFn<0x27>,  gpuPolySpanFn<0x28>,gpuPolySpanFn<0x29>,gpuPolySpanFn<0x2A>,gpuPolySpanFn<0x2B>,  gpuPolySpanFn<0x2C>,gpuPolySpanFn<0x2D>,gpuPolySpanFn<0x2E>,gpuPolySpanFn<0x2F>,
  gpuPolySpanFn<0x30>,gpuPolySpanFn<0x31>,gpuPolySpanFn<0x32>,gpuPolySpanFn<0x33>,  gpuPolySpanFn<0x34>,gpuPolySpanFn<0x35>,gpuPolySpanFn<0x36>,gpuPolySpanFn<0x37>,  gpuPolySpanFn<0x38>,gpuPolySpanFn<0x39>,gpuPolySpanFn<0x3A>,gpuPolySpanFn<0x3B>,  gpuPolySpanFn<0x3C>,gpuPolySpanFn<0x3D>,gpuPolySpanFn<0x3E>,gpuPolySpanFn<0x3F>,
  gpuPolySpanFn<0x40>,gpuPolySpanFn<0x41>,gpuPolySpanFn<0x42>,gpuPolySpanFn<0x43>,  gpuPolySpanFn<0x44>,gpuPolySpanFn<0x45>,gpuPolySpanFn<0x46>,gpuPolySpanFn<0x47>,  gpuPolySpanFn<0x48>,gpuPolySpanFn<0x49>,gpuPolySpanFn<0x4A>,gpuPolySpanFn<0x4B>,  gpuPolySpanFn<0x4C>,gpuPolySpanFn<0x4D>,gpuPolySpanFn<0x4E>,gpuPolySpanFn<0x4F>,
  gpuPolySpanFn<0x50>,gpuPolySpanFn<0x51>,gpuPolySpanFn<0x52>,gpuPolySpanFn<0x53>,  gpuPolySpanFn<0x54>,gpuPolySpanFn<0x55>,gpuPolySpanFn<0x56>,gpuPolySpanFn<0x57>,  gpuPolySpanFn<0x58>,gpuPolySpanFn<0x59>,gpuPolySpanFn<0x5A>,gpuPolySpanFn<0x5B>,  gpuPolySpanFn<0x5C>,gpuPolySpanFn<0x5D>,gpuPolySpanFn<0x5E>,gpuPolySpanFn<0x5F>,
  gpuPolySpanFn<0x60>,gpuPolySpanFn<0x61>,gpuPolySpanFn<0x62>,gpuPolySpanFn<0x63>,  gpuPolySpanFn<0x64>,gpuPolySpanFn<0x65>,gpuPolySpanFn<0x66>,gpuPolySpanFn<0x67>,  gpuPolySpanFn<0x68>,gpuPolySpanFn<0x69>,gpuPolySpanFn<0x6A>,gpuPolySpanFn<0x6B>,  gpuPolySpanFn<0x6C>,gpuPolySpanFn<0x6D>,gpuPolySpanFn<0x6E>,gpuPolySpanFn<0x6F>,
  gpuPolySpanFn<0x70>,gpuPolySpanFn<0x71>,gpuPolySpanFn<0x72>,gpuPolySpanFn<0x73>,  gpuPolySpanFn<0x74>,gpuPolySpanFn<0x75>,gpuPolySpanFn<0x76>,gpuPolySpanFn<0x77>,  gpuPolySpanFn<0x78>,gpuPolySpanFn<0x79>,gpuPolySpanFn<0x7A>,gpuPolySpanFn<0x7B>,  gpuPolySpanFn<0x7C>,gpuPolySpanFn<0x7D>,gpuPolySpanFn<0x7E>,gpuPolySpanFn<0x7F>,

  gpuPolySpanFn<0x80>,gpuPolySpanFn<0x81>,gpuPolySpanFn<0x82>,gpuPolySpanFn<0x83>,  gpuPolySpanFn<0x84>,gpuPolySpanFn<0x85>,gpuPolySpanFn<0x86>,gpuPolySpanFn<0x87>,  gpuPolySpanFn<0x88>,gpuPolySpanFn<0x89>,gpuPolySpanFn<0x8A>,gpuPolySpanFn<0x8B>,  gpuPolySpanFn<0x8C>,gpuPolySpanFn<0x8D>,gpuPolySpanFn<0x8E>,gpuPolySpanFn<0x8F>,
  gpuPolySpanFn<0x90>,gpuPolySpanFn<0x91>,gpuPolySpanFn<0x92>,gpuPolySpanFn<0x93>,  gpuPolySpanFn<0x94>,gpuPolySpanFn<0x95>,gpuPolySpanFn<0x96>,gpuPolySpanFn<0x97>,  gpuPolySpanFn<0x98>,gpuPolySpanFn<0x99>,gpuPolySpanFn<0x9A>,gpuPolySpanFn<0x9B>,  gpuPolySpanFn<0x9C>,gpuPolySpanFn<0x9D>,gpuPolySpanFn<0x9E>,gpuPolySpanFn<0x9F>,
  gpuPolySpanFn<0xa0>,gpuPolySpanFn<0xa1>,gpuPolySpanFn<0xa2>,gpuPolySpanFn<0xa3>,  gpuPolySpanFn<0xa4>,gpuPolySpanFn<0xa5>,gpuPolySpanFn<0xa6>,gpuPolySpanFn<0xa7>,  gpuPolySpanFn<0xa8>,gpuPolySpanFn<0xa9>,gpuPolySpanFn<0xaA>,gpuPolySpanFn<0xaB>,  gpuPolySpanFn<0xaC>,gpuPolySpanFn<0xaD>,gpuPolySpanFn<0xaE>,gpuPolySpanFn<0xaF>,
  gpuPolySpanFn<0xb0>,gpuPolySpanFn<0xb1>,gpuPolySpanFn<0xb2>,gpuPolySpanFn<0xb3>,  gpuPolySpanFn<0xb4>,gpuPolySpanFn<0xb5>,gpuPolySpanFn<0xb6>,gpuPolySpanFn<0xb7>,  gpuPolySpanFn<0xb8>,gpuPolySpanFn<0xb9>,gpuPolySpanFn<0xbA>,gpuPolySpanFn<0xbB>,  gpuPolySpanFn<0xbC>,gpuPolySpanFn<0xbD>,gpuPolySpanFn<0xbE>,gpuPolySpanFn<0xbF>,
  gpuPolySpanFn<0xc0>,gpuPolySpanFn<0xc1>,gpuPolySpanFn<0xc2>,gpuPolySpanFn<0xc3>,  gpuPolySpanFn<0xc4>,gpuPolySpanFn<0xc5>,gpuPolySpanFn<0xc6>,gpuPolySpanFn<0xc7>,  gpuPolySpanFn<0xc8>,gpuPolySpanFn<0xc9>,gpuPolySpanFn<0xcA>,gpuPolySpanFn<0xcB>,  gpuPolySpanFn<0xcC>,gpuPolySpanFn<0xcD>,gpuPolySpanFn<0xcE>,gpuPolySpanFn<0xcF>,
  gpuPolySpanFn<0xd0>,gpuPolySpanFn<0xd1>,gpuPolySpanFn<0xd2>,gpuPolySpanFn<0xd3>,  gpuPolySpanFn<0xd4>,gpuPolySpanFn<0xd5>,gpuPolySpanFn<0xd6>,gpuPolySpanFn<0xd7>,  gpuPolySpanFn<0xd8>,gpuPolySpanFn<0xd9>,gpuPolySpanFn<0xdA>,gpuPolySpanFn<0xdB>,  gpuPolySpanFn<0xdC>,gpuPolySpanFn<0xdD>,gpuPolySpanFn<0xdE>,gpuPolySpanFn<0xdF>,
  gpuPolySpanFn<0xe0>,gpuPolySpanFn<0xe1>,gpuPolySpanFn<0xe2>,gpuPolySpanFn<0xe3>,  gpuPolySpanFn<0xe4>,gpuPolySpanFn<0xe5>,gpuPolySpanFn<0xe6>,gpuPolySpanFn<0xe7>,  gpuPolySpanFn<0xe8>,gpuPolySpanFn<0xe9>,gpuPolySpanFn<0xeA>,gpuPolySpanFn<0xeB>,  gpuPolySpanFn<0xeC>,gpuPolySpanFn<0xeD>,gpuPolySpanFn<0xeE>,gpuPolySpanFn<0xeF>,
  gpuPolySpanFn<0xf0>,gpuPolySpanFn<0xf1>,gpuPolySpanFn<0xf2>,gpuPolySpanFn<0xf3>,  gpuPolySpanFn<0xf4>,gpuPolySpanFn<0xf5>,gpuPolySpanFn<0xf6>,gpuPolySpanFn<0xf7>,  gpuPolySpanFn<0xf8>,gpuPolySpanFn<0xf9>,gpuPolySpanFn<0xfA>,gpuPolySpanFn<0xfB>,  gpuPolySpanFn<0xfC>,gpuPolySpanFn<0xfD>,gpuPolySpanFn<0xfE>,gpuPolySpanFn<0xfF>
};



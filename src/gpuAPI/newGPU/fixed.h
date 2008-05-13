#ifndef FIXED_H
#define FIXED_H
///////////////////////////////////////////////////////////////////////////
typedef s64     fixed64;
typedef s32     fixed32;
typedef fixed32 fixed;

///////////////////////////////////////////////////////////////////////////
static  const u32 TABLE_BITS=16;
static  const s32 FIXED_BITS=16;

///////////////////////////////////////////////////////////////////////////
const fixed fixed_ZERO = 0;
const fixed fixed_ONE  = (1<<FIXED_BITS);
const fixed fixed_TWO  = (2<<FIXED_BITS);
const fixed fixed_HALF = fixed(.5f*(1<<FIXED_BITS));

extern s32 s_invTable[(1<<TABLE_BITS)];
void  xInit();

///////////////////////////////////////////////////////////////////////////
INLINE  fixed f2x(const float _x) { return  (int)((_x)*(float)(1<<FIXED_BITS)); }
INLINE  float x2f(const fixed _x) { return  ( ((float)(_x))/(float)(1<<FIXED_BITS) );  }

INLINE  fixed i2x(const int   _x) { return  ((_x)<<FIXED_BITS); }
INLINE  fixed x2i(const fixed _x) { return  ((_x)>>FIXED_BITS); }

template<int FIXED>
INLINE  fixed xMulxT    (const fixed _a, const fixed _b)                  { return  (fixed)( (s64)((s64)_a*(s64)_b)>>FIXED ); }

INLINE  fixed xMulx     (const fixed _a, const fixed _b)                  { return  (fixed)( (s64)((s64)_a*(s64)_b)>>FIXED_BITS ); }
INLINE  fixed xDivx     (const fixed _a, const fixed _b)                  { return  (fixed)( (s64)((s64)(_a)<<FIXED_BITS)/_b );  }
INLINE  fixed xMulxMulx (const fixed _a, const fixed _b, const fixed _c)  { return  xMulx(xMulx(_a,_b),_c); }

INLINE  int   Log2(int _a);
INLINE  void  xHiInv    (const fixed _b, s32& iFactor_, s32& iShift_)
{
  u32 uD = (_b<0) ? -_b : _b;
  if(uD>1)
  {
    u32 uLog = Log2(uD);
    uLog = uLog>(TABLE_BITS-1) ? uLog-(TABLE_BITS-1) : 0;
    u32 uDen = uD>>uLog;
    s32 t0 = s_invTable[uDen-1];
    s32 t1 = s_invTable[uDen+0];
    iFactor_ = t0 + ( ( (t1-t0)*s32(uD&((1<<uLog)-1)) )>>uLog );
    iFactor_ = (_b<0) ? -iFactor_ :iFactor_;
    iShift_  = 15+uLog;
  }
  else
  {
    iFactor_= _b;
    iShift_ = 0;
  }
}

INLINE  void  xInv      (const fixed _b, s32& iFactor_, s32& iShift_)
{
  u32 uD = (_b<0) ? -_b : _b;
  if(uD>1)
  {
    u32 uLog = Log2(uD);
    uLog = uLog>(TABLE_BITS-1) ? uLog-(TABLE_BITS-1) : 0;
    u32 uDen = uD>>uLog;
    iFactor_ = s_invTable[uDen-1];
    iFactor_ = (_b<0) ? -iFactor_ :iFactor_;
    iShift_  = 15+uLog;
  }
  else
  {
    iFactor_=_b;
    iShift_ = 0;
  }
}

INLINE  fixed xInvMulx  (const fixed _a, const s32 _iFact, const s32 _iShift)
{
  return fixed( ((s64)(_a)*(s64)(_iFact))>>(_iShift) );
}

INLINE  fixed xHiDivx   (const fixed _a, const fixed _b)
{
  s32 iFact, iShift;
  xHiInv(_b, iFact, iShift);
  return fixed( ((s64)(_a)*(s64)(iFact))>>(iShift) );
}

INLINE  fixed xLoDivx   (const fixed _a, const fixed _b)
{
  s32 iFact, iShift;
  xInv(_b, iFact, iShift);
  return fixed( ((s64)(_a)*(s64)(iFact))>>(iShift) );
}

/////////////
INLINE  fixed xLoDivx_fake   (const fixed _a, const fixed _b)
{
  return _b?xDivx(_a,_b):_a;
}

INLINE  void  xInv_fake      (const fixed _b, s32& iFactor_, s32& iShift_)
{
  iFactor_ = _b;
  return;
}

INLINE  fixed xInvMulx_fake  (const fixed _a, const s32 _iFact, const s32 _iShift)
{
  return _iFact?xDivx(_a,_iFact):_a;
}

/*
INLINE  fixed xSqrt(fixed _uVal)
{
  return f2x(sqrt(x2f(_uVal)));
}
*/

///////////////////////////////////////////////////////////////////////////
INLINE int Log2(int _a)
{
  const unsigned int b[] = {0x2, 0xC, 0xF0, 0xFF00, 0xFFFF0000};
  const unsigned int S[] = {1, 2, 4, 8, 16};
  unsigned int c = 0; // result of log2(v) will go here
  if (_a & b[4]) { _a >>= S[4]; c |= S[4];  }
  if (_a & b[3]) { _a >>= S[3]; c |= S[3];  }
  if (_a & b[2]) { _a >>= S[2]; c |= S[2];  }
  if (_a & b[1]) { _a >>= S[1]; c |= S[1];  }
  if (_a & b[0]) { _a >>= S[0]; c |= S[0];  }
  return c;
}

///////////////////////////////////////////////////////////////////////////
template<typename T>
INLINE  T Min2 (const T _a, const T _b)             { return (_a<_b)?_a:_b; }

template<typename T>
INLINE  T Min3 (const T _a, const T _b, const T _c) { return  Min2(Min2(_a,_b),_c); }

///////////////////////////////////////////////////////////////////////////
template<typename T>
INLINE  T Max2 (const T _a, const T _b)             { return  (_a>_b)?_a:_b; }

template<typename T>
INLINE  T Max3 (const T _a, const T _b, const T _c) { return  Max2(Max2(_a,_b),_c); }

///////////////////////////////////////////////////////////////////////////
#endif  //FIXED_H

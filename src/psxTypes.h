#ifndef PSX_TYPES_H
#define PSX_TYPES_H

///////////////////////////////////////////////////////////////////////////////
#ifdef PSP

#define s8 char
#define u8 unsigned char
#define s16 short
#define u16 unsigned short
#define s32 long
#define u32 unsigned long
#define s64 long long
#define u64 unsigned long long

#else

typedef char				s8;
typedef signed short		s16;
typedef signed long			s32;
typedef signed long long	s64;

typedef unsigned char		u8;
typedef unsigned short		u16;
typedef unsigned long		u32;
typedef unsigned long long	u64;

#endif

///////////////////////////////////////////////////////////////////////////////
#ifndef INLINE
  #define INLINE inline __attribute__((always_inline))
#endif

///////////////////////////////////////////////////////////////////////////////
#ifdef USE_GL
  #define PSX4ALL_WIDTH 1024
  #define PSX4ALL_HEIGHT 512
  #define PSX4ALL_PITCH 2048
#elif defined(PSP)
  #define PSX4ALL_WIDTH 512
  #define PSX4ALL_HEIGHT 240
  #define PSX4ALL_PITCH 1024
#elif defined(GP2X) && !defined(GP2X_SDL)
  #define PSX4ALL_WIDTH 480
  #define PSX4ALL_HEIGHT 240
  #define PSX4ALL_PITCH 960
#else
  #define PSX4ALL_WIDTH 320
  #define PSX4ALL_HEIGHT 240
  #define PSX4ALL_PITCH 640
#endif


///////////////////////////////////////////////////////////////////////////////
#endif //PSX_TYPES_H

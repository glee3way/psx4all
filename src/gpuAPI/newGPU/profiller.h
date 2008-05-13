#ifndef PROFILLER_H
#define PROFILLER_H

///////////////////////////////////////////////////////////////////////////////
//  Profilling helper functions, minimal libs are needed for timer.
#include "minimal.h"

///////////////////////////////////////////////////////////////////////////////
//  Minimal extensions used for better accuracy and speed.
/*
unsigned long gp2x_timer_raw(void)
{
  return gp2x_memregl[0x0A00>>2];
}

unsigned long gp2x_timer_raw_to_ticks(unsigned long ticks)
{
  return ticks/gp2x_ticks_per_second;
}

unsigned long gp2x_timer_raw_second()
{
  return 7372800;
}
*/

///////////////////////////////////////////////////////////////////////////////
//  Profiler Interface/Macro Definitions
///////////////////////////////////////////////////////////////////////////////
#ifndef ENABLE_PROFILLER
  
  #define PROFILE_NAMED_FENCE(name,time,count) 
  #define PROFILE_NAMED_PAUSE(name) 
  #define PROFILE_NAMED_RESUME(name)

  #define PROFILE_FENCE(time,count) 
  #define PROFILE_PAUSE() 
  #define PROFILE_RESUME() 

  #define PROFILE_RATIO(time,total)  (.0f)
  #define PROFILE_RESET(time,count)

#else
  ///////////////////////////////////////////////////////////////////////////////
  //  Profiller claas implementation
  class Profiller
  {
    u32& uTotalTime;
    u32  uLocalTime;

  public:
    Profiller(u32& _uTime, u32& _uCount)  : uTotalTime(_uTime)
    {
      ++_uCount;
      uLocalTime = gp2x_timer_raw();
    }

    void  Pause()
    {
      uTotalTime += gp2x_timer_raw()-uLocalTime;
    }

    void  Resume()
    {
      uLocalTime = gp2x_timer_raw();
    }

    ~Profiller()
    {
      uTotalTime += gp2x_timer_raw()-uLocalTime;
    }

    static  void  Reset(u32& _uTime, u32& _uCount)
    {
      _uTime = _uCount = 0;
    }

    static  float Ratio(u32 _uTime, u32 _uTotal)
    {
      return float(_uTime)/float(_uTotal);
    }
  };

  ///////////////////////////////////////////////////////////////////////////////
  //  Profiller macros
  #define PROFILE_NAMED_FENCE(name,time,count)  Profiller __myProfiller##name(time,count)
  #define PROFILE_NAMED_PAUSE(name)                       __myProfiller##name.Pause()
  #define PROFILE_NAMED_RESUME(name)                      __myProfiller##name.Resume()

  #define PROFILE_FENCE(time,count) PROFILE_NAMED_FENCE(0,time,count)
  #define PROFILE_PAUSE()           PROFILE_NAMED_PAUSE(0)
  #define PROFILE_RESUME()          PROFILE_NAMED_RESUME(0)

  #define PROFILE_RATIO(time,total) Profiller::Ratio(time,total)
  #define PROFILE_RESET(time,count) Profiller::Reset(time,count)


#endif  //ENABLE_PROFILLER


///////////////////////////////////////////////////////////////////////////////
#endif  // PROFILLER_H

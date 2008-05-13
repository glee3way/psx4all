/* 
  SDL wrapper by Chui. Preliminar version.
*/

/*

  GP2X minimal library v0.A by rlyeh, (c) 2005. emulnation.info@rlyeh (swap it!)

  Thanks to Squidge, Robster, snaff, Reesy and NK, for the help & previous work! :-)

  License
  =======

  Free for non-commercial projects (it would be nice receiving a mail from you).
  Other cases, ask me first.

  GamePark Holdings is not allowed to use this library and/or use parts from it.

*/

#ifndef __MINIMAL_H__
#define __MINIMAL_H__

#include <math.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdarg.h>
#include <SDL.h>
#include "SDL_gp2x.h"
#include "../common.h"


#define DISPLAY_X 320
#define DISPLAY_Y 240

#define GP2X_BYTE_SIZE_SCREEN (320*240)
#define AUDIO_BUFFER_SIZE (1024*2)


enum  { GP2X_UP=0x1,       GP2X_LEFT=0x4,       GP2X_DOWN=0x10,  GP2X_RIGHT=0x40,
        GP2X_START=1<<8,   GP2X_SELECT=1<<9,    GP2X_L=1<<10,    GP2X_R=1<<11,
        GP2X_A=1<<12,      GP2X_B=1<<13,        GP2X_X=1<<14,    GP2X_Y=1<<15,
        GP2X_VOL_UP=1<<23, GP2X_VOL_DOWN=1<<22, GP2X_PUSH=1<<27 };
                                            
#define gp2x_video_RGB_color16(R,G,B) ((unsigned short)(((((R)&0xF8)<<8)|(((G)&0xFC)<<3)|(((B)&0xF8)>>3))))

extern unsigned long	gp2x_ticks_per_second;
extern void		*gp2x_sdlwrapper_screen_pixels;
extern SDL_Surface	*gp2x_sdlwrapper_screen;
#define sdlscreen gp2x_sdlwrapper_screen
extern double		gp2x_sdlwrapper_ticksdivisor;
#define gp2x_screen16	((unsigned short *)gp2x_sdlwrapper_screen_pixels)

typedef struct gp2x_font        { int x,y,w,wmask,h,fg,bg,solid; unsigned char *data; } gp2x_font;

extern void 		gp2x_sound_play(void *buff, int len);
extern unsigned long 	gp2x_joystick_read(void);
extern void 		gp2x_printf(gp2x_font *, int, int, const char *, ...);
extern void 		gp2x_printf_init(gp2x_font *, int, int, void *, int, int, int);

extern void 		gp2x_init(int, int, int, int, int, int, int);
extern void 		gp2x_deinit(void);

extern void 		gp2x_timer_delay_raw(unsigned long raws);
extern void 		gp2x_timer_delay(unsigned long ticks);
extern unsigned long 	gp2x_timer_raw(void);
extern unsigned long 	gp2x_timer_read(void);

#define gp2x_timer_raw_second() (7372800)
#define gp2x_timer_raw_to_ticks(t) ((t)/gp2x_ticks_per_second)

#define gp2x_video_flip()	/*SDL_Flip(sdlscreen);*/
#define gp2x_video_flip_single() gp2x_video_flip()

#define gp2x_sound_volume(LEFT,RIGHT)

extern void gp2x_change_res(int w, int h);
extern void gp2x_video_RGB_clearscreen16(void);

#endif

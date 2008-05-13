/*

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; version 2
 of the License.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#ifndef APP_IPHONE_H
#define APP_IPHONE_H

#import <CoreSurface/CoreSurface.h>
#import <AudioToolbox/AudioQueue.h>

#define BIT_U		0x1
#define BIT_D		0x10
#define BIT_L 		0x4
#define BIT_R	 	0x40
#define BIT_SEL		(1<<9)
#define BIT_ST		(1<<8)
#define BIT_LPAD	(1<<10)
#define BIT_RPAD	(1<<11)
#define BIT_A		(1<<12)
#define BIT_B		(1<<13)
#define BIT_X		(1<<14)
#define BIT_Y		(1<<15)
#define BIT_VOL_UP	(1<<23)
#define BIT_VOL_DOWN	(1<<22)
#define BIT_PUSH	(1<<27)
#define BIT_MENU	(1<<31)

typedef unsigned char byte;

struct app_Preferences {
    byte frameSkip;
    byte debug;
    byte canDeleteROMs;
    byte landscape;
    byte allowSuspend;
    byte scaled;
    byte muted;
    byte selectedSkin;
    byte interlace;
    byte bios;
};

void setDefaultPreferences();
int app_SavePreferences();
int app_LoadPreferences();

/* STUBs to emulator core */

void *app_Thread_Start(void *args);
void *app_Thread_Resume(void *args);
void app_Halt(void);
void app_Resume(void);
int app_LoadROM(const char *fileName);
void app_DeleteTempState(void);
int app_OpenSound(int samples_per_sync, int sample_rate);
void app_CloseSound(void);
void app_StopSound();
void app_StartSound();
unsigned long joystick_read();
FILE* fopen_home(char* filename, char* fileop);

extern byte IS_DEBUG;
extern byte IS_CHANGING_ORIENTATION;
extern unsigned short  *BaseAddress;
extern int __screenOrientation;
extern struct app_Preferences preferences;
extern unsigned short *videobuffer;
//extern int   __emulation_run;
extern char appworkdir[512];

/* Audio Resources */
#define AUDIO_BUFFERS 2
#define AUDIO_PRECACHE 3
#define WAVE_BUFFER_SIZE 735
#define WAVE_BUFFER_BANKS 25

typedef struct AQCallbackStruct {
    AudioQueueRef queue;
    UInt32 frameCount;
    AudioQueueBufferRef mBuffers[AUDIO_BUFFERS];
    AudioStreamBasicDescription mDataFormat;
} AQCallbackStruct;

#ifndef GUI_DEBUG
#define LOGDEBUG(...) while(0){}
#else
void LOGDEBUG(const char *text, ...);
#endif

#endif

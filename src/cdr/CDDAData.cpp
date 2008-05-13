/************************************************************************

Copyright mooby 2002

CDRMooby2 CDDAData.cpp
http://mooby.psxfanatics.com

  This file is protected by the GNU GPL which should be included with
  the source code distribution.

************************************************************************/

#include "../common.h"

#include "CDDAData.hpp"
using namespace std;

extern std::string programName;

#if 0
#define OSS_MODE_STEREO	1
#define OSS_MODE_MONO	0
#define OSS_SPEED_44100	44100
static int cdda_oss_audio_fd = -1;

/* Sound Thread variables */
#define DEFAULT_SAMPLE_NUM_BUFF 16 								// Number of sound buffers
pthread_t cdda_sound_thread=0;									// Thread for gp2x_sound_thread_play()
volatile int cdda_sound_thread_exit=0;								// Flag to end gp2x_sound_thread_play() thread
volatile int cdda_sound_buffer=0;								// Current sound buffer
short cdda_sound_buffers[32768*DEFAULT_SAMPLE_NUM_BUFF];					// Sound buffers
volatile int sndlen=32768;									// Current sound buffer length

/* Sound Thread*/
static void *cdda_sound_thread_play(void *none)
{
	int nbuff=cdda_sound_buffer;								// Number of the sound buffer to play
	do {
		write(cdda_oss_audio_fd, (const void *)&cdda_sound_buffers[32768*nbuff], sndlen); 	// Play the sound buffer
		ioctl(cdda_oss_audio_fd, SOUND_PCM_SYNC, 0); 					// Synchronize Audio
		if (nbuff!=cdda_sound_buffer) {							// Try to follow the write sound buffer
			nbuff++; if (nbuff==DEFAULT_SAMPLE_NUM_BUFF) nbuff=0;			// Increase the sound buffer to play
		}
	} while(!cdda_sound_thread_exit);							// Until the end of the sound thread
	return NULL;
}

/* Mute Sound Thread*/
void cdda_sound_thread_mute(void)
{
	memset(cdda_sound_buffers,0,32768*DEFAULT_SAMPLE_NUM_BUFF*2);
	sndlen=32768;
}

/* Start Sound Core */
void cdda_SetupSound(void)
{
#if 0 //ndef NOSOUND
 	int pspeed=44100;
 	int pstereo;
 	int format;
 	int fragsize = 0;
 	int myfrag;
 	int oss_speed, oss_stereo;
 	int L, R;

	pstereo=OSS_MODE_MONO;

 	oss_speed = pspeed;
 	oss_stereo = pstereo;

 	if((cdda_oss_audio_fd=open("/dev/dsp",O_WRONLY,0))==-1)
  	{
   		printf("Sound device not available!\n");
   		return;
  	}

 	if(ioctl(cdda_oss_audio_fd,SNDCTL_DSP_RESET,0)==-1)
  	{
   		printf("Sound reset failed\n");
   		return;
  	}

 	format = AFMT_S16_LE;
 	if(ioctl(cdda_oss_audio_fd,SNDCTL_DSP_SETFMT,&format) == -1)
  	{
   		printf("Sound format not supported!\n");
   		return;
  	}

 	if(format!=AFMT_S16_LE)
  	{
   		printf("Sound format not supported!\n");
   		return;
  	}

 	if(ioctl(cdda_oss_audio_fd,SNDCTL_DSP_STEREO,&oss_stereo)==-1)
  	{
   		printf("Stereo mode not supported!\n");
   		return;
  	}

 	if(ioctl(cdda_oss_audio_fd,SNDCTL_DSP_SPEED,&oss_speed)==-1)
  	{
   		printf("Sound frequency not supported\n");
   		return;
  	}
 	if(oss_speed!=pspeed)
 	{
   		printf("Sound frequency not supported\n");
   		return;
  	}

 	L=(((100*0x50)/100)<<8)|((100*0x50)/100);          
 	ioctl(cdda_oss_audio_fd, SOUND_MIXER_WRITE_PCM, &L); 

	/* Initialize sound thread */
	cdda_sound_thread_mute();
	cdda_sound_thread_exit=0;
	pthread_create( &cdda_sound_thread, NULL, cdda_sound_thread_play, NULL);

#endif
}

/* Stop Sound Core */
void RemoveSound(void)
{
#if 0 //ndef NOSOUND
 	cdda_sound_thread_exit=1;
 	cdda_timer_delay(1000L);
 	if(cdda_oss_audio_fd != -1 )
  	{
   		close(cdda_oss_audio_fd);
   		cdda_oss_audio_fd = -1;
  	}
#endif
}

/* Feed Sound Data */
void SoundFeedStreamData(unsigned char* pSound,long lBytes)
{
#if 0 //ndef NOSOUND
 	if(cdda_oss_audio_fd == -1) return;
	int nbuff=cdda_sound_buffer; nbuff++; if (nbuff==DEFAULT_SAMPLE_NUM_BUFF) nbuff=0;	// Number of the sound buffer to write
	memcpy(&cdda_sound_buffers[32768*nbuff],pSound,lBytes);					// Write the sound buffer
	sndlen=lBytes;										// Update the sound buffer length
	cdda_sound_buffer=nbuff;								// Update the current sound buffer
#endif
}
#endif

#if 0 //
// this callback repeats one track over and over
int CDDACallbackRepeat(  void *inputBuffer, void *outputBuffer,
                     unsigned long framesPerBuffer,
                     PaTimestamp outTime, void *userData )
{
   unsigned int i;
/* Cast data passed through stream to our structure type. */
   PlayCDDAData* data = (PlayCDDAData*)userData;
   short* out = (short*)outputBuffer;
    
   data->theCD->seek(data->CDDAPos);
   short* buffer = (short*)data->theCD->getBuffer();
   
   buffer += data->frameOffset;

   double volume = data->volume;

      // buffer the data
   for( i=0; i<framesPerBuffer; i++ )
   {
    /* Stereo channels are interleaved. */
      *out++ = *buffer++ * volume;              /* left */
      *out++ = *buffer++ * volume;             /* right */
      data->frameOffset += 4;

         // at the end of a frame, get the next one
      if (data->frameOffset == bytesPerFrame)
      {
         data->CDDAPos += CDTime(0,0,1);

            // when at the end of this track, loop to the start
            // of this track
         if (data->CDDAPos == data->CDDAEnd)
         {
            data->CDDAPos = data->CDDAStart;
         }

         data->theCD->seek(data->CDDAPos);
         data->frameOffset = 0;
         buffer = (short*)data->theCD->getBuffer();
      }
   }
   return 0;
}

// this callback plays through one track once and stops
int CDDACallbackOneTrackStop(  void *inputBuffer, void *outputBuffer,
                     unsigned long framesPerBuffer,
                     PaTimestamp outTime, void *userData )
{
   unsigned int i;
/* Cast data passed through stream to our structure type. */
   PlayCDDAData* data = (PlayCDDAData*)userData;
   short* out = (short*)outputBuffer;
   short* buffer;

      // seek to the current CDDA read position
   if (!data->endOfTrack)
   {
      data->theCD->seek(data->CDDAPos);
      buffer = (short*)data->theCD->getBuffer();
   }
   else
   {
      buffer = (short*)data->nullAudio;
   }

   buffer += data->frameOffset;

   double volume = data->volume;

      // buffer the data
   for( i=0; i<framesPerBuffer; i++ )
   {
    /* Stereo channels are interleaved. */
      *out++ = *buffer++ * volume;              /* left */
      *out++ = *buffer++ * volume;             /* right */
      data->frameOffset += 4;

         // at the end of a frame, get the next one
      if (data->frameOffset == bytesPerFrame)
      {
         data->CDDAPos += CDTime(0,0,1);

            // when at the end of this track, use null audio
         if (data->CDDAPos == data->CDDAEnd)
         {
            data->endOfTrack = true;
            buffer = (short*)data->nullAudio;
            data->CDDAPos -= CDTime(0,0,1);
            data->frameOffset = 0;
         }
            // not at end of track, just do normal buffering
         else
         {
            data->theCD->seek(data->CDDAPos);
            data->frameOffset = 0;
            buffer = (short*)data->theCD->getBuffer();
         }
      }
   }
   return 0;
}
#endif

// initialize the CDDA file data and initalize the audio stream
void PlayCDDAData::openFile(const std::string& file) 
{
//   PaError err;
   std::string extension;
   theCD = FileInterfaceFactory(file, extension);
   if (pregap)
      theCD->setPregap(true, CDTime(0,0,0));
#if 0 //
   err = Pa_Initialize();
   if( err != paNoError )
   {
      Exception e(string("PA Init error: ") + string(Pa_GetErrorText( err )));
      THROW(e);
   }
#endif
}
   
// start playing the data
int PlayCDDAData::play(const CDTime& startTime)
{
   CDTime localStartTime = startTime + CDTime(0,2,0);
   
      // if play was called with the same time as the previous call,
      // dont restart it.  this fixes a problem with FPSE's play call.
      // of course, if play is called with a different track, 
      // stop playing the current stream.
   if (playing)
   {
      if (startTime == InitialTime)
      {
         return 0;
      }
      else
      {
         stop();
      }
   }

   InitialTime = startTime;
#if 0 //   
   // make sure there's a valid option chosen
   if ((prefs.prefsMap[repeatString] != repeatOneString) &&
       (prefs.prefsMap[repeatString] != repeatAllString) &&
       (prefs.prefsMap[repeatString] != playOneString))
   {
      prefs.prefsMap[repeatString] = repeatAllString;
      prefs.write();
   }
#endif
      // set the cdda position, start and end times
   CDDAPos = localStartTime;

#if 0 //
      // figure out which track to play to set the end time
   if ( (prefs.prefsMap[repeatString] == repeatOneString) ||
        (prefs.prefsMap[repeatString] == playOneString))
#endif
   {
      unsigned int i = 1;
      while ( (i < trackList.size()) && (startTime > trackList[i].trackStart) )
      {
         i++;
      }
      CDDAStart = localStartTime;
      CDDAEnd = trackList[i].trackStart + trackList[i].trackLength;
   }
#if 0 //
   else if (prefs.prefsMap[repeatString] == repeatAllString)
   {
      CDDAEnd = trackList[trackList.size() - 1].trackStart +
         trackList[trackList.size() - 1].trackLength;
      CDDAStart = trackList[2].trackStart;
   }
#endif
   endOfTrack = false;
#if 0 //
      // open a stream - pass in this CDDA object as the user data.
      // depending on the play mode, use a different callback
   PaError err;
   
   if (prefs.prefsMap[repeatString] == repeatAllString)
      err = Pa_OpenDefaultStream(&stream, 0, 2, paInt16, 44100, 5880, 
                                 0, CDDACallbackRepeat, this);
   else if (prefs.prefsMap[repeatString] == repeatOneString)
      err = Pa_OpenDefaultStream(&stream, 0, 2, paInt16, 44100, 5880, 
                                 0, CDDACallbackRepeat, this);
   else if (prefs.prefsMap[repeatString] == playOneString)
      err = Pa_OpenDefaultStream(&stream, 0, 2, paInt16, 44100, 5880, 
                                 0, CDDACallbackOneTrackStop, this);

   if( err != paNoError )
   {
     return 0;
   }
  
      // start the stream.  the CDDACallback will automatically get 
      // called to buffer the audio
   err = Pa_StartStream( stream );

   if( err != paNoError )
   {
     return 0;
   }
#endif
   playing = true;
   return 0;
}

// close the stream - nice and simple
int PlayCDDAData::stop()
{
   if (playing)
   {
#if 0 //
      PaError err = Pa_CloseStream( stream );
      if( err != paNoError )
      {  
         Exception e(string("PA Close Stream error: ") + string(Pa_GetErrorText( err )));
         THROW(e);
      }
#endif
      playing = false;
   }
   return 0;
}


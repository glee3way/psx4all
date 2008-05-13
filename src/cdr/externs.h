/************************************************************************

Copyright mooby 2002

CDRMooby2 externs.h
http://mooby.psxfanatics.com

  This file is protected by the GNU GPL which should be included with
  the source code distribution.

************************************************************************/



#ifndef EXTERNS_H
#define EXTERNS_H

#include "defines.h"

#if 0
// sets all the callbacks as extern "c" for linux compatability
extern "C"
{

void  CDR_about(void);
long  CDR_test(void);
long  CDR_configure(void);
long  CDR_close(void);
long  CDR_open(void);
long  CDR_shutdown(void);
long  CDR_play(unsigned char * sector);
long  CDR_stop(void);

char  CDR_getDriveLetter(void);
long  CDR_init(void);
long  CDR_getTN(unsigned char *buffer);
unsigned char *  CDR_getBufferSub(void);
long  CDR_getTD(unsigned char track, unsigned char *buffer);
long  CDR_readTrack(unsigned char *time);
unsigned char * CDR_getBuffer(void);


void   CD_About(UINT32 *par);
int CD_Wait(void);
void CD_Close(void);
int CD_Open(unsigned int* par);
int CD_Play(unsigned char * sector);
int CD_Stop(void);
int CD_GetTN(char* result);
unsigned char* CD_GetSeek(void);
unsigned char* CD_Read(unsigned char* time);
int CD_GetTD(char* result, int track);
int    CD_Configure(UINT32 *par);

}
#endif

#endif

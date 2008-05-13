/************************************************************************

Copyright mooby 2002

CDRMooby2 Open.cpp
http://mooby.psxfanatics.com

  This file is protected by the GNU GPL which should be included with
  the source code distribution.

************************************************************************/


#include <iostream>
#include <string>

#include "../common.h"

#include "CDInterface.hpp"
#include "defines.h"
#include "Preferences.hpp"


#include "externs.h"
#include <stdlib.h>

using namespace std;

extern CDInterface* theCD;
extern int rc;
extern TDTNFormat tdtnformat;
extern EMUMode mode;
extern s8* packfile;

int CD_Wait(void)
{
   return FPSE_OK;
}

void closeIt(void)
{
   if (theCD)
   {
      delete theCD;
      theCD = NULL;
   }
}

long CDR_close(void)
{
   closeIt();
   return 0;
}

void CD_Close(void)
{
   closeIt();
}

void openIt(void)
{
   std::string theFile;
   if (theCD)
      CDR_close();
   theCD = new CDInterface();
   theFile = packfile;
   theCD->open(theFile);
}

// psemu open call - call open
s32 CDR_open()
{
   mode = psemu;
   openIt();
   return 0;
}

int CD_Open(unsigned int* par)
{
   mode = fpse;
   openIt();
   return FPSE_OK;
}

s32 CDR_shutdown()
{
   return CDR_close();
}

s32 CDR_play(unsigned char * sector)
{  
   return theCD->playTrack(CDTime(sector, msfint));
}

int CD_Play(unsigned char * sector)
{
   return theCD->playTrack(CDTime(sector, msfint));
}

s32 CDR_stop(void)
{
	return theCD->stopTrack();
}

int CD_Stop(void)
{
   return theCD->stopTrack();
}

s8* CDR_getDriveLetter(void)
{
   return NULL;
}


s32 CDR_init(void)
{
   theCD=NULL;
   return 0;
}

s32 CDR_getTN(unsigned char *buffer)
{
   buffer[0] = 1;
   if (tdtnformat == fsmint)
      buffer[1] = (char)theCD->getNumTracks();
   else
      buffer[1] = intToBCD((char)theCD->getNumTracks());
   return 0;
}

int CD_GetTN(char* buffer)
{
   buffer[1] = 1;
   buffer[2] = (char)theCD->getNumTracks();
   return FPSE_OK;
}

u8* CDR_getBufferSub(void)
{
   return theCD->readSubchannelPointer();
}

u8* CD_GetSeek(void)
{
   return theCD->readSubchannelPointer() + 12;
}

s32 CDR_getTD(unsigned char track, unsigned char *buffer)
{
   if (tdtnformat == fsmint)
      memcpy(buffer, theCD->getTrackInfo(track).trackStart.getMSFbuf(tdtnformat), 3);
   else
      memcpy(buffer, theCD->getTrackInfo(BCDToInt(track)).trackStart.getMSFbuf(tdtnformat), 3);
   return 0;
}

s32 CD_GetTD(char* result, int track)
{
   MSFTime now = theCD->getTrackInfo(BCDToInt(track)).trackStart.getMSF();
   result[1] = now.m();
   result[2] = now.s();

   return FPSE_OK;
}

s32 CDR_readTrack(unsigned char *time)
{
   try
   {
      CDTime now(time, msfbcd);
      theCD->moveDataPointer(now);
   }
   catch(Exception &e)
   {
      ostringstream out;
      out << "oops" << endl << e.text();
      moobyMessage(out.str());
      exit(0);      
   }
   catch(std::exception& e)
   {
      ostringstream out;
      out << "oops" << endl << e.what();
      moobyMessage(out.str());
      exit(0);      
   }
   catch(...)
   {
      moobyMessage("unknown error");
      exit(0);      
   }
   return 0;
}

unsigned char* CD_Read(unsigned char* time)
{
   CDTime now(time, msfint);
   theCD->moveDataPointer(now);
   return theCD->readDataPointer() + 12;
}

u8* CDR_getBuffer(void)
{
   return theCD->readDataPointer() + 12;
}

s32 CDR_getStatus(CdrStat* stat) 
{
   MSFTime now;
   if (theCD->isPlaying())
   {
      stat->Type = 0x02;
      stat->Status = 0x80;
   }
   else
   {
      stat->Type = 0x01;
      stat->Status = 0x20;
   }
   now = theCD->readTime().getMSF();
   stat->Time[0] = intToBCD(now.m());
   stat->Time[1] = intToBCD(now.s());
   stat->Time[2] = intToBCD(now.f());
   return 0;
}

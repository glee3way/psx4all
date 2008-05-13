#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>

#include <zlib.h>

#include "cdriso.h"

extern s8* packfile;

s8* IsoFile;
FILE *cdHandle = NULL;
u8* cdbuffer = NULL;
u8* cdlastbuffer;
u32 cdlastsector;
s32 fmode;
u8* Ztable = NULL;

s32 CDR_init() {
	return 0;
}

s32 CDR_shutdown() {
	return 0;
}

s32 UpdateZmode() 
{
	s32 len = strlen(IsoFile);

	if (len >= 2) 
	{
		if (!strncasecmp(IsoFile+(len-2), ".z", 2)) 
		{
			return 1;
		}
	}

	if (len >= 4) 
	{
		if (!strncasecmp(IsoFile+(len-4), ".znx", 4)) 
		{
			return 3;
		}
	}
	
	return 0;
}

s32 CDR_open() {
	long filesize = 0;
	s32 Zmode;
	u32 i;

	cdbuffer = (u8*)malloc(BUFFER_SIZE);
	cdlastbuffer = cdbuffer;
	cdlastsector = 0x7FFFFFFF;

	if (cdHandle != NULL)
		return 0;				/* it's already open */

	//cdbuffer = (u8*)malloc(BUFFER_SIZE);
	//if( cdbuffer == NULL)
	//	return -1;

	IsoFile = packfile;

	Zmode = UpdateZmode();

	if (Zmode) {
		FILE *f;
		s8 table[512];

		fmode = Zmode;
#ifdef PANDORA
		s8 shortfile[512];
		int shortlen;
		sprintf(shortfile, "%s", IsoFile);
		shortlen = strlen(shortfile);
	
		if(Zmode == 1)
		{
			if(shortlen > 3)
			{
				shortfile[shortlen-2] = '\0';
				if(shortlen < 8+2)
				{
					sprintf(table, "%sz~1.tab", shortfile);
				}
				else
				{
					sprintf(table, "%s.tab", shortfile);				
				}
			}
		}
		else if(Zmode == 3)
		{
			if(shortlen > 4)
			{
				shortfile[shortlen-4] = '\0';
				if(shortlen < 8+4)
				{
					sprintf(table, "%sznx~1.tab", shortfile);
				}
				else
				{
					sprintf(table, "%s.tab", shortfile);				
				}
			}
		}
		else if(Zmode == 2)
		{
			if(shortlen > 3)
			{
				shortfile[shortlen-3] = '\0';
				if(shortlen < 8+3)
				{
					sprintf(table, "%sbz~1.ind", shortfile);
				}
				else
				{
					sprintf(table, "%s.ind", shortfile);				
				}
			}
		}
		printf("Loading %s\n", table);
		f = fopen(table, "rb");
		if( f == NULL )
#endif
		{
			if (Zmode == 1 || Zmode == 3) sprintf(table, "%s.table", IsoFile);
			else sprintf(table, "%s.index", IsoFile);			
			f = fopen(table, "rb");
			if( f == NULL )
			{
					cdHandle = NULL;
					return -1;
			}
		}
		
		fseek(f, 0, SEEK_END);
		filesize = ftell(f);
		fseek(f, 0, SEEK_SET);
		Ztable = (u8*)malloc(filesize);	
		if (Ztable == NULL) 
		{
			SysPrintf("Error loading %s", table);
			gp2x_timer_delay(3000);
			SysPrintf("Couldn't allocate %ld bytes", filesize);
			gp2x_timer_delay(3000);
			cdHandle = NULL;
			return -1;
		}

		fread(Ztable, 1, filesize, f);
		fclose(f);
	} else {
		fmode = 0;
	}

	cdHandle = fopen(IsoFile, "rb");
	if (cdHandle == NULL) {
		SysPrintf("Error loading %s\n", IsoFile);
		gp2x_timer_delay(1000);
		return -1;
	}

	return 0;
}

s32 CDR_close() {
	if (cdHandle == NULL)
		return 0;

	fclose(cdHandle);
	cdHandle = NULL;
	if(cdbuffer) { free(cdbuffer); cdbuffer = NULL; }

	if(Ztable) { free(Ztable); Ztable = NULL; }

	return 0;
}

// return Starting and Ending Track
// buffer:
//  byte 0 - start track
//  byte 1 - end track
s32 CDR_getTN(u8 *buffer) {
	buffer[0] = 1;
	buffer[1] = 1;

	return 0;
}

// return Track Time
// buffer:
//  byte 0 - frame
//  byte 1 - second
//  byte 2 - minute
s32 CDR_getTD(u8 track, u8 *buffer) {
	buffer[2] = 0;
	buffer[1] = 2;
	buffer[0] = 0;

	return 0;
}

// read track
// time : int 0 - minute ; int 1 - second ; int 2 - frame
// uses int format
s32 CDR_readTrack(u8 *time) {
	// check cache for current sector
	u32 curr_sector = MSF2SECT((time[0]), (time[1]), (time[2]));
	s32 i;
	u8 minute = (time[0]);

	if( 	curr_sector >= cdlastsector && curr_sector < cdlastsector + BUFFER_SECTORS	)
	{
		cdlastbuffer = (u8*)((u32)cdbuffer + ((curr_sector - cdlastsector) * CD_FRAMESIZE_RAW));
		return 0;
	}

	// Not found in cache...
	cdlastsector = curr_sector;
	cdlastbuffer = cdbuffer;

	if (!fmode) {
		fseek(cdHandle, curr_sector * CD_FRAMESIZE_RAW, SEEK_SET);
		fread(cdlastbuffer, (CD_FRAMESIZE_RAW*BUFFER_SECTORS), 1, cdHandle);
	} else if (fmode == 1) { //.Z
		int ret;
		for(i = 0; i < BUFFER_SECTORS; i++)
		{
			u32 pos, p;
			u32 size;
			u8 Zbuf[CD_FRAMESIZE_RAW+256];

			p = curr_sector + i;

			pos = (u32)(Ztable[p * 6] | (Ztable[p * 6 + 1] << 8) | 
				(Ztable[p * 6 + 2] << 16) | (Ztable[p * 6 + 3] << 24));
			fseek(cdHandle, pos, SEEK_SET);

			p = (u16)(Ztable[p * 6 + 4] | (Ztable[p * 6 + 4 + 1] << 8));
      fread(Zbuf, p, 1, cdHandle);

			size = CD_FRAMESIZE_RAW;
			ret = uncompress(cdlastbuffer + (i * CD_FRAMESIZE_RAW), &size, Zbuf, p);
			if( ret != Z_OK )
			{
				printf("uncompress error %d !\n", ret);
				gp2x_deinit();
			}
		}
	} 
	else if (fmode == 2) 
	{ // .bz
		SysPrintf("BZ FORMAT IS NOT SUPPORTED.");
	}
	else { // .ZNX
		for(i = 0; i < BUFFER_SECTORS; i++)
		{
			int ret;
			u32 pos, p;
			u32 size;
			u8 Zbuf[CD_FRAMESIZE_RAW+256];

			p = curr_sector + i;

			pos = (u32)(Ztable[p * 10] | (Ztable[p * 10 + 1] << 8) | (Ztable[p * 10 + 2] << 16) | (Ztable[p * 10 + 3] << 24));
			fseek(cdHandle, pos, SEEK_SET);

			p = (u16)(Ztable[p * 10 + 4] | (Ztable[p * 10 + 4 + 1] << 8));
      fread(Zbuf, p, 1, cdHandle);
			size = CD_FRAMESIZE_RAW;
			ret = uncompress(cdlastbuffer + (i * CD_FRAMESIZE_RAW), &size, Zbuf, p);
			if( ret != Z_OK )
			{
				printf("uncompress error %d !\n", ret);
				gp2x_deinit();
			}
		}
	}

	return 0;
}

// return the last read buffer
u8* CDR_getBuffer() {
	return (u8*)cdlastbuffer + 12;
}

// plays cdda audio
// sector : byte 0 - minute ; byte 1 - second ; byte 2 - frame
// does NOT uses bcd format
s32 CDR_play(u8 *sector) {
	return 0;
}

// stops cdda audio
s32 CDR_stop() {
	return 0;
}

s32 CDR_test() {
	if (*IsoFile == 0)
		return 0;
	cdHandle = fopen(IsoFile, "rb");
	if (cdHandle == NULL)
		return -1;
	fclose(cdHandle);
	cdHandle = NULL;
	return 0;
}

u8* CDR_getBufferSub() { return NULL; }

s32 CDR_getStatus(struct CdrStat* cdrstat) {
#if 0
    if (cdOpenCase) cdrstat->Status = 0x10;
    else		cdrstat->Status = 0;
#endif
	cdrstat->Status = 0;

    return 0;
}
